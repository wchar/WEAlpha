
cbuffer LightBuffer
{    
	float2 g_f2RTSize;                  // Used by HDAO shaders for scaling texture coords
	float g_fHDAORejectRadius;          // HDAO param
    float g_fHDAOIntensity;             // HDAO param
    float g_fHDAOAcceptRadius;          // HDAO param

	float g_fQ;                         // far / (far - near)
    float g_fQTimesZNear;               // Q * near
    float g_fNormalScale;               // Normal scale
    float g_fAcceptAngle;               // Accept angle
};


Texture2D g_txDepth	 : register(t0);
Texture2D g_txNormal : register(t1);

SamplerState g_SamplePoint : register(s0);

struct PS_INPUT
{    
	float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD;
};

// Gather defines
#define RING_1		1
#define RING_2		2
#define RING_3		3
#define RING_4		4
#define NUM_RING_1_GATHERS		2
#define NUM_RING_2_GATHERS		6
#define NUM_RING_3_GATHERS		12
#define NUM_RING_4_GATHERS		20

// Ring sample pattern
static const float2 g_f2HDAORingPattern[NUM_RING_4_GATHERS] = 
{
    // Ring 1
    { 1, -1 },
    { 0,  1 },
    
    // Ring 2
    { 0,  3 },
    { 2,  1 },
    { 3, -1 },
    { 1, -3 },
        
    // Ring 3
    { 1, -5 },
    { 3, -3 },
    { 5, -1 },
    { 4,  1 },
    { 2,  3 },
    { 0,  5 },
    
    // Ring 4
    { 0,  7 },
    { 2,  5 },
    { 4,  3 },
    { 6,  1 },
    { 7, -1 },
    { 5, -3 },
    { 3, -5 },
    { 1, -7 },
};

// Ring weights
static const float4 g_f4HDAORingWeight[NUM_RING_4_GATHERS] = 
{
    // Ring 1 (Sum = 5.30864)
    { 1.00000, 0.50000, 0.44721, 0.70711 },
    { 0.50000, 0.44721, 0.70711, 1.00000 },
    
    // Ring 2 (Sum = 6.08746)
    { 0.30000, 0.29104, 0.37947, 0.40000 },
    { 0.42426, 0.33282, 0.37947, 0.53666 },
    { 0.40000, 0.30000, 0.29104, 0.37947 },
    { 0.53666, 0.42426, 0.33282, 0.37947 },
    
    // Ring 3 (Sum = 6.53067)
    { 0.31530, 0.29069, 0.24140, 0.25495 },
    { 0.36056, 0.29069, 0.26000, 0.30641 },
    { 0.26000, 0.21667, 0.21372, 0.25495 },
    { 0.29069, 0.24140, 0.25495, 0.31530 },
    { 0.29069, 0.26000, 0.30641, 0.36056 },
    { 0.21667, 0.21372, 0.25495, 0.26000 },
    
    // Ring 4 (Sum = 7.00962)
    { 0.17500, 0.17365, 0.19799, 0.20000 },
    { 0.22136, 0.20870, 0.24010, 0.25997 },
    { 0.24749, 0.21864, 0.24010, 0.28000 },
    { 0.22136, 0.19230, 0.19799, 0.23016 },
    { 0.20000, 0.17500, 0.17365, 0.19799 },
    { 0.25997, 0.22136, 0.20870, 0.24010 },
    { 0.28000, 0.24749, 0.21864, 0.24010 },
    { 0.23016, 0.22136, 0.19230, 0.19799 },
};

static const float g_fRingWeightsTotal[RING_4] =
{
    5.30864,
    11.39610,
    17.92677,
    24.93639,
};

#define NUM_NORMAL_LOADS 4
static const int2 g_i2NormalLoadPattern[NUM_NORMAL_LOADS] = 
{
    { 1,  8 },
    { 8, -1 },
    { 5,  4 },
    { 4, -4 },
};


