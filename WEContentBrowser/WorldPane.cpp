#include "WorldPane.h"

///////////////////////////////////////////////////////////////////////////

fbWorldPane::fbWorldPane( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
    this->SetSizeHints( wxDefaultSize, wxDefaultSize );
    this->SetFont( wxFont( 10, 74, 90, 92, false, wxT("Arial") ) );
    this->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_WINDOW ) );

    wxBoxSizer* bSizer77;
    bSizer77 = new wxBoxSizer( wxVERTICAL );

    m_propertyGrid5 = new wxPropertyGrid(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxPG_DEFAULT_STYLE|wxPG_SPLITTER_AUTO_CENTER);
    wxPGProperty* m_propertyGridItem23;
    m_propertyGridItem23 = m_propertyGrid5->Append( new wxPropertyCategory( wxT("World Size"), wxT("World Size") ) ); 
    m_pgiWSX = m_propertyGrid5->Append( new wxFloatProperty( wxT("X"), wxT("X") ) ); 
    m_pgiWSY = m_propertyGrid5->Append( new wxFloatProperty( wxT("Y"), wxT("Y") ) ); 
    m_pgiWSZ = m_propertyGrid5->Append( new wxFloatProperty( wxT("Z"), wxT("Z") ) ); 
    wxPGProperty* m_propertyGridItem26;
    m_propertyGridItem26 = m_propertyGrid5->Append( new wxPropertyCategory( wxT("Light Camera Position"), wxT("Light Camera Position") ) ); 
    m_pgiLCPX = m_propertyGrid5->Append( new wxFloatProperty( wxT("X"), wxT("X") ) ); 
    m_pgiLCPY = m_propertyGrid5->Append( new wxFloatProperty( wxT("Y"), wxT("Y") ) ); 
    m_pgiLCPZ = m_propertyGrid5->Append( new wxFloatProperty( wxT("Z"), wxT("Z") ) ); 
    wxPGProperty* m_propertyGridItem30;
    m_propertyGridItem30 = m_propertyGrid5->Append( new wxPropertyCategory( wxT("Light Direction"), wxT("Light Direction") ) ); 
    m_pgiLDX = m_propertyGrid5->Append( new wxFloatProperty( wxT("X"), wxT("X") ) ); 
    m_pgiLDY = m_propertyGrid5->Append( new wxFloatProperty( wxT("Y"), wxT("Y") ) ); 
    m_pgiLDZ = m_propertyGrid5->Append( new wxFloatProperty( wxT("Z"), wxT("Z") ) ); 
    bSizer77->Add( m_propertyGrid5, 1, wxALL|wxEXPAND, 5 );


    this->SetSizer( bSizer77 );
    this->Layout();

    this->Centre( wxBOTH );

    // Connect Events
    m_propertyGrid5->Connect( wxEVT_PG_CHANGED, wxPropertyGridEventHandler( fbWorldPane::OnChanged ), NULL, this );
}

fbWorldPane::~fbWorldPane()
{
    // Disconnect Events
    m_propertyGrid5->Disconnect( wxEVT_PG_CHANGED, wxPropertyGridEventHandler( fbWorldPane::OnChanged ), NULL, this );

}



///////////////////////////////////////////////////////////////////////////

WorldPane::WorldPane(wxWindow* parent)
    : fbWorldPane(parent)
{

}


WorldPane::~WorldPane()
{

}

void WorldPane::OnChanged( wxPropertyGridEvent& event )
{

}