#pragma once

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/string.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/menu.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/statbmp.h>
#include <wx/panel.h>
#include <wx/stattext.h>
#include <wx/sizer.h>
#include <wx/frame.h>


///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class fbMainFrame
///////////////////////////////////////////////////////////////////////////////
class fbMainFrame : public wxFrame 
{
private:

protected:
    wxPanel* m_panelD3DInit;
    wxStaticText* m_staticTextInfo;

    // Virtual event handlers, overide them in your derived class
    virtual void OnExitAll( wxCommandEvent& event ) { event.Skip(); }
    virtual void OnMeshContent( wxCommandEvent& event ) { event.Skip(); }
    virtual void OnSkeletonMeshContent( wxCommandEvent& event ) { event.Skip(); }
    virtual void OnParticleContent( wxCommandEvent& event ) { event.Skip(); }
    virtual void OnBeamContent( wxCommandEvent& event ) { event.Skip(); }
    virtual void OnAnimTrailContent( wxCommandEvent& event ) { event.Skip(); }


public:

    fbMainFrame( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("WEContentTools"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 554,361 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );

    ~fbMainFrame();

};




///////////////////////////////////////////////////////////////////////////////
/// Class MainFrame
///////////////////////////////////////////////////////////////////////////////
class MainFrame : public fbMainFrame
{
public:
    MainFrame(wxWindow* parent);

protected:
    virtual void OnExitAll( wxCommandEvent& event );
    virtual void OnMeshContent( wxCommandEvent& event );
    virtual void OnSkeletonMeshContent( wxCommandEvent& event );
    virtual void OnParticleContent( wxCommandEvent& event );
    virtual void OnBeamContent( wxCommandEvent& event );
    virtual void OnAnimTrailContent( wxCommandEvent& event );
};