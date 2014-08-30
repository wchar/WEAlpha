#include "MaterialModifierPanel.h"

///////////////////////////////////////////////////////////////////////////

fbMaterialModifierPanel::fbMaterialModifierPanel( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
    wxBoxSizer* bSizer70;
    bSizer70 = new wxBoxSizer( wxVERTICAL );

    wxStaticText* m_staticText7;
    m_staticText7 = new wxStaticText( this, wxID_ANY, wxT("MaterialSet"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE );
    m_staticText7->Wrap( -1 );
    m_staticText7->SetFont( wxFont( 14, 74, 90, 90, false, wxT("Arial") ) );
    m_staticText7->SetForegroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_HIGHLIGHT ) );
    m_staticText7->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_INFOTEXT ) );

    bSizer70->Add( m_staticText7, 0, wxEXPAND, 5 );

    m_splitter_h = new wxSplitterWindow( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_3D|wxSP_LIVE_UPDATE );
    m_splitter_h->Connect( wxEVT_IDLE, wxIdleEventHandler( fbMaterialModifierPanel::m_splitter_hOnIdle ), NULL, this );

    m_panelMaterialSet = new wxScrolledWindow( m_splitter_h, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHSCROLL|wxVSCROLL );
    m_panelMaterialSet->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_INACTIVECAPTION ) );

    wxBoxSizer* bSizer73;
    bSizer73 = new wxBoxSizer( wxVERTICAL );


    m_panelMaterialSet->SetSizer( bSizer73 );
    m_panelMaterialSet->Layout();
    bSizer73->Fit( m_panelMaterialSet );
    wxPanel* m_panel57;
    m_panel57 = new wxPanel( m_splitter_h, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
    wxBoxSizer* bSizer72;
    bSizer72 = new wxBoxSizer( wxVERTICAL );

    wxStaticText* m_staticText71;
    m_staticText71 = new wxStaticText( m_panel57, wxID_ANY, wxT("MaterialProperties"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE );
    m_staticText71->Wrap( -1 );
    m_staticText71->SetFont( wxFont( 14, 74, 90, 90, false, wxT("Arial") ) );
    m_staticText71->SetForegroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_HIGHLIGHT ) );
    m_staticText71->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_INFOTEXT ) );

    bSizer72->Add( m_staticText71, 0, wxEXPAND, 5 );

    m_propertyGrid = new wxPropertyGrid(m_panel57, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxPG_DEFAULT_STYLE|wxPG_SPLITTER_AUTO_CENTER);
    wxPGProperty* m_propertyGridItem26;
    m_propertyGridItem26 = m_propertyGrid->Append( new wxPropertyCategory( wxT("ModelType"), wxT("ModelType") ) ); 
    m_pgiIsPhong = m_propertyGrid->Append( new wxBoolProperty( wxT("PhongModel"), wxT("PhongModel") ) ); 
    wxPGProperty* m_propertyGridItem20;
    m_propertyGridItem20 = m_propertyGrid->Append( new wxPropertyCategory( wxT("Color"), wxT("Color") ) ); 
    m_pgiAlpha = m_propertyGrid->Append( new wxFloatProperty( wxT("Alpha"), wxT("Alpha") ) ); 
    m_pgiShininess = m_propertyGrid->Append( new wxFloatProperty( wxT("Shininess"), wxT("Shininess") ) ); 

    m_pgiDiffuse = m_propertyGrid->Append( new wxColourProperty( wxT("Diffuse"), wxT("Diffuse") ) ); 
    m_pgiEmissive = m_propertyGrid->Append( new wxColourProperty( wxT("Emissive"), wxT("Emissive") ) ); 
    m_pgiAmbient = m_propertyGrid->Append( new wxColourProperty( wxT("Ambient"), wxT("Ambient") ) ); 
    m_pgiSpecular = m_propertyGrid->Append( new wxColourProperty( wxT("Specular"), wxT("Specular") ) ); 
    wxPGProperty* m_propertyGridItem27;
    m_propertyGridItem27 = m_propertyGrid->Append( new wxPropertyCategory( wxT("Texture"), wxT("Texture") ) ); 
    m_pgiDiffuseTex = m_propertyGrid->Append( new wxFileProperty( wxT("Diffuse Texture"), wxT("Diffuse Texture") ) ); 
    m_pgiEmissiveTex = m_propertyGrid->Append( new wxFileProperty( wxT("Emissive Texture"), wxT("Emissive Texture") ) ); 
    m_pgiNormalMap = m_propertyGrid->Append( new wxFileProperty( wxT("Normal Map"), wxT("Normal Map") ) ); 
    bSizer72->Add( m_propertyGrid, 1, wxEXPAND, 5 );


    m_panel57->SetSizer( bSizer72 );
    m_panel57->Layout();
    bSizer72->Fit( m_panel57 );
    m_splitter_h->SplitHorizontally( m_panelMaterialSet, m_panel57, 170 );
    bSizer70->Add( m_splitter_h, 1, wxEXPAND, 5 );


    this->SetSizer( bSizer70 );
    this->Layout();

    // Connect Events
    m_propertyGrid->Connect( wxEVT_PG_CHANGED, wxPropertyGridEventHandler( fbMaterialModifierPanel::OnPropertiesChange ), NULL, this );
}

