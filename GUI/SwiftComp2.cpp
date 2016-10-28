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

#include "PrecompiledHeaders.h"
#include "SwiftComp2.h"
#include "WindowIDs.h"
#include "WeavePatternCtrl.h"
#include "PythonConverter.h"

#include "Wizard.xpm"
#include "RangeValidator.h"

#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <string>
#include <string.h>
#include <locale>
#include <sstream>


BEGIN_EVENT_TABLE(CSwiftComp2, wxWizard)
//EVT_WIZARD_PAGE_CHANGING(wxID_ANY, CSwiftComp2::OnWizardPageChanging)
EVT_CHECKBOX(ID_SG_2D, CSwiftComp2::Onmodel)
EVT_CHECKBOX(ID_SG_Solid, CSwiftComp2::Onmodel)
EVT_CHECKBOX(ID_SG_Type1, CSwiftComp2::Ontype)
EVT_CHECKBOX(ID_SG_Type2, CSwiftComp2::Ontype)
EVT_INIT_DIALOG(CSwiftComp2::OnInit)

//	EVT_BUTTON(ID_WeavePattern, CWeaveWizard::OnWeavePattern)
END_EVENT_TABLE()

CSwiftComp2::CSwiftComp2(wxWindow* parent, wxWindowID id)
: wxWizard(parent, id, wxT("SwiftComp Wizard"), wxBitmap(Wizard_xpm))
  , m_pFirstPage(NULL),passf(false),	\
  m_bSolid(true), m_b2D(false),\
  m_btype1(true), m_btype2(false),\
  m_b2Dmodel1(false), m_b2Dmodel2(false),\
  xvoxel(wxT("15")),yvoxel(wxT("15")),zvoxel(wxT("15"))
{
  BuildPages();
  GetPageAreaSizer()->Add(m_pFirstPage);
}

CSwiftComp2::~CSwiftComp2(void)
{
  //  if (SCdialog)
  //  SCdialog->Destroy();
}

bool CSwiftComp2::RunIt()
{
	return RunWizard(m_pFirstPage);
}

void CSwiftComp2::BuildPages()
{
	m_pFirstPage = BuildFirstPage();
	//	SCdialog = BuildSavePage();

//	wxWizardPageSimple::Chain(m_pFirstPage, m_pSecondPage);
}

wxWizardPageSimple* CSwiftComp2::BuildFirstPage()
{
	wxWizardPageSimple *pPage = new wxWizardPageSimple(this);

	wxBoxSizer *pMainSizer = new wxBoxSizer(wxVERTICAL);
	wxSizerFlags SizerFlags(0);
	SizerFlags.Border();
	SizerFlags.Expand();

	wxSizer *pSubSizer;

	pMainSizer->Add(new wxStaticText(pPage, wxID_ANY, wxT("This wizard will create SwiftComp input file for you.")), SizerFlags);

	//Create abaqus voxel information
	SizerFlags.Align(wxALIGN_CENTER_VERTICAL);
	pSubSizer = new wxFlexGridSizer(2);
	{
	  pSubSizer->Add(new wxStaticText(pPage, wxID_ANY, wxT("Assign voxel seed in each direction:")), SizerFlags);
	  pSubSizer->AddSpacer(0);
	  
	  wxTextCtrl* pControl;
	  pSubSizer->Add(new wxStaticText(pPage, wxID_ANY, wxT("X Voxel Count:")), SizerFlags);
	  pSubSizer->Add(pControl = new wxTextCtrl(pPage, ID_Xvoxel, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxTextValidator(wxFILTER_NUMERIC, &xvoxel)), SizerFlags);

	  pSubSizer->Add(new wxStaticText(pPage, wxID_ANY, wxT("Y Voxel Count:")), SizerFlags);
	  pSubSizer->Add(pControl = new wxTextCtrl(pPage, ID_Yvoxel, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxTextValidator(wxFILTER_NUMERIC, &yvoxel)), SizerFlags);

	  pSubSizer->Add(new wxStaticText(pPage, wxID_ANY, wxT("Z Voxel Count:")), SizerFlags);
	  pSubSizer->Add(pControl = new wxTextCtrl(pPage, ID_Zvoxel, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxTextValidator(wxFILTER_NUMERIC, &zvoxel)), SizerFlags);
	}
	pMainSizer->Add(pSubSizer, SizerFlags);
	SizerFlags.Align(0);

	
	
	SizerFlags.Align(wxALIGN_CENTER_VERTICAL);
	pSubSizer = new wxFlexGridSizer(3);
	{
	  pSubSizer->Add(new wxStaticText(pPage, wxID_ANY, wxT("Type of analysis:")), SizerFlags);
	  wxCheckBox* SC_typeBox1;
	  pSubSizer->Add(SC_typeBox1 = new wxCheckBox(pPage, ID_SG_Type1, wxT("Elastic"), wxDefaultPosition, wxDefaultSize, 0, wxGenericValidator(&m_btype1)), SizerFlags);
	  wxCheckBox* SC_typeBox2;
	  pSubSizer->Add(SC_typeBox2 = new wxCheckBox(pPage, ID_SG_Type2, wxT("Thermoelastic"), wxDefaultPosition, wxDefaultSize, 0, wxGenericValidator(&m_btype2)), SizerFlags);

	  pSubSizer->Add(new wxStaticText(pPage, wxID_ANY, wxT("Type of models")), SizerFlags);
	  wxCheckBox* SC_H3DBox;
	  pSubSizer->Add(SC_H3DBox = new wxCheckBox(pPage, ID_SG_Solid, wxT("Solid Model"), wxDefaultPosition, wxDefaultSize, 0, wxGenericValidator(&m_bSolid)), SizerFlags);		
	  wxCheckBox* SC_H2DBox;
	  pSubSizer->Add(SC_H2DBox = new wxCheckBox(pPage, ID_SG_2D, wxT("Plate/Shell Model"), wxDefaultPosition, wxDefaultSize, 0, wxGenericValidator(&m_b2D)), SizerFlags);
	  

	  pSubSizer->Add(new wxStaticText(pPage, wxID_ANY, wxT("Type of plate theory")), SizerFlags);
	  wxCheckBox* SC_2DmodelBox1;
	  pSubSizer->Add(SC_2DmodelBox1 = new wxCheckBox(pPage, ID_SG_2Dmodel1, wxT("Kirchhoff-Love plate"), wxDefaultPosition, wxDefaultSize, 0, wxGenericValidator(&m_b2Dmodel1)), SizerFlags);
	  wxCheckBox* SC_2DmodelBox2;
	  pSubSizer->Add(SC_2DmodelBox2 = new wxCheckBox(pPage, ID_SG_2Dmodel2, wxT("Reissner-Mindlin plate"), wxDefaultPosition, wxDefaultSize, 0, wxGenericValidator(&m_b2Dmodel2)), SizerFlags);

        
	  if(!m_b2D){
	    SC_2DmodelBox1->Disable();
	    SC_2DmodelBox2->Disable();
	  }
	    
	}
	pMainSizer->Add(pSubSizer, SizerFlags);
	SizerFlags.Align(0);

	pPage->SetSizer(pMainSizer);
	pMainSizer->Fit(pPage);


	return pPage;
}

