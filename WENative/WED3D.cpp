#include "WED3D.h"
#include "WEBaseCamera.h"

WED3D::WED3D()
{	
	m_pd3dDevice = NULL;
	m_pImmediateContext = NULL;

    m_pRenderTargetView = NULL;
    m_pDepthStencil = NULL;
    m_pDepthStencilView = NULL;	
    m_pSwapChain = NULL;

	m_hwnd = NULL;
}

WED3D::~WED3D()
{
	SAFE_RELEASE(m_pd3dDevice);
	SAFE_RELEASE(m_pImmediateContext);

    SAFE_RELEASE(m_pRenderTargetView);
    SAFE_RELEASE(m_pDepthStencil);
    SAFE_RELEASE(m_pDepthStencilView);		
    SAFE_RELEASE(m_pSwapChain);
}

HRESULT WED3D::Create(HWND hwnd)
{
	HRESULT hr = S_OK;

	if (hwnd == NULL)
		return S_FALSE;

	m_hwnd = hwnd;

	RECT rc;
	GetClientRect(hwnd, &rc);
	UINT width = rc.right - rc.left;
	UINT height = rc.bottom - rc.top;
	UINT createDeviceFlags = 0;

	D3D_DRIVER_TYPE driverTypes[] =
	{
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE,
	};
	UINT numDriverTypes = ARRAYSIZE(driverTypes);

	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_0
	};
	UINT numFeatureLevels = ARRAYSIZE(featureLevels);

	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 2;
	sd.BufferDesc.Width = width;
	sd.BufferDesc.Height = height;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = hwnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;

	for(UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
	{
		m_DriverType = driverTypes[driverTypeIndex];
		hr = D3D11CreateDeviceAndSwapChain(NULL, m_DriverType, NULL, createDeviceFlags, featureLevels, numFeatureLevels,
			D3D11_SDK_VERSION, &sd, &m_pSwapChain, &m_pd3dDevice, &m_FeatureLevel, &m_pImmediateContext);
		if (SUCCEEDED(hr))
			break;
	}
	if (FAILED(hr))
		return hr;

    V_RETURN(CreateViews(width, height));

	return S_OK;
}

void WED3D::GetScreen(UINT* width, UINT* height)
{
    RECT rc;
    GetClientRect(m_hwnd, &rc);
    *width = rc.right - rc.left;
    *height = rc.bottom - rc.top;
}

HRESULT WED3D::SetWindow(HWND hWnd)
{	
    HRESULT hr = S_OK;

    m_hwnd = hWnd;

    IDXGIDevice * pDXGIDevice;
    m_pd3dDevice->QueryInterface(__uuidof(IDXGIDevice), (void **)&pDXGIDevice);

    IDXGIAdapter * pDXGIAdapter;
    pDXGIDevice->GetParent(__uuidof(IDXGIAdapter), (void **)&pDXGIAdapter);

    IDXGIFactory * pIDXGIFactory;
    pDXGIAdapter->GetParent(__uuidof(IDXGIFactory), (void **)&pIDXGIFactory);

    RECT rc;
    GetClientRect( hWnd, &rc );
    UINT width = rc.right - rc.left;
    UINT height = rc.bottom - rc.top;

    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory( &sd, sizeof( sd ) );
    sd.BufferCount = 1;
    sd.BufferDesc.Width = width;
    sd.BufferDesc.Height = height;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;

    SAFE_RELEASE(m_pSwapChain);
    pIDXGIFactory->CreateSwapChain( m_pd3dDevice, &sd, &m_pSwapChain );

    V_RETURN(OnResizeWindow());

    return S_OK;
}

