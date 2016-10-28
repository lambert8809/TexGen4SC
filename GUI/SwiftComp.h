/*=============================================================================
TexGen: Geometric textile modeller.
Copyright (C) 2006 Martin Sherburn

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
=============================================================================*/
#include <vector>
#include <string>

#pragma once

class CSwiftComp : public wxWizard
{
public:
	CSwiftComp(wxWindow* parent, wxWindowID id = wxID_ANY);
	~CSwiftComp(void);

	bool RunIt();
	string GetSwiftCompPara(string ExistingPara = "");
	void readinp(string jobname,vector<string> &inpc);
	int analysis_type();
	//	void LoadSettings(const CTextileWeave2D &Weave);
	//	void LoadSettings(const CShearedTextileWeave2D &Weave);

protected:
	void OnWizardPageChanging(wxWizardEvent& event);
	void BuildPages();
	wxWizardPageSimple* BuildFirstPage();
	void OnAreaChanged(wxCommandEvent& event) { m_bAreaChanged = true; }
	void OnInit(wxInitDialogEvent& event) {m_bAreaChanged = false;}

	//	void OnRefine(wxCommandEvent& event);

	void Onmodel(wxCommandEvent& event);
	void Ontype(wxCommandEvent& event);
	void RefreshGapTextBox();

	bool m_btype1; // elastic
	bool m_btype2; // thermoelasitc
	bool m_bSolid;  // Solid model	
	bool m_b2D;   // 2D model
	bool m_b2Dmodel1; // Kirchhoff-love plate
	bool m_b2Dmodel2; // Reissner-Mindlin plate
	int analysis;
	
	bool m_bAreaChanged;

	wxWizardPageSimple *m_pFirstPage;

	DECLARE_EVENT_TABLE()
};


