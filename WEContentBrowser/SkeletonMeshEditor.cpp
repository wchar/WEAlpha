#include "SkeletonMeshEditor.h"

///////////////////////////////////////////////////////////////////////////
fbSkeletonMeshEditor::fbSkeletonMeshEditor( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
    this->SetSizeHints( wxDefaultSize, wxDefaultSize );
    this->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_ACTIVECAPTION ) );

    wxBoxSizer* bSizer18;
    bSizer18 = new wxBoxSizer( wxVERTICAL );

    m_panelToolBar = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
    wxBoxSizer* bSizer24;
    bSizer24 = new wxBoxSizer( wxHORIZONTAL );

    wxToolBar* m_toolBar4;
    m_toolBar4 = new wxToolBar( m_panelToolBar, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_HORIZONTAL ); 
    m_toolBar4->SetBackgroundColour( wxColour( 90, 125, 160 ) );


    m_toolOpen = m_toolBar4->AddTool( wxID_ANY, wxT("tool"), wxBitmap( wxT("../../wxFormBuilder/res/Get Document.png"), wxBITMAP_TYPE_ANY ), wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString, NULL ); 


    m_toolSave = m_toolBar4->AddTool( wxID_ANY, wxT("tool"), wxBitmap( wxT("../../wxFormBuilder/res/Send Document.png"), wxBITMAP_TYPE_ANY ), wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString, NULL ); 


    m_toolImport = m_toolBar4->AddTool( wxID_ANY, wxT("tool"), wxBitmap( wxT("../../wxFormBuilder/res/Import Document.png"), wxBITMAP_TYPE_ANY ), wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString, NULL ); 

    m_toolBar4->Realize(); 

    bSizer24->Add( m_toolBar4, 0, wxEXPAND, 5 );

    wxToolBar* m_toolBar8;
    m_toolBar8 = new wxToolBar( m_panelToolBar, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_HORIZONTAL ); 
    m_toolBar8->SetBackgroundColour( wxColour( 72, 100, 128 ) );

    m_toolPlay = m_toolBar8->AddTool( wxID_ANY, wxT("tool"), wxBitmap( wxT("../../wxFormBuilder/res/Play.png"), wxBITMAP_TYPE_ANY ), wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString, NULL ); 

    m_toolStop = m_toolBar8->AddTool( wxID_ANY, wxT("tool"), wxBitmap( wxT("../../wxFormBuilder/res/Stop.png"), wxBITMAP_TYPE_ANY ), wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString, NULL ); 

    m_toolPause = m_toolBar8->AddTool( wxID_ANY, wxT("tool"), wxBitmap( wxT("../../wxFormBuilder/res/Pause.png"), wxBITMAP_TYPE_ANY ), wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString, NULL ); 

    m_toolBar8->Realize(); 

    bSizer24->Add( m_toolBar8, 0, wxEXPAND, 5 );

    wxToolBar* m_toolBar9;
    m_toolBar9 = new wxToolBar( m_panelToolBar, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_HORIZONTAL ); 
    m_toolBar9->SetBackgroundColour( wxColour( 96, 131, 166 ) );

    m_toolAdd = m_toolBar9->AddTool( wxID_ANY, wxT("tool"), wxBitmap( wxT("../../wxFormBuilder/res/Add.png"), wxBITMAP_TYPE_ANY ), wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString, NULL ); 

    m_tooRemove = m_toolBar9->AddTool( wxID_ANY, wxT("tool"), wxBitmap( wxT("../../wxFormBuilder/res/Remove.png"), wxBITMAP_TYPE_ANY ), wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString, NULL ); 

    m_toolBar9->Realize(); 

    bSizer24->Add( m_toolBar9, 0, wxEXPAND, 5 );

    wxToolBar* m_toolBar7;
    m_toolBar7 = new wxToolBar( m_panelToolBar, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_HORIZONTAL ); 
    m_toolBar7->SetBackgroundColour( wxColour( 79, 108, 138 ) );

    m_toolSetting = m_toolBar7->AddTool( wxID_ANY, wxT("tool"), wxBitmap( wxT("../../wxFormBuilder/res/Gear Alt.png"), wxBITMAP_TYPE_ANY ), wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString, NULL ); 

    m_toolInfo = m_toolBar7->AddTool( wxID_ANY, wxT("tool"), wxBitmap( wxT("../../wxFormBuilder/res/Get Info Blue Button.png"), wxBITMAP_TYPE_ANY ), wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString, NULL ); 

    m_toolBar7->Realize(); 

    bSizer24->Add( m_toolBar7, 1, wxEXPAND, 5 );


    m_panelToolBar->SetSizer( bSizer24 );
    m_panelToolBar->Layout();
    bSizer24->Fit( m_panelToolBar );
    bSizer18->Add( m_panelToolBar, 0, wxEXPAND, 5 );

    m_splitter_v = new wxSplitterWindow( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_3D|wxSP_LIVE_UPDATE );
    m_splitter_v->Connect( wxEVT_IDLE, wxIdleEventHandler( fbSkeletonMeshEditor::m_splitter_vOnIdle ), NULL, this );

    wxPanel* m_panel13;
    m_panel13 = new wxPanel( m_splitter_v, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
    wxBoxSizer* bSizer19;
    bSizer19 = new wxBoxSizer( wxVERTICAL );

    m_notebook = new wxNotebook( m_panel13, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNB_FIXEDWIDTH );
    m_panelAnimation = new wxPanel( m_notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
    wxBoxSizer* bSizer21;
    bSizer21 = new wxBoxSizer( wxVERTICAL );

    wxStaticText* m_staticText6;
    m_staticText6 = new wxStaticText( m_panelAnimation, wxID_ANY, wxT("AnimationSet"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE );
    m_staticText6->Wrap( -1 );
    m_staticText6->SetFont( wxFont( 14, 74, 90, 90, false, wxT("Arial") ) );
    m_staticText6->SetForegroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_HIGHLIGHT ) );
    m_staticText6->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_INACTIVECAPTIONTEXT ) );

    bSizer21->Add( m_staticText6, 0, wxEXPAND, 5 );

    wxBoxSizer* bSizer211;
    bSizer211 = new wxBoxSizer( wxHORIZONTAL );

    wxStaticText* m_staticText8;
    m_staticText8 = new wxStaticText( m_panelAnimation, wxID_ANY, wxT("StartFrame"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE );
    m_staticText8->Wrap( -1 );
    m_staticText8->SetFont( wxFont( 12, 74, 90, 90, false, wxT("Arial") ) );
    m_staticText8->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_INACTIVECAPTION ) );

    bSizer211->Add( m_staticText8, 1, 0, 5 );

    wxStaticText* m_staticText81;
    m_staticText81 = new wxStaticText( m_panelAnimation, wxID_ANY, wxT("EndFrame"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE );
    m_staticText81->Wrap( -1 );
    m_staticText81->SetFont( wxFont( 12, 74, 90, 90, false, wxT("Arial") ) );
    m_staticText81->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_INACTIVECAPTION ) );

    bSizer211->Add( m_staticText81, 1, 0, 5 );

    wxStaticText* m_staticText811;
    m_staticText811 = new wxStaticText( m_panelAnimation, wxID_ANY, wxT("AnimationRate"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE );
    m_staticText811->Wrap( -1 );
    m_staticText811->SetFont( wxFont( 12, 74, 90, 90, false, wxT("Arial") ) );
    m_staticText811->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_INACTIVECAPTION ) );

    bSizer211->Add( m_staticText811, 1, 0, 5 );


    bSizer21->Add( bSizer211, 0, wxEXPAND, 5 );

    m_splitter_h = new wxSplitterWindow( m_panelAnimation, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_3D|wxSP_LIVE_UPDATE );
    m_splitter_h->Connect( wxEVT_IDLE, wxIdleEventHandler( fbSkeletonMeshEditor::m_splitter_hOnIdle ), NULL, this );

    m_panelAnimationSet = new wxScrolledWindow( m_splitter_h, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHSCROLL|wxVSCROLL );
    m_panelAnimationSet->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_ACTIVECAPTION ) );

    wxBoxSizer* bSizer201;
    bSizer201 = new wxBoxSizer( wxVERTICAL );


    m_panelAnimationSet->SetSizer( bSizer201 );
    m_panelAnimationSet->Layout();
    bSizer201->Fit( m_panelAnimationSet );
    wxPanel* m_panel171;
    m_panel171 = new wxPanel( m_splitter_h, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
    wxBoxSizer* bSizer23;
    bSizer23 = new wxBoxSizer( wxVERTICAL );

    wxStaticText* m_staticText61;
    m_staticText61 = new wxStaticText( m_panel171, wxID_ANY, wxT("AnimationProperties"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE );
    m_staticText61->Wrap( -1 );
    m_staticText61->SetFont( wxFont( 14, 74, 90, 90, false, wxT("Arial") ) );
    m_staticText61->SetForegroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_HIGHLIGHT ) );
    m_staticText61->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_INACTIVECAPTIONTEXT ) );

    bSizer23->Add( m_staticText61, 0, wxEXPAND, 5 );

    m_propertyGridAnimation = new wxPropertyGrid(m_panel171, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxPG_HIDE_MARGIN|wxPG_SPLITTER_AUTO_CENTER);
    m_propertyGridItemStart = m_propertyGridAnimation->Append( new wxIntProperty( wxT("StartFrame"), wxT("StartFrame") ) ); 
    m_propertyGridItemEnd = m_propertyGridAnimation->Append( new wxIntProperty( wxT("EndFrame"), wxT("EndFrame") ) ); 
    m_propertyGridItemRate = m_propertyGridAnimation->Append( new wxFloatProperty( wxT("Rate"), wxT("Rate") ) ); 
    bSizer23->Add( m_propertyGridAnimation, 1, wxEXPAND, 5 );


    m_panel171->SetSizer( bSizer23 );
    m_panel171->Layout();
    bSizer23->Fit( m_panel171 );
    m_splitter_h->SplitHorizontally( m_panelAnimationSet, m_panel171, 100 );
    bSizer21->Add( m_splitter_h, 1, wxEXPAND, 5 );


    m_panelAnimation->SetSizer( bSizer21 );
    m_panelAnimation->Layout();
    bSizer21->Fit( m_panelAnimation );
    m_notebook->AddPage( m_panelAnimation, wxT("Animation"), false );
    m_panelMaterial = new wxPanel( m_notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
    wxBoxSizer* bSizer64;
    bSizer64 = new wxBoxSizer( wxVERTICAL );


    m_panelMaterial->SetSizer( bSizer64 );
    m_panelMaterial->Layout();
    bSizer64->Fit( m_panelMaterial );
    m_notebook->AddPage( m_panelMaterial, wxT("Material"), true );

    bSizer19->Add( m_notebook, 1, wxEXPAND|wxTOP, 5 );


    m_panel13->SetSizer( bSizer19 );
    m_panel13->Layout();
    bSizer19->Fit( m_panel13 );
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

    m_sliderFrame = new wxSlider( m_panel14, wxID_ANY, 50, 0, 100, wxDefaultPosition, wxDefaultSize, wxSL_BOTH|wxSL_HORIZONTAL );
    bSizer20->Add( m_sliderFrame, 0, wxEXPAND, 5 );


    m_panel14->SetSizer( bSizer20 );
    m_panel14->Layout();
    bSizer20->Fit( m_panel14 );
    m_splitter_v->SplitVertically( m_panel13, m_panel14, 389 );
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

    wxMenu* m_menu5;
    m_menu5 = new wxMenu();
    wxMenuItem* m_menuItem8;
    m_menuItem8 = new wxMenuItem( m_menu5, wxID_ANY, wxString( wxT("Play") ) , wxEmptyString, wxITEM_NORMAL );
    m_menu5->Append( m_menuItem8 );

    wxMenuItem* m_menuItem9;
    m_menuItem9 = new wxMenuItem( m_menu5, wxID_ANY, wxString( wxT("Stop") ) , wxEmptyString, wxITEM_NORMAL );
    m_menu5->Append( m_menuItem9 );

    wxMenuItem* m_menuItem10;
    m_menuItem10 = new wxMenuItem( m_menu5, wxID_ANY, wxString( wxT("Pause") ) , wxEmptyString, wxITEM_NORMAL );
    m_menu5->Append( m_menuItem10 );

    m_menubar3->Append( m_menu5, wxT("Edit") ); 

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
    this->Connect( m_toolOpen->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( fbSkeletonMeshEditor::OnOpen ) );
    this->Connect( m_toolSave->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( fbSkeletonMeshEditor::OnSave ) );
    this->Connect( m_toolImport->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( fbSkeletonMeshEditor::OnImport ) );
    this->Connect( m_toolPlay->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( fbSkeletonMeshEditor::OnPlay ) );
    this->Connect( m_toolStop->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( fbSkeletonMeshEditor::OnStop ) );
    this->Connect( m_toolPause->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( fbSkeletonMeshEditor::OnPause ) );
    this->Connect( m_toolAdd->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( fbSkeletonMeshEditor::OnAdd ) );
    this->Connect( m_tooRemove->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( fbSkeletonMeshEditor::OnRemove ) );
    this->Connect( m_toolInfo->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( fbSkeletonMeshEditor::OnAbout ) );
    this->Connect( m_menuItem3->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( fbSkeletonMeshEditor::OnOpen ) );
    this->Connect( m_menuItem4->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( fbSkeletonMeshEditor::OnSave ) );
    this->Connect( m_menuItem5->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( fbSkeletonMeshEditor::OnImport ) );
    this->Connect( m_menuItem7->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( fbSkeletonMeshEditor::OnExit ) );
    this->Connect( m_menuItem8->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( fbSkeletonMeshEditor::OnPlay ) );
    this->Connect( m_menuItem9->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( fbSkeletonMeshEditor::OnStop ) );
    this->Connect( m_menuItem10->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( fbSkeletonMeshEditor::OnPause ) );
    this->Connect( m_menuItem11->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( fbSkeletonMeshEditor::OnToolBar ) );
    this->Connect( m_menuItem13->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( fbSkeletonMeshEditor::OnAbout ) );
}

