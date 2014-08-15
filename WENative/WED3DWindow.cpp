#include "WED3DWindow.h"


WED3DWindow::WED3DWindow(void)
{
	m_pRenderTargetView = NULL;
	m_pDepthStencil = NULL;
	m_pDepthStencilView = NULL;	
	m_pSwapChain = NULL;

	m_hwnd = NULL;
}


WED3DWindow::~WED3DWindow(void)
{
	SAFE_RELEASE(m_pRenderTargetView);
	SAFE_RELEASE(m_pDepthStencil);
	SAFE_RELEASE(m_pDepthStencilView);		
	SAFE_RELEASE(m_pSwapChain);
}

HRESULT WED3DWindow::Create(HWND hwnd)
{
	HRESULT hr;

	if (!hwnd)
		return S_FALSE;
	m_hwnd = hwnd;

	IDXGIDevice * pDXGIDevice;
	WE::D3DDevice()->QueryInterface(__uuidof(IDXGIDevice), (void **)&pDXGIDevice);

	IDXGIAdapter * pDXGIAdapter;
	pDXGIDevice->GetParent(__uuidof(IDXGIAdapter), (void **)&pDXGIAdapter);

	IDXGIFactory * pIDXGIFactory;
	pDXGIAdapter->GetParent(__uuidof(IDXGIFactory), (void **)&pIDXGIFactory);

	RECT rc;
	GetClientRect(m_hwnd, &rc);
	UINT width = rc.right - rc.left;
	UINT height = rc.bottom - rc.top;

	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 1;
	sd.BufferDesc.Width = width;
	sd.BufferDesc.Height = height;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = m_hwnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;

	V_RETURN(pIDXGIFactory->CreateSwapChain(WE::D3DDevice(), &sd, &m_pSwapChain));

	//// Create a render target view
	//ID3D11Texture2D* pBackBuffer = NULL;
	//V_RETURN(m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer));
    //
	//hr = WE::D3DDevice()->CreateRenderTargetView(pBackBuffer, NULL, &m_pRenderTargetView);
	//pBackBuffer->Release();
	//if(FAILED(hr))
	//	return hr;
    CreateViews(width, height);

	return S_OK;
}

HRESULT WED3DWindow::Resize()
{
	HRESULT hr;

    SAFE_RELEASE(m_pRenderTargetView);
    SAFE_RELEASE(m_pDepthStencil);
    SAFE_RELEASE(m_pDepthStencilView);

	RECT rc;
	GetClientRect(m_hwnd, &rc);
	UINT width = rc.right - rc.left;
	UINT height = rc.bottom - rc.top;

    // release back buffer.
    ID3D11Texture2D* pBackBuffer = NULL;
    V_RETURN(m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer));
    pBackBuffer->Release();

	V_RETURN(m_pSwapChain->ResizeBuffers(2, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, 0));
	V_RETURN(CreateViews(width, height));

	return S_OK;
}

HRESULT WED3DWindow::CreateViews(UINT width, UINT height)
{
	HRESULT hr;

	// Create a render target view
	ID3D11Texture2D* pBackBuffer = NULL;
	V_RETURN(m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer));

	hr = WE::D3DDevice()->CreateRenderTargetView(pBackBuffer, NULL, &m_pRenderTargetView);
	pBackBuffer->Release();
	if(FAILED(hr))
		return hr;

	// Create depth stencil texture
	D3D11_TEXTURE2D_DESC descDepth;
	ZeroMemory(&descDepth, sizeof(descDepth));
	descDepth.Width = width;
	descDepth.Height = height;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;
	V_RETURN(WE::D3DDevice()->CreateTexture2D(&descDepth, NULL, &m_pDepthStencil));

	// Create the depth stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	ZeroMemory(&descDSV, sizeof(descDSV));
	descDSV.Format = descDepth.Format;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;
	V_RETURN(WE::D3DDevice()->CreateDepthStencilView(m_pDepthStencil, &descDSV, &m_pDepthStencilView));

	// Setup the view port
	m_pViewport.Width = (FLOAT)width;
	m_pViewport.Height = (FLOAT)height;
	m_pViewport.MinDepth = 0.0f;
	m_pViewport.MaxDepth = 1.0f;
	m_pViewport.TopLeftX = 0;
	m_pViewport.TopLeftY = 0;

	return S_OK;
}

void WED3DWindow::Update()
{
    m_pSwapChain->Present(0, 0);
}

void WED3DWindow::Focus()
{
    WE::ImmediateContext()->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);
    WE::ImmediateContext()->RSSetViewports(1, &m_pViewport);
}

void WED3DWindow::Clear()
{    
    float ClearColor[4] = { 0.176f, 0.196f, 0.667f, 0.0f };
    WE::ImmediateContext()->ClearRenderTargetView(m_pRenderTargetView, ClearColor);
    WE::ImmediateContext()->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0, 0 );
}