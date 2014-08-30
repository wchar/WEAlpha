#pragma once

#include "WED3D.h"

static const int MAX_CASCADES = 8;
static const int MAXIMUM_BLUR_LEVELS = 7;

enum SHADOW_FILTER
{
    SHADOW_FILTER_ANISOTROPIC_16,
    SHADOW_FILTER_ANISOTROPIC_8,
    SHADOW_FILTER_ANISOTROPIC_4,
    SHADOW_FILTER_ANISOTROPIC_2,
    SHADOW_FILTER_LINEAR,
    SHADOW_FILTER_POINT
};


struct CascadeShadowBufferType
{
    XMFLOAT4 m_vCascadeOffset[8];
    XMFLOAT4 m_vCascadeScale[8];

    INT         m_nCascadeLevels; // Number of Cascades
    INT         m_iVisualizeCascades; // 1 is to visualize the cascades in different colors. 0 is to just draw the scene.
    
    // For Map based selection scheme, this keeps the pixels inside of the the valid range.
    // When there is no boarder, these values are 0 and 1 respectivley.
    FLOAT       m_fMinBorderPadding;     
    FLOAT       m_fMaxBorderPadding;

    FLOAT       m_fCascadeBlendArea; // Amount to overlap when blending between cascades.
    FLOAT       m_fTexelSize; // Shadow map texel size.
    FLOAT       m_fNativeTexelSizeInX; // Texel size in native map ( textures are packed ).
    FLOAT       m_fPaddingForCB3;// Padding variables CBs must be a multiple of 16 bytes.
    
    FLOAT       m_fCascadeFrustumsEyeSpaceDepths[8]; // The values along Z that seperate the cascades.   
    
    XMFLOAT4    m_vLightDir;  
    XMFLOAT3    m_vEyePos;
    FLOAT       m_fPadding;
};