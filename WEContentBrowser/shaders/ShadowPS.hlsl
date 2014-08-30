//--------------------------------------------------------------------------------------
// Globals
//--------------------------------------------------------------------------------------

// This flag enables the shadow to blend between cascades.  This is most useful when the 
// the shadow maps are small and artifact can be seen between the various cascade layers.
#ifndef BLEND_BETWEEN_CASCADE_LAYERS_FLAG
#define BLEND_BETWEEN_CASCADE_LAYERS_FLAG 1
#endif

// There are two methods for selecting the proper cascade a fragment lies in.  Interval selection
// compares the depth of the fragment against the frustum's depth partition.
// Map based selection compares the texture coordinates against the acutal cascade maps.
// Map based selection gives better coverage.  
// Interval based selection is easier to extend and understand.
#ifndef SELECT_CASCADE_BY_INTERVAL_FLAG
#define SELECT_CASCADE_BY_INTERVAL_FLAG 1
#endif

// The number of cascades 
#ifndef CASCADE_COUNT_FLAG
#define CASCADE_COUNT_FLAG 3
#endif

cbuffer cbAllShadowData : register(b0)
{
    float4          g_vCascadeOffset[8];
    float4          g_vCascadeScale[8];

    int             g_nCascadeLevels; // Number of Cascades
    int             g_iVisualizeCascades; // 1 is to visualize the cascades in different colors. 0 is to just draw the scene
    // For Map based selection scheme, this keeps the pixels inside of the the valid range.
    // When there is no boarder, these values are 0 and 1 respectivley.
    float           g_fMinBorderPadding;     
    float           g_fMaxBorderPadding;

                                           
    float           g_fCascadeBlendArea; // Amount to overlap when blending between cascades.
    float           g_fTexelSize; // Padding variables exist because CBs must be a multiple of 16 bytes.
    float           g_fNativeTexelSizeInX;
    float           g_fPadding; 
    float4          g_fCascadeFrustumsEyeSpaceDepthsData[2];  // The values along Z that seperate the cascades.
    // This code creates an array based pointer that points towards the vectorized input data.
    // This is the only way to index arbitrary arrays of data.
    // If the array is used at run time, the compiler will generate code that uses logic to index the correct component.

    static float    g_fCascadeFrustumsEyeSpaceDepths[8] = (float[8])g_fCascadeFrustumsEyeSpaceDepthsData;
    
    float3          g_vLightDir;
    float           g_fPadding2;    
};

Texture2D g_texTexShadow    : register(t0);
Texture2D g_texNormal       : register(t1);
Texture2D g_texInterpPos    : register(t2);
Texture2D g_texPosition     : register(t3);
Texture2D g_texMisc         : register(t4);


Texture2DArray g_texShadow  : register(t5);

SamplerState g_samLinear    : register(s0);
SamplerState g_samShadow    : register(s5);


struct PS_INPUT
{   
    float4 position     : SV_POSITION;
    float2 texcoord     : TEXCOORD0;
};


struct PS_OUTPUT
{
    float light        : SV_TARGET0;
};





PS_OUTPUT PSMain(PS_INPUT input) : SV_TARGET
{
    PS_OUTPUT output;





    return output;
}