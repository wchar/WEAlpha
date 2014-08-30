#pragma once
#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/propgrid/propgrid.h>
#include <wx/propgrid/advprops.h>
#include <wx/splitter.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class fbMaterialModifierPanel
///////////////////////////////////////////////////////////////////////////////
class fbMaterialModifierPanel : public wxPanel 
{
private:

protected:
    wxSplitterWindow* m_splitter_h;
    wxScrolledWindow* m_panelMaterialSet;
    wxPropertyGrid* m_propertyGrid;
    wxPGProperty* m_pgiIsPhong;
    wxPGProperty* m_pgiAlpha;
    wxPGProperty* m_pgiShininess;
    wxPGProperty* m_pgiDiffuse;
    wxPGProperty* m_pgiEmissive;
    wxPGProperty* m_pgiAmbient;
    wxPGProperty* m_pgiSpecular;
    wxPGProperty* m_pgiDiffuseTex;
    wxPGProperty* m_pgiEmissiveTex;
    wxPGProperty* m_pgiNormalMap;

    // Virtual event handlers, overide them in your derived class
    virtual void OnPropertiesChange( wxPropertyGridEvent& event ) { event.Skip(); }
    
public:

    fbMaterialModifierPanel( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 288,645 ), long style = wxTAB_TRAVERSAL ); 
    ~fbMaterialModifierPanel();

    void m_splitter_hOnIdle( wxIdleEvent& )
    {
        m_splitter_h->SetSashPosition( 170 );
        m_splitter_h->Disconnect( wxEVT_IDLE, wxIdleEventHandler( fbMaterialModifierPanel::m_splitter_hOnIdle ), NULL, this );
    }

};




#include "WENative.h"
///////////////////////////////////////////////////////////////////////////////
/// Class MaterialModifierPanel
///////////////////////////////////////////////////////////////////////////////
class MaterialModifierPanel : public fbMaterialModifierPanel
{
public:
    MaterialModifierPanel(wxWindow* parent);


    void Init(vector<WEMaterialContent*>* pWEContents, vector<WEMaterial*>* pWEMaterials);


    void FocusMaterial(int idx);
    virtual void OnPropertiesChange( wxPropertyGridEvent& event );
protected:
    int m_iMaterialCount;
    int m_iCurrentIndex;
    vector<WEMaterialContent*>* m_pWEContents;
    vector<WEMaterial*>* m_pWEMaterials;    
    
    void InsertMaterial(wxString name);
    void FillPropertyGrid(WEMaterialContent* WEMaterialContent);

    wxVariant VectorToColor(XMFLOAT3& v);
    XMFLOAT3 ColorToVector(wxVariant& c);

    wxString m_strDiffuseTexture;
    wxString m_strEmissiveTexture;
    wxString m_strNormalMap;

};