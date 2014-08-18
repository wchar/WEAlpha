#include "App.h"
#include "MainFrame.h"

IMPLEMENT_APP(CEApp);

bool CEApp::OnInit()
{
    wxInitAllImageHandlers();
	MainFrame* frame = new MainFrame(0L);
	frame->Show();
	return true;
}
