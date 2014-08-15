cbuffer LightBuffer
{
    float3 lightDirection;
    float padding;
};

Texture2D colorTexture : register(t0);
Texture2D normalTexture : register(t1);

SamplerState samplePoint : register(s0);

struct PS_INPUT
{    
	float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD;
};

float PSMain(PS_INPUT input) : SV_TARGET
{	
    float4 colors = colorTexture.Sample(samplePoint, input.texcoord);

    float4 normals = normalTexture.Sample(samplePoint, input.texcoord);

    // Invert the light direction for calculations.
    float3 lightDir = -lightDirection;
    
	// Calculate the amount of light on this pixel.
    float lightIntensity = saturate(dot(normals.xyz, lightDir));

	return lightIntensity;
    // Determine the final amount of diffuse color based on the color of the pixel combined with the light intensity.
    float4 outputColor = colors * lightIntensity;

    return outputColor;
}


