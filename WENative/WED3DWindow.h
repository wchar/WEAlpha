#pragma once
#include "WED3D.h"

// d3dwindow class for multi window application
class DLL_EXPORT WED3DWindow
{
public:
	WED3DWindow();
	~WED3DWindow();

    // create new window
	HRESULT Create(HWND hwnd);

    // use to resize the back buffer
	HRESULT Resize();
    
    // show the rebder target on screen.
    void Update();

    // D3D only draw image on the focused window.
    void Focus();

    // clear render target view and depth stencil view.
    void Clear();
protected:
	HRESULT CreateViews(UINT width, UINT height);

	HWND m_hwnd;
	
	IDXGISwapChain* m_pSwapChain;
	ID3D11RenderTargetView* m_pRenderTargetView;
	ID3D11Texture2D* m_pDepthStencil;
	ID3D11DepthStencilView* m_pDepthStencilView;
	D3D11_VIEWPORT m_pViewport;

	friend class WED3D;
};

