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
#include "TexGenMainFrame.h"
#include "WindowIDs.h"
#include "WeaveWizard.h"
#include "SwiftComp.h" //Prof Yu
#include "SwiftComp2.h" //Prof Yu
#include "DomainPlanesDialog.h"
#include "DropTarget.h"
#include "TexGenApp.h"
#include "CustomWidget.h"
#include "LoggerGUI.h"
#include "WeaveWizard3D.h"
#include "hyperlink.h"
#include "RangeValidator.h"
#include "TextileLayersDialog.h"
#include "PythonConverter.h"
#include "LayersOffsetDialog.h"
#include "PatternDraftDialog.h"

#include <fstream>

#include "TexGen.xpm"

//Prof Yu
#include <vector>
#include <math.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string>
//End X

#define EVT_BUTTON_MENU_RANGE(id1, id2, func) EVT_COMMAND_RANGE(id1, id2, wxEVT_COMMAND_BUTTON_CLICKED, func) EVT_COMMAND_RANGE(id1, id2, wxEVT_COMMAND_MENU_SELECTED, func)
#define EVT_CHECKBOX_MENU_RANGE(id1, id2, func) EVT_COMMAND_RANGE(id1, id2, wxEVT_COMMAND_CHECKBOX_CLICKED, func) EVT_COMMAND_RANGE(id1, id2, wxEVT_COMMAND_MENU_SELECTED, func)
#define EVT_RADIOBUTTON_MENU_RANGE(id1, id2, func) EVT_COMMAND_RANGE(id1, id2, wxEVT_COMMAND_RADIOBUTTON_SELECTED, func) EVT_COMMAND_RANGE(id1, id2, wxEVT_COMMAND_MENU_SELECTED, func)


BEGIN_EVENT_TABLE(CTexGenMainFrame, wxFrame)
	EVT_MENU(ID_Quit, CTexGenMainFrame::OnQuit)
	EVT_MENU(ID_About, CTexGenMainFrame::OnAbout)
	EVT_MENU(ID_UserGuide, CTexGenMainFrame::OnUserGuide)
	EVT_MENU(ID_Open, CTexGenMainFrame::OnOpen)
	EVT_MENU(ID_Save, CTexGenMainFrame::OnSave)
	EVT_MENU(ID_SaveScreenshot, CTexGenMainFrame::OnSaveScreenshot)
	EVT_MENU(ID_OpenWiseTex, CTexGenMainFrame::OnOpenWiseTex)
	EVT_MENU(ID_OpenTexGenv2, CTexGenMainFrame::OnOpenTexGenv2)
	EVT_MENU(ID_OpenWeavePattern, CTexGenMainFrame::OnOpenWeavePattern)
	EVT_MENU(ID_SaveGrid, CTexGenMainFrame::OnSaveGrid)
	EVT_MENU(ID_SaveVoxel, CTexGenMainFrame::OnSaveVoxel)
	EVT_MENU(ID_SaveVolumeMesh, CTexGenMainFrame::OnSaveVolumeMesh)
	EVT_MENU(ID_SaveSurfaceMesh, CTexGenMainFrame::OnSaveSurfaceMesh)
	EVT_MENU(ID_SaveIGES, CTexGenMainFrame::OnSaveIGES)
	EVT_MENU(ID_SaveSTEP, CTexGenMainFrame::OnSaveSTEP)
	EVT_MENU(ID_SaveABAQUS, CTexGenMainFrame::OnSaveABAQUS)
	EVT_MENU(ID_SaveABAQUSVoxels, CTexGenMainFrame::OnSaveABAQUSVoxels)
        EVT_MENU(ID_SaveSCfile, CTexGenMainFrame::OnSaveSCfile)
	EVT_MENU(ID_ToggleControls, CTexGenMainFrame::OnWindow)
	EVT_MENU(ID_ToggleLogWindow, CTexGenMainFrame::OnWindow)
	EVT_MENU(ID_ToggleOutliner, CTexGenMainFrame::OnWindow)
	EVT_MENU(ID_SaveTetgenMesh, CTexGenMainFrame::OnSaveTetgenMesh)

	EVT_AUINOTEBOOK_PAGE_CHANGED(ID_LogNoteBook, CTexGenMainFrame::OnLogNotebook)
	EVT_AUINOTEBOOK_PAGE_CHANGED(ID_ViewerNoteBook, CTexGenMainFrame::OnViewerNotebookPageChanged)
	EVT_AUINOTEBOOK_PAGE_CLOSE(ID_ViewerNoteBook, CTexGenMainFrame::OnViewerNotebookClose)

	EVT_CHECKBOX_MENU_RANGE(ID_RenderNodes, ID_TrimtoDomain, CTexGenMainFrame::OnRendering)
	EVT_BUTTON_MENU_RANGE(ID_ChangeBackgroundColor, ID_ChangeSurfaceColor, CTexGenMainFrame::OnRendering)
//EVT_BUTTON_MENU_RANGE(ID_SaveABAQUSVoxels, ID_SaveSCfile, CTexGenMainFrame::OnSaveSCfile)//Prof Yu
	EVT_BUTTON_MENU_RANGE(ID_CreateEmptyTextile, ID_MaxNestLayers, CTexGenMainFrame::OnTextiles)
	EVT_BUTTON_MENU_RANGE(ID_CreateYarn, ID_YarnFibreVolumeFraction, CTexGenMainFrame::OnModeller)
	EVT_RADIOBUTTON_MENU_RANGE(ID_SelectTool, ID_ScaleTool, CTexGenMainFrame::OnModeller)
	EVT_CHECKBOX_MENU_RANGE(ID_FilterNodes, ID_Relative, CTexGenMainFrame::OnModeller)
	EVT_BUTTON_MENU_RANGE(ID_CreateDomainPlanes, ID_DeleteDomain, CTexGenMainFrame::OnDomains)
	EVT_BUTTON_MENU_RANGE(ID_RunScript, ID_RecordMacro, CTexGenMainFrame::OnPython)
	//EVT_CHECKBOX_MENU_RANGE( ID_OutputMessages, ID_OutputMessages, CTexGenMainFrame::OnOptions)
	EVT_BUTTON_MENU_RANGE(ID_PatternDraft, ID_DomainVolumeFraction, CTexGenMainFrame::OnOptions)

	EVT_TEXT_ENTER(ID_PositionX, CTexGenMainFrame::OnPosition)
	EVT_TEXT_ENTER(ID_PositionY, CTexGenMainFrame::OnPosition)
	EVT_TEXT_ENTER(ID_PositionZ, CTexGenMainFrame::OnPosition)
	EVT_TEXT_ENTER(ID_SnapSize, CTexGenMainFrame::OnSnapSize)

	EVT_TEXT(ID_PositionX, CTexGenMainFrame::OnPosition)
	EVT_TEXT(ID_PositionY, CTexGenMainFrame::OnPosition)
	EVT_TEXT(ID_PositionZ, CTexGenMainFrame::OnPosition)
	EVT_TEXT(ID_SnapSize, CTexGenMainFrame::OnSnapSize)

	EVT_AUI_PANE_BUTTON(CTexGenMainFrame::OnPaneButton)

END_EVENT_TABLE()

BEGIN_EVENT_TABLE(CNoteBookHandler, wxEvtHandler)
	EVT_SET_FOCUS(CNoteBookHandler::OnSetFocus)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(CAbaqusVoxelInput, wxDialog)
	EVT_UPDATE_UI( XRCID("Offset"), CAbaqusVoxelInput::OnOffsetUpdate)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(CVolumeMeshOptions, wxDialog)
	EVT_UPDATE_UI( XRCID("PeriodicBoundaries"), CVolumeMeshOptions::OnPeriodicBoundariesUpdate)
END_EVENT_TABLE()


//Prof Yu

void getcoor(vector<string > &inpc, vector<vector<double> > &coor);//allocate coordinates
void getele(vector<string > &, vector<vector<int> > &);//allocate elements
void get_elset(vector<string > &, vector<string> &, vector<vector<int> > &);//alocate elset
void get_mats(vector<string> &, vector<vector<double> >&, vector<string> &,vector<vector<double> >&);//alocate material
void get_index(vector<string> &,int *, vector<string> &, vector<string> &, vector<vector<int> > &);//Assign elsets index
void get_ori(string, vector<string> &, int);//Read orienatation file
void get_elset2(vector<string> &, vector<int> &);
int find_nth(string , int );
void write_output(string,string,vector<vector<double> >,vector<vector<int> >,vector<vector<int> >, vector<vector<double> >, int *, vector<string>, vector<vector<double> >,int);
double volume(vector<vector<double> > &, string );//Calculate sg volume
int run_sc(string ,string );//run SwiftComp


CTexGenMainFrame::CTexGenMainFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
: wxFrame(NULL, wxID_ANY, title, pos, size)
, m_pViewerNotebook(NULL)
, m_pLogNotebook(NULL)
, m_pPythonConsole(NULL)
, m_pPythonOutput(NULL)
, m_pTexGenOutput(NULL)
, m_pControls(NULL)
, m_pOutliner(NULL)
{
	m_Manager.SetManagedWindow(this);

	wxMenu *pMenuFile = new wxMenu;

	pMenuFile->Append(ID_Open, wxT("&Open TexGen File..."));
	pMenuFile->Append(ID_Save, wxT("&Save TexGen File..."));
	pMenuFile->AppendSeparator();
	pMenuFile->Append(ID_SaveScreenshot, wxT("&Save Screenshot..."));
	pMenuFile->AppendSeparator();
	{
		wxMenu *pImportSubMenu = new wxMenu;
		pImportSubMenu->Append(ID_OpenTexGenv2, wxT("&TexGen v2 File..."));
		pImportSubMenu->Append(ID_OpenWiseTex, wxT("&WiseTex File..."));
		pImportSubMenu->Append(ID_OpenWeavePattern, wxT("&Weave Pattern File..."));
		pMenuFile->Append(wxID_ANY, wxT("&Import"), pImportSubMenu);
	}
	{
		wxMenu *pExportSubMenu = new wxMenu;
		{
			wxMenu *pInHouseSubMenu = new wxMenu;
			pInHouseSubMenu->Append(ID_SaveGrid, wxT("&Grid File..."));
			pInHouseSubMenu->Append(ID_SaveVoxel, wxT("&Voxel File..."));
			pExportSubMenu->Append(wxID_ANY, wxT("&In-House"), pInHouseSubMenu);
		}
		pExportSubMenu->Append(ID_SaveIGES, wxT("&IGES File..."));
		pExportSubMenu->Append(ID_SaveSTEP, wxT("&STEP File..."));
		pExportSubMenu->Append(ID_SaveSurfaceMesh, wxT("Sur&face Mesh..."));
		pExportSubMenu->Append(ID_SaveVolumeMesh, wxT("&Volume Mesh..."));
		pExportSubMenu->Append(ID_SaveTetgenMesh, wxT("&TetgenMesh..."));
		{
			wxMenu *pAbaqusSubMenu = new wxMenu;
			pAbaqusSubMenu->Append(ID_SaveABAQUS, wxT("&ABAQUS Dry Fibre File..."));
			pAbaqusSubMenu->Append(ID_SaveABAQUSVoxels, wxT("ABAQUS &Voxel File..."));
			pExportSubMenu->Append(wxID_ANY, wxT("&ABAQUS File"), pAbaqusSubMenu);
		}
		pExportSubMenu->Append(ID_SaveSCfile, wxT("SwiftComp File"));//Prof Yu
		pMenuFile->Append(wxID_ANY, wxT("&Export"), pExportSubMenu);
	}
	pMenuFile->AppendSeparator();
	pMenuFile->Append(ID_Quit, wxT("E&xit"));

	wxMenu *pMenuWindow = new wxMenu;
	pMenuWindow->Append(ID_ToggleControls, wxT("&Controls"), wxT(""), wxITEM_CHECK);
	pMenuWindow->Append(ID_ToggleLogWindow, wxT("&Logs"), wxT(""), wxITEM_CHECK);
	pMenuWindow->Append(ID_ToggleOutliner, wxT("&Outliner"), wxT(""), wxITEM_CHECK);


	wxMenuBar *pMenuBar = new wxMenuBar;
	pMenuBar->Append(pMenuFile, wxT("&File"));
	pMenuBar->Append(pMenuWindow, wxT("&Window"));





	// The icon is defined in the resource file
	SetIcon(wxICON(TexGen));

/*	CreateStatusBar();
	SetStatusText(wxT("Welcome to TexGen!"));
	SetStatusBarPane(-1);*/

	wxFont ConsoleFont(10, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);

	// This is our main window
	m_pViewerNotebook = new wxAuiNotebook(this, ID_ViewerNoteBook, wxDefaultPosition, wxDefaultSize, wxAUI_NB_TOP | wxAUI_NB_TAB_MOVE | wxAUI_NB_CLOSE_ON_ALL_TABS);

	m_pLogNotebook = new wxAuiNotebook(this, ID_LogNoteBook, wxDefaultPosition, wxDefaultSize, wxAUI_NB_TOP | wxAUI_NB_TAB_SPLIT | wxAUI_NB_TAB_MOVE | wxAUI_NB_SCROLL_BUTTONS);
	{
/*		wxSize ImageSize(16, 16);

		wxImageList *pImageList = new wxImageList(ImageSize.GetWidth(), ImageSize.GetHeight());

		// should be able to add them as icons in which case they will automatically be transparent
		// doesn't seem to work however!! Does work in the wxnotebook and arttest sample, don't know why...
		pImageList->Add(wxArtProvider::GetIcon(wxART_INFORMATION, wxART_OTHER, ImageSize));
		pImageList->Add(wxArtProvider::GetIcon(wxART_WARNING, wxART_OTHER, ImageSize));
		pImageList->Add(wxArtProvider::GetIcon(wxART_ERROR, wxART_OTHER, ImageSize));

		m_pLogNotebook->AssignImageList(pImageList);*/

		m_pPythonConsole = new CPythonConsole(m_PythonWrapper, m_pLogNotebook, wxID_ANY, wxT(">>> "),
			wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_DONTWRAP);
		m_pPythonConsole->SetFont(ConsoleFont);

		m_pPythonOutput = new wxTextCtrl(m_pLogNotebook, wxID_ANY, wxEmptyString,
			wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_READONLY | wxTE_DONTWRAP | wxTE_RICH);
		m_pPythonOutput->SetFont(ConsoleFont);

		m_pTexGenOutput = new wxTextCtrl(m_pLogNotebook, wxID_ANY, wxEmptyString,
			wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_READONLY | wxTE_DONTWRAP | wxTE_RICH);
		m_pTexGenOutput->SetFont(ConsoleFont);

		m_pLogNotebook->AddPage(m_pPythonConsole, wxT("Python Console"), true, wxArtProvider::GetBitmap(wxART_INFORMATION, wxART_OTHER, wxSize(16, 16)));
		m_pLogNotebook->AddPage(m_pPythonOutput, wxT("Python Output"), false, wxArtProvider::GetBitmap(wxART_INFORMATION, wxART_OTHER, wxSize(16, 16)));
		m_pLogNotebook->AddPage(m_pTexGenOutput, wxT("TexGen Output"), false, wxArtProvider::GetBitmap(wxART_INFORMATION, wxART_OTHER, wxSize(16, 16)));
	}

	// This is the control window where all the GUI controls go (buttons, etc...)
/*	wxSashLayoutWindow *pControlSashWindow = new wxSashLayoutWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxCLIP_CHILDREN);
	pControlSashWindow->SetOrientation(wxLAYOUT_VERTICAL);
	pControlSashWindow->SetAlignment(wxLAYOUT_LEFT);*/

	m_pControls = new CControlsWindow(pMenuBar, this, ID_ControlsWindow);
//	pControlSashWindow->SetDefaultSize(m_pControls->GetSize());

/*	wxLayoutAlgorithm layout;
	layout.LayoutFrame(this, m_pViewerNotebook);

	SetSizeHints(200, 200);*/

	m_pViewerNotebook->PushEventHandler(new CNoteBookHandler(*this));
	m_Manager.AddPane(m_pViewerNotebook, wxAuiPaneInfo().Name(wxT("Viewer")).CenterPane().PaneBorder(false));
	m_Manager.AddPane(m_pLogNotebook, wxAuiPaneInfo().Name(wxT("Logs")).Bottom().BestSize(wxSize(300, 300)).Caption(wxT("Log windows")));
	m_Manager.AddPane(m_pControls, wxAuiPaneInfo().Name(wxT("Controls")).Left().TopDockable(false).BottomDockable(false).Caption(wxT("Controls")));

	m_pOutliner = new wxPanel();
	if (wxXmlResource::Get()->LoadPanel(m_pOutliner, this, wxT("Outliner")))
	{
		m_pOutliner->PushEventHandler(new COutlineHandler(*this));
//		m_pOutliner->SetSize(200, 350);
//		UpdateOutlinerItems();
	}

	m_Manager.AddPane(m_pOutliner, wxAuiPaneInfo().Name(wxT("Outliner")).Right().Hide().TopDockable(false).BottomDockable(false).Caption(wxT("Outliner")));

	m_Manager.Update();

	bool bSuccess = m_Manager.LoadPerspective(wxConfigBase::Get()->Read(wxT("Perspective"), wxEmptyString));

	wxMenu *pMenuHelp = new wxMenu;
	pMenuHelp->Append(ID_About, wxT("&About..."));
	pMenuHelp->Append(ID_UserGuide, wxT("&User Guide"));
	pMenuBar->Append(pMenuHelp, wxT("&Help"));
	SetMenuBar(pMenuBar);

	UpdateWindowChecks();

	SetDropTarget(new CFileDropTarget());
}