HRESULT WED3D::OnResizeWindow()
{
    HRESULT hr;

   // m_pImmediateContext->OMSetRenderTargets(1, NULL, NULL);

    SAFE_RELEASE(m_pRenderTargetView);
    SAFE_RELEASE(m_pDepthStencil);
    SAFE_RELEASE(m_pDepthStencilView);

    RECT rc;
    GetClientRect(m_hwnd, &rc);
    UINT width = rc.right - rc.left;
    UINT height = rc.bottom - rc.top;

    ID3D11Texture2D* pBackBuffer = NULL;
    V_RETURN(m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer));
    pBackBuffer->Release();

    V_RETURN(m_pSwapChain->ResizeBuffers(2, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, 0));
    V_RETURN(CreateViews(width, height));

    m_pImmediateContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);
    m_pImmediateContext->RSSetViewports(1, &m_pViewport);

    // Update camera params.
    WEBaseCamera* pCamera = WE::Camera();
    if (pCamera)
    {
        pCamera->SetProjParams(width, height, pCamera->GetNearClip(), pCamera->GetFarClip());
    }

    return S_OK;
}

HRESULT WED3D::CreateViews(UINT width, UINT height)
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

void WED3D::Clear()
{
    float ClearColor[4] = { 0.4f, 0.7f, 1.0f, 0.0f };
    WE::ImmediateContext()->ClearRenderTargetView(m_pRenderTargetView, ClearColor);
    WE::ImmediateContext()->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0, 0 );
}

void WED3D::Present()
{
    m_pSwapChain->Present(0, 0);
}

void WED3D::ResetRenderTargetView()
{
    m_pImmediateContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);
    m_pImmediateContext->RSSetViewports(1, &m_pViewport);
}

namespace WE
{
    WED3D& D3D()
    {
        static WED3D g_d3d;
        return g_d3d;
    }

    inline ID3D11Device* D3DDevice()
    {
#ifndef DEBUG
        if (!D3D().GetD3DDevice())
        {
            MessageBox(NULL,L"D3DDevice ptr is null, because D3D has not created",L"Error",0x00020000L);
            exit(0);
        }
#endif
        return D3D().GetD3DDevice();
    }

    inline ID3D11DeviceContext* ImmediateContext()
    {
#ifndef DEBUG
        if (!D3D().GetD3DDevice())
        {
            MessageBox(NULL,L"D3DDeviceContext ptr is null, because D3D has not created",L"Error",0x00020000L);
            exit(0);
        }
#endif
        return D3D().GetD3DDeviceContext();
    }


    HRESULT CompileShaderFromFile( 
        WCHAR* szFileName, 
        D3D_SHADER_MACRO* pDefines, 
        LPCSTR szEntryPoint, 
        LPCSTR szShaderModel, 
        ID3DBlob** ppBlobOut)
    {
        HRESULT hr = S_OK;

        DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;

#if defined(DEBUG) || defined(_DEBUG)
        dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

        ID3DBlob* pErrorBlob;
        hr = D3DX11CompileFromFile(szFileName, pDefines, NULL, szEntryPoint, szShaderModel,
            dwShaderFlags, 0, NULL, ppBlobOut, &pErrorBlob, NULL);
        if (FAILED(hr))
        {
            if (pErrorBlob != NULL)
                OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());
            if (pErrorBlob) pErrorBlob->Release();
            return hr;
        }
        if (pErrorBlob) pErrorBlob->Release();

