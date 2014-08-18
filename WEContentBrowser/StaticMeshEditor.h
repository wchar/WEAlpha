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
#include <wx/splitter.h>
#include <wx/menu.h>
#include <wx/statusbr.h>
#include <wx/frame.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class fbStaticMeshEditor
///////////////////////////////////////////////////////////////////////////////
class fbStaticMeshEditor : public wxFrame 
{
private:

protected:
    wxPanel* m_panelToolBar;
    wxToolBarToolBase* m_toolOpen; 
    wxToolBarToolBase* m_toolSave; 
    wxToolBarToolBase* m_toolImport; 
    wxToolBarToolBase* m_toolSetting; 
    wxToolBarToolBase* m_toolInfo; 
    wxSplitterWindow* m_splitter_v;		
    wxPanel* m_panelMaterial;
    wxPanel* m_panelModelViewer;

    // Virtual event handlers, overide them in your derived class
    virtual void OnOpen( wxCommandEvent& event ) { event.Skip(); }
    virtual void OnSave( wxCommandEvent& event ) { event.Skip(); }
    virtual void OnImport( wxCommandEvent& event ) { event.Skip(); }
    virtual void OnAbout( wxCommandEvent& event ) { event.Skip(); }
    virtual void OnExit( wxCommandEvent& event ) { event.Skip(); }
    virtual void OnToolBar( wxCommandEvent& event ) { event.Skip(); }


public:

    fbStaticMeshEditor( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("StaticMeshEditor"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 863,608 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );

    ~fbStaticMeshEditor();

    void m_splitter_vOnIdle( wxIdleEvent& )
    {
        m_splitter_v->SetSashPosition( 389 );
        m_splitter_v->Disconnect( wxEVT_IDLE, wxIdleEventHandler( fbStaticMeshEditor::m_splitter_vOnIdle ), NULL, this );
    }

};

///////////////////////////////////////////////////////////////////////////////
/// Class StaticMeshEditor
///////////////////////////////////////////////////////////////////////////////
class StaticMeshEditor : public fbStaticMeshEditor
{
public:
    StaticMeshEditor(wxWindow* parent);

};