fbSkeletonMeshEditor::~fbSkeletonMeshEditor()
{
    // Disconnect Events
    this->Disconnect( m_toolOpen->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( fbSkeletonMeshEditor::OnOpen ) );
    this->Disconnect( m_toolSave->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( fbSkeletonMeshEditor::OnSave ) );
    this->Disconnect( m_toolImport->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( fbSkeletonMeshEditor::OnImport ) );
    this->Disconnect( m_toolPlay->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( fbSkeletonMeshEditor::OnPlay ) );
    this->Disconnect( m_toolStop->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( fbSkeletonMeshEditor::OnStop ) );
    this->Disconnect( m_toolPause->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( fbSkeletonMeshEditor::OnPause ) );
    this->Disconnect( m_toolAdd->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( fbSkeletonMeshEditor::OnAdd ) );
    this->Disconnect( m_tooRemove->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( fbSkeletonMeshEditor::OnRemove ) );
    this->Disconnect( m_toolInfo->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( fbSkeletonMeshEditor::OnAbout ) );
    this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( fbSkeletonMeshEditor::OnOpen ) );
    this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( fbSkeletonMeshEditor::OnSave ) );
    this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( fbSkeletonMeshEditor::OnImport ) );
    this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( fbSkeletonMeshEditor::OnExit ) );
    this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( fbSkeletonMeshEditor::OnPlay ) );
    this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( fbSkeletonMeshEditor::OnStop ) );
    this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( fbSkeletonMeshEditor::OnPause ) );
    this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( fbSkeletonMeshEditor::OnToolBar ) );
    this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( fbSkeletonMeshEditor::OnAbout ) );

}

