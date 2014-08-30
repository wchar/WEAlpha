#include "StaticMeshEditor.h"


///////////////////////////////////////////////////////////////////////////

fbStaticMeshEditor::fbStaticMeshEditor( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
    this->SetSizeHints( wxDefaultSize, wxDefaultSize );
    this->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_ACTIVECAPTION ) );

    wxBoxSizer* bSizer18;
    bSizer18 = new wxBoxSizer( wxVERTICAL );

    m_panelToolBar = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
    wxBoxSizer* bSizer241;
    bSizer241 = new wxBoxSizer( wxHORIZONTAL );

    wxToolBar* m_toolBar41;
    m_toolBar41 = new wxToolBar( m_panelToolBar, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_HORIZONTAL ); 
    m_toolBar41->SetBackgroundColour( wxColour( 90, 125, 160 ) );

    m_toolOpen = m_toolBar41->AddTool( wxID_ANY, wxT("tool"), wxBitmap( wxT("../../wxFormBuilder/res/Get Document.png"), wxBITMAP_TYPE_ANY ), wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString, NULL ); 

    m_toolSave = m_toolBar41->AddTool( wxID_ANY, wxT("tool"), wxBitmap( wxT("../../wxFormBuilder/res/Send Document.png"), wxBITMAP_TYPE_ANY ), wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString, NULL ); 

    m_toolImport = m_toolBar41->AddTool( wxID_ANY, wxT("tool"), wxBitmap( wxT("../../wxFormBuilder/res/Import Document.png"), wxBITMAP_TYPE_ANY ), wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString, NULL ); 

    m_toolBar41->Realize(); 

    bSizer241->Add( m_toolBar41, 0, wxEXPAND, 5 );

    wxToolBar* m_toolBar71;
    m_toolBar71 = new wxToolBar( m_panelToolBar, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_HORIZONTAL ); 
    m_toolBar71->SetBackgroundColour( wxColour( 79, 108, 138 ) );

    m_toolSetting = m_toolBar71->AddTool( wxID_ANY, wxT("tool"), wxBitmap( wxT("../../wxFormBuilder/res/Gear Alt.png"), wxBITMAP_TYPE_ANY ), wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString, NULL ); 

    m_toolInfo = m_toolBar71->AddTool( wxID_ANY, wxT("tool"), wxBitmap( wxT("../../wxFormBuilder/res/Get Info Blue Button.png"), wxBITMAP_TYPE_ANY ), wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString, NULL ); 

    m_toolBar71->Realize(); 

    bSizer241->Add( m_toolBar71, 1, wxEXPAND, 5 );


    m_panelToolBar->SetSizer( bSizer241 );
    m_panelToolBar->Layout();
    bSizer241->Fit( m_panelToolBar );
    bSizer18->Add( m_panelToolBar, 0, wxEXPAND, 5 );

    m_splitter_v = new wxSplitterWindow( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_3D|wxSP_LIVE_UPDATE );
    m_splitter_v->Connect( wxEVT_IDLE, wxIdleEventHandler( fbStaticMeshEditor::m_splitter_vOnIdle ), NULL, this );

    m_panelMaterial = new wxPanel( m_splitter_v, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
    wxBoxSizer* bSizer19;
    bSizer19 = new wxBoxSizer( wxVERTICAL );


    m_panelMaterial->SetSizer( bSizer19 );
    m_panelMaterial->Layout();
    bSizer19->Fit( m_panelMaterial );
    wxPanel* m_panel14;
    m_panel14 = new wxPanel( m_splitter_v, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
    wxBoxSizer* bSizer20;
    bSizer20 = new wxBoxSizer( wxVERTICAL );

    wxStaticText* m_staticText7;
    m_staticText7 = new wxStaticText( m_panel14, wxID_ANY, wxT("ModelViewer"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE );
    m_staticText7->Wrap( -1 );
    m_staticText7->SetFont( wxFont( 14, 74, 90, 90, false, wxT("Arial") ) );
    m_staticText7->SetForegroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_HIGHLIGHT ) );
    m_staticText7->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_INFOTEXT ) );

    bSizer20->Add( m_staticText7, 0, wxEXPAND, 5 );

    m_panelModelViewer = new wxPanel( m_panel14, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
    m_panelModelViewer->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_WINDOWFRAME ) );

    bSizer20->Add( m_panelModelViewer, 1, wxEXPAND, 5 );


    m_panel14->SetSizer( bSizer20 );
    m_panel14->Layout();
    bSizer20->Fit( m_panel14 );
    m_splitter_v->SplitVertically( m_panelMaterial, m_panel14, 389 );
    bSizer18->Add( m_splitter_v, 1, wxEXPAND, 5 );


    this->SetSizer( bSizer18 );
    this->Layout();
    wxMenuBar* m_menubar3;
    m_menubar3 = new wxMenuBar( 0 );
    wxMenu* m_menu4;
    m_menu4 = new wxMenu();
    wxMenuItem* m_menuItem3;
    m_menuItem3 = new wxMenuItem( m_menu4, wxID_ANY, wxString( wxT("Open") ) , wxEmptyString, wxITEM_NORMAL );
    m_menu4->Append( m_menuItem3 );

    wxMenuItem* m_menuItem4;
    m_menuItem4 = new wxMenuItem( m_menu4, wxID_ANY, wxString( wxT("Save") ) , wxEmptyString, wxITEM_NORMAL );
    m_menu4->Append( m_menuItem4 );

    wxMenuItem* m_menuItem5;
    m_menuItem5 = new wxMenuItem( m_menu4, wxID_ANY, wxString( wxT("Import") ) , wxEmptyString, wxITEM_NORMAL );
    m_menu4->Append( m_menuItem5 );

    m_menu4->AppendSeparator();

    wxMenuItem* m_menuItem6;
    m_menuItem6 = new wxMenuItem( m_menu4, wxID_ANY, wxString( wxT("Settings") ) , wxEmptyString, wxITEM_NORMAL );
    m_menu4->Append( m_menuItem6 );

    m_menu4->AppendSeparator();

    wxMenuItem* m_menuItem7;
    m_menuItem7 = new wxMenuItem( m_menu4, wxID_ANY, wxString( wxT("Exit") ) , wxEmptyString, wxITEM_NORMAL );
    m_menu4->Append( m_menuItem7 );

    m_menubar3->Append( m_menu4, wxT("File") ); 

    wxMenu* m_menu6;
    m_menu6 = new wxMenu();
    wxMenuItem* m_menuItem11;
    m_menuItem11 = new wxMenuItem( m_menu6, wxID_ANY, wxString( wxT("Toolbar") ) , wxEmptyString, wxITEM_NORMAL );
    m_menu6->Append( m_menuItem11 );

    m_menubar3->Append( m_menu6, wxT("View") ); 

    wxMenu* m_menu7;
    m_menu7 = new wxMenu();
    wxMenuItem* m_menuItem13;
    m_menuItem13 = new wxMenuItem( m_menu7, wxID_ANY, wxString( wxT("About") ) , wxEmptyString, wxITEM_NORMAL );
    m_menu7->Append( m_menuItem13 );

    m_menubar3->Append( m_menu7, wxT("Help") ); 

    this->SetMenuBar( m_menubar3 );

    wxStatusBar* m_statusBar2;
    m_statusBar2 = this->CreateStatusBar( 1, wxST_SIZEGRIP, wxID_ANY );

    this->Centre( wxBOTH );

    // Connect Events
    this->Connect( m_toolOpen->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( fbStaticMeshEditor::OnOpen ) );
    this->Connect( m_toolSave->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( fbStaticMeshEditor::OnSave ) );
    this->Connect( m_toolImport->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( fbStaticMeshEditor::OnImport ) );
    this->Connect( m_toolInfo->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( fbStaticMeshEditor::OnAbout ) );
    this->Connect( m_menuItem3->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( fbStaticMeshEditor::OnOpen ) );
    this->Connect( m_menuItem4->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( fbStaticMeshEditor::OnSave ) );
    this->Connect( m_menuItem5->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( fbStaticMeshEditor::OnImport ) );
    this->Connect( m_menuItem7->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( fbStaticMeshEditor::OnExit ) );
    this->Connect( m_menuItem11->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( fbStaticMeshEditor::OnToolBar ) );
    this->Connect( m_menuItem13->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( fbStaticMeshEditor::OnAbout ) );
    this->Connect( wxEVT_ACTIVATE, wxActivateEventHandler( fbStaticMeshEditor::OnActivate ) );
}

