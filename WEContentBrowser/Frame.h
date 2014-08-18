#pragma once

#include <wx/wx.h>
#include "WENative.h"
#include "MaterialEditor.h"

class CEFrame : public wxFrame
{
public:
	CEFrame(wxFrame *frame, const wxString& title);
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

	void OnClose(wxCloseEvent& event);
	void OnQuit(wxCommandEvent& event);

    WERenderCore* m_pRenderCore;

	DECLARE_EVENT_TABLE();
};