CTexGenMainFrame::~CTexGenMainFrame(void)
{
	while (m_pViewerNotebook->GetPageCount() != 0)
	{
		m_pViewerNotebook->RemovePage(0);
	}
	m_pViewerNotebook->PopEventHandler(true);
	map<string, CTexGenRenderWindow*>::iterator itRenderWindow;
	for (itRenderWindow = m_RenderWindows.begin(); itRenderWindow != m_RenderWindows.end(); ++itRenderWindow)
	{
		itRenderWindow->second->CleanUp();
		itRenderWindow->second->Delete();
	}
	m_RenderWindows.clear();

	wxConfigBase::Get()->Write(wxT("Perspective"), m_Manager.SavePerspective());

	m_pOutliner->PopEventHandler( true );

	m_Manager.UnInit();
}

void CTexGenMainFrame::OnInit()
{
	SendPythonCode("from _Embedded import *");
	SendPythonCode("from TexGen.Core import *");
	SendPythonCode("from TexGen.Renderer import *");
	SendPythonCode("from TexGen.Export import *");
	SendPythonCode("from TexGen.Abaqus import *");
	SendPythonCode("from TexGen.WeavePattern import *");
	SendPythonCode("from TexGen.WiseTex import *");
	SendPythonCode("from TexGen.FlowTex import *");
	SendPythonCode("import math");
	m_pPythonConsole->SetFocus();
}

void CTexGenMainFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
	Close(TRUE);
}

void CTexGenMainFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
	string Title("About TexGen v");
	Title += CTexGen::GetInstance().GetVersion();
	Title += ". Compiled on ";
	Title += __DATE__;
	Title += ".";

	string Message("TexGen is textile geometric modeller, Copyright (C) Martin Sherburn & Louise Brown\n");
	Message += ("This is free software licensed under the GNU General Public License.\n");
	Message += ("TexGen comes with ABSOLUTELY NO WARRANTY.");

	wxMessageBox(ConvertString(Message), ConvertString(Title), wxOK | wxICON_INFORMATION, this);
}

