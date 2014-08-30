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
#include <wx/combobox.h>
#include <wx/statbox.h>
#include <wx/frame.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class fbCascadePane
///////////////////////////////////////////////////////////////////////////////
class fbCascadePane : public wxFrame 
{
private:

protected:
    wxCheckBox* m_checkBoxVC;
    wxStaticText* m_staticTextTS;
    wxSlider* m_sliderTS;
    wxStaticText* m_staticTextSB;
    wxSlider* m_sliderSB;
    wxCheckBox* m_checkBoxCB;
    wxStaticText* m_staticTextCB;
    wxSlider* m_sliderCB;
    wxComboBox* m_comboBoxSelection;
    wxComboBox* m_comboBoxLevelCount;
    wxSlider* m_sliderL1;
    wxSlider* m_sliderL2;
    wxSlider* m_sliderL3;
    wxSlider* m_sliderL4;
    wxSlider* m_sliderL5;
    wxSlider* m_sliderL6;
    wxSlider* m_sliderL7;
    wxSlider* m_sliderL8;

    // Virtual event handlers, overide them in your derived class
    virtual void OnVCChecked( wxCommandEvent& event ) { event.Skip(); }
    virtual void OnTSChanged( wxScrollEvent& event ) { event.Skip(); }
    virtual void OnSBChanged( wxScrollEvent& event ) { event.Skip(); }
    virtual void OnCBChecked( wxCommandEvent& event ) { event.Skip(); }
    virtual void OnCBChanged( wxScrollEvent& event ) { event.Skip(); }
    virtual void OnSelectionChanged( wxCommandEvent& event ) { event.Skip(); }
    virtual void OnLevelCountChanged( wxCommandEvent& event ) { event.Skip(); }
    virtual void OnL1Changed( wxScrollEvent& event ) { event.Skip(); }
    virtual void OnL2Changed( wxScrollEvent& event ) { event.Skip(); }
    virtual void OnL3Changed( wxScrollEvent& event ) { event.Skip(); }
    virtual void OnL4Changed( wxScrollEvent& event ) { event.Skip(); }
    virtual void OnL5Changed( wxScrollEvent& event ) { event.Skip(); }
    virtual void OnL6Changed( wxScrollEvent& event ) { event.Skip(); }
    virtual void OnL7Changed( wxScrollEvent& event ) { event.Skip(); }
    virtual void OnL8Changed( wxScrollEvent& event ) { event.Skip(); }


public:

    fbCascadePane( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("CascadePane"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 251,636 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );

    ~fbCascadePane();

};


///////////////////////////////////////////////////////////////////////////////
/// Class CascadePane
///////////////////////////////////////////////////////////////////////////////
class CascadePane : public fbCascadePane
{
public:
    CascadePane(wxWindow* parent);
    ~CascadePane();


protected:
    virtual void OnVCChecked( wxCommandEvent& event );
    virtual void OnTSChanged( wxScrollEvent& event );
    virtual void OnSBChanged( wxScrollEvent& event );
    virtual void OnCBChecked( wxCommandEvent& event );
    virtual void OnCBChanged( wxScrollEvent& event );
    virtual void OnSelectionChanged( wxCommandEvent& event );
    virtual void OnLevelCountChanged( wxCommandEvent& event );
    virtual void OnL1Changed( wxScrollEvent& event ){ OnAnyLevelChanged(0); }
    virtual void OnL2Changed( wxScrollEvent& event ){ OnAnyLevelChanged(1); }
    virtual void OnL3Changed( wxScrollEvent& event ){ OnAnyLevelChanged(2); }
    virtual void OnL4Changed( wxScrollEvent& event ){ OnAnyLevelChanged(3); }
    virtual void OnL5Changed( wxScrollEvent& event ){ OnAnyLevelChanged(4); }
    virtual void OnL6Changed( wxScrollEvent& event ){ OnAnyLevelChanged(5); }
    virtual void OnL7Changed( wxScrollEvent& event ){ OnAnyLevelChanged(6); }
    virtual void OnL8Changed( wxScrollEvent& event ){ OnAnyLevelChanged(7); }

    void OnAnyLevelChanged(int level);
};