fbMaterialModifierPanel::~fbMaterialModifierPanel()
{
    // Disconnect Events
    m_propertyGrid->Disconnect( wxEVT_PG_CHANGED, wxPropertyGridEventHandler( fbMaterialModifierPanel::OnPropertiesChange ), NULL, this );

}



///////////////////////////////////////////////////////////////////////////////
/// Class MaterialSetItem
///////////////////////////////////////////////////////////////////////////////
class MaterialSetItem : public wxPanel 
{
private:

protected:
    wxStaticText* m_staticTextMaterialName;
    MaterialModifierPanel* m_pMaterialModifierPanel;
    int m_Index;
    // Virtual event handlers, overide them in your derived class
    virtual void OnSelected( wxMouseEvent& event );
public:
    MaterialSetItem( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxTAB_TRAVERSAL ); 
    ~MaterialSetItem();

    void FocusThis(bool bFocus);

    void Init(MaterialModifierPanel* pmmp, wxString name, int idx);
};

MaterialSetItem::MaterialSetItem( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
    wxBoxSizer* bSizer43;
    bSizer43 = new wxBoxSizer( wxVERTICAL );

    m_staticTextMaterialName = new wxStaticText( this, wxID_ANY, wxT("MaterialName"), wxDefaultPosition, wxDefaultSize, 0 );
    m_staticTextMaterialName->Wrap( -1 );
    m_staticTextMaterialName->SetFont( wxFont( 14, 74, 90, 90, false, wxT("Arial") ) );

    bSizer43->Add( m_staticTextMaterialName, 1, wxALL|wxEXPAND, 5 );


    this->SetSizer( bSizer43 );
    this->Layout();

    // Connect Events
    this->Connect( wxEVT_LEFT_DOWN, wxMouseEventHandler( MaterialSetItem::OnSelected ) );
    m_staticTextMaterialName->Connect( wxEVT_LEFT_DOWN, wxMouseEventHandler( MaterialSetItem::OnSelected ), NULL, this );
}

MaterialSetItem::~MaterialSetItem()
{
    // Disconnect Events
    this->Disconnect( wxEVT_LEFT_DOWN, wxMouseEventHandler( MaterialSetItem::OnSelected ) );
    m_staticTextMaterialName->Disconnect( wxEVT_LEFT_DOWN, wxMouseEventHandler( MaterialSetItem::OnSelected ), NULL, this );

}

void MaterialSetItem::Init(MaterialModifierPanel* pmmp, wxString name, int idx)
{
    m_pMaterialModifierPanel = pmmp;
    m_staticTextMaterialName->SetLabelText(name);
    m_Index = idx;    

    m_pMaterialModifierPanel->FocusMaterial(m_Index);
    FocusThis(true);
}

void MaterialSetItem::OnSelected( wxMouseEvent& event )
{
    m_pMaterialModifierPanel->FocusMaterial(m_Index);
    FocusThis(true);
}

void MaterialSetItem::FocusThis(bool bFocus)
{
    if (bFocus)
        SetBackgroundColour(wxSystemSettings::GetColour( wxSYS_COLOUR_HIGHLIGHT ));
    else
        SetBackgroundColour(wxColour( 142, 199, 255 ));

    Refresh(true);
}



///////////////////////////////////////////////////////////////////////////

MaterialModifierPanel::MaterialModifierPanel(wxWindow* parent)
    : fbMaterialModifierPanel(parent)
{
    m_panelMaterialSet->SetScrollRate(5, 5);

    m_iMaterialCount = 0;

    m_strDiffuseTexture = L"";
    m_strEmissiveTexture = L"";
    m_strNormalMap = L"";
}