        return S_OK;
    }


    HRESULT CompileShaderFromMemory( 
        LPCSTR pSrcData, 
        SIZE_T SrcDataLen, 
        D3D_SHADER_MACRO* pDefines,
        LPCSTR szEntryPoint, 
        LPCSTR szShaderModel, 
        ID3DBlob** ppBlobOut)
    {
        HRESULT hr = S_OK;

        DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;

#if defined(DEBUG) || defined(_DEBUG)
        dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

        ID3DBlob* pErrorBlob;
        hr = D3DX11CompileFromMemory( pSrcData, SrcDataLen, "None", pDefines, NULL, szEntryPoint, szShaderModel,
            dwShaderFlags, 0, NULL, ppBlobOut, &pErrorBlob, NULL);
        if (FAILED(hr))
        {
            if (pErrorBlob != NULL)
                OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());
            if (pErrorBlob) pErrorBlob->Release();
            return hr;
        }
        if (pErrorBlob) pErrorBlob->Release();

        return S_OK;
    }

    HRESULT CreateBuffer(UINT uElementSize, UINT uCount, VOID* pInitData, ID3D11Buffer** ppBufOut)
    {
        *ppBufOut = NULL;

        D3D11_BUFFER_DESC desc;
        ZeroMemory( &desc, sizeof(desc) );
        desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
        desc.ByteWidth = uElementSize * uCount;
        desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
        desc.StructureByteStride = uElementSize;

        if ( pInitData )
        {
            D3D11_SUBRESOURCE_DATA InitData;
            InitData.pSysMem = pInitData;
            return D3DDevice()->CreateBuffer( &desc, &InitData, ppBufOut );
        }
        else
        {
            return D3DDevice()->CreateBuffer( &desc, NULL, ppBufOut );
        }
    }

    HRESULT CreateBufferSRV( ID3D11Buffer* pBuffer, ID3D11ShaderResourceView** ppSRVOut )
    {
        D3D11_BUFFER_DESC descBuf;
        ZeroMemory( &descBuf, sizeof(descBuf) );
        pBuffer->GetDesc( &descBuf );

        D3D11_SHADER_RESOURCE_VIEW_DESC desc;
        ZeroMemory( &desc, sizeof(desc) );
        desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
        desc.BufferEx.FirstElement = 0;

        if ( descBuf.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS )
        {
            // This is a Raw Buffer
            desc.Format = DXGI_FORMAT_R32_TYPELESS;
            desc.BufferEx.Flags = D3D11_BUFFEREX_SRV_FLAG_RAW;
            desc.BufferEx.NumElements = descBuf.ByteWidth / 4;
        } 
        else if ( descBuf.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_STRUCTURED )
        {
            // This is a Structured Buffer
            desc.Format = DXGI_FORMAT_UNKNOWN;
            desc.BufferEx.NumElements = descBuf.ByteWidth / descBuf.StructureByteStride;
        }
        else
        {
            return E_INVALIDARG;
        }

        return D3DDevice()->CreateShaderResourceView( pBuffer, &desc, ppSRVOut );
    }

    HRESULT CreateBufferUAV(ID3D11Buffer* pBuffer, ID3D11UnorderedAccessView** ppUAVOut )
    {
        D3D11_BUFFER_DESC descBuf;
        ZeroMemory( &descBuf, sizeof(descBuf) );
        pBuffer->GetDesc( &descBuf );

        D3D11_UNORDERED_ACCESS_VIEW_DESC desc;
        ZeroMemory( &desc, sizeof(desc) );
        desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
        desc.Buffer.FirstElement = 0;

        if ( descBuf.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS )
        {
            // This is a Raw Buffer
            desc.Format = DXGI_FORMAT_R32_TYPELESS; // Format must be DXGI_FORMAT_R32_TYPELESS, when creating Raw Unordered Access View
            desc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_RAW;
            desc.Buffer.NumElements = descBuf.ByteWidth / 4; 
        } 
        else if ( descBuf.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_STRUCTURED )
        {
            // This is a Structured Buffer
            desc.Format = DXGI_FORMAT_UNKNOWN;      // Format must be must be DXGI_FORMAT_UNKNOWN, when creating a View of a Structured Buffer
            desc.Buffer.NumElements = descBuf.ByteWidth / descBuf.StructureByteStride; 
        }
        else
        {
            return E_INVALIDARG;
        }

        return D3DDevice()->CreateUnorderedAccessView( pBuffer, &desc, ppUAVOut );
    }

    HRESULT CreateSRVFromFile( const WCHAR* strFile, ID3D11ShaderResourceView** ppSRVout )
    {
        HRESULT hr = S_OK;

        V_RETURN( D3DX11CreateShaderResourceViewFromFile( D3DDevice(), strFile, NULL, NULL, ppSRVout, NULL ) );

        return hr;
    }
}
