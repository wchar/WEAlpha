#pragma once

#define SAFE_DELETE(p) { if(p) { delete (p);     (p)=NULL; } }
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }
#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }

#include <d3d11.h>
#include <d3dx11.h>
#include <d3d11shader.h>
#include <D3Dcompiler.h>
#include <xnamath.h>

#include <DxErr.h>

#ifndef V
#define V(x) { hr = (x); if( FAILED(hr) ) { MessageBox(NULL,L#x,L"Error",0x00020000L);} }
#endif
#ifndef V_RETURN
#define V_RETURN(x) { hr = (x); if( FAILED(hr) ) { MessageBox(NULL,L#x,L"Error",0x00020000L);} }
#endif

#define  SIMPLEDLL_EXPORT

#ifdef SIMPLEDLL_EXPORT
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT __declspec(dllimport)
#endif

#include <vector>
#include <list>
#include <set>
#include <hash_set>

using namespace std;

class WED3D;

namespace WE
{
    // Get D3D singleton.
	DLL_EXPORT WED3D& D3D();
    // Get d3ddevice.
	DLL_EXPORT ID3D11Device* D3DDevice();
    // Get immediate context.
	DLL_EXPORT ID3D11DeviceContext* ImmediateContext();

    DLL_EXPORT HRESULT CompileShaderFromFile( WCHAR* szFileName, 
        D3D_SHADER_MACRO* pDefines, 
        LPCSTR szEntryPoint, 
        LPCSTR szShaderModel, 
        ID3DBlob** ppBlobOut );

    DLL_EXPORT HRESULT CompileShaderFromMemory( LPCSTR pSrcData, 
        SIZE_T SrcDataLen, 
        D3D_SHADER_MACRO* pDefines, 
        LPCSTR szEntryPoint, 
        LPCSTR szShaderModel, 															
        ID3DBlob** ppBlobOut );

    DLL_EXPORT HRESULT CreateBuffer( UINT uElementSize, UINT uCount, VOID* pInitData, ID3D11Buffer** ppBufOut );
    DLL_EXPORT HRESULT CreateBufferSRV( ID3D11Buffer* pBuffer, ID3D11ShaderResourceView** ppSRVOut );
    DLL_EXPORT HRESULT CreateBufferUAV( ID3D11Buffer* pBuffer, ID3D11UnorderedAccessView** ppUAVOut );
    DLL_EXPORT HRESULT CreateSRVFromFile( const WCHAR* strFile, ID3D11ShaderResourceView** ppSRVout );
}

class DLL_EXPORT WED3D
{
public:
	friend WED3D& WE::D3D();

	// The hwnd is the window handle will be render for
	HRESULT Create(HWND hwnd);

    // Use this function to get width and height of the window.
    void GetScreen(UINT* width, UINT* height);

    //Use this function to change the window/
    HRESULT SetWindow(HWND hwnd);

    // This function should be called when the window was change.
    HRESULT OnResizeWindow();

    // This function clear the render target view and the depth stencil view.
    void Clear();

    // This function show the render result on the window.
    void Present();

	// Getters
	ID3D11Device* GetD3DDevice()
    {
        return m_pd3dDevice;
    }

	ID3D11DeviceContext* GetD3DDeviceContext()
    {
        return m_pImmediateContext;
    }

    ID3D11RenderTargetView* GetRenderTargetView()
    {
        return m_pRenderTargetView;
    }

    ID3D11Texture2D* GetDepthStencil()
    {
        return m_pDepthStencil;
    }

    ID3D11DepthStencilView* GetDepthStencilView()
    {
        return m_pDepthStencilView;
    }

    void ResetRenderTargetView();
private:
	WED3D();
	~WED3D();

	HWND                        m_hwnd;
	
	D3D_DRIVER_TYPE				m_DriverType;
	D3D_FEATURE_LEVEL			m_FeatureLevel;

	ID3D11Device*               m_pd3dDevice;
	ID3D11DeviceContext*        m_pImmediateContext;
	
	IDXGISwapChain*             m_pSwapChain;
	ID3D11RenderTargetView*     m_pRenderTargetView;
	ID3D11Texture2D*            m_pDepthStencil;
	ID3D11DepthStencilView*     m_pDepthStencilView;
    HRESULT CreateViews(UINT width, UINT height);

    D3D11_VIEWPORT              m_pViewport;
};