///////////////////////////////////////////////////////////////////////////////
/// Class AnimationSetItem
///////////////////////////////////////////////////////////////////////////////
class AnimationSetItem : public wxPanel 
{
private:

protected:
    wxStaticText* m_staticText1;
    wxStaticText* m_staticText2;
    wxStaticText* m_staticText3;

    // Virtual event handlers, overide them in your derived class
    virtual void OnSelected( wxMouseEvent& event );

    int m_Index;
    SkeletonMeshEditor* m_pSkeletonMeshEditor;
public:
    int m_iStartFrame;
    int m_iEndFrame;
    float m_fRate;

    AnimationSetItem( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxTAB_TRAVERSAL ); 
    ~AnimationSetItem();
        
    void FocusThis(bool bFocus);
    
    void Init(int start, int end, float rate,int idx, SkeletonMeshEditor* pEditor);
};


AnimationSetItem::AnimationSetItem( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
    this->SetFont( wxFont( 14, 74, 90, 90, false, wxT("Arial") ) );
    this->SetBackgroundColour( wxColour( 115, 185, 255 ) );

    wxBoxSizer* bSizer38;
    bSizer38 = new wxBoxSizer( wxHORIZONTAL );

    m_staticText1 = new wxStaticText( this, wxID_ANY, wxT("start"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE );
    m_staticText1->Wrap( -1 );
    m_staticText1->SetFont( wxFont( 12, 74, 90, 90, false, wxT("Arial") ) );

    bSizer38->Add( m_staticText1, 1, wxALL, 5 );

    m_staticText2 = new wxStaticText( this, wxID_ANY, wxT("end"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE );
    m_staticText2->Wrap( -1 );
    m_staticText2->SetFont( wxFont( 12, 74, 90, 90, false, wxT("Arial") ) );

    bSizer38->Add( m_staticText2, 1, wxALL, 5 );

    m_staticText3 = new wxStaticText( this, wxID_ANY, wxT("rate"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE );
    m_staticText3->Wrap( -1 );
    m_staticText3->SetFont( wxFont( 12, 74, 90, 90, false, wxT("Arial") ) );

    bSizer38->Add( m_staticText3, 1, wxALL, 5 );


    this->SetSizer( bSizer38 );
    this->Layout();

    // Connect Events
    this->Connect( wxEVT_LEFT_DOWN, wxMouseEventHandler( AnimationSetItem::OnSelected ) );
    m_staticText1->Connect( wxEVT_LEFT_DOWN, wxMouseEventHandler( AnimationSetItem::OnSelected ), NULL, this );
    m_staticText2->Connect( wxEVT_LEFT_DOWN, wxMouseEventHandler( AnimationSetItem::OnSelected ), NULL, this );
    m_staticText3->Connect( wxEVT_LEFT_DOWN, wxMouseEventHandler( AnimationSetItem::OnSelected ), NULL, this );
}

AnimationSetItem::~AnimationSetItem()
{
    // Disconnect Events
    this->Disconnect( wxEVT_LEFT_DOWN, wxMouseEventHandler( AnimationSetItem::OnSelected ) );
    m_staticText1->Disconnect( wxEVT_LEFT_DOWN, wxMouseEventHandler( AnimationSetItem::OnSelected ), NULL, this );
    m_staticText2->Disconnect( wxEVT_LEFT_DOWN, wxMouseEventHandler( AnimationSetItem::OnSelected ), NULL, this );
    m_staticText3->Disconnect( wxEVT_LEFT_DOWN, wxMouseEventHandler( AnimationSetItem::OnSelected ), NULL, this );

}

void AnimationSetItem::Init(int start, int end, float rate,int idx, SkeletonMeshEditor* pEditor)
{
    m_staticText1->SetLabel(wxLongLong(start).ToString());
    m_staticText2->SetLabel(wxLongLong(end).ToString());
    m_staticText3->SetLabel(wxString::FromDouble(rate));
    m_Index = idx;
    m_pSkeletonMeshEditor = pEditor;    
    
    m_pSkeletonMeshEditor->FocusAnimation(m_Index);
    FocusThis(true);
}

void AnimationSetItem::OnSelected( wxMouseEvent& event ) 
{ 
    m_pSkeletonMeshEditor->FocusAnimation(m_Index);
    FocusThis(true);
}

void AnimationSetItem::FocusThis(bool bFocus)
{
    if (bFocus)
        SetBackgroundColour(wxSystemSettings::GetColour( wxSYS_COLOUR_HIGHLIGHT ));
    else
        SetBackgroundColour(wxColour( 142, 199, 255 ));

    Refresh(true);
}

#include "MaterialModifierPanel.h"
///////////////////////////////////////////////////////////////////////////

SkeletonMeshEditor::SkeletonMeshEditor(wxWindow* parent)
    :fbSkeletonMeshEditor(parent)
{
    m_iAnimationCount = 0;
    m_panelAnimationSet->SetScrollRate(5, 5);
    MaterialModifierPanel* pMaterialPanel = new MaterialModifierPanel(m_panelMaterial);
    m_panelMaterial->GetSizer()->Add(pMaterialPanel, 1, wxEXPAND);
    m_panelMaterial->GetSizer()->Fit( m_panelMaterial );


    InsertAnimation();
    InsertAnimation();
    InsertAnimation();

}

void SkeletonMeshEditor::InsertAnimation()
{
    AnimationSetItem* pItem = new AnimationSetItem(m_panelAnimationSet);
    pItem->Init(1,1,1.0f, m_iAnimationCount++, this);
    wxSizer* pSizer = m_panelAnimationSet->GetSizer();
    pSizer->Add(pItem, 0, wxEXPAND, 5 );    

    // Update windows
    m_splitter_h->SetSashPosition(m_splitter_h->GetSashPosition()-1);    
    m_splitter_h->SetSashPosition(m_splitter_h->GetSashPosition()+1);
}

void SkeletonMeshEditor::FocusAnimation(int idx)
{
    wxWindowList list = m_panelAnimationSet->GetChildren();
    for (wxWindowList::iterator it = list.begin(); it != list.end(); it++)
    {
        AnimationSetItem* pItem = dynamic_cast<AnimationSetItem*>(*it);
        if(pItem)
        {
            pItem->FocusThis(false);
        }
    }

    m_iCurrentAnimIndex = idx;
}

void SkeletonMeshEditor::OnOpen( wxCommandEvent& event ) 
{ 
    event.Skip(); 
}
void SkeletonMeshEditor::OnSave( wxCommandEvent& event )
{ 
    event.Skip(); 
}
void SkeletonMeshEditor::OnImport( wxCommandEvent& event ) 
{ 
    event.Skip(); 
}
void SkeletonMeshEditor::OnPlay( wxCommandEvent& event ) 
{ 
    event.Skip(); 
}
void SkeletonMeshEditor::OnStop( wxCommandEvent& event ) 
{ 
    event.Skip();
}
void SkeletonMeshEditor::OnPause( wxCommandEvent& event )
{ 
    event.Skip(); 
}
void SkeletonMeshEditor::OnAdd( wxCommandEvent& event )
{     
    if(m_notebook->GetSelection() == 0)
    {
        InsertAnimation();
    }
}
void SkeletonMeshEditor::OnRemove( wxCommandEvent& event ) 
{ 
    if(m_notebook->GetSelection() == 0)
    {
        int i = 0;
        wxWindowList list = m_panelAnimationSet->GetChildren();
        for (wxWindowList::iterator it = list.begin(); it != list.end(); it++)
        {
            AnimationSetItem* pItem = dynamic_cast<AnimationSetItem*>(*it);
            if(pItem)
            {
                if (i == m_iCurrentAnimIndex)
                {
                    pItem->Hide();    
                    
                    // Update windows
                    m_splitter_h->SetSashPosition(m_splitter_h->GetSashPosition()-1);    
                    m_splitter_h->SetSashPosition(m_splitter_h->GetSashPosition()+1);
                    break;
                }
                i++;
            }
        }
    }
}
void SkeletonMeshEditor::OnAbout( wxCommandEvent& event ) 
{ 
    event.Skip(); 
}
void SkeletonMeshEditor::OnExit( wxCommandEvent& event )
{ 
    event.Skip(); 
}
void SkeletonMeshEditor::OnToolBar( wxCommandEvent& event ) 
{ 
    event.Skip(); 
}