#include "Frame.h"
#include "WENative.h"

#include "WEObjImporter.h"

WERenderCore* m_pRenderCore = NULL;
WEObjImporter* m_pObjImporter = NULL;
WEFbxImporter* m_pFbxImporter = NULL;
WEMeshContent* m_pMeshContent = NULL;
WESkeletonMeshContent* m_pSkeletonContent = NULL;
WEMesh* m_pMesh = NULL;
WESkeletonMesh* m_pSkeleton = NULL;


void OnCreate(HWND hwnd)
{    
    // Create RenderCore
    m_pRenderCore = new WERenderCore();
    m_pRenderCore->Create(hwnd);

    // Create Camera
    WE::Camera();
    WEModelViewerCamera* pCamera = new WEModelViewerCamera();
    pCamera->SetProjParams(600, 400, 0, 100.0f);
    pCamera->OnBegin( 0, 0 );
    pCamera->OnMove( 1, 1 );
    pCamera->OnEnd();
    WE::SetCamera(pCamera);

    // Create Camera
    WEModelViewerCamera* pLightCamera = WE::Light();
    //pCamera->SetCamera(XMFLOAT3(10000,0,0),XMFLOAT3(0,0,0));
    pLightCamera->SetProjParams(400, 400, 0, 100.0f);
    pLightCamera->OnBegin( 0, 0 );
    pLightCamera->OnMove( 1, 1 );
    pLightCamera->OnEnd();

    // Impoter
    m_pObjImporter = new WEObjImporter(L"mesh\\oa.obj");
    m_pFbxImporter = new WEFbxImporter(L"mesh\\cub.fbx");
    // Create Content
    m_pMeshContent = new WEMeshContent();
    m_pSkeletonContent = new WESkeletonMeshContent();

    //m_pMeshContent->Import(m_pObjImporter);
    m_pMeshContent->Import(m_pFbxImporter);
    m_pSkeletonContent->Import(m_pFbxImporter);

    // Create Mesh
    m_pMesh = m_pMeshContent->CreateMesh();
    m_pSkeleton = m_pSkeletonContent->CreateMesh();

}


void OnFrame()
{
   if (m_pRenderCore->BeginFrame())
   {
       //m_pRenderCore->PushMesh(m_pMesh);
       static double time = 0;
       time += 0.01;
       m_pSkeleton->TransformMesh(time);
       m_pRenderCore->PushMesh(m_pSkeleton);
       
       m_pRenderCore->EndFrame();
   }
}

void DrawCoordinate()
{		
    struct CB
    {
        XMMATRIX mWorldViewProj;
    };

    ID3D11Device*               pDevice = WE::D3DDevice();
    ID3D11DeviceContext*        pImmediateContext = WE::ImmediateContext();

    static ID3D11VertexShader*  g_pVertexShader = NULL;
    static ID3D11PixelShader*	g_pPixelShader = NULL;
    static ID3D11Buffer*        g_pCB = NULL;
    static const char* g_strBuffer =
        "	cbuffer cbPerObject : register( b0 )										\r\n"
        "	{																			\r\n"
        "		matrix      g_mWorldViewProjection  : packoffset( c0 );		            \r\n"
        "	};																			\r\n"
        "																				\r\n"
        "	struct VS_In																\r\n"
        "	{																			\r\n"
        "		uint    id      	: SV_VERTEXID;										\r\n"
        "	};																			\r\n"
        "																				\r\n"
        "	struct VS_Out																\r\n"
        "	{																			\r\n"
        "		float4 pos			: SV_POSITION;										\r\n"
        "		float4 color        : COLOR;											\r\n"
        "	};																			\r\n"
        "																				\r\n"
        "																				\r\n"
        "	cbuffer cbImmutable															\r\n"
        "	{																			\r\n"
        "		static float4 g_positions[6] =											\r\n"
        "		{																		\r\n"
        "			float4( 0.0f, 0.0f, 0.0f, 1.0f ),									\r\n"
        "			float4( 1.0f, 0.0f, 0.0f, 1.0f ),									\r\n"
        "			float4( 0.0f, 0.0f, 0.0f, 1.0f ),									\r\n"
        "			float4( 0.0f, 1.0f, 0.0f, 1.0f ),									\r\n"
        "			float4( 0.0f, 0.0f, 0.0f, 1.0f ),									\r\n"
        "			float4( 0.0f, 0.0f, 1.0f, 1.0f ),        							\r\n"
        "		};																		\r\n"
        "																				\r\n"
        "		static float4 g_colors[6] =												\r\n"
        "		{																		\r\n"
        "			float4( 1.0f, 0.0f, 0.0f, 1.0f ),									\r\n"
        "			float4( 1.0f, 0.0f, 0.0f, 1.0f ),									\r\n"
        "			float4( 0.0f, 1.0f, 0.0f, 1.0f ),									\r\n"
        "			float4( 0.0f, 1.0f, 0.0f, 1.0f ),									\r\n"
        "			float4( 0.0f, 0.0f, 1.0f, 1.0f ),									\r\n"
        "			float4( 0.0f, 0.0f, 1.0f, 1.0f ),        							\r\n"
        "		};																		\r\n"
        "	};																			\r\n"
        "	VS_Out VSMain(VS_In input)													\r\n"
        "	{																			\r\n"
        "		VS_Out output;															\r\n"
        "																				\r\n"
        "		output.pos = mul( g_positions[input.id] * 5, g_mWorldViewProjection );	\r\n"
        "		output.color = g_colors[input.id];										\r\n"
        "		return output;															\r\n"
        "	}																			\r\n"
        "																	    		\r\n"
        "	float4 PSMain(VS_Out input) : SV_Target							            \r\n"
        "	{   																		\r\n"
        "		return input.color;														\r\n"
        "		return float4( 1.0f, 1.0f, 0.0f, 1.0f );								\r\n"
        "	}																			\r\n"
        "";

    HRESULT hr;

    if ( g_pVertexShader == NULL )
    {
        // Create shaders
        ID3DBlob* pVSBlob = NULL;
        ID3DBlob* pPSBlob = NULL;

        UINT dwBufferSize = ( UINT )strlen( g_strBuffer ) + 1;
        V( WE::CompileShaderFromMemory( g_strBuffer, dwBufferSize, NULL, "VSMain", "vs_5_0", &pVSBlob ) );
        V( WE::CompileShaderFromMemory( g_strBuffer, dwBufferSize, NULL, "PSMain", "ps_5_0", &pPSBlob ) );

        V( pDevice->CreateVertexShader( pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), NULL, &g_pVertexShader ) );
        V( pDevice->CreatePixelShader( pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), NULL, &g_pPixelShader ) );

        SAFE_RELEASE( pVSBlob );
        SAFE_RELEASE( pPSBlob );

        // Create the constant buffers
        D3D11_BUFFER_DESC Desc;
        Desc.Usage = D3D11_USAGE_DYNAMIC;
        Desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        Desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        Desc.MiscFlags = 0;
        Desc.ByteWidth = sizeof( CB );
        V( pDevice->CreateBuffer( &Desc, NULL, &g_pCB ) );
    }

    D3D11_MAPPED_SUBRESOURCE MappedResource;
    V(pImmediateContext->Map( g_pCB, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource ) );
    CB* pPerFrame = ( CB* )MappedResource.pData;
    pPerFrame->mWorldViewProj = XMMatrixTranspose( WE::Camera()->GetViewProjMtx() );
    pImmediateContext->Unmap( g_pCB, 0 );
    pImmediateContext->VSSetConstantBuffers( 0, 1, &g_pCB );

    pImmediateContext->OMSetDepthStencilState( NULL, 0 );
    float vBlendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
    pImmediateContext->OMSetBlendState( NULL, vBlendFactor, 0xFFFFFFFF );
    pImmediateContext->RSSetState( NULL );

    pImmediateContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_LINELIST );

    pImmediateContext->VSSetShader( g_pVertexShader, NULL, 0 );
    pImmediateContext->PSSetShader( g_pPixelShader, NULL, 0 );
    pImmediateContext->GSSetShader( NULL, NULL, 0 );

    // Draw
    pImmediateContext->Draw( 6, 0 );
}



