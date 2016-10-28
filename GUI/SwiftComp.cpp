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
#include "SwiftComp.h"
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

int checkkeys(string line1,int *skip, int *skip2);

BEGIN_EVENT_TABLE(CSwiftComp, wxWizard)
//EVT_WIZARD_PAGE_CHANGING(wxID_ANY, CSwiftComp::OnWizardPageChanging)
EVT_CHECKBOX(ID_SG_2D, CSwiftComp::Onmodel)
EVT_CHECKBOX(ID_SG_Solid, CSwiftComp::Onmodel)
EVT_CHECKBOX(ID_SG_Type1, CSwiftComp::Ontype)
EVT_CHECKBOX(ID_SG_Type2, CSwiftComp::Ontype)
EVT_INIT_DIALOG(CSwiftComp::OnInit)

//	EVT_BUTTON(ID_WeavePattern, CWeaveWizard::OnWeavePattern)
END_EVENT_TABLE()

CSwiftComp::CSwiftComp(wxWindow* parent, wxWindowID id)
: wxWizard(parent, id, wxT("SwiftComp Wizard"), wxBitmap(Wizard_xpm))
, m_pFirstPage(NULL),\
  m_bSolid(true), m_b2D(false),\
  m_btype1(true), m_btype2(false),\
  m_b2Dmodel1(false), m_b2Dmodel2(false)
{
	BuildPages();
	GetPageAreaSizer()->Add(m_pFirstPage);
}

CSwiftComp::~CSwiftComp(void)
{
}

bool CSwiftComp::RunIt()
{
	return RunWizard(m_pFirstPage);
}

void CSwiftComp::BuildPages()
{
	m_pFirstPage = BuildFirstPage();
	//	m_pWeavePatternDialog = BuildWeavePatternDialog();

//	wxWizardPageSimple::Chain(m_pFirstPage, m_pSecondPage);
}

wxWizardPageSimple* CSwiftComp::BuildFirstPage()
{
	wxWizardPageSimple *pPage = new wxWizardPageSimple(this);

	wxBoxSizer *pMainSizer = new wxBoxSizer(wxVERTICAL);
	wxSizerFlags SizerFlags(0);
	SizerFlags.Border();
	SizerFlags.Expand();

	wxSizer *pSubSizer;

	pMainSizer->Add(new wxStaticText(pPage, wxID_ANY, wxT("This wizard will create SwiftComp input file for you.")), SizerFlags);


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




string CSwiftComp::GetSwiftCompPara(string ExistingPara)
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
  
  return StringStream.str();
}

int CSwiftComp::analysis_type(){
  return analysis;
}



void CSwiftComp::Onmodel(wxCommandEvent& event)
{
	RefreshGapTextBox();
}

void CSwiftComp::Ontype(wxCommandEvent& event)
{
	RefreshGapTextBox();
}

void CSwiftComp::readinp(string jobname,vector<string> &inpc){
  int i,j,k,jfile,skip=0,skip2=1;

  string line;
  // *nline=0;
 
  /* open the inp file */
  if (strcmp(jobname.substr(jobname.length()-4,jobname.length()).c_str(),".inp")!=0)
    jobname=jobname+".inp";
  
  ifstream ifile(jobname.c_str());
  if(!ifile){cout<<"ERROR in read: cannot open file "<<jobname<<endl;return;}

  
  /* starting to read the input file */
  
  
  while(getline(ifile,line)){
    i = -1, k = 0;
    /* get rid of blanks */
    do{
      i++;
      if(line[i]=='\0'||line[i]=='\n'||line[i]=='\r') break;
      if(line[i]==' '||line[i]=='\t') continue;
      line[k]=line[i];
      k++;
    }while(1);
    line=line.substr(0,k);
    
    /* check for blank lines and comments */
    if(k==0) continue;
    if(line[0]=='*'&&line[1]=='*') continue;
    

    /* changing to uppercase except filenames */
    locale loc;
    j=0;jfile=0;
    do{
      if(j>=6)
	if(strcmp(line.substr(j-6,6).c_str(),"INPUT=")==0) jfile=1;
      if(j>=7)
	if(strcmp(line.substr(j-7,7).c_str(),"OUTPUT=")==0) jfile=1;
      if(j>=9)
	if(strcmp(line.substr(j-9,9).c_str(),"FILENAME=")==0) jfile=1;
      if(jfile==1){
	do{
	  if(line[j]!=',') j++;
	  else{jfile=0;break;}
	}while(j<k);
      }else{
	line[j]=toupper(line[j],loc);
      }
      j++;
    }while(j<k);

    /* Skip unnecessary information */
    if(checkkeys(line,&skip,&skip2)==0) continue;
    if(skip2==1) continue;
    //cout << "skip:" << skip << endl; 

    /* adding a line */
    //(*nline)++;

    /* copying into inpc */
    inpc.push_back(line);

  }
  ifile.close();

  //for (i=0;i<inpc.size();i++)
  //cout << inpc[i] << endl;
  

}

int checkkeys(string line1,int *skip, int *skip2){
  int i;
  string keys2[]={"*NODE","*ELEMENT","*ELSET","*MATERIAL","*SOLIDSECTION","*ELASTIC","*EXPANSION"};
  for(i=*skip;i<7;i++){
    if(line1.find(keys2[i])!=string::npos && i==0) {*skip=1;*skip2=0; return 1;}
    if(line1.find(keys2[i])!=string::npos && i==1) {*skip=2;*skip2=0; return 1;}
    if(line1.find(keys2[i])!=string::npos) {*skip2=0; return 1;}
    if(line1.substr(0,1)!="*") return 1;
  }
  if(line1.substr(0,1)=="*"&&i==7) {*skip2=1;return 0;}
  return 0;
}


void CSwiftComp::RefreshGapTextBox()
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
 
