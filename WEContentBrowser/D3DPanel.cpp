#include "D3DPanel.h"

BEGIN_EVENT_TABLE( D3DPanel, wxPanel )
    EVT_PAINT( D3DPanel::OnPaint )
    EVT_MOUSE_EVENTS( D3DPanel::OnMouseEvent )
    EVT_SIZE( D3DPanel::OnSize )
END_EVENT_TABLE()

D3DPanel::D3DPanel(wxWindow* parent, WERenderCore* weCore)
    :wxPanel(parent)
{
    m_pWERenderCore = weCore;
    m_pCamera = 0;
}


D3DPanel::~D3DPanel()
{
    SAFE_DELETE(m_pCamera);
}

void D3DPanel::ActiveD3D()
{
    m_pWERenderCore->SetWindow(this->GetHandle());
    WE::SetCamera(m_pCamera);
}

void D3DPanel::OnSize(wxSizeEvent& evt)
{
    SetSize(evt.GetSize());
    m_pWERenderCore->OnResizeWindow();
}

void D3DPanel::OnPaint(wxPaintEvent& evt)
{
    evt.Skip();
}


//-----------------------------------------------------
ModelViewerWindow::ModelViewerWindow(wxWindow* parent, WERenderCore* weCore)
    :D3DPanel(parent, weCore)
{
    m_pCamera = new WEModelViewerCamera();
    WE::SetCamera(m_pCamera);
}

void ModelViewerWindow::OnMouseEvent(wxMouseEvent& evt)
{
    WEModelViewerCamera* pCamera = (WEModelViewerCamera*)m_pCamera;

    wxPoint pt(evt.GetPosition());

    if (evt.LeftDown())
    {
        pCamera->OnBegin(pt.x, pt.y);
        SetFocus();
    }
    else if (evt.Dragging()&&evt.LeftIsDown())
    {
        pCamera->OnMove(pt.x, pt.y);
    }
    else if (evt.LeftUp())
    {
        pCamera->OnEnd();
    }

    if (evt.GetWheelRotation() != 0)
    {
        if (evt.GetWheelRotation() > 0)
            pCamera->Closer();
        else
            pCamera->Further();
    }
}
