#include "PostProcessPane.h"


///////////////////////////////////////////////////////////////////////////

fbPostProcessPane::fbPostProcessPane( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
    this->SetSizeHints( wxDefaultSize, wxDefaultSize );
    this->SetFont( wxFont( 10, 74, 90, 90, false, wxT("Arial") ) );
    this->SetForegroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_BACKGROUND ) );
    this->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_WINDOW ) );

    wxBoxSizer* bSizer78;
    bSizer78 = new wxBoxSizer( wxVERTICAL );

    wxStaticText* m_staticText62;
    m_staticText62 = new wxStaticText( this, wxID_ANY, wxT("MyLabel"), wxDefaultPosition, wxDefaultSize, 0 );
    m_staticText62->Wrap( -1 );
    m_staticText62->Hide();

    bSizer78->Add( m_staticText62, 0, wxALL, 5 );


    bSizer78->Add( 430, 0, 0, 0, 5 );

    wxStaticBoxSizer* sbSizer6;
    sbSizer6 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("HDAO Options") ), wxVERTICAL );

    m_checkBoxHDAO = new wxCheckBox( this, wxID_ANY, wxT("Enable HDAO"), wxDefaultPosition, wxDefaultSize, 0 );
    m_checkBoxHDAO->SetValue(true); 
    sbSizer6->Add( m_checkBoxHDAO, 0, wxALL, 5 );

    m_checkBoxUseNormal = new wxCheckBox( this, wxID_ANY, wxT("Use Normal"), wxDefaultPosition, wxDefaultSize, 0 );
    m_checkBoxUseNormal->SetValue(true); 
    sbSizer6->Add( m_checkBoxUseNormal, 0, wxALL, 5 );

    wxBoxSizer* bSizer86;
    bSizer86 = new wxBoxSizer( wxHORIZONTAL );

    wxBoxSizer* bSizer79;
    bSizer79 = new wxBoxSizer( wxVERTICAL );

    wxStaticText* m_staticText63;
    m_staticText63 = new wxStaticText( this, wxID_ANY, wxT("Reject Radius :"), wxDefaultPosition, wxDefaultSize, 0 );
    m_staticText63->Wrap( -1 );
    bSizer79->Add( m_staticText63, 0, wxBOTTOM|wxRIGHT|wxLEFT, 8 );

    wxStaticText* m_staticText64;
    m_staticText64 = new wxStaticText( this, wxID_ANY, wxT("Accept Radius :"), wxDefaultPosition, wxDefaultSize, 0 );
    m_staticText64->Wrap( -1 );
    bSizer79->Add( m_staticText64, 0, wxBOTTOM|wxRIGHT|wxLEFT, 8 );

    wxStaticText* m_staticText65;
    m_staticText65 = new wxStaticText( this, wxID_ANY, wxT("Intensity :"), wxDefaultPosition, wxDefaultSize, 0 );
    m_staticText65->Wrap( -1 );
    bSizer79->Add( m_staticText65, 0, wxBOTTOM|wxRIGHT|wxLEFT, 8 );

    wxStaticText* m_staticText66;
    m_staticText66 = new wxStaticText( this, wxID_ANY, wxT("Normal Scale :"), wxDefaultPosition, wxDefaultSize, 0 );
    m_staticText66->Wrap( -1 );
    bSizer79->Add( m_staticText66, 0, wxBOTTOM|wxRIGHT|wxLEFT, 8 );

    wxStaticText* m_staticText67;
    m_staticText67 = new wxStaticText( this, wxID_ANY, wxT("Accept Angle :"), wxDefaultPosition, wxDefaultSize, 0 );
    m_staticText67->Wrap( -1 );
    bSizer79->Add( m_staticText67, 0, wxBOTTOM|wxRIGHT|wxLEFT, 8 );


    bSizer86->Add( bSizer79, 0, wxEXPAND, 5 );

    wxBoxSizer* bSizer80;
    bSizer80 = new wxBoxSizer( wxVERTICAL );

    m_sliderRR = new wxSlider( this, wxID_ANY, 43, 0, 86, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL );
    bSizer80->Add( m_sliderRR, 0, wxEXPAND, 5 );

    m_sliderAR = new wxSlider( this, wxID_ANY, 312, 0, 624, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL );
    bSizer80->Add( m_sliderAR, 0, wxEXPAND, 5 );

    m_sliderIn = new wxSlider( this, wxID_ANY, 214, 0, 428, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL );
    bSizer80->Add( m_sliderIn, 0, wxEXPAND, 5 );

    m_sliderNS = new wxSlider( this, wxID_ANY, 30, 0, 60, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL );
    bSizer80->Add( m_sliderNS, 0, wxEXPAND, 5 );

    m_sliderAA = new wxSlider( this, wxID_ANY, 98, 0, 196, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL );
    bSizer80->Add( m_sliderAA, 0, wxEXPAND, 5 );


    bSizer86->Add( bSizer80, 1, wxEXPAND, 5 );

    wxBoxSizer* bSizer791;
    bSizer791 = new wxBoxSizer( wxVERTICAL );

    m_staticTextRR = new wxStaticText( this, wxID_ANY, wxT("0.00"), wxDefaultPosition, wxDefaultSize, 0 );
    m_staticTextRR->Wrap( -1 );
    bSizer791->Add( m_staticTextRR, 1, wxBOTTOM|wxRIGHT|wxLEFT|wxEXPAND, 8 );

    m_staticTextAR = new wxStaticText( this, wxID_ANY, wxT("0.00000"), wxDefaultPosition, wxDefaultSize, 0 );
    m_staticTextAR->Wrap( -1 );
    bSizer791->Add( m_staticTextAR, 0, wxBOTTOM|wxRIGHT|wxLEFT, 8 );

    m_staticTextIn = new wxStaticText( this, wxID_ANY, wxT("2.14"), wxDefaultPosition, wxDefaultSize, 0 );
    m_staticTextIn->Wrap( -1 );
    bSizer791->Add( m_staticTextIn, 0, wxBOTTOM|wxRIGHT|wxLEFT, 8 );

    m_staticTextNS = new wxStaticText( this, wxID_ANY, wxT("0.30"), wxDefaultPosition, wxDefaultSize, 0 );
    m_staticTextNS->Wrap( -1 );
    bSizer791->Add( m_staticTextNS, 0, wxBOTTOM|wxRIGHT|wxLEFT, 8 );

    m_staticTextAA = new wxStaticText( this, wxID_ANY, wxT("0.98"), wxDefaultPosition, wxDefaultSize, 0 );
    m_staticTextAA->Wrap( -1 );
    bSizer791->Add( m_staticTextAA, 0, wxBOTTOM|wxRIGHT|wxLEFT, 8 );


    bSizer791->Add( 70, 0, 1, wxEXPAND, 5 );


    bSizer86->Add( bSizer791, 0, 0, 5 );


    sbSizer6->Add( bSizer86, 1, wxEXPAND|wxTOP|wxBOTTOM, 10 );


    bSizer78->Add( sbSizer6, 0, wxALL|wxEXPAND, 5 );

    wxStaticBoxSizer* sbSizer61;
    sbSizer61 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("HDR Options") ), wxVERTICAL );

    m_checkBoxHDR = new wxCheckBox( this, wxID_ANY, wxT("Enable HDR"), wxDefaultPosition, wxDefaultSize, 0 );
    m_checkBoxHDR->SetValue(true); 
    sbSizer61->Add( m_checkBoxHDR, 0, wxALL, 5 );

    m_checkBoxBloom = new wxCheckBox( this, wxID_ANY, wxT("Enable Bloom"), wxDefaultPosition, wxDefaultSize, 0 );
    m_checkBoxBloom->SetValue(true); 
    sbSizer61->Add( m_checkBoxBloom, 0, wxALL, 5 );

    wxBoxSizer* bSizer861;
    bSizer861 = new wxBoxSizer( wxHORIZONTAL );

    wxBoxSizer* bSizer792;
    bSizer792 = new wxBoxSizer( wxVERTICAL );

    wxStaticText* m_staticText632;
    m_staticText632 = new wxStaticText( this, wxID_ANY, wxT("Lum Key :"), wxDefaultPosition, wxDefaultSize, 0 );
    m_staticText632->Wrap( -1 );
    bSizer792->Add( m_staticText632, 0, wxBOTTOM|wxRIGHT|wxLEFT, 8 );

    wxStaticText* m_staticText6721;
    m_staticText6721 = new wxStaticText( this, wxID_ANY, wxT("Lum White :"), wxDefaultPosition, wxDefaultSize, 0 );
    m_staticText6721->Wrap( -1 );
    bSizer792->Add( m_staticText6721, 0, wxBOTTOM|wxRIGHT|wxLEFT, 8 );

    wxStaticText* m_staticText672;
    m_staticText672 = new wxStaticText( this, wxID_ANY, wxT("Middle Gray  :"), wxDefaultPosition, wxDefaultSize, 0 );
    m_staticText672->Wrap( -1 );
    bSizer792->Add( m_staticText672, 0, wxBOTTOM|wxRIGHT|wxLEFT, 8 );

    wxStaticText* m_staticText642;
    m_staticText642 = new wxStaticText( this, wxID_ANY, wxT("Bloom Key £º"), wxDefaultPosition, wxDefaultSize, 0 );
    m_staticText642->Wrap( -1 );
    bSizer792->Add( m_staticText642, 0, wxBOTTOM|wxRIGHT|wxLEFT, 8 );

    wxStaticText* m_staticText67211;
    m_staticText67211 = new wxStaticText( this, wxID_ANY, wxT("Bright Threshold :"), wxDefaultPosition, wxDefaultSize, 0 );
    m_staticText67211->Wrap( -1 );
    bSizer792->Add( m_staticText67211, 0, wxBOTTOM|wxRIGHT|wxLEFT, 8 );

    wxStaticText* m_staticText652;
    m_staticText652 = new wxStaticText( this, wxID_ANY, wxT("Bloom Deviation :"), wxDefaultPosition, wxDefaultSize, 0 );
    m_staticText652->Wrap( -1 );
    bSizer792->Add( m_staticText652, 0, wxBOTTOM|wxRIGHT|wxLEFT, 8 );

    wxStaticText* m_staticText662;
    m_staticText662 = new wxStaticText( this, wxID_ANY, wxT("Bloom Multiplier :"), wxDefaultPosition, wxDefaultSize, 0 );
    m_staticText662->Wrap( -1 );
    bSizer792->Add( m_staticText662, 0, wxBOTTOM|wxRIGHT|wxLEFT, 8 );


    bSizer861->Add( bSizer792, 0, wxEXPAND, 5 );

    wxBoxSizer* bSizer801;
    bSizer801 = new wxBoxSizer( wxVERTICAL );

    m_sliderLK = new wxSlider( this, wxID_ANY, 43, 0, 86, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL );
    bSizer801->Add( m_sliderLK, 0, wxEXPAND, 5 );

    m_sliderLW = new wxSlider( this, wxID_ANY, 312, 0, 624, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL );
    bSizer801->Add( m_sliderLW, 0, wxEXPAND, 5 );

    m_sliderMG = new wxSlider( this, wxID_ANY, 214, 0, 428, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL );
    bSizer801->Add( m_sliderMG, 0, wxEXPAND, 5 );

    m_sliderBK = new wxSlider( this, wxID_ANY, 214, 0, 428, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL );
    bSizer801->Add( m_sliderBK, 0, wxEXPAND, 5 );

    m_sliderBT = new wxSlider( this, wxID_ANY, 214, 0, 428, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL );
    bSizer801->Add( m_sliderBT, 0, wxEXPAND, 5 );

    m_sliderBD = new wxSlider( this, wxID_ANY, 30, 0, 60, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL );
    bSizer801->Add( m_sliderBD, 0, wxEXPAND, 5 );

    m_sliderBM = new wxSlider( this, wxID_ANY, 98, 0, 196, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL );
    bSizer801->Add( m_sliderBM, 0, wxEXPAND, 5 );


    bSizer861->Add( bSizer801, 1, wxEXPAND, 5 );

    wxBoxSizer* bSizer7911;
    bSizer7911 = new wxBoxSizer( wxVERTICAL );

    m_staticTextLK = new wxStaticText( this, wxID_ANY, wxT("0.00"), wxDefaultPosition, wxDefaultSize, 0 );
    m_staticTextLK->Wrap( -1 );
    bSizer7911->Add( m_staticTextLK, 1, wxBOTTOM|wxRIGHT|wxLEFT|wxEXPAND, 8 );

    m_staticTextLW = new wxStaticText( this, wxID_ANY, wxT("0.00"), wxDefaultPosition, wxDefaultSize, 0 );
    m_staticTextLW->Wrap( -1 );
    bSizer7911->Add( m_staticTextLW, 0, wxBOTTOM|wxRIGHT|wxLEFT, 8 );

    m_staticTextMG = new wxStaticText( this, wxID_ANY, wxT("2.14"), wxDefaultPosition, wxDefaultSize, 0 );
    m_staticTextMG->Wrap( -1 );
    bSizer7911->Add( m_staticTextMG, 0, wxBOTTOM|wxRIGHT|wxLEFT, 8 );

    m_staticTextBK = new wxStaticText( this, wxID_ANY, wxT("0.30"), wxDefaultPosition, wxDefaultSize, 0 );
    m_staticTextBK->Wrap( -1 );
    bSizer7911->Add( m_staticTextBK, 0, wxBOTTOM|wxRIGHT|wxLEFT, 8 );

    m_staticTextBT = new wxStaticText( this, wxID_ANY, wxT("0.98"), wxDefaultPosition, wxDefaultSize, 0 );
    m_staticTextBT->Wrap( -1 );
    bSizer7911->Add( m_staticTextBT, 0, wxBOTTOM|wxRIGHT|wxLEFT, 8 );

    m_staticTextBD = new wxStaticText( this, wxID_ANY, wxT("0.98"), wxDefaultPosition, wxDefaultSize, 0 );
    m_staticTextBD->Wrap( -1 );
    bSizer7911->Add( m_staticTextBD, 0, wxBOTTOM|wxRIGHT|wxLEFT, 8 );

    m_staticTextBM = new wxStaticText( this, wxID_ANY, wxT("0.98"), wxDefaultPosition, wxDefaultSize, 0 );
    m_staticTextBM->Wrap( -1 );
    bSizer7911->Add( m_staticTextBM, 0, wxBOTTOM|wxRIGHT|wxLEFT, 8 );


    bSizer7911->Add( 50, 0, 1, wxEXPAND, 5 );


    bSizer861->Add( bSizer7911, 0, 0, 5 );


    sbSizer61->Add( bSizer861, 1, wxEXPAND|wxTOP|wxBOTTOM, 10 );


    bSizer78->Add( sbSizer61, 0, wxEXPAND|wxALL, 5 );


    this->SetSizer( bSizer78 );
    this->Layout();

    this->Centre( wxBOTH );

    // Connect Events
    m_checkBoxHDAO->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( fbPostProcessPane::OnHDAOChecked ), NULL, this );
    m_checkBoxUseNormal->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( fbPostProcessPane::OnUseNormalChecked ), NULL, this );
    m_sliderRR->Connect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( fbPostProcessPane::OnRR ), NULL, this );
    m_sliderAR->Connect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( fbPostProcessPane::OnAR ), NULL, this );
    m_sliderIn->Connect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( fbPostProcessPane::OnIn ), NULL, this );
    m_sliderNS->Connect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( fbPostProcessPane::OnNS ), NULL, this );
    m_sliderAA->Connect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( fbPostProcessPane::OnAA ), NULL, this );
    m_checkBoxHDR->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( fbPostProcessPane::OnHDRChecked ), NULL, this );
    m_checkBoxBloom->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( fbPostProcessPane::OnBloomChecked ), NULL, this );
    m_sliderLK->Connect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( fbPostProcessPane::OnLK ), NULL, this );
    m_sliderLW->Connect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( fbPostProcessPane::OnLW ), NULL, this );
    m_sliderMG->Connect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( fbPostProcessPane::OnMG ), NULL, this );
    m_sliderBK->Connect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( fbPostProcessPane::OnBK ), NULL, this );
    m_sliderBT->Connect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( fbPostProcessPane::OnBT ), NULL, this );
    m_sliderBD->Connect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( fbPostProcessPane::OnBD ), NULL, this );
    m_sliderBM->Connect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( fbPostProcessPane::OnBM ), NULL, this );
}