fbStaticMeshEditor::~fbStaticMeshEditor()
{
    // Disconnect Events
    this->Disconnect( m_toolOpen->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( fbStaticMeshEditor::OnOpen ) );
    this->Disconnect( m_toolSave->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( fbStaticMeshEditor::OnSave ) );
    this->Disconnect( m_toolImport->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( fbStaticMeshEditor::OnImport ) );
    this->Disconnect( m_toolInfo->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( fbStaticMeshEditor::OnAbout ) );
    this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( fbStaticMeshEditor::OnOpen ) );
    this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( fbStaticMeshEditor::OnSave ) );
    this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( fbStaticMeshEditor::OnImport ) );
    this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( fbStaticMeshEditor::OnExit ) );
    this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( fbStaticMeshEditor::OnToolBar ) );
    this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( fbStaticMeshEditor::OnAbout ) );
	this->Disconnect( wxEVT_ACTIVATE, wxActivateEventHandler( fbStaticMeshEditor::OnActivate ) );
}

///////////////////////////////////////////////////////////////////////////
StaticMeshEditor::StaticMeshEditor(wxWindow* parent, WERenderCore* pRenderCore)
    : fbStaticMeshEditor(parent)
{
    // init material modifier
    m_pMaterialModifierPanel = new MaterialModifierPanel(m_panelMaterial);
    m_panelMaterial->GetSizer()->Add(m_pMaterialModifierPanel, 1, wxEXPAND);
    m_panelMaterial->GetSizer()->Fit( m_panelMaterial );

    // init model viewer
    m_pRenderCore = pRenderCore;
    m_pModelViewerWindow = new ModelViewerWindow(m_panelModelViewer, pRenderCore);
    wxBoxSizer* bSizer = new wxBoxSizer( wxVERTICAL );
    bSizer->Add(m_pModelViewerWindow, 1, wxEXPAND);
    m_panelModelViewer->SetSizer(bSizer);    
    m_panelModelViewer->Layout();
    bSizer->Fit( m_panelModelViewer ); 
    m_splitter_v->SetSashPosition(m_splitter_v->GetSashPosition()-1);    
    m_splitter_v->SetSashPosition(m_splitter_v->GetSashPosition()+1);
    
    // Timer
    m_Timer.SetOwner( this, wxID_ANY );
    this->Connect( wxID_ANY, wxEVT_TIMER, wxTimerEventHandler( StaticMeshEditor::OnTick ) );
    m_Timer.Start(10);

    m_pMesh = NULL;
    m_pMeshContent = NULL;
}