//--------------------------------------------------------------------------------------
// Used as an early rejection test - based on normals
//--------------------------------------------------------------------------------------
float NormalRejectionTest( int2 i2ScreenCoord )
{
   float3 f3N1;
   float3 f3N2;
   float fDot;
   float fSummedDot = 0.0f;
   int2 i2MirrorPattern;
   int2 i2OffsetScreenCoord;
   int2 i2MirrorOffsetScreenCoord;
 
   for( int iNormal=0; iNormal<NUM_NORMAL_LOADS; iNormal++ )
   {
       i2MirrorPattern = ( g_i2NormalLoadPattern[iNormal] + int2( 1, 1 ) ) * int2( -1, -1 );
       i2OffsetScreenCoord = i2ScreenCoord + g_i2NormalLoadPattern[iNormal];
       i2MirrorOffsetScreenCoord = i2ScreenCoord + i2MirrorPattern;
       
       // Clamp our test to screen coordinates
       i2OffsetScreenCoord = ( i2OffsetScreenCoord > ( g_f2RTSize - float2( 1.0f, 1.0f ) ) ) ? ( g_f2RTSize - float2( 1.0f, 1.0f ) ) : ( i2OffsetScreenCoord );
       i2MirrorOffsetScreenCoord = ( i2MirrorOffsetScreenCoord > ( g_f2RTSize - float2( 1.0f, 1.0f ) ) ) ? ( g_f2RTSize - float2( 1.0f, 1.0f ) ) : ( i2MirrorOffsetScreenCoord );
       i2OffsetScreenCoord = ( i2OffsetScreenCoord < 0 ) ? ( 0 ) : ( i2OffsetScreenCoord );
       i2MirrorOffsetScreenCoord = ( i2MirrorOffsetScreenCoord < 0 ) ? ( 0 ) : ( i2MirrorOffsetScreenCoord );
                       
 
       f3N1.zxy = g_txNormal.Load( int3( i2OffsetScreenCoord, 0) ).xyz;
       f3N2.zxy = g_txNormal.Load( int3( i2MirrorOffsetScreenCoord, 0) ).xyz;
       
       fDot = dot( f3N1, f3N2 );
       
       fSummedDot += ( fDot > g_fAcceptAngle ) ? ( 0.0f ) : ( 1.0f - ( abs( fDot ) * 0.25f ) );
   }
       
   return ( 0.5f + fSummedDot * 0.25f  );
}

//#define USE_NORMAL

