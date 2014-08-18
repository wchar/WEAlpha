#include "MaterialEditor.h"

BEGIN_EVENT_TABLE( MaterialEditor, wxFrame )
    EVT_TIMER(idTimer, MaterialEditor::OnTimer)
    EVT_ACTIVATE(MaterialEditor::OnActivate)
END_EVENT_TABLE()

/*
MaterialEditor::MaterialEditor(wxWindow* parent, wxWindowID id, const wxString& title, WERenderCore* weCore)
    :wxFrame(parent, id, title), m_Timer(this, idTimer)  
{
    SetMinSize(wxSize(600, 400));

    m_pModelViewer = new ModelViewerWindow(this, weCore);
    m_pRenderCore = weCore;

    m_Timer.Start(6);
}
*/

MaterialEditor::MaterialEditor( wxWindow* parent, 
    wxWindowID id, 
    const wxString& title, 
    const wxPoint& pos, 
    const wxSize& size, 
    long style ) 
    : wxFrame( parent, id, title, pos, size, style )
{
    this->SetSizeHints( wxDefaultSize, wxDefaultSize );

    wxBoxSizer* bSizer1;
    bSizer1 = new wxBoxSizer( wxVERTICAL );

    m_splitter = new wxSplitterWindow( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_LIVE_UPDATE );
    m_splitter->Connect( wxEVT_IDLE, wxIdleEventHandler( MaterialEditor::m_splitterOnIdle ), NULL, this );

    m_panelProperties = new wxPanel( m_splitter, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
    m_panelProperties->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_ACTIVECAPTION ) );

    wxBoxSizer* bSizer6;
    bSizer6 = new wxBoxSizer( wxVERTICAL );

    m_staticTextPropertyGridTitle = new wxStaticText( m_panelProperties, wxID_ANY, wxT("Properties"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE );
    m_staticTextPropertyGridTitle->Wrap( -1 );
    m_staticTextPropertyGridTitle->SetFont( wxFont( 14, 74, 90, 90, false, wxT("Arial") ) );
    m_staticTextPropertyGridTitle->SetForegroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_BTNHIGHLIGHT ) );
    m_staticTextPropertyGridTitle->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_WINDOWTEXT ) );

    bSizer6->Add( m_staticTextPropertyGridTitle, 0, wxEXPAND, 5 );

    m_propertyGrid2 = new wxPropertyGrid(m_panelProperties, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxPG_DEFAULT_STYLE);
    m_propertyGridItem2 = m_propertyGrid2->Append( new wxPropertyCategory( wxT("Name1"), wxT("Name1") ) ); 
    m_propertyGridItem3 = m_propertyGrid2->Append( new wxColourProperty( wxT("Name2"), wxT("Name2") ) ); 
    m_propertyGridItem4 = m_propertyGrid2->Append( new wxStringProperty( wxT("Name44"), wxT("Name44") ) ); 
    m_propertyGridItem5 = m_propertyGrid2->Append( new wxStringProperty( wxT("Name4"), wxT("Name4") ) ); 
    bSizer6->Add( m_propertyGrid2, 1, wxEXPAND, 5 );


    m_panelProperties->SetSizer( bSizer6 );
    m_panelProperties->Layout();
    bSizer6->Fit( m_panelProperties );
    m_panelModelViewer = new wxPanel( m_splitter, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
    m_panelModelViewer->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_BTNFACE ) );

    wxBoxSizer* bSizer7;
    bSizer7 = new wxBoxSizer( wxVERTICAL );

    m_staticText4 = new wxStaticText( m_panelModelViewer, wxID_ANY, wxT("Viewer"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE|wxALIGN_LEFT );
    m_staticText4->Wrap( -1 );
    m_staticText4->SetFont( wxFont( 14, 74, 90, 90, false, wxT("Arial") ) );
    m_staticText4->SetForegroundColour( wxColour( 255, 255, 255 ) );
    m_staticText4->SetBackgroundColour( wxColour( 0, 0, 0 ) );

    bSizer7->Add( m_staticText4, 0, wxEXPAND, 5 );

    m_panel6 = new wxPanel( m_panelModelViewer, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
    m_panel6->SetBackgroundColour( wxColour( 145, 77, 138 ) );

    bSizer7->Add( m_panel6, 1, wxEXPAND, 5 );


    m_panelModelViewer->SetSizer( bSizer7 );
    m_panelModelViewer->Layout();
    bSizer7->Fit( m_panelModelViewer );
    m_splitter->SplitVertically( m_panelProperties, m_panelModelViewer, 284 );
    bSizer1->Add( m_splitter, 1, wxEXPAND, 5 );


    this->SetSizer( bSizer1 );
    this->Layout();

    this->Centre( wxBOTH );
}

MaterialEditor::~MaterialEditor()
{

}

void MaterialEditor::Create(WERenderCore* weCore)
{
    m_pModelViewer = new ModelViewerWindow(m_panel6, weCore);
    m_pRenderCore = weCore;
}

void MaterialEditor::OnActivate(wxActivateEvent& event)
{
    m_pModelViewer->ActiveD3D();
}

void MaterialEditor::OnTimer(wxTimerEvent& event)
{
    if (m_pRenderCore->BeginFrame())
    {  
        m_pRenderCore->EndFrame();
    }
}