BEGIN_EVENT_TABLE( CEFrame, wxFrame )
	EVT_CLOSE( CEFrame::OnClose )
	EVT_MENU( idMenuQuit, CEFrame::OnQuit )

    EVT_PAINT( CEFrame::OnPaint )
    EVT_MOUSE_EVENTS( CEFrame::OnMouseEvent )
    EVT_SIZE( CEFrame::OnSize )
    EVT_TIMER( idTimer, CEFrame::OnTimer )
END_EVENT_TABLE()

CEFrame::CEFrame( wxFrame *frame, const wxString& title )
	: wxFrame( frame, -1, title, wxDefaultPosition, wxSize( 1100,700 ) ),
    m_Timer( this, idTimer )
{

	SetMinSize( wxSize( 600, 400 ) );

    OnCreate(GetHandle());

    m_pRenderCore->SetWindow(GetHandle());

    m_Timer.Start( 6 );
}

CEFrame::~CEFrame()
{

}

void CEFrame::OnClose( wxCloseEvent &event )
{
	Destroy();
}

void CEFrame::OnQuit( wxCommandEvent &event )
{
	Destroy();
}


void CEFrame::OnSize( wxSizeEvent& evt )
{
    m_pRenderCore->OnResizeWindow();    
    OnFrame();
}

void CEFrame::OnMouseEvent( wxMouseEvent& evt )
{
    wxPoint pt( evt.GetPosition() );
    WEModelViewerCamera* pCamera = (WEModelViewerCamera*)WE::Camera();
    if (GetAsyncKeyState( 'Q' ))
        pCamera = WE::Light();
    if ( evt.LeftDown() )
    {
        pCamera->OnBegin( pt.x, pt.y );
        SetFocus();
    }
    else if ( evt.Dragging()&&evt.LeftIsDown() )
    {
        pCamera->OnMove( pt.x, pt.y );
        //CEGetCascadeSystem()->OnFrame();
    }
    else if ( evt.LeftUp() )
    {
        pCamera->OnEnd();
    }

    if ( evt.GetWheelRotation() != 0 )
    {
        if ( evt.GetWheelRotation() > 0 )
            pCamera->Closer();
        else
            pCamera->Further();
        //CEGetCascadeSystem()->OnFrame();
    }
    /**/
}

void CEFrame::OnTimer( wxTimerEvent& event )
{
    OnFrame();
}

void CEFrame::OnPaint( wxPaintEvent& evt )
{
    // render
    OnFrame();

    evt.Skip();
}