void CTexGenMainFrame::OnUserGuide(wxCommandEvent& WXUNUSED(event))
{
	wxDialog* pDialog = new wxDialog( NULL, wxID_ANY, wxT("User Guide"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE);

	wxBoxSizer *BoxSizer = new wxBoxSizer (wxVERTICAL);
	wxSizerFlags SizerFlags(0);
	SizerFlags.Border(wxALL,10);
	SizerFlags.Expand();

	BoxSizer->Add(new wxStaticText( pDialog, wxID_ANY, wxT("The TexGen User Guide can be found here:") ), SizerFlags );
	BoxSizer->Add( new wxHyperLink( pDialog, -1, wxT("http://texgen.sourceforge.net/index.php/User_Guide")), SizerFlags );
	pDialog->SetSizer( BoxSizer );
	BoxSizer->Fit( pDialog );

	pDialog->ShowModal();
	pDialog->Destroy();
}

void CTexGenMainFrame::ReceiveOutput(string Text, OUTPUT_TYPE OutputType, bool bError, bool bInteractive)
{
	wxTextAttr PrevStyle;
	int iPage;
	wxTextCtrl *pOutputWindow;
	if (OutputType == OUTPUT_PYTHON)
	{
		if (bInteractive)
		{
			iPage = 0;
			pOutputWindow = m_pPythonConsole;
		}
		else
		{
			iPage = 1;
			pOutputWindow = m_pPythonOutput;
		}
	}
	else if (OutputType == OUTPUT_TEXGEN)
	{
		iPage = 2;
		pOutputWindow = m_pTexGenOutput;
	}
	else
		return;

	PrevStyle = pOutputWindow->GetDefaultStyle();
	if (bError)
		pOutputWindow->SetDefaultStyle(wxTextAttr(*wxRED));
	pOutputWindow->AppendText(ConvertString(Text));
	pOutputWindow->SetDefaultStyle(PrevStyle);
	if (m_pLogNotebook->GetSelection() != iPage)
	{
		if (bError)
			m_pLogNotebook->SetPageBitmap(iPage, wxArtProvider::GetBitmap(wxART_ERROR, wxART_OTHER, wxSize(16, 16)));
		else/* if (m_pLogNotebook->GetPageBitmap(iPage) != 2)*/
			m_pLogNotebook->SetPageBitmap(iPage, wxArtProvider::GetBitmap(wxART_WARNING, wxART_OTHER, wxSize(16, 16)));
	}
	// Will update the frame, this is usefull for operations which take a long time
	// to complete in python. Each time a message is sent from python the window
	// will be updated so that it avoids the user thinking the program has crashed.
//	Update();		// Doesn't seem to work...
	wxSafeYield();
}

void CTexGenMainFrame::OnOpen(wxCommandEvent& event)
{
	wxFileDialog dialog
	(
		this,
		wxT("Open TexGen file"),
		wxGetCwd(),
		wxEmptyString,
		wxT("TexGen XML file (*.tg3)|*.tg3"),
		wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_CHANGE_DIR
	);
	dialog.CentreOnParent();
	if (dialog.ShowModal() == wxID_OK)
	{
		string Command;
		Command = "ReadFromXML(r\"";
		Command += ConvertString(dialog.GetPath());
		Command += "\")";
		SendPythonCode(Command);
	}
}

void CTexGenMainFrame::OnSave(wxCommandEvent& event)
{
	wxArrayString Options;
	Options.Add(wxT("Minimal (Textile data only)"));
	Options.Add(wxT("Standard (Textile and yarn data)"));
	Options.Add(wxT("Full (Textile, yarn and mesh data)"));
	wxSingleChoiceDialog SaveTypeDialog(this, wxT("Please select how much data to save."), wxT("Save dialog"), Options);
	SaveTypeDialog.SetSelection(1);
	if (SaveTypeDialog.ShowModal() == wxID_OK)
	{
		wxFileDialog dialog
		(
			this,
			wxT("Save TexGen file"),
			wxGetCwd(),
			wxEmptyString,
			wxT("TexGen XML file (*.tg3)|*.tg3"),
			wxFD_SAVE | wxFD_OVERWRITE_PROMPT | wxFD_CHANGE_DIR
		);
		dialog.CentreOnParent();
		if (dialog.ShowModal() == wxID_OK)
		{
			string Command;
			Command = "SaveToXML(r\"";
			Command += ConvertString(dialog.GetPath());
			Command += "\", \"" + GetTextileSelection();
			switch (SaveTypeDialog.GetSelection())
			{
			case 0:
				Command += "\", OUTPUT_MINIMAL)";
				break;
			case 1:
				Command += "\", OUTPUT_STANDARD)";
				break;
			case 2:
				Command += "\", OUTPUT_FULL)";
				break;
			}
			SendPythonCode(Command);
		}
	}
}

void CTexGenMainFrame::OnSaveScreenshot(wxCommandEvent& event)
{
	wxDialog ScreenShot;
	
	if (wxXmlResource::Get()->LoadDialog(&ScreenShot, this, wxT("ScreenShot")))
	{
		if ( ScreenShot.ShowModal() == wxID_OK )
		{
			wxFileDialog dialog
			(
				this,
				wxT("Save Screenshot"),
				wxGetCwd(),
				wxEmptyString,
				wxT("PNG image (*.png)|*.png"),
				wxFD_SAVE | wxFD_OVERWRITE_PROMPT | wxFD_CHANGE_DIR
			);
			dialog.CentreOnParent();
			if (dialog.ShowModal() == wxID_OK)
			{
				string Command;
				Command = "GetRenderWindow().TakeScreenShot(r\"";
				Command += ConvertString(dialog.GetPath());
				Command += "\", ";
				
				wxSpinCtrl* MagCtrl = (wxSpinCtrl*)FindWindow(XRCID("Magnification"));
				
				Command += stringify(MagCtrl->GetValue());
				Command += ")";
				SendPythonCode(Command);
			}
		}
	}
}

void CTexGenMainFrame::OnOpenWiseTex(wxCommandEvent& event)
{
	wxFileDialog dialog
	(
		this,
		wxT("Open WiseTex file"),
		wxGetCwd(),
		wxEmptyString,
		wxT("WiseTex FlexComp file (*.cfl)|*.cfl"),
		wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_CHANGE_DIR
	);
	dialog.CentreOnParent();
	if (dialog.ShowModal() == wxID_OK)
	{
		string Command;
		Command = "from TexGen.WiseTex import *\n";
		Command += "ImportWiseTex(r\"";
		Command += ConvertString(dialog.GetPath());
		Command += "\")";
		SendPythonCode(Command);
	}
}

void CTexGenMainFrame::OnOpenTexGenv2(wxCommandEvent& event)
{
	wxFileDialog dialog
	(
		this,
		wxT("Open TexGen v2 file"),
		wxGetCwd(),
		wxEmptyString,
		wxT("TexGen v2 path file (*.pth)|*.pth"),
		wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_CHANGE_DIR
	);
	dialog.CentreOnParent();
	if (dialog.ShowModal() == wxID_OK)
	{
		string Command;
		Command = "from TexGen.TexGenv2 import *\n";
		Command += "ImportTexGenv2(r\"";
		Command += ConvertString(dialog.GetPath());
		Command += "\")";
		SendPythonCode(Command);
	}
}

void CTexGenMainFrame::OnOpenWeavePattern(wxCommandEvent& event)
{
	wxFileDialog dialog
	(
		this,
		wxT("Open Weave Pattern file"),
		wxGetCwd(),
		wxEmptyString,
		wxT("Weave Pattern file (*.txt)|*.txt"),
		wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_CHANGE_DIR
	);
	dialog.CentreOnParent();
	if (dialog.ShowModal() == wxID_OK)
	{
		string Command;
		Command = "from TexGen.WeavePattern import *\n";
		Command += "ImportWeavePattern(r\"";
		Command += ConvertString(dialog.GetPath());
		Command += "\")";
		SendPythonCode(Command);
	}
}

void CTexGenMainFrame::OnSaveGrid(wxCommandEvent& event)
{
	wxFileDialog dialog
	(
		this,
		wxT("Save Grid file"),
		wxGetCwd(),
		wxEmptyString,
		wxT("Grid file (*.grd)|*.grd"),
		wxFD_SAVE | wxFD_OVERWRITE_PROMPT | wxFD_CHANGE_DIR
	);
	dialog.CentreOnParent();
	string TextileName = GetTextileSelection();


	wxString X, Y;
	wxDialog GridResolution;
	if (wxXmlResource::Get()->LoadDialog(&GridResolution, this, wxT("GridResolution")))
	{
		XRCCTRL(GridResolution, "X", wxTextCtrl)->SetValidator(wxTextValidator(wxFILTER_NUMERIC, &X));
		XRCCTRL(GridResolution, "Y", wxTextCtrl)->SetValidator(wxTextValidator(wxFILTER_NUMERIC, &Y));
		XRCCTRL(GridResolution, "Z", wxTextCtrl)->Disable();

		if (GridResolution.ShowModal() == wxID_OK)
		{
			long iX, iY;
			X.ToLong(&iX);
			Y.ToLong(&iY);

			if (dialog.ShowModal() == wxID_OK)
			{
				string Command;
				Command = "from TexGen.GridFile import *\n";
				Command += "ExportGridFile(r\"";
				Command += ConvertString(dialog.GetPath());
				Command += "\", \"";
				Command += TextileName;
				Command += "\", (";
				Command += stringify(iX) + ", ";
				Command += stringify(iY);
				Command += "))";
				SendPythonCode(Command);
			}
		}
	}
}

void CTexGenMainFrame::OnSaveVoxel(wxCommandEvent& event)
{
	wxFileDialog dialog
	(
		this,
		wxT("Save Voxel file"),
		wxGetCwd(),
		wxEmptyString,
		wxT("Voxel file (*.vox)|*.vox"),
		wxFD_SAVE | wxFD_OVERWRITE_PROMPT | wxFD_CHANGE_DIR
	);
	dialog.CentreOnParent();
	string TextileName = GetTextileSelection();

	wxString X, Y, Z;
	wxDialog GridResolution;
	if (wxXmlResource::Get()->LoadDialog(&GridResolution, this, wxT("GridResolution")))
	{
		XRCCTRL(GridResolution, "X", wxTextCtrl)->SetValidator(wxTextValidator(wxFILTER_NUMERIC, &X));
		XRCCTRL(GridResolution, "Y", wxTextCtrl)->SetValidator(wxTextValidator(wxFILTER_NUMERIC, &Y));
		XRCCTRL(GridResolution, "Z", wxTextCtrl)->SetValidator(wxTextValidator(wxFILTER_NUMERIC, &Z));

		if (GridResolution.ShowModal() == wxID_OK)
		{
			long iX, iY, iZ;
			X.ToLong(&iX);
			Y.ToLong(&iY);
			Z.ToLong(&iZ);

			if (dialog.ShowModal() == wxID_OK)
			{
				string Command;
				Command = "from TexGen.FlowTex import *\n";
				Command += "ExportFlowTex(r\"";
				Command += ConvertString(dialog.GetPath());
				Command += "\", \"";
				Command += TextileName;
				Command += "\", (";
				Command += stringify(iX) + ", ";
				Command += stringify(iY) + ", ";
				Command += stringify(iZ);
				Command += "))";
				SendPythonCode(Command);
			}
		}
	}
}

void CTexGenMainFrame::OnSaveVolumeMesh(wxCommandEvent& event)
{
	string TextileName = GetTextileSelection();
	CTextile* pTextile = TEXGEN.GetTextile(TextileName);

	double dSeedSize = 1;

	if ( pTextile )
	{
		const CDomain* pDomain = pTextile->GetDomain();
		if ( pDomain )
		{
			pair<XYZ,XYZ> DomainSize = pDomain->GetMesh().GetAABB();
			dSeedSize = GetLength(DomainSize.first, DomainSize.second)/20.0;
		}
	}

//	int iMeshType = 0;
	int iElementOrder = 0;
	bool bProjectMidSideNodes = true;
	bool bPeriodic = false;
	int iBoundaryConditions;

	wxString SeedSize = wxString::Format(wxT("%.4f"), dSeedSize);
	
	wxString MergeTolerance = wxT("0.001");
	CVolumeMeshOptions MeshOptions(this);
	
	{
//		XRCCTRL(MeshOptions, "MeshType", wxRadioBox)->SetValidator(wxGenericValidator(&iMeshType));
		XRCCTRL(MeshOptions, "ElementOrder", wxRadioBox)->SetValidator(wxGenericValidator(&iElementOrder));
		XRCCTRL(MeshOptions, "ProjectMidSideNodes", wxCheckBox)->SetValidator(wxGenericValidator(&bProjectMidSideNodes));
		XRCCTRL(MeshOptions, "Periodic", wxCheckBox)->SetValidator(wxGenericValidator(&bPeriodic));
		XRCCTRL(MeshOptions, "SeedSize", wxTextCtrl)->SetValidator(wxTextValidator(wxFILTER_NUMERIC, &SeedSize));
		XRCCTRL(MeshOptions, "MergeTolerance", wxTextCtrl)->SetValidator(wxTextValidator(wxFILTER_NUMERIC, &MergeTolerance));
		XRCCTRL(MeshOptions, "PeriodicBoundaries", wxRadioBox)->SetValidator(wxGenericValidator(&iBoundaryConditions));

		wxRadioBox* PeriodicBoundariesCtrl = (wxRadioBox*)FindWindow(XRCID("PeriodicBoundaries"));
		PeriodicBoundariesCtrl->Enable((unsigned int)STAGGERED_BC, false);

		if (MeshOptions.ShowModal() == wxID_OK)
		{
			wxFileDialog dialog
			(
				this,
				wxT("Save Mesh file"),
				wxGetCwd(),
				wxEmptyString,
				wxT("ABAQUS input file (*.inp)|*.inp|")
				wxT("VTK unstructured grid file (*.vtu)|*.vtu"),
				wxFD_SAVE | wxFD_OVERWRITE_PROMPT | wxFD_CHANGE_DIR
			);
			dialog.CentreOnParent();
			if (dialog.ShowModal() == wxID_OK)
			{
				stringstream Command;
				if ( bPeriodic )
					Command << "mesher = CMesher(" << iBoundaryConditions << ")" << endl;
				else
					Command << "mesher = CMesher()" << endl;
/*				if (iMeshType == 1)
					Command << "mesher.SetHybrid(True)" << endl;*/
				if (iElementOrder == 1)
					Command << "mesher.SetQuadratic(True)" << endl;
				if (!bProjectMidSideNodes)
					Command << "mesher.SetProjectMidSideNodes(False)" << endl;
				if (bPeriodic)
					Command << "mesher.SetPeriodic(True)" << endl;
				if (!SeedSize.IsEmpty())
					Command << "mesher.SetSeed(" << ConvertString(SeedSize) << ")" << endl;
				if (!MergeTolerance.IsEmpty())
					Command << "mesher.SetMergeTolerance(" << ConvertString(MergeTolerance) << ")" << endl;
				Command << "mesher.CreateMesh(r'" << TextileName << "')" << endl;
				switch (dialog.GetFilterIndex())
				{
				case 0:
					Command << "mesher.SaveVolumeMeshToABAQUS(r'" << ConvertString(dialog.GetPath()) << "', '" << TextileName << "')" << endl;
					break;
				case 1:
					Command << "mesher.SaveVolumeMeshToVTK(r'" << ConvertString(dialog.GetPath()) << "')" << endl;
					break;
				}
				SendPythonCode(Command.str());
			}
		}
	}
}

void CTexGenMainFrame::OnSaveSurfaceMesh(wxCommandEvent& event)
{
	string TextileName = GetTextileSelection();

	bool bExportDomain = false, bTrimSurface = true;
	wxDialog MeshOptions;
	if (wxXmlResource::Get()->LoadDialog(&MeshOptions, this, wxT("SurfaceMeshOptions")))
	{
		XRCCTRL(MeshOptions, "TrimSurface", wxCheckBox)->SetValidator(wxGenericValidator(&bTrimSurface));
		XRCCTRL(MeshOptions, "ExportDomain", wxCheckBox)->SetValidator(wxGenericValidator(&bExportDomain));

		if (MeshOptions.ShowModal() == wxID_OK)
		{
			wxFileDialog dialog
			(
				this,
				wxT("Save Mesh file"),
				wxGetCwd(),
				wxEmptyString,
				wxT("VTK unstructured grid file (*.vtu)|*.vtu|")
				wxT("ASCII STL file (*.stl)|*.stl|")
				wxT("Binary STL file (*.stl;*stlb)|*.stl;*stlb|")
				wxT("SCIRun file (*.pts)|*.pts|"),
				wxFD_SAVE | wxFD_OVERWRITE_PROMPT | wxFD_CHANGE_DIR
			);
			dialog.CentreOnParent();
			if (dialog.ShowModal() == wxID_OK)
			{
				stringstream Command;
				Command << "mesh = CMesh()" << endl;
				Command << "textile = GetTextile(r'" << TextileName << "')" << endl;
				if (bTrimSurface)
					Command << "textile.AddSurfaceToMesh(mesh, True)" << endl;
				else
					Command << "textile.AddSurfaceToMesh(mesh, False)" << endl;
				if (bExportDomain)
					Command << "mesh.InsertMesh(textile.GetDomain().GetMesh())" << endl;
				switch (dialog.GetFilterIndex())
				{
				case 0:
					Command << "mesh.SaveToVTK(r'" << ConvertString(dialog.GetPath()) << "')" << endl;
					break;
				case 1:
					Command << "mesh.SaveToSTL(r'" << ConvertString(dialog.GetPath()) << "', False)" << endl;
					break;
				case 2:
					Command << "mesh.SaveToSTL(r'" << ConvertString(dialog.GetPath()) << "', True)" << endl;
					break;
				case 3:
					Command << "mesh.SaveToSCIRun(r'" << ConvertString(dialog.GetPath()) << "')" << endl;
					break;
				}
				SendPythonCode(Command.str());
			}
		}
	}
}

void CTexGenMainFrame::OnSaveIGES(wxCommandEvent& event)
{
	string TextileName = GetTextileSelection();

	int iYarnSurface = 1;
	bool bExportDomain = true, bSubtractYarns = false, bJoinYarns = true;
	wxDialog ExportOptions;
	if (wxXmlResource::Get()->LoadDialog(&ExportOptions, this, wxT("ExportOptions")))
	{
		XRCCTRL(ExportOptions, "YarnSurface", wxRadioBox)->SetValidator(wxGenericValidator(&iYarnSurface));
		XRCCTRL(ExportOptions, "YarnSurface", wxRadioBox)->SetLabel(wxT("Yarn Surface"));
		XRCCTRL(ExportOptions, "ExportDomain", wxCheckBox)->SetValidator(wxGenericValidator(&bExportDomain));
		XRCCTRL(ExportOptions, "SubtractYarns", wxCheckBox)->SetValidator(wxGenericValidator(&bSubtractYarns));
		XRCCTRL(ExportOptions, "JoinYarns", wxCheckBox)->SetValidator(wxGenericValidator(&bJoinYarns));

		if (ExportOptions.ShowModal() == wxID_OK)
		{
			wxFileDialog dialog
			(
				this,
				wxT("Save IGES file"),
				wxGetCwd(),
				wxEmptyString,
				wxT("IGES file (*.igs;*.iges)|*.igs;*.iges"),
				wxFD_SAVE | wxFD_OVERWRITE_PROMPT | wxFD_CHANGE_DIR
			);
			dialog.CentreOnParent();

			if (dialog.ShowModal() == wxID_OK)
			{
				string Command;
				Command = "Exporter = CExporter()\n";
				Command += "Exporter.SetFaceted(" + stringify(iYarnSurface == 1) + ")\n";
				Command += "Exporter.SetExportDomain(" + stringify(bExportDomain) + ")\n";
				Command += "Exporter.SetSubtractYarns(" + stringify(bSubtractYarns) + ")\n";
				Command += "Exporter.SetJoinYarns(" + stringify(bJoinYarns) + ")\n";
				Command += "Exporter.OutputTextileToIGES(r\"";
				Command += ConvertString(dialog.GetPath());
				Command += "\", \"";
				Command += TextileName;
				Command += "\")";
				SendPythonCode(Command);
			}
		}
	}
}

void CTexGenMainFrame::OnSaveSTEP(wxCommandEvent& event)
{
	wxFileDialog dialog
	(
		this,
		wxT("Save STEP file"),
		wxGetCwd(),
		wxEmptyString,
		wxT("STEP file (*.stp;*.step)|*.stp;*.step"),
		wxFD_SAVE | wxFD_OVERWRITE_PROMPT | wxFD_CHANGE_DIR
	);
	dialog.CentreOnParent();
	string TextileName = GetTextileSelection();

	int iYarnSurface = 1;
	bool bExportDomain = true, bSubtractYarns = false, bJoinYarns = true;
	wxDialog ExportOptions;
	if (wxXmlResource::Get()->LoadDialog(&ExportOptions, this, wxT("ExportOptions")))
	{
		XRCCTRL(ExportOptions, "YarnSurface", wxRadioBox)->SetValidator(wxGenericValidator(&iYarnSurface));
		XRCCTRL(ExportOptions, "YarnSurface", wxRadioBox)->SetLabel(wxT("Yarn Surface"));
		XRCCTRL(ExportOptions, "ExportDomain", wxCheckBox)->SetValidator(wxGenericValidator(&bExportDomain));
		XRCCTRL(ExportOptions, "SubtractYarns", wxCheckBox)->SetValidator(wxGenericValidator(&bSubtractYarns));
		XRCCTRL(ExportOptions, "JoinYarns", wxCheckBox)->SetValidator(wxGenericValidator(&bJoinYarns));

		if (ExportOptions.ShowModal() == wxID_OK)
		{
			if (dialog.ShowModal() == wxID_OK)
			{
				string Command;
				Command = "Exporter = CExporter()\n";
				Command += "Exporter.SetFaceted(" + stringify(iYarnSurface == 1) + ")\n";
				Command += "Exporter.SetExportDomain(" + stringify(bExportDomain) + ")\n";
				Command += "Exporter.SetSubtractYarns(" + stringify(bSubtractYarns) + ")\n";
				Command += "Exporter.SetJoinYarns(" + stringify(bJoinYarns) + ")\n";
				Command += "Exporter.OutputTextileToSTEP(r\"";
				Command += ConvertString(dialog.GetPath());
				Command += "\", \"";
				Command += TextileName;
				Command += "\")";
				SendPythonCode(Command);
			}
		}
	}
}



void CTexGenMainFrame::OnSaveABAQUS(wxCommandEvent& event)
{

	string TextileName = GetTextileSelection();
	stringstream Command;

	wxString XScale = wxT("1.0");
	wxString YScale = wxT("1.0");
	wxString ZScale = wxT("1.0");
	//wxString YoungsModulus = wxT("13000.0");
	//wxString PoissonsRatio = wxT("0.3");
	wxString IntersectionTolerance = wxT("0.0000001");
	bool bAdjustIntersections = false;
	bool bIncludePlates = false;
	bool bRegenerateMesh = false;
	int iElementType = 0;
	
	wxFileDialog dialog
	(
		this,
		wxT("Save Abaqus file"),
		wxGetCwd(),
		wxEmptyString,
		wxT("Abaqus input file (*.inp)|*.inp"),
		wxFD_SAVE | wxFD_OVERWRITE_PROMPT | wxFD_CHANGE_DIR
	);
	dialog.CentreOnParent();

	

	wxDialog AbaqusInput;
	if (wxXmlResource::Get()->LoadDialog(&AbaqusInput, this, wxT("AbaqusOptions")))
	{
		XRCCTRL(AbaqusInput, "XScale", wxTextCtrl)->SetValidator(wxTextValidator(wxFILTER_NUMERIC, &XScale));
		XRCCTRL(AbaqusInput, "YScale", wxTextCtrl)->SetValidator(wxTextValidator(wxFILTER_NUMERIC, &YScale));
		XRCCTRL(AbaqusInput, "ZScale", wxTextCtrl)->SetValidator(wxTextValidator(wxFILTER_NUMERIC, &ZScale));
		/*XRCCTRL(AbaqusInput, "YoungsModulus", wxTextCtrl)->SetValidator(wxTextValidator(wxFILTER_NUMERIC, &YoungsModulus));
		XRCCTRL(AbaqusInput, "PoissonsRatio", wxTextCtrl)->SetValidator(wxTextValidator(wxFILTER_NUMERIC, &PoissonsRatio));*/
		XRCCTRL(AbaqusInput, "AdjustIntersections", wxCheckBox)->SetValidator(wxGenericValidator(&bAdjustIntersections));
		XRCCTRL(AbaqusInput, "IntersectionTolerance", wxTextCtrl)->SetValidator(wxTextValidator(wxFILTER_NUMERIC, &IntersectionTolerance));
		XRCCTRL(AbaqusInput, "IncludePlates", wxCheckBox)->SetValidator(wxGenericValidator(&bIncludePlates));
		XRCCTRL(AbaqusInput, "RegenerateMesh", wxCheckBox)->SetValidator(wxGenericValidator(&bRegenerateMesh));
		XRCCTRL(AbaqusInput, "ElementType", wxRadioBox)->SetValidator(wxGenericValidator(&iElementType));

		if (AbaqusInput.ShowModal() == wxID_OK)
		{
			if (dialog.ShowModal() == wxID_OK)
			{
				Command << "tension = CLinearTransformation()" << endl;
				Command << "tension.AddScale(" << ConvertString(XScale) << "," << ConvertString(YScale) << "," << ConvertString(ZScale) << ")" << endl;

				Command << "deformer = TextileDeformerAbaqus()" << endl;
				Command << "deformer.SetIncludePlates(" << bIncludePlates << ")" << endl;
				Command << "deformer.AddDeformationStep(tension)" << endl;

				//#deformer.SetFortranProgram('LinearElasticUMAT.for')
				//Command << "deformer.SetMaterial('ISO', [" << ConvertString(YoungsModulus) << "," << ConvertString(PoissonsRatio) << "])" << endl;
				//#deformer.SetSimulationFilesPrefix(Abaquspre)

				//name = baseName + '.tg3'
				//ReadFromXML(name)
				Command << "textile = GetTextile('" + TextileName + "')" << endl;

				//#name = deformer.GetSimulationFilesPrefix()
				//Command << "deformer.CreateAbaqusInputFile(textile, 'TestAbaqus' + '.inp')" << endl;
				double Tolerance;
				IntersectionTolerance.ToDouble( &Tolerance );
				Command << "deformer.CreateAbaqusInputFile(textile, r\'" << ConvertString(dialog.GetPath()) << "'," << bRegenerateMesh << "," << iElementType << "," 
															<< bAdjustIntersections << "," << Tolerance <<  ")" << endl;

				SendPythonCode(Command.str());
			}
		}
	}

	

/*if not deformer.CreateAbaqusInputFile(textile, baseName + '.inp'):
	raise RuntimeError('Unable to create ABAQUS input file')
else:
    print 'Abaqus input files are complete now.'*/


}

void CTexGenMainFrame::OnSaveABAQUSVoxels(wxCommandEvent& event)
{

  string TextileName = GetTextileSelection();
  stringstream Command;

  wxString XVoxels = wxT("15");
  wxString YVoxels = wxT("15");
  wxString ZVoxels = wxT("15");
  wxString XOffset = wxT("0.0");
  
  bool bOutputMatrix = true;
  bool bOutputYarns = true;
  int  iBoundaryConditions = 0;
  int iElementType = 0;
  
  wxFileDialog dialog
    (
     this,
     wxT("Save Abaqus file"),
     wxGetCwd(),
     wxEmptyString,
     wxT("Abaqus input file (*.inp)|*.inp"),
     wxFD_SAVE | wxFD_OVERWRITE_PROMPT | wxFD_CHANGE_DIR
     );
  dialog.CentreOnParent();
  
  CAbaqusVoxelInput AbaqusVoxelInput(this);
  
  {
    XRCCTRL(AbaqusVoxelInput, "XVoxelCount", wxTextCtrl)->SetValidator(wxTextValidator(wxFILTER_NUMERIC, &XVoxels));
    XRCCTRL(AbaqusVoxelInput, "YVoxelCount", wxTextCtrl)->SetValidator(wxTextValidator(wxFILTER_NUMERIC, &YVoxels));
    XRCCTRL(AbaqusVoxelInput, "ZVoxelCount", wxTextCtrl)->SetValidator(wxTextValidator(wxFILTER_NUMERIC, &ZVoxels));
    XRCCTRL(AbaqusVoxelInput, "OutputMatrix", wxCheckBox)->SetValidator(wxGenericValidator(&bOutputMatrix));
    XRCCTRL(AbaqusVoxelInput, "OutputYarns", wxCheckBox)->SetValidator(wxGenericValidator(&bOutputYarns));
    XRCCTRL(AbaqusVoxelInput, "PeriodicBoundaries", wxRadioBox)->SetValidator(wxGenericValidator(&iBoundaryConditions));
    XRCCTRL(AbaqusVoxelInput, "ElementType", wxRadioBox)->SetValidator(wxGenericValidator(&iElementType));
    XRCCTRL(AbaqusVoxelInput, "Offset", wxTextCtrl)->SetValidator(RangeValidator(&XOffset, 0.0, 1.0));
    
    if (AbaqusVoxelInput.ShowModal() == wxID_OK)
      {
	if (dialog.ShowModal() == wxID_OK)
	  {
	    scstring = ConvertString(dialog.GetPath());//Prof Yu
	    
	    if (scstring.find(".sc")!=string::npos) remove(scstring.c_str());
	    if ( !bOutputMatrix && !bOutputYarns )
	      {
		TGERROR("No output selected");
		return;
	      }
	    
	    if ( iBoundaryConditions == SHEARED_BC )
	      {
		Command << "Vox = CShearedVoxelMesh('CShearedPeriodicBoundaries')" << endl;
	      }
	    else if ( iBoundaryConditions == STAGGERED_BC )
	      {
		Command << "Vox = CStaggeredVoxelMesh('CStaggeredPeriodicBoundaries')" << endl;
		Command << "Vox.SetOffset(" << ConvertString(XOffset) << ")" << endl;
	      }
	    else
	      Command << "Vox = CRectangularVoxelMesh('CPeriodicBoundaries')" << endl;
	    Command << "Vox.SaveVoxelMesh(GetTextile('" + TextileName + "'), r\'" << ConvertString(dialog.GetPath()) << "', " << ConvertString(XVoxels) << "," << ConvertString(YVoxels) << "," << ConvertString(ZVoxels) 
		    << "," << bOutputMatrix << "," << bOutputYarns << ","<< iBoundaryConditions << "," << iElementType << ")" << endl;


	    
	    SendPythonCode(Command.str());
	  }
			
      }
  }


  
/*if not deformer.CreateAbaqusInputFile(textile, baseName + '.inp'):
	raise RuntimeError('Unable to create ABAQUS input file')
else:
    print 'Abaqus input files are complete now.'*/

}


void CTexGenMainFrame::OnSaveSCfile(wxCommandEvent& event)
{
   switch (event.GetId())
     {
      //Prof YU
     case ID_SaveSCfile:
       {
	 bool bOutputMatrix = true;
	 bool bOutputYarns = true;
	 int  iBoundaryConditions = 0;
	 int iElementType = 1;

	 int swh = 0;

	 string TextileName = GetTextileSelection();
	 stringstream Command;
	 string sc_head;
	 
	 CSwiftComp2 Wizard(this, wxID_ANY);
	 if (Wizard.RunIt())
	   {
	     string Command1 = Wizard.GetSwiftCompPara();
	     int type_flags=Wizard.analysis_type();
	     if (!Command1.empty()){
	       sc_head=Command1;
		   swh = 1;
	     }
	     
	   }
	 if (swh == 1){
		 wxFileDialog dialog
			 (
			 this,
			 wxT("Save SC file"),
			 wxGetCwd(),
			 wxEmptyString,
			 wxT("SwiftComp file (*.sc)|*.sc"),
			 wxFD_SAVE | wxFD_OVERWRITE_PROMPT | wxFD_CHANGE_DIR
			 );
		 dialog.CentreOnParent();

		 if (dialog.ShowModal() == wxID_OK)
		 {
		   scstring = ConvertString(dialog.GetPath());//Prof Yu
		   // string temp = scstring;
		   //if (temp.find(".sc") == string::npos) temp = temp + ".sc";
		   if (scstring.find(".sc") == string::npos) scstring = scstring + ".sc";
		   ofstream scfile;
		   scfile.open(scstring.c_str());
		   scfile << sc_head << "\n";
		   scfile.close();
		   
		   Command << "Vox = CRectangularVoxelMesh('CPeriodicBoundaries')" << endl;
		   Command << "Vox.SaveVoxelMesh(GetTextile('" + TextileName + "'), r\'" << scstring << "', " << ConvertString(Wizard.XVoxels()) << "," << ConvertString(Wizard.YVoxels()) << "," << ConvertString(Wizard.ZVoxels()) << "," << bOutputMatrix << "," << bOutputYarns << ","<< iBoundaryConditions << "," << iElementType << ")" << endl;
		   
		   SendPythonCode(Command.str());	     
		 }	 
	 }
       }
       break;
     }	  
}


void CTexGenMainFrame::OnPeriodicBoundaries(wxCommandEvent& event)
{
	int selection = event.GetSelection();
}

void CTexGenMainFrame::OnSaveTetgenMesh( wxCommandEvent& event )
{
	string TextileName = GetTextileSelection();
	stringstream Command;

	wxString params = wxT("pqAY");
	wxString seed = wxT("0.1");
	bool bPeriodic = true;

	wxFileDialog dialog
	(
		this,
		wxT("Save Tetgen Mesh file"),
		wxGetCwd(),
		wxEmptyString,
		wxT("Abaqus input file (*.inp)|*.inp"),
		wxFD_SAVE | wxFD_OVERWRITE_PROMPT | wxFD_CHANGE_DIR
	);
	dialog.CentreOnParent();

	wxDialog TetgenInput;
	if (wxXmlResource::Get()->LoadDialog(&TetgenInput, this, wxT("TetgenOptions")))
	{
		XRCCTRL(TetgenInput, "Param", wxTextCtrl)->SetValidator(wxTextValidator(wxFILTER_NONE, &params));
		XRCCTRL(TetgenInput, "SeedSize", wxTextCtrl)->SetValidator(wxTextValidator(wxFILTER_NUMERIC, &seed));
		XRCCTRL(TetgenInput, "Periodic", wxCheckBox)->SetValidator(wxGenericValidator(&bPeriodic));
		if (TetgenInput.ShowModal() == wxID_OK)
		{
			if (dialog.ShowModal() == wxID_OK)
			{
				Command << "TetMesh = CTetgenMesh(" + ConvertString(seed) + ")" << endl;
				Command << "TetMesh.SaveTetgenMesh(GetTextile('" + TextileName + "'), r\'" << ConvertString(dialog.GetPath())<< "', '" + ConvertString(params) + "'," << bPeriodic << + ")" << endl;

				SendPythonCode(Command.str());
			}
		}
	}
}

void CTexGenMainFrame::UpdateWindowChecks()
{
	// Find out if the widget should be checked or not
	bool bControls = m_Manager.GetPane(m_pControls).IsShown();
	bool bLogs = m_Manager.GetPane(m_pLogNotebook).IsShown();
	bool bOutliner = m_Manager.GetPane(m_pOutliner).IsShown();
	GetMenuBar()->Check(ID_ToggleControls, bControls);
	GetMenuBar()->Check(ID_ToggleLogWindow, bLogs);
	GetMenuBar()->Check(ID_ToggleOutliner, bOutliner);

}

void CTexGenMainFrame::OnWindow(wxCommandEvent& event)
{
	switch (event.GetId())
	{
	case ID_ToggleControls:
		{
			wxAuiPaneInfo& Pane = m_Manager.GetPane(m_pControls);
			Pane.Show(!Pane.IsShown());
		}
		break;
	case ID_ToggleLogWindow:
		{
			wxAuiPaneInfo& Pane = m_Manager.GetPane(m_pLogNotebook);
			Pane.Show(!Pane.IsShown());
		}
		break;
	case ID_ToggleOutliner:
		{
			wxAuiPaneInfo& Pane = m_Manager.GetPane(m_pOutliner);
			Pane.Show(!Pane.IsShown());
		}
		break;
	}
    m_Manager.Update();
	UpdateWindowChecks();
}

CTexGenRenderWindow *CTexGenMainFrame::GetRenderWindow(string WindowName)
{
	if (WindowName.empty())
	{
		int iPage = m_pViewerNotebook->GetSelection();
		if (iPage != -1)
			WindowName = ConvertString(m_pViewerNotebook->GetPageText(iPage));
	}
	// Find the window with given name
	map<string, CTexGenRenderWindow*>::iterator itRenderWindow;
	itRenderWindow = m_RenderWindows.find(WindowName);
	if (itRenderWindow != m_RenderWindows.end())
	{
		return itRenderWindow->second;
	}
	return NULL;
}

CTexGenRenderWindow *CTexGenMainFrame::CreateRenderWindow(string WindowName)
{
	if (WindowName.empty())
	{
		string DefaultName = "RenderWindow";
		WindowName = DefaultName;
		int iNumber = 1;
		// Check that the entry does not exist, and the name given is not blank
		while (m_RenderWindows.find(WindowName) != m_RenderWindows.end())
		{
			WindowName = DefaultName + "-" + stringify(++iNumber);
		}
	}
	// Check that the entry does not exist, and the name given is not blank
	if (m_RenderWindows.find(WindowName) != m_RenderWindows.end())
	{
		TGERROR("Unable to create render window, window with that name already exists: " + WindowName);
		return NULL;
	}

	CTexGenRenderWindow *pRenderWindow = new CTexGenRenderWindow(m_pViewerNotebook, wxID_ANY);
	// Attach the modeller to it
	pRenderWindow->AttachModeller(WindowName);
	// Add the section to the list using the name as the key
	m_RenderWindows[WindowName] = pRenderWindow;
	// This line must be after the render window has been added to m_RenderWindows
	// otherwise the function to update the rendering page will not work correctly
	m_pViewerNotebook->AddPage(pRenderWindow, ConvertString(WindowName), true);
	TGLOG("Created render window \"" << WindowName << "\"");
	return pRenderWindow;
}

bool CTexGenMainFrame::DeleteRenderWindow(string WindowName)
{
	if (WindowName.empty())
	{
		int iSelected = m_pViewerNotebook->GetSelection();
		if (iSelected == -1)
			return false;
		WindowName = ConvertString(m_pViewerNotebook->GetPageText(iSelected));
	}
	map<string, CTexGenRenderWindow*>::iterator itRenderWindow;
	itRenderWindow = m_RenderWindows.find(WindowName);
	if (itRenderWindow == m_RenderWindows.end())
		return false;
	size_t i;
	for (i=0; i<m_pViewerNotebook->GetPageCount(); ++i)
	{
		if (m_pViewerNotebook->GetPage(i) == itRenderWindow->second)
		{
			m_pViewerNotebook->RemovePage(i);
			break;
		}
	}
	itRenderWindow->second->CleanUp();
	itRenderWindow->second->Delete();
	m_RenderWindows.erase(itRenderWindow);
	return true;
}

void CTexGenMainFrame::SendPythonCode(string Code)
{
	if (m_ScriptRecordFile.is_open())
	{
		m_ScriptRecordFile << Code << endl;
	}
	stringstream StringStream;
	StringStream << Code;
	string Line;
	string Prefix;
	int iLine = 0;
	while (StringStream.good())
	{
		Prefix = "<" + stringify(++iLine) + "> ";
		getline(StringStream, Line);
		if (!Line.empty())
		{
			ReceiveOutput(Prefix + Line + "\n", OUTPUT_PYTHON, false, false);
		}
	}

	if (!m_PythonWrapper.SendCodeBlock(Code))
	{
		ReceiveOutput("<!> Error compiling code!", OUTPUT_PYTHON, true, false);
	}
}

void CTexGenMainFrame::OnTextiles(wxCommandEvent& event)
{
  switch (event.GetId())
    {
      //Prof YU
    case ID_SwiftCompH:
      {
	if(scstring=="") {wxMessageBox( wxT("Please export ABAQUS voxel file first."), wxT("Warning"), wxICON_INFORMATION);return;}
	string orifile;
	if (strcmp(scstring.substr(scstring.length()-4,scstring.length()).c_str(),".inp")==0)
	  orifile=scstring.substr(0,scstring.length()-4)+".ori";
	else
	  orifile=scstring+".ori";
	string scfile_H=scstring+".sc";
	CSwiftComp Wizard(this, wxID_ANY);
        vector<string> inpc;
	vector<vector<double> > coor;
	vector<vector<int> > ele;
	vector<string> elset_name;
	vector<vector<int> > elsets;
	vector<vector<double> > mat;
	vector<vector<double> > expan;
	vector<string> mat_name;
	vector<string> ori;
	Wizard.readinp(scstring,inpc);
	getcoor(inpc,coor);
	getele(inpc,ele);
	get_elset(inpc,elset_name,elsets);
	get_mats(inpc,mat,mat_name,expan);
	int ne=ele.size();
	int index[ne];
	get_index(inpc,index,elset_name,mat_name,elsets);
	get_ori(orifile,ori,ne);
	if (Wizard.RunIt())
	  {
	    string Command = Wizard.GetSwiftCompPara();
	    int type_flags=Wizard.analysis_type();
	    if (!Command.empty())
	      {
		write_output(scfile_H,Command,coor,ele,elsets,mat,index,ori,expan,type_flags);
	      }
	  }
	
	HD="H";//Decide homo or dehomo
      }
    break;
    case ID_SwiftCompD:
    {
      CSwiftComp2 Wizard(this, wxID_ANY);
      if(scstring=="") {wxMessageBox( wxT("Please export ABAQUS voxel file first."), wxT("Warning"), wxICON_INFORMATION);return;}
      if (Wizard.RunIt())
	{
	  string D_scfile=scstring+".sc.glb";
	  string Command = Wizard.GetSwiftCompPara();
	  ofstream outD;
	  outD.open(D_scfile.c_str());
	  outD << Command << endl;
	  outD.close();      
	}
      HD="L";
    }
    break;
    case ID_SwiftCompR:
    {
      if(scstring=="") {wxMessageBox( wxT("Please generate the input file for SwiftComp first."), wxT("Warning"), wxICON_INFORMATION);return;}
      HD="H";
      if(run_sc(scstring,HD)==1) return;
    }
    break;
    //end X
    case ID_DeleteTextile:
      {
	string TextileName = GetTextileSelection();
	if (!TextileName.empty())
	  {
	    if ( wxMessageBox( wxT("Do you wish to delete the textile?"), wxT("Delete textile"), wxNO_DEFAULT|wxYES_NO|wxICON_INFORMATION, this ) == wxYES )
	      {
		string Command = "DeleteTextile('" + TextileName + "')";
		SendPythonCode(Command);	
	      }
	  }
      }
      break;
    case ID_EditTextile:
      {
	string TextileName = GetTextileSelection();
	if (!TextileName.empty())
	  {
	    CTextile* pTextile = CTexGen::GetInstance().GetTextile(TextileName);
	    string Type = pTextile->GetType();
	    if (pTextile && (Type == "CTextileWeave2D" || Type == "CShearedTextileWeave2D" ||
			     Type == "CTextileOrthogonal" || Type == "CTextileAngleInterlock" || Type == "CTextileOffsetAngleInterlock" || Type == "CTextileLayerToLayer"))
	      {
		string Command;
		if (Type == "CTextileWeave2D" || Type == "CShearedTextileWeave2D")
		  {
		    CWeaveWizard Wizard(this, wxID_ANY);
		    if (Type == "CTextileWeave2D")
		      Wizard.LoadSettings(*((CTextileWeave2D*)pTextile));
		    else
		      Wizard.LoadSettings(*((CShearedTextileWeave2D*)pTextile));

		    if ( Wizard.RunIt() )
		      {
			Command = Wizard.GetCreateTextileCommand(TextileName);
		      }
		  }
		else // if (pTextile->GetType() == "CTextileWeave3D")
		  {
		    CWeaveWizard3D Wizard(this, wxID_ANY);
		    Wizard.LoadSettings(*((CTextile3DWeave*)pTextile));
		    if ( Wizard.RunIt() )
		      {
			Command = Wizard.GetCreateTextileCommand(TextileName);
		      }
		  }
		
		if (!Command.empty())
		  {
		    SendPythonCode(Command);
		    RefreshTextile(TextileName);
		  }
	      }
	  }
      }
      break;
    case ID_CreateEmptyTextile:
      {
	wxTextEntryDialog dlg(this, wxT("Please enter the name of the textile to create (or leave blank for default):"), wxT("Textile name"));
	if (dlg.ShowModal() != wxID_OK)
	  return;
	wxString TextileName = dlg.GetValue();
	
	stringstream StringStream; 
	if (TextileName.IsEmpty())
	  StringStream << "AddTextile(CTextile())" << endl;
	else
	  StringStream << "AddTextile('" << ConvertString(TextileName) << "', CTextile())" << endl;
	SendPythonCode(StringStream.str());
	/*else if (!TEXGEN.AddTextile(ConvertString(TextileName), CTextile()))
	  {
	  wxMessageBox(wxT("Textile with name \"") + TextileName + wxT("\" already exists!"), wxT("Error"), wxOK | wxICON_EXCLAMATION);
	  }*/
      }
      break;
    case ID_Create2DWeave:
      {
	CWeaveWizard Wizard(this, wxID_ANY); 
	if (Wizard.RunIt())
	  {
	    string Command = Wizard.GetCreateTextileCommand();
	    if (!Command.empty())
	      {
		SendPythonCode(Command);
	      }
	  }
      }
      break;
	case ID_GeometrySolve:
		{
			OnGeometrySolve(event);
		}
		break;

	case ID_Create3DTextile:
		{
			CWeaveWizard3D Wizard(this, wxID_ANY);
			if ( Wizard.RunIt() )
			{
				string Command = Wizard.GetCreateTextileCommand();
				if ( !Command.empty() )
				{
					SendPythonCode(Command);
				}
			}
		}
		break;
	case ID_CreateLayeredTextile:
		{
			CTextileLayersDialog Dialog;
			if ( Dialog.ShowModal() == wxID_OK )
			{
				vector<string> LayerNames;
				Dialog.GetLayerNames( LayerNames );
				if ( !LayerNames.empty() ) 
				{
					// Create python code with list of names
					string Command = ConvertMultiWeaveLayered( LayerNames );
					SendPythonCode( Command );
				}
			}
		}
		break;
	case ID_SetLayerOffsets:
		{
			string TextileName = GetTextileSelection();
			if (!TextileName.empty())
			{
				CTextile* pTextile = CTexGen::GetInstance().GetTextile(TextileName);
				string Type = pTextile->GetType();
				if ( Type != "CTextileLayered" )
				{
					TGERROR("Cannot set layers: not a layered textile");
					break;
				}
				
				CLayersOffsetDialog Dialog;
				XYZ Min,Max;
				CDomain* Domain = pTextile->GetDomain();
				((CDomainPlanes*)Domain)->GetBoxLimits(Min, Max);
				XY DomainSize( Max.x-Min.x, Max.y-Min.y );
				Dialog.LoadSettings( dynamic_cast<CTextileLayered*>(pTextile)->GetOffsets(), DomainSize );
				
				if (Dialog.ShowModal() == wxID_OK)
				{
					vector<XY> LayerOffsets;
					int iOption = Dialog.GetOption();
					stringstream StringStream;

					switch ( iOption )
					{
						case CONSTANT:
							{
							XY Offset;
							Dialog.GetConstantOffset( Offset );
							StringStream << "textile = GetTextile('" << TextileName << "')" << endl;
							StringStream << "LayeredTextile = textile.GetLayeredTextile()" << endl;
							StringStream << "Offset = XY(" << Offset << ")" << endl;
							StringStream << "LayeredTextile.SetOffsets( Offset )" << endl;
							break;
							}
						case RANDOM:
						case EDIT:
							Dialog.GetEditOffsets( LayerOffsets );
							StringStream << "textile = GetTextile('" << TextileName << "')" << endl;
							StringStream << "LayeredTextile = textile.GetLayeredTextile()" << endl;
							StringStream << "Offsets = XYVector()" << endl;
							for (int i=0; i<(int)LayerOffsets.size(); ++i)
							{
								StringStream << "Offsets.append(XY(" << LayerOffsets[i] << "))" << endl;
							}
							StringStream << "LayeredTextile.SetOffsets( Offsets )" << endl;
							break;
					}
					SendPythonCode( StringStream.str() );
					RefreshTextile( TextileName );
				}
			}
		}
		break;
	case ID_NestLayers:
	case ID_MaxNestLayers:
		{
			string TextileName = GetTextileSelection();
			if (!TextileName.empty())
			{
				CTextile* pTextile = CTexGen::GetInstance().GetTextile(TextileName);
				string Type = pTextile->GetType();
				if ( Type != "CTextileLayered" )
				{
					TGERROR("Cannot nest layers: not a layered textile");
					break;
				}

				stringstream StringStream;
				StringStream << "textile = GetTextile('" << TextileName << "')" << endl;
				StringStream << "LayeredTextile = textile.GetLayeredTextile()" << endl;
				if ( event.GetId() == ID_NestLayers )
					StringStream << "LayeredTextile.NestLayers()" << endl;
				else
					StringStream << "LayeredTextile.MaxNestLayers()" << endl;
				SendPythonCode( StringStream.str() );
				RefreshTextile( TextileName );
			}
		}
		break;
	}
}

void CTexGenMainFrame::OnGeometrySolve(wxCommandEvent& event)
{
	string TextileName = GetTextileSelection();

//	int iMeshType = 0;
	CGeometrySolver Solver;	// Create a geometry solver object just to get the default values

	wxString SeedSize; SeedSize << Solver.GetSeed();
	wxString TensileStress; TensileStress << Solver.GetTensileStress();
	wxString LongitudinalBendingModulus; LongitudinalBendingModulus << Solver.GetLongitudinalBendingModulus();
	wxString TransverseBendingModulus; TransverseBendingModulus << Solver.GetTransverseBendingModulus();
	wxString ContactStiffness; ContactStiffness << Solver.GetContactStiffness();
	wxString DisabledStiffness; DisabledStiffness << Solver.GetDisabledStiffness();
	bool bCreateCopy = false;
	wxDialog GeometrySolveOptions;
	if (wxXmlResource::Get()->LoadDialog(&GeometrySolveOptions, this, wxT("GeometrySolveOptions")))
	{
		XRCCTRL(GeometrySolveOptions, "SeedSize", wxTextCtrl)->SetValidator(wxTextValidator(wxFILTER_NUMERIC, &SeedSize));
		XRCCTRL(GeometrySolveOptions, "TensileStress", wxTextCtrl)->SetValidator(wxTextValidator(wxFILTER_NUMERIC, &TensileStress));
		XRCCTRL(GeometrySolveOptions, "LongitudinalBendingModulus", wxTextCtrl)->SetValidator(wxTextValidator(wxFILTER_NUMERIC, &LongitudinalBendingModulus));
		XRCCTRL(GeometrySolveOptions, "TransverseBendingModulus", wxTextCtrl)->SetValidator(wxTextValidator(wxFILTER_NUMERIC, &TransverseBendingModulus));
		XRCCTRL(GeometrySolveOptions, "ContactStiffness", wxTextCtrl)->SetValidator(wxTextValidator(wxFILTER_NUMERIC, &ContactStiffness));
		XRCCTRL(GeometrySolveOptions, "DisabledStiffness", wxTextCtrl)->SetValidator(wxTextValidator(wxFILTER_NUMERIC, &DisabledStiffness));
		XRCCTRL(GeometrySolveOptions, "CreateCopyCheckBox", wxCheckBox)->SetValidator(wxGenericValidator(&bCreateCopy));
		if (GeometrySolveOptions.ShowModal() == wxID_OK)
		{
			stringstream Command;
			Command << "solver = CGeometrySolver()" << endl;
			if (!SeedSize.IsEmpty())
				Command << "solver.SetSeed(" << ConvertString(SeedSize) << ")" << endl;
			if (!TensileStress.IsEmpty())
				Command << "solver.SetTensileStress(" << ConvertString(TensileStress) << ")" << endl;
			if (!LongitudinalBendingModulus.IsEmpty())
				Command << "solver.SetLongitudinalBendingModulus(" << ConvertString(LongitudinalBendingModulus) << ")" << endl;
			if (!TransverseBendingModulus.IsEmpty())
				Command << "solver.SetTransverseBendingModulus(" << ConvertString(TransverseBendingModulus) << ")" << endl;
			if (!ContactStiffness.IsEmpty())
				Command << "solver.SetContactStiffness(" << ConvertString(ContactStiffness) << ")" << endl;
			if (!DisabledStiffness.IsEmpty())
				Command << "solver.SetDisabledStiffness(" << ConvertString(DisabledStiffness) << ")" << endl;
			Command << "solver.CreateSystem('" << TextileName << "')" << endl;
			Command << "solver.SolveSystem()" << endl;
			if (bCreateCopy)
				Command << "solver.GetDeformedCopyOfTextile()" << endl;
			else
				Command << "solver.DeformTextile()" << endl;
			SendPythonCode(Command.str());
			RefreshTextile(TextileName);
		}
/*
		if (MeshOptions.ShowModal() == wxID_OK)
		{
			wxFileDialog dialog
			(
				this,
				wxT("Save Mesh file"),
				wxGetCwd(),
				wxEmptyString,
				wxT("VTK unstructured grid file (*.vtu)|*.vtu|"
					"ABAQUS input file (*.inp)|*.inp"),
				wxFD_SAVE | wxFD_OVERWRITE_PROMPT | wxFD_CHANGE_DIR
			);
			dialog.CentreOnParent();
			if (dialog.ShowModal() == wxID_OK)
			{
				stringstream Command;
				Command << "mesher = CMesher()" << endl;
				if (iMeshType == 1)
					Command << "mesher.SetHybrid(True)" << endl;
				if (iElementOrder == 1)
					Command << "mesher.SetQuadratic(True)" << endl;
				if (bPeriodic)
					Command << "mesher.SetPeriodic(True)" << endl;
				if (!SeedSize.IsEmpty())
					Command << "mesher.SetSeed(" << ConvertString(SeedSize) << ")" << endl;
				if (!MergeTolerance.IsEmpty())
					Command << "mesher.SetMergeTolerance(" << ConvertString(MergeTolerance) << ")" << endl;
				Command << "mesher.CreateMesh(r'" << TextileName << "')" << endl;
				switch (dialog.GetFilterIndex())
				{
				case 0:
					Command << "mesher.SaveVolumeMeshToVTK(r'" << ConvertString(dialog.GetPath()) << "')" << endl;
					break;
				case 1:
					Command << "mesher.SaveVolumeMeshToABAQUS(r'" << ConvertString(dialog.GetPath()) << "')" << endl;
					break;
				}
				SendPythonCode(Command.str());
			}
		}*/
	}
}

void CTexGenMainFrame::OnModeller(wxCommandEvent& event)
{
	CTexGenRenderWindow* pRenderWindow = GetRenderWindow();
	CModeller* pModeller = NULL;
	if (pRenderWindow)
	{
		pModeller = pRenderWindow->GetModeller();
	}
	if ( !pModeller )
	{
		wxMessageBox(wxT("Please create textile before using Modeller options"), wxT("Modeller Error"), wxOK | wxICON_ERROR, this);
		return;
	}
	switch (event.GetId())
	{
	case ID_CreateYarn:
		pModeller->CreateYarn();
		break;
	case ID_AssignSection:
		pModeller->AssignSectionToSelectedObjects();
		break;
	case ID_AssignInterpolation:
		pModeller->AssignInterpolationToSelectedObjects();
		break;
	case ID_AssignRepeats:
		pModeller->AssignRepeatsToSelectedObjects();
		break;
	case ID_AssignProperties:
		pModeller->AssignPropertiesToSelectedObjects();
		break;
	case ID_AssignMatrixProperties:
		pModeller->AssignMatrixProperties();
		break;
	case ID_YarnFibreVolumeFraction:
		pModeller->DisplayVolumeFractionForSelectedObjects();
		break;
/*	case ID_ShowOutliner:
		pModeller->ShowOutliner(true);
		break;*/
	case ID_SelectTool:
		{
			pModeller->SetActiveTool(TOOL_SELECT);
			m_pControls->UpdateModellingPage(pModeller);
		}
		break;
	case ID_MoveTool:
		{
			pModeller->SetActiveTool(TOOL_MOVE);
			m_pControls->UpdateModellingPage(pModeller);
		}
		break;
	case ID_RotateTool:
		{
			pModeller->SetActiveTool(TOOL_ROTATE);
			m_pControls->UpdateModellingPage(pModeller);
		}
		break;
	case ID_ScaleTool:
		{
			pModeller->SetActiveTool(TOOL_SCALE);
			m_pControls->UpdateModellingPage(pModeller);
		}
		break;
	case ID_FilterNodes:
		{
			pModeller->SetSelectNodes(event.IsChecked());
			m_pControls->UpdateModellingPage(pModeller);
		}
		break;
	case ID_FilterPath:
		{
			pModeller->SetSelectPaths(event.IsChecked());
			m_pControls->UpdateModellingPage(pModeller);
		}
		break;
	case ID_FilterSurface:
		{
			pModeller->SetSelectSurfaces(event.IsChecked());
			m_pControls->UpdateModellingPage(pModeller);
		}
		break;
	case ID_Snap:
		{
			pModeller->GetWidget()->SetSnap(event.IsChecked());
			m_pControls->UpdateModellingPage(pModeller);
		}
		break;
	case ID_Relative:
		{
			pModeller->GetWidget()->SetRelativeSnap(event.IsChecked());
			m_pControls->UpdateModellingPage(pModeller);
		}
		break;
/*	case ID_SnapSize:
		{
			pModeller->GetWidget()->SetSnapSize();
			m_pControls->UpdateModellingPage(pModeller);
		}
		break;*/
	}
}

void CTexGenMainFrame::OnPosition(wxCommandEvent &event)
{
	if (m_pControls && m_pControls->m_bUpdatingPositionText)
		return;
	int iCoord;
	switch (event.GetId())
	{
	case ID_PositionX:
		iCoord = 0;
		break;
	case ID_PositionY:
		iCoord = 1;
		break;
	case ID_PositionZ:
		iCoord = 2;
		break;
	default:
		return;
	}
	wxTextCtrl* pTextCtrl = dynamic_cast<wxTextCtrl*>(event.GetEventObject());
	if (pTextCtrl)
	{
		wxValidator* pValidator = pTextCtrl->GetValidator();
		if (pValidator)
		{
			pValidator->TransferFromWindow();
			wxString Text = pTextCtrl->GetValue();
			double dValue;
			if (Text.ToDouble(&dValue))
			{
				CTexGenRenderWindow* pRenderWindow = GetRenderWindow();
				if (pRenderWindow)
				{
					CModeller* pModeller = pRenderWindow->GetModeller();
					if (pModeller)
						pModeller->SetPosition(iCoord, dValue);
				}
			}
		}
	}
}

void CTexGenMainFrame::OnSnapSize(wxCommandEvent &event)
{
	wxTextCtrl* pTextCtrl = dynamic_cast<wxTextCtrl*>(event.GetEventObject());
	if (pTextCtrl)
	{
		wxValidator* pValidator = pTextCtrl->GetValidator();
		if (pValidator)
		{
			pValidator->TransferFromWindow();
			wxString Text = pTextCtrl->GetValue();
			double dValue;
			if (Text.ToDouble(&dValue))
			{
				CTexGenRenderWindow* pRenderWindow = GetRenderWindow();
				if (pRenderWindow)
				{
					CModeller* pModeller = pRenderWindow->GetModeller();
					if (pModeller)
						pModeller->GetWidget()->SetSnapSize(dValue);
				}
			}
		}
	}
}

void CTexGenMainFrame::OnDomains(wxCommandEvent& event)
{
	switch (event.GetId())
	{
	case ID_DeleteDomain:
		{
			string TextileName = GetTextileSelection();
			if (!TextileName.empty())
			{
				string Command = "textile = GetTextile('" + TextileName + "')\n";
				Command += "textile.RemoveDomain()";
				SendPythonCode(Command);
				RefreshTextile(TextileName);
			}
		}
		break;
	case ID_EditDomain:
		{
			string TextileName = GetTextileSelection();
			if (!TextileName.empty())
			{
				CTextile* pTextile = TEXGEN.GetTextile(TextileName);
				if (pTextile)
				{
					const CDomain* pDomain = pTextile->GetDomain();
					if (pDomain && pDomain->GetType() == "CDomainPlanes")
					{
						CDomainPlanesDialog Dialog(this, wxID_ANY);
						Dialog.LoadSettings(*((CDomainPlanes*)pDomain));
						if (Dialog.ShowModal() == wxID_OK)
						{
							string Command = Dialog.GetCreateDomainCommand();
							if (!Command.empty())
							{
								Command += "GetTextile('" + TextileName + "').AssignDomain(domain)";
								SendPythonCode(Command);
								RefreshTextile(TextileName);
							}
						}
					}
				}
			}
		}
		break;
	case ID_CreateDomainPlanes:
		{
			CDomainPlanesDialog Dialog(this, wxID_ANY);
			if (Dialog.ShowModal() == wxID_OK)
			{
				string TextileName = GetTextileSelection();
				if (!TextileName.empty())
				{
					string Command = Dialog.GetCreateDomainCommand();
					if (!Command.empty())
					{
						Command += "GetTextile('" + TextileName + "').AssignDomain(domain)";
						SendPythonCode(Command);
						RefreshTextile(TextileName);
					}
				}
			}
		}
		break;
	case ID_CreateDomainBox:
		{
			CDomainPlanesDialog Dialog(this, wxID_ANY);
			if (Dialog.CreateBox())
			{
				string TextileName = GetTextileSelection();
				if (!TextileName.empty())
				{
					string Command = Dialog.GetCreateDomainCommand();
					if (!Command.empty())
					{
						Command += "GetTextile('" + TextileName + "').AssignDomain(domain)";
						SendPythonCode(Command);
						RefreshTextile(TextileName);
					}
				}
			}
		}
		break;
	}
}

void CTexGenMainFrame::OnRendering(wxCommandEvent& event)
{
	string TextileName = GetTextileSelection();
	if (TextileName.empty())
		return;
	CTextile* pTextile = CTexGen::GetInstance().GetTextile(TextileName);
	if (!pTextile)
		return;
	string Command;
	switch (event.GetId())
	{
	case ID_RenderNodes:
		if (event.IsChecked())
			Command = "GetRenderWindow('" + TextileName + "').RenderNodes('" + TextileName + "')";
		else
			Command = "GetRenderWindow('" + TextileName + "').RemoveNodes()";
		break;
	case ID_RenderPaths:
		if (event.IsChecked())
			Command = "GetRenderWindow('" + TextileName + "').RenderPaths('" + TextileName + "')";
		else
			Command = "GetRenderWindow('" + TextileName + "').RemovePaths()";
		break;
	case ID_RenderSurface:
		if (event.IsChecked())
			Command = "GetRenderWindow('" + TextileName + "').RenderTextile('" + TextileName + "')";
		else
			Command = "GetRenderWindow('" + TextileName + "').RemoveTextiles()";
		break;
	case ID_RenderVolume:
		if (event.IsChecked())
			Command = "GetRenderWindow('" + TextileName + "').RenderMesh('" + TextileName + "')";
		else
			Command = "GetRenderWindow('" + TextileName + "').RemoveMeshes()";
		break;
	case ID_RenderInterference:
		if (event.IsChecked())
			Command = "GetRenderWindow('" + TextileName + "').RenderInterference('" + TextileName + "')";
		else
			Command = "GetRenderWindow('" + TextileName + "').RemoveInterference()";
		break;
	case ID_RenderInterferenceDepth:
		if (event.IsChecked())
			Command = "GetRenderWindow('" + TextileName + "').RenderInterference('" + TextileName + "', True)";
		else
			Command = "GetRenderWindow('" + TextileName + "').RemoveInterferenceDepth()";
		break;
	case ID_RenderOrientation:
		if (event.IsChecked())
			Command = "GetRenderWindow('" + TextileName + "').RenderGrid('" + TextileName + "')";
		else
			Command = "GetRenderWindow('" + TextileName + "').RemoveOrientations()";
		break;
	case ID_RenderDomain:
		if (event.IsChecked())
			Command = "GetRenderWindow('" + TextileName + "').RenderDomain('" + TextileName + "')";
		else
			Command = "GetRenderWindow('" + TextileName + "').RemoveDomains()";
		break;
	case ID_RenderDomainAxes:
		if (event.IsChecked())
			Command = "GetRenderWindow('" + TextileName + "').RenderDomainAxes('" + TextileName + "')";
		else
			Command = "GetRenderWindow('" + TextileName + "').RemoveDomainAxes()";
		break;
	case ID_RenderAxes:
		if (event.IsChecked())
			Command = "GetRenderWindow('" + TextileName + "').RenderAxes()";
		else
			Command = "GetRenderWindow('" + TextileName + "').RemoveAxes()";
		break;
	case ID_XRay:
		if (event.IsChecked())
			Command = "GetRenderWindow('" + TextileName + "').SetXRay(True)\n";
		else
			Command = "GetRenderWindow('" + TextileName + "').SetXRay(False)\n";
		{
			CTexGenRenderWindow* pRenderWindow = GetRenderWindow(TextileName);
			if (pRenderWindow)
			{
				CTexGenRenderer* pRenderer = pRenderWindow->GetRenderer();
				if (pRenderer)
				{
					if (pRenderer->GetNumProps(CTexGenRenderer::PROP_SURFACE))
					{
						Command += "GetRenderWindow('" + TextileName + "').RemoveTextiles()\n";
						Command += "GetRenderWindow('" + TextileName + "').RenderTextile('" + TextileName + "')";
					}
				}
			}
		}
		break;
	case ID_ParallelProjection:
		if (event.IsChecked())
			Command = "GetRenderWindow('" + TextileName + "').SetParallelProjection(True)\n";
		else
			Command = "GetRenderWindow('" + TextileName + "').SetParallelProjection(False)\n";
/*		{
			CTexGenRenderWindow* pRenderWindow = GetRenderWindow(TextileName);
			if (pRenderWindow)
			{
				CTexGenRenderer* pRenderer = pRenderWindow->GetRenderer();
				if (pRenderer)
				{
					if (pRenderer->GetNumProps(CTexGenRenderer::PROP_SURFACE))
					{
						Command += "GetRenderWindow('" + TextileName + "').RemoveTextiles()\n";
						Command += "GetRenderWindow('" + TextileName + "').RenderTextile('" + TextileName + "')";
					}
				}
			}
		}*/
		break;
	case ID_TrimtoDomain:
		if (event.IsChecked())
			Command = "GetRenderWindow('" + TextileName + "').SetTrimToDomain(True)\n";
		else
			Command = "GetRenderWindow('" + TextileName + "').SetTrimToDomain(False)\n";
		{
			CTexGenRenderWindow* pRenderWindow = GetRenderWindow(TextileName);
			if (pRenderWindow)
			{
				CTexGenRenderer* pRenderer = pRenderWindow->GetRenderer();
				if (pRenderer)
				{
					if (pRenderer->GetNumProps(CTexGenRenderer::PROP_SURFACE))
					{
						Command += "GetRenderWindow('" + TextileName + "').RemoveTextiles()\n";
						Command += "GetRenderWindow('" + TextileName + "').RenderTextile('" + TextileName + "')\n";
					}
					if ( pRenderer->GetNumProps(CTexGenRenderer::PROP_INTERFERENCE) )
					{
						Command += "GetRenderWindow('" + TextileName + "').RemoveInterference()\n";
						Command += "GetRenderWindow('" + TextileName + "').RenderInterference('" + TextileName + "')\n";
					}
					if ( pRenderer->GetNumProps(CTexGenRenderer::PROP_INTERFERENCE_DEPTH) )
					{
						Command += "GetRenderWindow('" + TextileName + "').RemoveInterferenceDepth()\n";
						Command += "GetRenderWindow('" + TextileName + "').RenderInterference('" + TextileName + "', True)\n";
					}
					if ( pRenderer->GetNumProps(CTexGenRenderer::PROP_ORIENTATION) )
					{
						Command += "GetRenderWindow('" + TextileName + "').RemoveOrientations()\n";
						Command += "GetRenderWindow('" + TextileName + "').RenderGrid('" + TextileName + "')\n";
					}
				}
			}
		}
		break;
	case ID_ChangeBackgroundColor:
		{
			string Color = GetUserSelectedColor();
			if (!Color.empty())
				Command = "GetRenderWindow('" + TextileName + "').SetBackgroundColor(" + Color + ")";
		}
		break;
	case ID_ChangeSurfaceColor:
		{
			string Color = GetUserSelectedColor();
			if (!Color.empty())
				Command = "GetRenderWindow('" + TextileName + "').SetPropsColor(CTexGenRenderer.PROP_SURFACE, " + Color + ")";
		}
		break;
	}
	if (!Command.empty())
	{
		SendPythonCode(Command);
	}
	UpdateRenderingPage();
}

void CTexGenMainFrame::OnPython(wxCommandEvent& event)
{
	switch (event.GetId())
	{
	case ID_RunScript:
		{
			wxFileDialog dialog
			(
				this,
				wxT("Run python script"),
				wxGetCwd(),
				wxEmptyString,
				wxT("Python script (*.py)|*.py"),
				wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_CHANGE_DIR
			);

			dialog.CentreOnParent();

			if (dialog.ShowModal() == wxID_OK)
			{
				string Command;
				Command = "execfile(r\"";
				Command += ConvertString(dialog.GetPath());
				Command += "\")";
				SendPythonCode(Command);
			}
		}
		break;
	case ID_RecordMacro:
		{
			if (m_ScriptRecordFile.is_open())
			{
				m_ScriptRecordFile.close();
			}
			else
			{
				wxFileDialog dialog
				(
					this,
					wxT("Record python script"),
					wxGetCwd(),
					wxEmptyString,
					wxT("Python script (*.py)|*.py"),
					wxFD_SAVE | wxFD_OVERWRITE_PROMPT | wxFD_CHANGE_DIR
				);

				dialog.CentreOnParent();

				if (dialog.ShowModal() == wxID_OK)
				{
					m_ScriptRecordFile.open(dialog.GetPath().mb_str());
					if (!m_ScriptRecordFile.is_open())
					{
						wxMessageBox(wxT("Unable to open file for recording:\n") + dialog.GetPath(), wxT("Error saving file"), wxOK | wxICON_ERROR, this);
					}
					else
					{
						m_ScriptRecordFile << "# Script recorded by TexGen v" << TEXGEN.GetVersion() << endl;
					}
				}
			}
			if (m_pControls)
				m_pControls->UpdatePythonPage(m_ScriptRecordFile.is_open());
		}
		break;
	}
}

void CTexGenMainFrame::OnLogNotebook(wxAuiNotebookEvent& event)
{
	if (event.GetSelection() != -1)
		m_pLogNotebook->SetPageBitmap(event.GetSelection(), wxArtProvider::GetBitmap(wxART_INFORMATION, wxART_OTHER, wxSize(16, 16)));
}

void CTexGenMainFrame::UpdateRenderingPage()
{
	CTexGenRenderWindow* pRenderWindow = GetRenderWindow();
	if (pRenderWindow)
	{
		CTexGenRenderer* pRenderer = pRenderWindow->GetRenderer();
		if (pRenderer && m_pControls)
		{
			m_pControls->UpdateRenderingPage(pRenderer);
		}
	}
}

void CTexGenMainFrame::UpdateModellingPage()
{
	CTexGenRenderWindow* pRenderWindow = GetRenderWindow();
	if (pRenderWindow)
	{
		CModeller* pModeller = pRenderWindow->GetModeller();
		if (pModeller && m_pControls)
		{
			m_pControls->UpdateModellingPage(pModeller);
		}
	}
}

void CTexGenMainFrame::UpdateOutliner()
{
	CTexGenRenderWindow* pRenderWindow = GetRenderWindow();
	if (pRenderWindow)
	{
		CModeller* pModeller = pRenderWindow->GetModeller();
		if (pModeller && m_pControls)
		{
			pModeller->UpdateOutlinerItems();
			return;
		}
	}

	if (!m_pOutliner)
		return;

	wxTreeCtrl* pTreeCtrl = XRCCTRL(*m_pOutliner, "TreeCtrl", wxTreeCtrl);
	if (pTreeCtrl)
	{
		pTreeCtrl->DeleteAllItems();
	}
}

void CTexGenMainFrame::UpdatePositionText()
{
	CTexGenRenderWindow* pRenderWindow = GetRenderWindow();
	if (pRenderWindow)
	{
		CModeller* pRenderer = pRenderWindow->GetModeller();
		if (pRenderer && m_pControls)
		{
			m_pControls->UpdatePositionText(pRenderer);
		}
	}
}

void CTexGenMainFrame::OnViewerNotebookPageChanged(wxAuiNotebookEvent& event)
{
	if (event.GetSelection() != -1)
	{
		UpdateRenderingPage();
		UpdateModellingPage();
		UpdateOutliner();
	}
}

void CTexGenMainFrame::OnViewerNotebookClose(wxAuiNotebookEvent& event)
{
	// Prevent the event from actually deleting the window, this will be done automagically
	// when the textile is deleted
	event.Veto();
	// Delete the textile...
	string TextileName = ConvertString(m_pViewerNotebook->GetPageText(event.GetSelection()));
//	string TextileName = GetTextileSelection();
	if (!TextileName.empty())
	{
		string Command = "DeleteTextile('" + TextileName + "')";
		SendPythonCode(Command);
	}
}

/*void CTexGenMainFrame::RefreshDomain(string DomainName)
{
//	map<string, CTexGenRenderWindow*>::iterator itRenderWindow;
//	CTextile* pTextile;
//	for (itRenderWindow = m_RenderWindows.begin(); itRenderWindow != m_RenderWindows.end(); ++itRenderWindow)
//	{
//		pTextile = CTexGen::GetInstance().GetTextile(itRenderWindow->first);
//		if (pTextile && pTextile->GetAssignedDomain() == DomainName)
//		{
//			RefreshTextile(itRenderWindow->first);
//		}
//	}
}*/

bool CTexGenMainFrame::RefreshTextile(string TextileName)
{
	if (TextileName.empty())
	{
		TextileName = GetTextileSelection();
	}
	CTextile* pTextile = TEXGEN.GetTextile(TextileName);
	CTexGenRenderWindow* pRenderWindow = GetRenderWindow(TextileName);
	if (pTextile && pRenderWindow)
	{
/*		string DomainName = pTextile->GetAssignedDomain();
		if (!CTexGen::GetInstance().GetDomain(DomainName))
		{
			DomainName = "";
			pTextile->AssignDomain(DomainName);
		}*/
		CTexGenRenderer* pRenderer = pRenderWindow->GetRenderer();
		if (pRenderer)
		{
			if (pRenderer->GetNumProps(CTexGenRenderer::PROP_NODE))
			{
				pRenderer->RemoveNodes();
				pRenderer->RenderNodes(TextileName);
			}
			if (pRenderer->GetNumProps(CTexGenRenderer::PROP_PATH))
			{
				pRenderer->RemovePaths();
				pRenderer->RenderPaths(TextileName);
			}
			if (pRenderer->GetNumProps(CTexGenRenderer::PROP_SURFACE))
			{
				pRenderer->RemoveTextiles();
				pRenderer->RenderTextile(TextileName);
			}
			if (pRenderer->GetNumProps(CTexGenRenderer::PROP_VOLUME))
			{
				pRenderer->RemoveMeshes();
				pRenderer->RenderMesh(TextileName);
			}
			if (pRenderer->GetNumProps(CTexGenRenderer::PROP_INTERFERENCE))
			{
				pRenderer->RemoveInterference();
				pRenderer->RenderInterference(TextileName);
			}
			if ( pRenderer->GetNumProps(CTexGenRenderer::PROP_INTERFERENCE_DEPTH))
			{
				pRenderer->RemoveInterferenceDepth();
				pRenderer->RenderInterference( TextileName, true );
			}
			if (pRenderer->GetNumProps(CTexGenRenderer::PROP_ORIENTATION))
			{
				pRenderer->RemoveOrientations();
				pRenderer->RenderGrid(TextileName);
			}
			if (pRenderer->GetNumProps(CTexGenRenderer::PROP_DOMAIN))
			{
				pRenderer->RemoveDomains();
				pRenderer->RenderDomain(TextileName);
			}
			if (pRenderer->GetNumProps(CTexGenRenderer::PROP_DOMAINAXES))
			{
				pRenderer->RemoveDomainAxes();
				pRenderer->RenderDomainAxes(TextileName);
			}
			return true;
		}
	}
	return false;
}

string CTexGenMainFrame::GetUserSelectedColor()
{
	wxColourDialog ColorDialog(this);
	int i;
	for (i=0; i<16; ++i)
	{
		COLOR IndexedColor = GetIndexedColor(i);
		wxColour Color;
		Color.Set((unsigned char)(IndexedColor.r*255), (unsigned char)(IndexedColor.g*255), (unsigned char)(IndexedColor.b*255));
		ColorDialog.GetColourData().SetCustomColour(i, Color);
	}
	if (ColorDialog.ShowModal() == wxID_OK)
	{
		wxColour Color = ColorDialog.GetColourData().GetColour();
		double r, g, b;
		r = Color.Red()/255.0;
		g = Color.Green()/255.0;
		b = Color.Blue()/255.0;
		string ReturnVal = "COLOR(";
		ReturnVal += stringify(r) + ", ";
		ReturnVal += stringify(g) + ", ";
		ReturnVal += stringify(b) + ")";
		return ReturnVal;
	}
	return "";
}


string CTexGenMainFrame::GetTextileSelection()
{
	if (m_pViewerNotebook)
	{
		int iIndex = m_pViewerNotebook->GetSelection();
		if (iIndex != -1)
		{
			return ConvertString(m_pViewerNotebook->GetPageText(iIndex));
		}
	}
	return "";
/*	const map<string, CTextile*> &Textiles = CTexGen::GetInstance().GetTextiles();

	if (Textiles.empty())
	{
		wxMessageBox("The textile list is empty!", "No textiles", wxOK | wxICON_ERROR, this);
		return "";
	}

	wxArrayString TextileNames;
	map<string, CTextile*>::const_iterator itTextile;
	for (itTextile = Textiles.begin(); itTextile != Textiles.end(); ++itTextile)
	{
		TextileNames.Add(itTextile->first.c_str());
	}

	return wxGetSingleChoice("Select a textile from the list below", "Select textile", TextileNames, this).c_str();*/
}

void CTexGenMainFrame::ProcessFiles(const wxArrayString& filenames)
{
	int i;
	for (i=0; i<(int)filenames.GetCount(); ++i)
	{
		const wxString &Filename = filenames[i];
		if (Filename.AfterLast('.') == wxT("tg3"))
		{
			string Command;
			Command = "ReadFromXML(r\"";
			Command += ConvertString(Filename);
			Command += "\")";
			SendPythonCode(Command);
		}
		else if (Filename.AfterLast('.') == wxT("py"))
		{
			string Command;
			Command = "execfile(r\"";
			Command += ConvertString(Filename);
			Command += "\")";
			SendPythonCode(Command);
		}
	}
}


void CTexGenMainFrame::TextileCallback(string TextileName, bool bAdded)
{
	DeleteRenderWindow(TextileName);  //Need to delete window both if deleting textile or overwriting
	if (bAdded)
	{
		CTexGenRenderWindow* pRenderWindow = CreateRenderWindow(TextileName);  
		CTexGenRenderer* pRenderer;
		CTextile* pTextile = CTexGen::GetInstance().GetTextile(TextileName);
		if (pRenderWindow && (pRenderer = pRenderWindow->GetRenderer()) && pTextile)
		{
			pRenderer->RenderTextile(TextileName);
			pRenderer->RenderDomain(TextileName);
			pRenderer->RenderDomainAxes(TextileName);
			pRenderer->RenderAxes();
		}
	}
}

void CTexGenMainFrame::RendererCallback()
{
	UpdateRenderingPage();
}

void CNoteBookHandler::OnSetFocus(wxFocusEvent &event)
{
	CTexGenRenderWindow* pRenderWindow = m_MainFrame.GetRenderWindow();
	if (pRenderWindow)
		pRenderWindow->SetFocus();
/*	if (pRenderWindow)
		pRenderWindow->OnKeyDown(event);
	else
		event.Skip();*/
}

void CTexGenMainFrame::OnPaneButton(wxAuiManagerEvent& event)
{
	if (event.GetButton() == wxAUI_BUTTON_CLOSE)
	{
		event.GetPane()->Hide();
		UpdateWindowChecks();
		m_Manager.Update();
	}
	else
		event.Skip();
}

void COutlineHandler::OnSelectionChanged(wxTreeEvent& event)
{
	CTexGenRenderWindow* pRenderWindow = m_MainFrame.GetRenderWindow();
	if (pRenderWindow)
	{
		CModeller* pModeller = pRenderWindow->GetModeller();
		if (pModeller)
		{
			pModeller->OnOutlinerSelectionChanged(m_MainFrame.GetOutliner());
		}
	}
}

void COutlineHandler::OnDeleteSelected(wxCommandEvent& event)
{
	CTexGenRenderWindow* pRenderWindow = m_MainFrame.GetRenderWindow();
	if (pRenderWindow)
	{
		CModeller* pModeller = pRenderWindow->GetModeller();
		if (pModeller)
		{
			pModeller->DeleteSelectedObjects();
		}
	}
}

void COutlineHandler::OnInsertNode(wxCommandEvent& event)
{
	CTexGenRenderWindow* pRenderWindow = m_MainFrame.GetRenderWindow();
	if (pRenderWindow)
	{
		CModeller* pModeller = pRenderWindow->GetModeller();
		if (pModeller)
		{
			pModeller->InsertNodes();
		}
	}
}

void COutlineHandler::OnDuplicateYarn(wxCommandEvent& event)
{
	CTexGenRenderWindow* pRenderWindow = m_MainFrame.GetRenderWindow();
	if (pRenderWindow)
	{
		CModeller* pModeller = pRenderWindow->GetModeller();
		if (pModeller)
		{
			pModeller->DuplicateYarns();
		}
	}
}

void CTexGenMainFrame::OnOptions(wxCommandEvent& event)
{
	string Command;
	switch (event.GetId())
	{
	case ID_OutputMessages:
		if (event.IsChecked())
			CTexGen::GetInstance().SetMessages( true, CLoggerGUI() );
		else
			CTexGen::GetInstance().SetMessages( false, CLoggerNull() );
		break;
	case ID_PatternDraft:
		{
			string TextileName = GetTextileSelection();
			CTextile* pTextile = TEXGEN.GetTextile( TextileName );
			if ( !pTextile )
			{
				wxMessageBox(wxT("Cannot create pattern draft - no textile loaded"), wxT("Pattern Draft Error"), wxOK | wxICON_ERROR, this);
				return;
			}
			string Type = pTextile->GetType();
			stringstream Command;
			
			// Get appropriate weave type
			if ( Type == "CTextileWeave2D" )
				Command << "Weave = GetTextile( '" << TextileName << "' ).GetWeave()" << endl;
			else if ( Type == "CTextile3DWeave" )
				Command << "Weave = GetTextile( '" << TextileName << "' ).Get3DWeave()" << endl;
			else if ( Type == "CTextileOrthogonal" )
				Command << "Weave = GetTextile( '" << TextileName << "' ).GetOrthogonalWeave()" << endl;
			else if ( Type == "CTextileLayerToLayer" )
				Command << "Weave = GetTextile( '" << TextileName << "' ).GetLayerToLayerWeave()" << endl;
			else
			{
				wxMessageBox(wxT("Cannot create pattern draft - incorrect weave type"), wxT("Pattern Draft Error"), wxOK | wxICON_ERROR, this);
				return;
			}

			Command << "Weave.ConvertToPatternDraft()" << endl;
			Command << "PatternDraft = Weave.GetPatternDraft()" << endl;
			Command << "PatternDraft.CreatePatternDraft()" << endl;
			
			SendPythonCode( Command.str() );

			CPatternDraftDialog Dialog;
			if ( Type == "CTextileWeave2D" )
				Dialog.SetPatternDraft( ((CTextileWeave2D*)pTextile)->GetPatternDraft());
			else if ( Type == "CTextile3DWeave" )
				Dialog.SetPatternDraft( ((CTextile3DWeave*)pTextile)->GetPatternDraft());
			else if ( Type == "CTextileOrthogonal" )
				Dialog.SetPatternDraft( ((CTextileOrthogonal*)pTextile)->GetPatternDraft());
			else 
				Dialog.SetPatternDraft( ((CTextileLayerToLayer*)pTextile)->GetPatternDraft());
			
			if (Dialog.ShowModal() == wxID_OK)
			{
				
			}
			
		}
		break;
	case ID_DomainVolumeFraction:
		{
			string TextileName = GetTextileSelection();
			CTextile* pTextile = TEXGEN.GetTextile( TextileName );
			if ( !pTextile )
			{
				wxMessageBox(wxT("Cannot calculate volume fraction - no textile loaded"), wxT("Volume Fraction Error"), wxOK | wxICON_ERROR, this);
				return;
			}

			if ( !pTextile->GetDomain() )
			{
				wxMessageBox(wxT("Cannot calculate volume fraction - no domain specified"), wxT("Volume Fraction Error"), wxOK | wxICON_ERROR, this);
				return;
			}
			
			double VolumeFraction = pTextile->GetDomainVolumeFraction();
			wxString VfMessage = wxString::Format(wxT("Domain fibre volume fraction = %.6f"), VolumeFraction );
			wxMessageBox( VfMessage, wxT("Volume Fraction"), wxOK|wxCENTRE, this );
		}
		break;
	case ID_QuickDomainVolumeFraction:
		{
			string TextileName = GetTextileSelection();
			CTextile* pTextile = TEXGEN.GetTextile( TextileName );
			if ( !pTextile )
			{
				wxMessageBox(wxT("Cannot calculate volume fraction - no textile loaded"), wxT("Volume Fraction Error"), wxOK | wxICON_ERROR, this);
				return;
			}

			if ( !pTextile->GetDomain() )
			{
				wxMessageBox(wxT("Cannot calculate volume fraction - no domain specified"), wxT("Volume Fraction Error"), wxOK | wxICON_ERROR, this);
				return;
			}
			
			double VolumeFraction = pTextile->GetQuickDomainVolumeFraction();
			wxString VfMessage = wxString::Format(wxT("Domain fibre volume fraction = %.6f\nCalculated using fibre volume fraction for whole yarns\nUse Calculate Domain Volume Fraction to use localised Vf where yarn cross-section varies"), VolumeFraction );
			wxMessageBox( VfMessage, wxT("Volume Fraction"), wxOK|wxCENTRE, this );
		}
		break;
	default:
		break;
	}
}

CAbaqusVoxelInput::CAbaqusVoxelInput(wxWindow* parent)
{
	wxXmlResource::Get()->LoadDialog(this, parent, wxT("AbaqusVoxelOptions"));
}

void CAbaqusVoxelInput::OnOffsetUpdate(wxUpdateUIEvent& event)
{
	wxRadioBox* PeriodicBoundariesCtrl = (wxRadioBox*)FindWindow(XRCID("PeriodicBoundaries"));
	if ( PeriodicBoundariesCtrl->GetSelection() == STAGGERED_BC )
	{
		event.Enable(true);
	}
	else
	{
		event.Enable(false);
	}
}

CVolumeMeshOptions::CVolumeMeshOptions(wxWindow* parent)
{
	wxXmlResource::Get()->LoadDialog(this, parent, wxT("VolumeMeshOptions"));
}

void CVolumeMeshOptions::OnPeriodicBoundariesUpdate(wxUpdateUIEvent& event)
{
	wxCheckBox* PeriodicCtrl = (wxCheckBox*)FindWindow(XRCID("Periodic"));
	if ( PeriodicCtrl->GetValue() )
	{
		event.Enable(true);
		wxRadioBox* PeriodicBoundariesCtrl = (wxRadioBox*)FindWindow(XRCID("PeriodicBoundaries"));
		PeriodicBoundariesCtrl->Enable(STAGGERED_BC, false);
	}
	else
	{
		event.Enable(false);
	}
}


//Prof Yu
void getcoor(vector<string> &inpc, vector<vector<double> > &coor){
  int flags=0;
  for(unsigned int i=0;i<inpc.size();i++){
    if(inpc[i].find("*NODE")!=string::npos) {flags=1; continue;}
    if(inpc[i][0]=='*'&&flags==1) return;
    if(flags==1){
      for(unsigned int j=0;j<inpc[i].length();j++)
	if(inpc[i][j]==',') inpc[i][j]=' ';
      stringstream s(inpc[i]);
      double index,x1,x2,x3;
      if(s>>index>>x1>>x2>>x3){
	vector<double> temp;
	temp.push_back(index);
	temp.push_back(x1);
	temp.push_back(x2);
	temp.push_back(x3);
	coor.push_back(temp);
      }
    } 
  }
}

void getele(vector<string> &inpc, vector<vector<int> > &ele){
  int flags=0;
  string eletype;
  for(unsigned int i=0;i<inpc.size();i++){
    if(inpc[i].find("*ELEMENT")!=string::npos) {
      eletype=inpc[i].substr(14);
      flags=1; continue;}
    if(inpc[i][0]=='*'&&flags==1) return;
    if(flags==1){
      for(unsigned int j=0;j<inpc[i].length();j++)
	if(inpc[i][j]==',') inpc[i][j]=' ';
      stringstream s(inpc[i]);
      int index,e1,e2,e3,e4,e5,e6,e7,e8;
      if(eletype=="C3D8R"||eletype=="C3D8"){
	if(s>>index>>e1>>e2>>e3>>e4>>e5>>e6>>e7>>e8){
	  vector<int> temp;
	  temp.push_back(index);
	  temp.push_back(e1);
	  temp.push_back(e2);
	  temp.push_back(e3);
	  temp.push_back(e4);
	  temp.push_back(e5);
	  temp.push_back(e6);
	  temp.push_back(e7);
	  temp.push_back(e8);
	  ele.push_back(temp);
	}
      } 
    }
  }
}


void get_elset(vector<string> &inpc, vector<string> &elsetname, vector<vector<int> > &elsets){
  int flags=0;
  vector<string> temp;
  vector<vector<int> > elset;//Note elset has a blank element at the fr=irst position
  for(unsigned int i=0;i<inpc.size();i++){
    if(inpc[i].find("*ELSET")!=string::npos){
      elsetname.push_back(inpc[i].substr(13));
      flags=1;
      vector<int> temp2;
      get_elset2(temp,temp2);
      temp.clear();
      elset.push_back(temp2);
      continue;}
    //if(inpc[i][0]=='*'&&flags==1) return;
    if(flags==1&&inpc[i][0]!='*')
      temp.push_back(inpc[i]);
    else if(inpc[i][0]=='*'&&flags==1&&inpc[i].substr(0,6)!="*ELSET") {
      flags=0;
      vector<int> temp2;
      get_elset2(temp,temp2);
      temp.clear();
      elset.push_back(temp2); }          
  }
  for(unsigned int i=1;i<elset.size();i++)
    elsets.push_back(elset[i]);
  
  // for(unsigned int k=0; k < elsetname.size();k++)
  //  cout <<elsetname[k] << endl;
  //for(unsigned int k=0; k < elset[2].size();k++)
  // cout << elset[2][k];
}

void get_elset2(vector<string> &temp, vector<int> &temp2){
  for (unsigned int j=0;j<temp.size();j++){
    stringstream ss(temp[j]);
    int ii;
    while(ss>>ii){
      temp2.push_back(ii);
      if(ss.peek()==',')
	ss.ignore();}
  }
}

void get_mats(vector<string> &inpc, vector<vector<double> > &mat, vector<string> &mat_name, vector<vector<double> > &expan){
  int flags=0;
  vector<vector<string> > temp;//record elastic
  vector<string> temp2;//record elastic
  vector<string> exp_tmp;

  for(unsigned int i=0;i<inpc.size();i++){
    if(inpc[i].find("*MATERIAL")!=string::npos){
      if(flags==1) {temp.push_back(temp2);temp2.clear();}
      mat_name.push_back(inpc[i].substr(15));
      continue;}
    if(inpc[i].substr(0,8)=="*ELASTIC") {flags=1;continue;}
   
    if(inpc[i].find("*EXPANSION")!=string::npos){
      flags=2;
      temp.push_back(temp2);
      temp2.clear();
      continue;
    }

    if(flags==2&&inpc[i][0]!='*'){
      exp_tmp.push_back(inpc[i]);
      flags=0;
    }
    /*
    if(flags==2&&inpc[i][0]=='*'){
      flags=0;
      exp_tmp.push_back(exp_tmp2);
      exp_tmp2.clear();
      }*/

    
    if(flags==1&&inpc[i][0]!='*') temp2.push_back(inpc[i]);
    //    else if(flags==1&&inpc[i][0]=='*'){
    // flags=0;
    // temp.push_back(temp2);
    // temp2.clear();
    //}
  }
  //delete comma
  for(unsigned int i=0; i < temp.size();i++){
    vector<double> temp3;
    for(unsigned int j=0; j< temp[i].size();j++){
      stringstream ss(temp[i][j]);
      double ii;
      while(ss>>ii){
	temp3.push_back(ii);
	if(ss.peek()==',')
	  ss.ignore();}
    }
    mat.push_back(temp3);
  }

  for(unsigned int i=0; i < exp_tmp.size();i++){
    vector<double> exp_tmp3;
    stringstream ss(exp_tmp[i]);
    double ii;
    while(ss>>ii){
      exp_tmp3.push_back(ii);
      if(ss.peek()==',')
	ss.ignore();}
    expan.push_back(exp_tmp3);
  }
}


void get_index(vector<string> &inpc, int index[], vector<string> &elset_name, vector<string> &mat_name, vector<vector<int> > &elsets){
  vector<string> setname2;
  vector<string> matname2;
  for(unsigned int i=0;i<inpc.size();i++){
    if(inpc[i].find("*SOLIDSECTION")!=string::npos){
      setname2.push_back(inpc[i].substr(20,find_nth(inpc[i],2)-20));
      matname2.push_back(inpc[i].substr(find_nth(inpc[i],2)+10,find_nth(inpc[i],3)-find_nth(inpc[i],2)-10));
   }    
  }
  for(unsigned int i=0;i<setname2.size();i++){
    for(unsigned int j=0;j<elset_name.size();j++){
      if(setname2[i]==elset_name[j]){
	for(unsigned int k=0;k<mat_name.size();k++)
	  if(matname2[i]==mat_name[k]){
	    for(unsigned int kk=0;kk<elsets[j].size();kk++)
	      index[elsets[j][kk]-1]=k+1;
	  }
      }
    }
  }  
}


int find_nth(string line, int nth){
  int count=0;
  for (unsigned i=0;i<line.length();i++){
    if(line[i]==',') count++;
    if(count==nth||line[i]=='\n'||line[i]=='\0') return i;
  }
  return line.length();
}


void get_ori(string orifile, vector<string> &ori, int ne){
  int count=0;
  string line;
  ifstream ifile(orifile.c_str());
  if(!ifile) {cout<<"Orientation file does not exist" << endl;return;}
  while(getline(ifile,line)){
    count++;
    if(count==1) continue;
    for(unsigned int i=0;i<line.length();i++){
      if(line[i]==',') line[i]=' ';
    }
    //line=line.substr(0,line.length()-1);//remove "\n"
    ori.push_back(line);
  }
  if(count!=ne+1) {cout<<"Missing elements definition"<<endl;return;}
}

void write_output(string jobname,string Command, vector<vector<double> > coor1,vector<vector<int> > ele1,vector<vector<int> > elsets1, vector<vector<double> > mat1, int index[], vector<string> ori, vector<vector<double> > expan1,int type_flags1){
  
  int numnodes=coor1.size(),numeles=ele1.size(),nummat=mat1.size();
  ofstream out2;
  out2.open(jobname.c_str());

  out2 << Command << "\n";
  out2 << "3 " << numnodes << " " << numeles << " " << nummat << " 0 0 ";
  out2 << "                #nSG nnode nelem nmate nslave nlayer\n\n"; 

  for(unsigned int i=0;i<coor1.size();i++)
    out2 << coor1[i][0] << " " << coor1[i][1] << " " <<coor1[i][2] << " " << coor1[i][3] << "\n";
  out2 << "\n";
  
  for(unsigned int i=0;i<ele1.size();i++){
    out2 << ele1[i][0] << " " << index[i] << " " << ele1[i][1] << " " << ele1[i][2] << " " << ele1[i][3] << " ";
    out2 << ele1[i][4] <<" "<< ele1[i][5] <<" "<< ele1[i][6] <<" "<< ele1[i][7] <<" "<< ele1[i][8];
    out2 << " 0 0 0 0 0 0 0 0 0 0 0 0\n";
  }
  out2 << "\n";
  for(unsigned int i=0;i<ori.size();i++)
    out2 << ori[i] << "  0 0 0\n";
  
  out2 << "\n\n";
  for(unsigned int i=0;i<mat1.size();i++){
    if(mat1[i].size()==2){
      out2 << i+1 << " 0 1\n" << "0 0\n";
      out2 << mat1[i][0] << " " << mat1[i][1] << "\n";
      if(type_flags1==1)
	out2 << expan1[i][0] << " 0\n";
      out2 << "\n";
    }
    else if(mat1[i].size()==9){
      out2 << i+1 << " 1 1\n" << "0 0\n";
      out2 << mat1[i][0] << " " << mat1[i][1] << " " << mat1[i][2] << "\n";
      out2 << mat1[i][6] << " " << mat1[i][7] << " " << mat1[i][8] << "\n";
      out2 << mat1[i][3] << " " << mat1[i][4] << " " << mat1[i][5] << "\n";
      if(type_flags1==1)
	out2 << expan1[i][0] << " " << expan1[i][1] << " " << expan1[i][2] << " 0\n";
      out2 << "\n";
    }
  }

  out2 << volume(coor1,jobname) << "\n";

  out2.close();
}

double volume(vector<vector<double> > &coor2, string scfile2){
  ifstream dim_sc(scfile2.c_str());
  string line1st;
  int flags=0;//check 2D or 3D
  if(dim_sc.good()){
    getline(dim_sc,line1st);
    if(line1st.find("plate")!=string::npos)
      flags=1;
  }
  dim_sc.close();
  double Xmax=0,Xmin=0,Ymax=0,Ymin=0,Zmax=0,Zmin=0;
  //double temp1=0,temp2=0;
  for (int i=0;i<coor2.size();i++){
    if(coor2[i][1]>Xmax)
      Xmax=coor2[i][1];
    else if(coor2[i][1]<Xmin)
      Xmin=coor2[i][1];
    if(coor2[i][2]>Ymax)
      Ymax=coor2[i][2];
    else if(coor2[i][2]<Ymin)
      Ymin=coor2[i][2];
    if(coor2[i][3]>Zmax)
      Zmax=coor2[i][3];
    else if(coor2[i][3]<Zmin)
      Zmin=coor2[i][3];
  }
  if (flags==0) return fabs((Xmax-Xmin)*(Ymax-Ymin)*(Zmax-Zmin));
  else if (flags==1) return fabs((Xmax-Xmin)*(Ymax-Ymin));
  else return 0;
}

int run_sc(string scfile,string hd){
  //string scfile=SCname+".sc";
  string sgD="3D";

  ifstream dim_sc(scfile.c_str());
  string line1st;
  //check 2D or 3D
  if(dim_sc.good()){
    getline(dim_sc,line1st);
    if(line1st.find("plate")!=string::npos)
      sgD="2D";
  }
  dim_sc.close();
  
  //string hpar = "H";
  //string depar = "L";
  const char *exe1=scfile.c_str();
  const char *exe2=sgD.c_str();
  const char *exe3=hd.c_str();
  //const char *exe4=depar.c_str();
  int iii;
  char buf[1024];
  int rslt = readlink("/proc/self/exe", buf, 1024);
  buf[rslt] = '\0';
  for (iii = rslt; iii >= 0; iii--){
    if (buf[iii] == '/'){
      buf[iii + 1] = '\0';
      break;
    }
  }
  std::stringstream ss;
  std::string wok;
  ss << buf;
  ss >> wok;
  std::string pathname = wok + "SwiftComp";
  std::string pathname1 = "texgen4sc";
  //const char *exe5 = pathname.c_str(); //for testing purpose  
  const char *exe5 = pathname1.c_str();

  // Prof. Yu, get working directory of data file
  std::string workingdir = scfile;
  std::string spec = "/";
  std::string add = "";
  std::string address = "";
  size_t pos = 0;
  while ((pos = workingdir.find(spec)) != std::string::npos){
    add = workingdir.substr(0, pos);
    address += add + "/";
    workingdir.erase(0, pos + spec.length());
  }
  address.erase(address.length() - 1, address.length());
  const char *addworking = address.c_str();

  std::string result = scfile + ".k";
  remove(result.c_str());
  
  pid_t pid;
  pid=fork();
  if(pid==0) {
    chdir(addworking);
    // if (type == "homo") execlp( exe4, exe4, exe1, exe2, exe5, NULL);
    // else if (type == "dehomo") execlp( exe4, exe4, exe1, exe2, exe3, NULL);
    execlp( exe5, exe5, exe1, exe2, exe3, NULL);
  }
  else
    wait(NULL);
  	
  ifstream ifile(result.c_str());
  if (ifile){
    const char *resultfile = result.c_str();
    pid=fork();
    if(pid==0)
      execlp("geany","geany", resultfile, NULL);
    else
      wait(NULL);
  }
  return 1;
}
