#include "MainFrame.h"
#include "SkeletonMeshEditor.h"
#include "StaticMeshEditor.h"

///////////////////////////////////////////////////////////////////////////

fbMainFrame::fbMainFrame( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
    this->SetSizeHints( wxDefaultSize, wxDefaultSize );
    this->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_BTNHIGHLIGHT ) );

    wxMenuBar* m_menubar5;
    m_menubar5 = new wxMenuBar( 0 );
    wxMenu* m_menu9;
    m_menu9 = new wxMenu();
    wxMenuItem* m_menuItem14;
    m_menuItem14 = new wxMenuItem( m_menu9, wxID_ANY, wxString( wxT("ExitAll") ) , wxEmptyString, wxITEM_NORMAL );
    m_menu9->Append( m_menuItem14 );

    m_menubar5->Append( m_menu9, wxT("File") ); 

    wxMenu* m_menu10;
    m_menu10 = new wxMenu();
    wxMenuItem* m_menuItem15;
    m_menuItem15 = new wxMenuItem( m_menu10, wxID_ANY, wxString( wxT("MeshContent") ) , wxEmptyString, wxITEM_NORMAL );
    m_menu10->Append( m_menuItem15 );

    wxMenuItem* m_menuItem16;
    m_menuItem16 = new wxMenuItem( m_menu10, wxID_ANY, wxString( wxT("SkeletonMeshContent") ) , wxEmptyString, wxITEM_NORMAL );
    m_menu10->Append( m_menuItem16 );

    wxMenuItem* m_menuItem17;
    m_menuItem17 = new wxMenuItem( m_menu10, wxID_ANY, wxString( wxT("ParticleContent") ) , wxEmptyString, wxITEM_NORMAL );
    m_menu10->Append( m_menuItem17 );

    wxMenuItem* m_menuItem18;
    m_menuItem18 = new wxMenuItem( m_menu10, wxID_ANY, wxString( wxT("BeamContent") ) , wxEmptyString, wxITEM_NORMAL );
    m_menu10->Append( m_menuItem18 );

    wxMenuItem* m_menuItem19;
    m_menuItem19 = new wxMenuItem( m_menu10, wxID_ANY, wxString( wxT("AnimTrailContent") ) , wxEmptyString, wxITEM_NORMAL );
    m_menu10->Append( m_menuItem19 );

    m_menubar5->Append( m_menu10, wxT("Tools") ); 

    wxMenu* m_menu11;
    m_menu11 = new wxMenu();
    wxMenuItem* m_menuItem20;
    m_menuItem20 = new wxMenuItem( m_menu11, wxID_ANY, wxString( wxT("About") ) , wxEmptyString, wxITEM_NORMAL );
    m_menu11->Append( m_menuItem20 );

    m_menubar5->Append( m_menu11, wxT("Help") ); 

    this->SetMenuBar( m_menubar5 );

    wxBoxSizer* bSizer26;
    bSizer26 = new wxBoxSizer( wxVERTICAL );

    wxStaticBitmap* m_bitmap1;
    m_bitmap1 = new wxStaticBitmap( this, wxID_ANY, wxBitmap( wxT("res/windmill.jpg"), wxBITMAP_TYPE_ANY ), wxDefaultPosition, wxDefaultSize, 0 );
    bSizer26->Add( m_bitmap1, 0, 0, 5 );

    m_panelD3DInit = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
    m_panelD3DInit->Hide();

    bSizer26->Add( m_panelD3DInit, 1, wxEXPAND, 5 );

    m_staticTextInfo = new wxStaticText( this, wxID_ANY, wxT("windmill alpha"), wxDefaultPosition, wxDefaultSize, 0 );
    m_staticTextInfo->Wrap( -1 );
    m_staticTextInfo->SetFont( wxFont( 12, 74, 90, 90, false, wxT("Arial") ) );

    bSizer26->Add( m_staticTextInfo, 0, wxALL|wxEXPAND, 5 );


    this->SetSizer( bSizer26 );
    this->Layout();

    this->Centre( wxBOTH );

    // Connect Events
    this->Connect( m_menuItem14->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( fbMainFrame::OnExitAll ) );
    this->Connect( m_menuItem15->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( fbMainFrame::OnMeshContent ) );
    this->Connect( m_menuItem16->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( fbMainFrame::OnSkeletonMeshContent ) );
    this->Connect( m_menuItem17->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( fbMainFrame::OnParticleContent ) );
    this->Connect( m_menuItem18->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( fbMainFrame::OnBeamContent ) );
    this->Connect( m_menuItem19->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( fbMainFrame::OnAnimTrailContent ) );
}

fbMainFrame::~fbMainFrame()
{
    // Disconnect Events
    this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( fbMainFrame::OnExitAll ) );
    this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( fbMainFrame::OnMeshContent ) );
    this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( fbMainFrame::OnSkeletonMeshContent ) );
    this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( fbMainFrame::OnParticleContent ) );
    this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( fbMainFrame::OnBeamContent ) );
    this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( fbMainFrame::OnAnimTrailContent ) );

}




//-------------------------------------------------------------

MainFrame::MainFrame(wxWindow* parent)
    :fbMainFrame(parent)
{

}

void MainFrame::OnExitAll( wxCommandEvent& event ) 
{ 
    event.Skip(); 
}
void MainFrame::OnMeshContent( wxCommandEvent& event )
{ 
    StaticMeshEditor* frame = new StaticMeshEditor(this);
    frame->Show(true);
}
void MainFrame::OnSkeletonMeshContent( wxCommandEvent& event ) 
{
    SkeletonMeshEditor* frame = new SkeletonMeshEditor(this);
    frame->Show(true);
}
void MainFrame::OnParticleContent( wxCommandEvent& event )
{ 
    event.Skip();
}
void MainFrame::OnBeamContent( wxCommandEvent& event )
{ 
    event.Skip(); 
}
void MainFrame::OnAnimTrailContent( wxCommandEvent& event ) 
{
    event.Skip(); 
}