/*
void CSwiftComp2::OnWizardPageChanging(wxWizardEvent& event)
{
  if (event.GetPage() == m_pFirstPage && event.GetDirection() == true)
    {
      //RebuildWeavePatternCtrl();
      if (SCdialog->ShowModal() != wxID_OK)
	event.Veto();
    }

}


wxFileDialog* CSwiftComp2::BuildSavePage(){
  string path;
  wxFileDialog *dialog=new wxFileDialog();
  dialog->Create(this,wxT("Save Abaqus file"),wxGetCwd(),wxEmptyString,wxT("Abaqus input file (*.inp)|*.inp"),wxFD_SAVE | wxFD_OVERWRITE_PROMPT | wxFD_CHANGE_DIR);
  
  dialog->CentreOnParent();

  if (dialog->ShowModal() ==wxID_OK)
    path=ConvertString(dialog->GetPath());


  return dialog;
   
}

bool CSwiftComp2::HasNextPage(wxWizardPage *page)
{
	if (page == m_pFirstPage)
		return true;
	return wxWizard::HasNextPage(page);
}
*/


string CSwiftComp2::GetSwiftCompPara(string ExistingPara)
{
  stringstream StringStream;
  if(m_btype1&&!m_btype2){
    analysis=0;}
  else if (m_btype2&&!m_btype1){
    analysis=1;}
  else
    {wxMessageBox( wxT("Please choose ONE analysis type."), wxT("Warning"), wxICON_INFORMATION);return StringStream.str();}
  
  // m_YarnSpacing.ToDouble(&sg_area);
  if(m_bSolid&&!m_b2D){
    StringStream << analysis << " 0 1 0" << "       #analysis elemflag transflag tempflag";
    // StringStream << "0 0 1" << endl;
  }
  else if(!m_bSolid&&m_b2D){
    if(m_b2Dmodel1)
      StringStream << "0        #plate/shell model\n";
    else if (m_b2Dmodel2)
      StringStream << "1        #plate/shell model\n";
    else
      {wxMessageBox( wxT("Please choose ONE Model."), wxT("Warning"), wxICON_INFORMATION);return StringStream.str();}
    StringStream << "0 0\n";
    StringStream << analysis << " 0 1 0" << "       #analysis elemflag transflag tempflag";
  }
  else
    {wxMessageBox( wxT("Please choose ONE Model."), wxT("Warning"), wxICON_INFORMATION);return StringStream.str();}

  passf=true;
  
  return StringStream.str();
}

int CSwiftComp2::analysis_type(){
  return analysis;
}

bool CSwiftComp2::pass_flags(){
  return passf;
}

wxString CSwiftComp2::XVoxels(){
  return xvoxel;
}

wxString CSwiftComp2::YVoxels(){
  return yvoxel;
}

wxString CSwiftComp2::ZVoxels(){
  return zvoxel;
}



void CSwiftComp2::Onmodel(wxCommandEvent& event)
{
	RefreshGapTextBox();
}

void CSwiftComp2::Ontype(wxCommandEvent& event)
{
	RefreshGapTextBox();
}


void CSwiftComp2::RefreshGapTextBox()
{
  wxCheckBox* pSG2D = (wxCheckBox*)FindWindowById(ID_SG_2D, this);
  wxCheckBox* pSG2D1 = (wxCheckBox*)FindWindowById(ID_SG_2Dmodel1, this);
  wxCheckBox* pSG2D2 = (wxCheckBox*)FindWindowById(ID_SG_2Dmodel2, this);
  wxCheckBox* pSG3D = (wxCheckBox*)FindWindowById(ID_SG_Solid, this);
  wxCheckBox* pSGtype1 = (wxCheckBox*)FindWindowById(ID_SG_Type1, this);
  wxCheckBox* pSGtype2 = (wxCheckBox*)FindWindowById(ID_SG_Type2, this);
  if(pSG2D->IsChecked()){
    pSG2D1->Enable();
    pSG2D2->Enable();
  }

}
 
