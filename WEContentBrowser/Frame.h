#pragma once


#include <wx/wx.h>

class CEFrame : public wxFrame
{
public:
	CEFrame( wxFrame *frame, const wxString& title );
	~CEFrame();
private:
	enum
	{
		idMenuQuit = 1000,
		idMenuAbout,
		idBGColor,
		idDrawCoordinate,
		idPause,
		idReset,
		idImport,
		idExport,
        idTimer = 0
	};

	void OnClose( wxCloseEvent& event );
	void OnQuit( wxCommandEvent& event );

    void OnPaint( wxPaintEvent& event );
    void OnSize( wxSizeEvent& event );
    void OnMouseEvent( wxMouseEvent& evt );
    void OnTimer( wxTimerEvent& event );

    wxTimer m_Timer;

	wxButton* m_pBGColorBtn;
	wxButton* m_pResetBtn;

	DECLARE_EVENT_TABLE();
};
