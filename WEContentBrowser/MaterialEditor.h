#pragma once

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/propgrid/propgrid.h>
#include <wx/propgrid/advprops.h>
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/splitter.h>
#include <wx/frame.h>


#include "WENative.h"
#include "D3DPanel.h"

class MaterialEditor : public wxFrame
{
public:
    //MaterialEditor(wxWindow* parent, wxWindowID id, const wxString& title, WERenderCore* weCore);
    void Create(WERenderCore* weCore);
protected:
    enum
    {
        idTimer = 0
    };
    void OnTimer(wxTimerEvent& event);
    void OnActivate(wxActivateEvent& event);
    wxTimer m_Timer;

    ModelViewerWindow* m_pModelViewer;
    WERenderCore* m_pRenderCore;

    // wxFormBuilder codes
protected:
    wxSplitterWindow* m_splitter;
    wxPanel* m_panelProperties;
    wxStaticText* m_staticTextPropertyGridTitle;
    wxPropertyGrid* m_propertyGrid2;
    wxPGProperty* m_propertyGridItem2;
    wxPGProperty* m_propertyGridItem3;
    wxPGProperty* m_propertyGridItem4;
    wxPGProperty* m_propertyGridItem5;
    wxPanel* m_panelModelViewer;
    wxStaticText* m_staticText4;
    wxPanel* m_panel6;

public:
    MaterialEditor( wxWindow* parent, 
        wxWindowID id = wxID_ANY, 
        const wxString& title = wxT("MaterialEditor"), 
        const wxPoint& pos = wxDefaultPosition, 
        const wxSize& size = wxSize( 781,452 ), 
        long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );

    ~MaterialEditor();

    void m_splitterOnIdle( wxIdleEvent& )
    {
        m_splitter->SetSashPosition( 284 );
        m_splitter->Disconnect( wxEVT_IDLE, wxIdleEventHandler( MaterialEditor::m_splitterOnIdle ), NULL, this );
    }


    DECLARE_EVENT_TABLE();
};