float4 PSMain(PS_INPUT input) : SV_TARGET
{	
    // Locals
    float  fCenterZ;
    float2 f2MirrorScreenCoord;
    float2 f2TexCoord;
    float2 f2MirrorTexCoord;
    float4 f4SampledZ[2];
    float4 f4Diff;
    float4 f4Compare[2];
    float4 f4Occlusion = 0.0f;

#ifdef USE_NORMAL   
    float  fCenterNormalZ;    
	float  fOffsetCenterZ;    
	float4 f4OffsetSampledZ[2];
    float4 f4SampledNormalZ[2];
#endif

    float  fDot = 1.0f;
    float2 f2KernelScale = float2( g_f2RTSize.x / 1024.0f, g_f2RTSize.y / 768.0f );
                        
    // Compute integer screen coord, and store off the inverse of the RT Size
    float2 f2InvRTSize = 1.0f / g_f2RTSize;
    float2 f2ScreenCoord = input.texcoord * g_f2RTSize;
    int2   i2ScreenCoord = int2( f2ScreenCoord );
              
    // Test the normals to see if we should apply occlusion
    fDot = NormalRejectionTest( i2ScreenCoord );    

    if( fDot > 0.5f )
    {                
        // Sample the center pixel for camera Z
        f2TexCoord = float2( f2ScreenCoord * f2InvRTSize );
     
		float fDepth = g_txDepth.SampleLevel( g_SamplePoint, f2TexCoord, 0 ).x;
		fCenterZ = -g_fQTimesZNear / ( fDepth - g_fQ );
        
#ifdef USE_NORMAL   
        fCenterNormalZ = g_txNormal.SampleLevel( g_SamplePoint, f2TexCoord, 0 ).x;          
        fOffsetCenterZ = fCenterZ + fCenterNormalZ * g_fNormalScale;
#endif

        // Loop through each gather location, and compare with its mirrored location
        for(int iGather=0; iGather < NUM_RING_4_GATHERS; iGather++ )
        {
            f2MirrorScreenCoord = ( ( f2KernelScale * g_f2HDAORingPattern[iGather] ) + float2( 1.0f, 1.0f ) ) * float2( -1.0f, -1.0f );
            
            f2TexCoord = float2( ( f2ScreenCoord + ( f2KernelScale * g_f2HDAORingPattern[iGather] ) ) * f2InvRTSize );
            f2MirrorTexCoord = float2( ( f2ScreenCoord + ( f2MirrorScreenCoord ) ) * f2InvRTSize );
            

            // Sample   
			f4SampledZ[0] = g_txDepth.Gather( g_SamplePoint, f2TexCoord );
			f4SampledZ[1] = g_txDepth.Gather( g_SamplePoint, f2MirrorTexCoord );				
			
			f4SampledZ[0] = -g_fQTimesZNear.xxxx / ( f4SampledZ[0] - g_fQ.xxxx );
			f4SampledZ[1] = -g_fQTimesZNear.xxxx / ( f4SampledZ[1] - g_fQ.xxxx );
           
                        
            // Detect valleys
            f4Diff = fCenterZ.xxxx - f4SampledZ[0];
            f4Compare[0] = ( f4Diff < g_fHDAORejectRadius.xxxx ) ? ( 1.0f ) : ( 0.0f );
            f4Compare[0] *= ( f4Diff > g_fHDAOAcceptRadius.xxxx ) ? ( 1.0f ) : ( 0.0f );
            
            f4Diff = fCenterZ.xxxx - f4SampledZ[1];
            f4Compare[1] = ( f4Diff < g_fHDAORejectRadius.xxxx ) ? ( 1.0f ) : ( 0.0f );
            f4Compare[1] *= ( f4Diff > g_fHDAOAcceptRadius.xxxx ) ? ( 1.0f ) : ( 0.0f );
            
            f4Occlusion.xyzw += ( g_f4HDAORingWeight[iGather].xyzw * ( f4Compare[0].xyzw * f4Compare[1].zwxy ) * fDot );    
                

#ifdef USE_NORMAL
            // Sample normals
			f4SampledNormalZ[0] = g_txNormal.Gather( g_SamplePoint, f2TexCoord );
			f4SampledNormalZ[1] = g_txNormal.Gather( g_SamplePoint, f2MirrorTexCoord );

                    
            // Scale normals
            f4OffsetSampledZ[0] = f4SampledZ[0] + ( f4SampledNormalZ[0] * g_fNormalScale );
            f4OffsetSampledZ[1] = f4SampledZ[1] + ( f4SampledNormalZ[1] * g_fNormalScale );
              
			            
            // Detect valleys
            f4Diff = fOffsetCenterZ.xxxx - f4OffsetSampledZ[0];
            f4Compare[0] = ( f4Diff < g_fHDAORejectRadius.xxxx ) ? ( 1.0f ) : ( 0.0f );
            f4Compare[0] *= ( f4Diff > g_fHDAOAcceptRadius.xxxx ) ? ( 1.0f ) : ( 0.0f );
            
            f4Diff = fOffsetCenterZ.xxxx - f4OffsetSampledZ[1];
            f4Compare[1] = ( f4Diff < g_fHDAORejectRadius.xxxx ) ? ( 1.0f ) : ( 0.0f );
            f4Compare[1] *= ( f4Diff > g_fHDAOAcceptRadius.xxxx ) ? ( 1.0f ) : ( 0.0f );
            

            f4Occlusion.xyzw += ( g_f4HDAORingWeight[iGather].xyzw * ( f4Compare[0].xyzw * f4Compare[1].zwxy ) * fDot );    
#endif
        }
    }

    // Finally calculate the HDAO occlusion value
#ifdef USE_NORMAL
	float fOcclusion = ( ( f4Occlusion.x + f4Occlusion.y + f4Occlusion.z + f4Occlusion.w ) / ( 3.0f * g_fRingWeightsTotal[RING_4 - 1] ) );
#else
    float fOcclusion = ( ( f4Occlusion.x + f4Occlusion.y + f4Occlusion.z + f4Occlusion.w ) / ( 2.0f * g_fRingWeightsTotal[RING_4 - 1] ) );
#endif

    fOcclusion *= ( g_fHDAOIntensity );
    fOcclusion = 1.0f - saturate( fOcclusion );
    
	return float4(fOcclusion,fOcclusion,fOcclusion,1);
}