StaticMeshEditor::~StaticMeshEditor()
{
    m_Timer.Stop();
    Sleep(100);
}

void StaticMeshEditor::OnOpen( wxCommandEvent& event ) 
{ 
    event.Skip();
}
void StaticMeshEditor::OnSave( wxCommandEvent& event )
{ 
    event.Skip();
}
void StaticMeshEditor::OnImport( wxCommandEvent& event )
{
    event.Skip();
}
void StaticMeshEditor::OnAbout( wxCommandEvent& event )
{ 
    event.Skip(); 
}
void StaticMeshEditor::OnExit( wxCommandEvent& event )
{ 
    event.Skip(); 
}
void StaticMeshEditor::OnToolBar( wxCommandEvent& event ) 
{ 
    event.Skip(); 
}
void StaticMeshEditor::OnActivate( wxActivateEvent& event )
{   
    m_pModelViewerWindow->ActiveD3D();    
}

void StaticMeshEditor::Init(WEMeshContent* pMeshContent)
{
    m_pMeshContent = pMeshContent;
    m_pMesh = m_pMeshContent->CreateMesh();

    m_pMaterialModifierPanel->Init(m_pMeshContent->GetMaterialContents(), m_pMesh->GetMaterials());
}

void StaticMeshEditor::OnTick( wxTimerEvent& event )
{    
    if (m_pRenderCore->BeginFrame())
    {
        if (m_pMesh)
            m_pRenderCore->PushMesh(m_pMesh);
        m_pRenderCore->EndFrame();
    }
}