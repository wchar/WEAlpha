#pragma once

#include <wx/wx.h>
#include "WENative.h"

class D3DPanel : public wxPanel
{
protected:
    WERenderCore* m_pWERenderCore;
    WEBaseCamera* m_pCamera;

public:
    D3DPanel(wxWindow* parent, WERenderCore* weCore);
    virtual ~D3DPanel();

    void ActiveD3D();
    void OnPaint(wxPaintEvent& evt);
    void OnSize(wxSizeEvent& evt);
    virtual void OnMouseEvent(wxMouseEvent& evt) = 0;
    DECLARE_EVENT_TABLE();
};

class ModelViewerWindow : public D3DPanel
{
public:
    ModelViewerWindow(wxWindow* parent, WERenderCore* weCore);
    void OnMouseEvent(wxMouseEvent& evt);
};
