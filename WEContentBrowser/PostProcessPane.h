#pragma once


#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/checkbox.h>
#include <wx/sizer.h>
#include <wx/slider.h>
#include <wx/statbox.h>
#include <wx/frame.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class fbPostProcessPane
///////////////////////////////////////////////////////////////////////////////
class fbPostProcessPane : public wxFrame 
{
private:

protected:
    wxCheckBox* m_checkBoxHDAO;
    wxCheckBox* m_checkBoxUseNormal;
    wxSlider* m_sliderRR;
    wxSlider* m_sliderAR;
    wxSlider* m_sliderIn;
    wxSlider* m_sliderNS;
    wxSlider* m_sliderAA;
    wxStaticText* m_staticTextRR;
    wxStaticText* m_staticTextAR;
    wxStaticText* m_staticTextIn;
    wxStaticText* m_staticTextNS;
    wxStaticText* m_staticTextAA;
    wxCheckBox* m_checkBoxHDR;
    wxCheckBox* m_checkBoxBloom;
    wxSlider* m_sliderLK;
    wxSlider* m_sliderLW;
    wxSlider* m_sliderMG;
    wxSlider* m_sliderBK;
    wxSlider* m_sliderBT;
    wxSlider* m_sliderBD;
    wxSlider* m_sliderBM;
    wxStaticText* m_staticTextLK;
    wxStaticText* m_staticTextLW;
    wxStaticText* m_staticTextMG;
    wxStaticText* m_staticTextBK;
    wxStaticText* m_staticTextBT;
    wxStaticText* m_staticTextBD;
    wxStaticText* m_staticTextBM;

    // Virtual event handlers, overide them in your derived class
    virtual void OnHDAOChecked( wxCommandEvent& event ) { event.Skip(); }
    virtual void OnUseNormalChecked( wxCommandEvent& event ) { event.Skip(); }
    virtual void OnRR( wxScrollEvent& event ) { event.Skip(); }
    virtual void OnAR( wxScrollEvent& event ) { event.Skip(); }
    virtual void OnIn( wxScrollEvent& event ) { event.Skip(); }
    virtual void OnNS( wxScrollEvent& event ) { event.Skip(); }
    virtual void OnAA( wxScrollEvent& event ) { event.Skip(); }
    virtual void OnHDRChecked( wxCommandEvent& event ) { event.Skip(); }
    virtual void OnBloomChecked( wxCommandEvent& event ) { event.Skip(); }
    virtual void OnLK( wxScrollEvent& event ) { event.Skip(); }
    virtual void OnLW( wxScrollEvent& event ) { event.Skip(); }
    virtual void OnMG( wxScrollEvent& event ) { event.Skip(); }
    virtual void OnBK( wxScrollEvent& event ) { event.Skip(); }
    virtual void OnBT( wxScrollEvent& event ) { event.Skip(); }
    virtual void OnBD( wxScrollEvent& event ) { event.Skip(); }
    virtual void OnBM( wxScrollEvent& event ) { event.Skip(); }


public:

    fbPostProcessPane( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 503,600 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );

    ~fbPostProcessPane();

};



///////////////////////////////////////////////////////////////////////////////
/// Class PostProcessPane
///////////////////////////////////////////////////////////////////////////////
class PostProcessPane : public fbPostProcessPane
{
public:
    PostProcessPane(wxWindow* parent);
    ~PostProcessPane(void);
};

