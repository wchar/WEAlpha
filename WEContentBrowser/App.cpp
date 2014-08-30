#include "App.h"
#include "MainFrame.h"


///////////////////////////////////////////////////////////////////////////////
/// Class PrepareView
///////////////////////////////////////////////////////////////////////////////
class PrepareView : public wxFrame 
{
public:
    PrepareView( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 487,273 ), long style = 0|wxTAB_TRAVERSAL )
        : wxFrame( parent, id, title, pos, size, style )
    {
        this->SetSizeHints( wxDefaultSize, wxDefaultSize );
        this->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_HIGHLIGHTTEXT ) );

        wxBoxSizer* bSizer38;
        bSizer38 = new wxBoxSizer( wxVERTICAL );

        wxStaticBitmap* m_bitmap2;
        m_bitmap2 = new wxStaticBitmap( this, wxID_ANY, wxBitmap( wxT("res/windmill.jpg"), wxBITMAP_TYPE_ANY ), wxDefaultPosition, wxDefaultSize, 0 );
        m_bitmap2->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_BTNHIGHLIGHT ) );

        bSizer38->Add( m_bitmap2, 1, wxEXPAND, 5 );


        this->SetSizer( bSizer38 );
        this->Layout();

        this->Centre( wxBOTH );
    }

};

IMPLEMENT_APP(CEApp);


MainFrame* g_mainFrame;
PrepareView* g_pPrepareView;

// Function for simulation
DWORD WINAPI InitFrame(LPVOID p)
{	
    g_mainFrame->InitSystem();    
    
    g_pPrepareView->Destroy();

    g_mainFrame->Show(); 
    return 0;
}


bool CEApp::OnInit()
{    
    wxApp::OnInit();
        
    wxInitAllImageHandlers();

	g_mainFrame = new MainFrame(0L);

    g_pPrepareView = new PrepareView(g_mainFrame);
    g_pPrepareView->Show(true);
    	
    DWORD threadId;
    CreateThread(NULL,0,InitFrame,(LPVOID)0,0,&threadId);

	return true;
}
