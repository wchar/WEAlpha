#pragma once


#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/string.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/propgrid/propgrid.h>
#include <wx/propgrid/advprops.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/sizer.h>
#include <wx/frame.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class fbWorldPane
///////////////////////////////////////////////////////////////////////////////
class fbWorldPane : public wxFrame 
{
private:

protected:
    wxPropertyGrid* m_propertyGrid5;
    wxPGProperty* m_pgiWSX;
    wxPGProperty* m_pgiWSY;
    wxPGProperty* m_pgiWSZ;
    wxPGProperty* m_pgiLCPX;
    wxPGProperty* m_pgiLCPY;
    wxPGProperty* m_pgiLCPZ;
    wxPGProperty* m_pgiLDX;
    wxPGProperty* m_pgiLDY;
    wxPGProperty* m_pgiLDZ;
    		
    // Virtual event handlers, overide them in your derived class		
    virtual void OnChanged( wxPropertyGridEvent& event ) { event.Skip(); }
public:

    fbWorldPane( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("WorldProperties"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 522,504 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );

    ~fbWorldPane();

};


///////////////////////////////////////////////////////////////////////////////
/// Class WorldPane
///////////////////////////////////////////////////////////////////////////////
class WorldPane : public fbWorldPane
{
public:
    WorldPane(wxWindow* parent);
    ~WorldPane();

protected:
    virtual void OnChanged( wxPropertyGridEvent& event );
};

