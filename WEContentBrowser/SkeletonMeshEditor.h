#pragma once

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/string.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/gdicmn.h>
#include <wx/toolbar.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/stattext.h>
#include <wx/propgrid/propgrid.h>
#include <wx/propgrid/advprops.h>
#include <wx/splitter.h>
#include <wx/notebook.h>
#include <wx/slider.h>
#include <wx/menu.h>
#include <wx/statusbr.h>
#include <wx/frame.h>


///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class fbSkeletonMeshEditor
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Jun  5 2014)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __NONAME_H__
#define __NONAME_H__

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/string.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/gdicmn.h>
#include <wx/toolbar.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/stattext.h>
#include <wx/propgrid/propgrid.h>
#include <wx/propgrid/advprops.h>
#include <wx/splitter.h>
#include <wx/notebook.h>
#include <wx/slider.h>
#include <wx/menu.h>
#include <wx/statusbr.h>
#include <wx/frame.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class fbSkeletonMeshEditor
///////////////////////////////////////////////////////////////////////////////
class fbSkeletonMeshEditor : public wxFrame 
{
private:

protected:
    wxPanel* m_panelToolBar;
    wxToolBarToolBase* m_toolOpen; 
    wxToolBarToolBase* m_toolSave; 
    wxToolBarToolBase* m_toolImport; 
    wxToolBarToolBase* m_toolPlay; 
    wxToolBarToolBase* m_toolStop; 
    wxToolBarToolBase* m_toolPause; 
    wxToolBarToolBase* m_toolAdd; 
    wxToolBarToolBase* m_tooRemove; 
    wxToolBarToolBase* m_toolSetting; 
    wxToolBarToolBase* m_toolInfo; 
    wxSplitterWindow* m_splitter_v;
    wxNotebook* m_notebook;
    wxPanel* m_panelAnimation;
    wxSplitterWindow* m_splitter_h;
    wxScrolledWindow* m_panelAnimationSet;
    wxPropertyGrid* m_propertyGridAnimation;
    wxPGProperty* m_propertyGridItemStart;
    wxPGProperty* m_propertyGridItemEnd;
    wxPGProperty* m_propertyGridItemRate;
    wxPanel* m_panelMaterial;
    wxPanel* m_panelModelViewer;
    wxSlider* m_sliderFrame;

    // Virtual event handlers, overide them in your derived class
    virtual void OnOpen( wxCommandEvent& event ) { event.Skip(); }
    virtual void OnSave( wxCommandEvent& event ) { event.Skip(); }
    virtual void OnImport( wxCommandEvent& event ) { event.Skip(); }
    virtual void OnPlay( wxCommandEvent& event ) { event.Skip(); }
    virtual void OnStop( wxCommandEvent& event ) { event.Skip(); }
    virtual void OnPause( wxCommandEvent& event ) { event.Skip(); }
    virtual void OnAdd( wxCommandEvent& event ) { event.Skip(); }
    virtual void OnRemove( wxCommandEvent& event ) { event.Skip(); }
    virtual void OnAbout( wxCommandEvent& event ) { event.Skip(); }
    virtual void OnExit( wxCommandEvent& event ) { event.Skip(); }
    virtual void OnToolBar( wxCommandEvent& event ) { event.Skip(); }


public:

    fbSkeletonMeshEditor( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("SkeletonMeshEditor"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 845,590 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );

    ~fbSkeletonMeshEditor();

    void m_splitter_vOnIdle( wxIdleEvent& )
    {
        m_splitter_v->SetSashPosition( 389 );
        m_splitter_v->Disconnect( wxEVT_IDLE, wxIdleEventHandler( fbSkeletonMeshEditor::m_splitter_vOnIdle ), NULL, this );
    }

    void m_splitter_hOnIdle( wxIdleEvent& )
    {
        m_splitter_h->SetSashPosition( 100 );
        m_splitter_h->Disconnect( wxEVT_IDLE, wxIdleEventHandler( fbSkeletonMeshEditor::m_splitter_hOnIdle ), NULL, this );
    }

};

#endif //__NONAME_H__


///////////////////////////////////////////////////////////////////////////////
/// Class SkeletonMeshEditor
///////////////////////////////////////////////////////////////////////////////
class SkeletonMeshEditor : public fbSkeletonMeshEditor
{
public:
    SkeletonMeshEditor(wxWindow* parent);

    void InsertAnimation();
    void FocusAnimation(int idx);
protected:    
    int m_iAnimationCount;
    int m_iCurrentAnimIndex;

    virtual void OnOpen( wxCommandEvent& event );
    virtual void OnSave( wxCommandEvent& event );
    virtual void OnImport( wxCommandEvent& event );
    virtual void OnPlay( wxCommandEvent& event );
    virtual void OnStop( wxCommandEvent& event );
    virtual void OnPause( wxCommandEvent& event );
    virtual void OnAdd( wxCommandEvent& event );
    virtual void OnRemove( wxCommandEvent& event );
    virtual void OnAbout( wxCommandEvent& event );
    virtual void OnExit( wxCommandEvent& event );
    virtual void OnToolBar( wxCommandEvent& event );
};

