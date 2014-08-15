#include "App.h"
#include "Frame.h"

IMPLEMENT_APP(CEApp);

bool CEApp::OnInit()
{

	CEFrame* frame = new CEFrame(0L, _("SPFViewer"));
	frame->SetIcon(wxICON(aaaa)); // To Set App Icon
	frame->Show();
	return true;
}