void MaterialModifierPanel::InsertMaterial(wxString name)
{
    MaterialSetItem* pItem = new MaterialSetItem(m_panelMaterialSet);
    pItem->Init(this, name, m_iMaterialCount++);
    wxSizer* pSizer = m_panelMaterialSet->GetSizer();
    pSizer->Add(pItem, 0, wxEXPAND, 5 );    

    // Update windows
    m_splitter_h->SetSashPosition(m_splitter_h->GetSashPosition()-1);    
    m_splitter_h->SetSashPosition(m_splitter_h->GetSashPosition()+1);
}

void MaterialModifierPanel::FocusMaterial(int idx)
{
    m_iCurrentIndex = idx;

    wxWindowList list = m_panelMaterialSet->GetChildren();
    for (wxWindowList::iterator it = list.begin(); it != list.end(); it++)
    {
        MaterialSetItem* pItem = dynamic_cast<MaterialSetItem*>(*it);
        if(pItem)
        {
            pItem->FocusThis(false);
        }
    }

    FillPropertyGrid(m_pWEContents->at(idx));
}

void MaterialModifierPanel::Init(vector<WEMaterialContent*>* pWEContents, vector<WEMaterial*>* pWEMaterials)
{
    m_pWEContents = pWEContents;
    m_pWEMaterials = pWEMaterials;

    for (int i = 0; i < pWEContents->size(); i++)
    {
        InsertMaterial(pWEContents->at(i)->m_strMaterialName);
    }
}

void MaterialModifierPanel::FillPropertyGrid(WEMaterialContent* pMaterialContent)
{   
    m_pgiAlpha->SetValue(pMaterialContent->m_fAlpha);
    m_pgiShininess->SetValue(pMaterialContent->m_fShininess);
    m_pgiSpecular->SetValue(VectorToColor(pMaterialContent->m_vSpecular));
    m_pgiAmbient->SetValue(VectorToColor(pMaterialContent->m_vAmbient));
    m_pgiDiffuse->SetValue(VectorToColor(pMaterialContent->m_vDiffuse));
    m_pgiEmissive->SetValue(VectorToColor(pMaterialContent->m_vEmissive));
    m_pgiIsPhong->SetValue(pMaterialContent->m_bEnableSpecular);

    m_pgiDiffuseTex->SetValue(m_strDiffuseTexture);
    m_pgiEmissiveTex->SetValue(m_strEmissiveTexture);
    m_pgiNormalMap->SetValue(m_strNormalMap);

    m_pgiDiffuseTex->Enable(false);
    m_pgiEmissiveTex->Enable(false);
    m_pgiNormalMap->Enable(false);
}

wxVariant MaterialModifierPanel::VectorToColor(XMFLOAT3& v)
{
    wxColour color;
    color.Set(v.x * 255, v.y * 255, v.z * 255, 255);
    wxVariant variant;
    variant << wxColourPropertyValue(color);
    return variant;
}

XMFLOAT3 MaterialModifierPanel::ColorToVector(wxVariant& c)
{
    wxColour colour ;
    colour << c;

    XMFLOAT3 ret;
    ret.x = colour.Red() / 255.f;
    ret.y = colour.Green() / 255.f;
    ret.z = colour.Blue() / 255.f;

    return ret;
}


void MaterialModifierPanel::OnPropertiesChange( wxPropertyGridEvent& event )
{
    WEMaterial* pMaterial = m_pWEMaterials->at(m_iCurrentIndex);
    WEMaterialContent* pContent = m_pWEContents->at(m_iCurrentIndex);

    pContent->m_bEnableSpecular = pMaterial->m_bEnableSpecular = m_pgiIsPhong->GetValue().GetBool();
    pContent->m_fAlpha = pMaterial->m_fAlpha = m_pgiAlpha->GetValue().GetDouble();
    pContent->m_fShininess = pMaterial->m_fShininess = m_pgiShininess->GetValue().GetDouble();
    pContent->m_vDiffuse = pMaterial->m_vDiffuse = ColorToVector(m_pgiDiffuse->GetValue());
    pContent->m_vAmbient = pMaterial->m_vAmbient = ColorToVector(m_pgiAmbient->GetValue());
    pContent->m_vEmissive = pMaterial->m_vEmissive = ColorToVector(m_pgiEmissive->GetValue());
    pContent->m_vSpecular = pMaterial->m_vSpecular = ColorToVector(m_pgiSpecular->GetValue());

}