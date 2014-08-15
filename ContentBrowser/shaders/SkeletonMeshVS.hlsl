//--------------------------------------------------------------------------------------
// Globals
//--------------------------------------------------------------------------------------
#define MAX_BONE_MATRICES 80

cbuffer cbPerObject : register(b0)
{
	matrix g_mWorldViewProjection;
	matrix g_mWorldView;
	matrix g_mWorld;
	matrix g_mShadow;    
	matrix g_mConstBoneWorld[MAX_BONE_MATRICES];
};

//--------------------------------------------------------------------------------------
// Input / Output structures
//--------------------------------------------------------------------------------------
struct VS_INPUT
{
	float4 position		: POSITION;
	float3 normal		: NORMAL;
	float2 texcoord		: TEXCOORD0;	
	float3 tangent		: TANGENT;	    
	float4 weights      : WEIGHTS;		
    uint4  bones        : BONES;			
};

struct VS_OUTPUT
{	
	float4 position		: SV_POSITION;
	float3 normal		: NORMAL;
	float2 texcoord		: TEXCOORD0;	
	float3 tangent		: TANGENT;	
	    
	float4 texShadow	: TEXCOORD1;
    float4 interpPos	: TEXCOORD2; 
    float  depth		: TEXCOORD3;
};


//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_OUTPUT VSMain(VS_INPUT input)
{
	VS_OUTPUT output;
	
	float4 vWorldPos = float4( 0, 0, 0, 0 );
	vWorldPos += mul(input.position, g_mConstBoneWorld[input.bones.x]) * input.weights.x;
	vWorldPos += mul(input.position, g_mConstBoneWorld[input.bones.y]) * input.weights.y;
	vWorldPos += mul(input.position, g_mConstBoneWorld[input.bones.z]) * input.weights.z;
	vWorldPos += mul(input.position, g_mConstBoneWorld[input.bones.w]) * input.weights.w;
    
	float3 vWorldNormal = float3( 0, 0, 0 );
	vWorldNormal += mul(input.normal, (float3x3)g_mConstBoneWorld[input.bones.x]) * input.weights.x;
	vWorldNormal += mul(input.normal, (float3x3)g_mConstBoneWorld[input.bones.y]) * input.weights.y;
	vWorldNormal += mul(input.normal, (float3x3)g_mConstBoneWorld[input.bones.z]) * input.weights.z;
	vWorldNormal += mul(input.normal, (float3x3)g_mConstBoneWorld[input.bones.w]) * input.weights.w;
	
	output.position = mul(float4(vWorldPos.xyz, 1), g_mWorldViewProjection);
	//output.position = mul(input.position, g_mWorldViewProjection);

	output.normal = mul(vWorldNormal, (float3x3)g_mWorld );
	output.tangent = float3(0,0,0);
    output.texcoord = input.texcoord;

	output.texShadow = mul(vWorldPos, g_mShadow);
	output.interpPos = vWorldPos;
	output.depth = mul(vWorldPos, g_mWorldView).z ;

	return output;
}