fbPostProcessPane::~fbPostProcessPane()
{
    // Disconnect Events
    m_checkBoxHDAO->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( fbPostProcessPane::OnHDAOChecked ), NULL, this );
    m_checkBoxUseNormal->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( fbPostProcessPane::OnUseNormalChecked ), NULL, this );
    m_sliderRR->Disconnect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( fbPostProcessPane::OnRR ), NULL, this );
    m_sliderAR->Disconnect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( fbPostProcessPane::OnAR ), NULL, this );
    m_sliderIn->Disconnect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( fbPostProcessPane::OnIn ), NULL, this );
    m_sliderNS->Disconnect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( fbPostProcessPane::OnNS ), NULL, this );
    m_sliderAA->Disconnect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( fbPostProcessPane::OnAA ), NULL, this );
    m_checkBoxHDR->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( fbPostProcessPane::OnHDRChecked ), NULL, this );
    m_checkBoxBloom->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( fbPostProcessPane::OnBloomChecked ), NULL, this );
    m_sliderLK->Disconnect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( fbPostProcessPane::OnLK ), NULL, this );
    m_sliderLW->Disconnect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( fbPostProcessPane::OnLW ), NULL, this );
    m_sliderMG->Disconnect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( fbPostProcessPane::OnMG ), NULL, this );
    m_sliderBK->Disconnect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( fbPostProcessPane::OnBK ), NULL, this );
    m_sliderBT->Disconnect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( fbPostProcessPane::OnBT ), NULL, this );
    m_sliderBD->Disconnect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( fbPostProcessPane::OnBD ), NULL, this );
    m_sliderBM->Disconnect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( fbPostProcessPane::OnBM ), NULL, this );

}


///////////////////////////////////////////////////////////////////////////


PostProcessPane::PostProcessPane(wxWindow* parent)
    : fbPostProcessPane(parent)
{

}


PostProcessPane::~PostProcessPane()
{

}
