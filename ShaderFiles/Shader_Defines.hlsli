

sampler LinearSampler = sampler_state
{
	filter = min_mag_mip_linear;
	AddressU = wrap;
	AddressV = wrap;
};

sampler PointSampler = sampler_state
{
	filter = min_mag_mip_Point;
	AddressU = wrap;
	AddressV = wrap;
};

sampler ClampSampler = sampler_state
{
	filter = min_mag_mip_linear;
	AddressU = clamp;
	AddressV = clamp;
};

sampler BorderSampler = sampler_state
{
    filter = min_mag_mip_linear;
    AddressU = border;
    AddressV = border;
};

sampler VTSampler = sampler_state
{
    filter = min_mag_mip_Point;
    AddressU = Wrap;
    AddressV = Wrap;
};

RasterizerState RS_Default
{
	FillMode = Solid;
	CullMode = Back;
	FrontCounterClockwise = false;
};

RasterizerState RS_Fill_Wireframe
{
	FillMode = Wireframe;
	CullMode = Back;
	FrontCounterClockwise = false;
};

RasterizerState RS_Cull_None
{
	FillMode = Solid;
	CullMode = None;
	FrontCounterClockwise = false;
};


RasterizerState RS_ShadowDepthBias
{
    FillMode = Solid;
    CullMode = Front;
    FrontCounterClockwise = false;
    DepthBias = 8500;
    DepthBiasClamp = 0.0f;
    SlopeScaledDepthBias = 1.0f;
    DepthClipEnable = true;
};

DepthStencilState DSS_Default
{
	DepthEnable = true;
	DepthWriteMask = all;
	DepthFunc = less_equal;
};

DepthStencilState DSS_DepthEnable_WriteZero
{
    DepthEnable = true;
    DepthWriteMask = zero;
};

DepthStencilState DSS_None
{
	DepthEnable = false;
	DepthWriteMask = zero;
};

BlendState BS_Default
{
	BlendEnable[0] = false;
};

BlendState BS_AlphaBlend_Add
{
    BlendEnable[0] = true;
    BlendEnable[1] = true;
    BlendEnable[2] = true;
    BlendEnable[3] = true;

	SrcBlend = SRC_ALPHA;
	DestBlend = Inv_Src_Alpha;
	BlendOp = add;
};

BlendState BS_AlphaBlendEffect
{
    BlendEnable[0] = true;
    BlendEnable[1] = true;

    SrcBlend = Src_Alpha;
    DestBlend = Inv_Src_Alpha;
    BlendOp = Add;

    SrcBlendAlpha = Src_Alpha;
    DestBlendAlpha = Dest_Alpha;
    BlendOpAlpha = MAX;

    //SrcBlend = one;
    //DestBlend = Inv_Src_Alpha;
    //BlendOp = Add;

    //SrcBlendAlpha = one;
    //DestBlendAlpha = Inv_Src_Alpha;
    //BlendOpAlpha = MAX;
};

BlendState BS_Blend_Add
{
	BlendEnable[0] = true;
    BlendEnable[1] = true;
    BlendEnable[2] = true;
    BlendEnable[3] = true;
    BlendEnable[4] = true;

	SrcBlend = one;
	DestBlend = one;
	BlendOp = add;
};

//BlendState BS_Blend_Solid
//{
//    BlendEnable[0] = true;

//    SrcBlend = zero;
//    DestBlend = SrcColor;
//    BlendOp = add;
//};

// ������ RGB���� HSV�� ��ȯ�ϴ� �Լ�
float3 RGBtoHSV(float3 c)
{
    float4 K = float4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
    float4 p = lerp(float4(c.bg, K.wz), float4(c.gb, K.xy), step(c.b, c.g));
    float4 q = lerp(float4(p.xyw, c.r), float4(c.r, p.yzx), step(p.x, c.r));

    float d = q.x - min(q.w, q.y);
    float e = 1.0e-10;
    return float3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}

// HSV���� RGB�� ��ȯ�ϴ� �Լ�
float3 HSVtoRGB(float3 c)
{
	float4 K = float4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
	float3 p = abs(frac(c.xxx + K.xyz) * 6.0 - K.www);
	return c.z * lerp(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

//HSV ���� ���ؼ� �������ִ� �Լ�
float3 AdjustHSV(float3 rgb, float h, float s, float v)
{
	float3 hsv = RGBtoHSV(rgb);
	hsv.x *= h;
	hsv.y *= s;
	hsv.z *= v;
	return  HSVtoRGB(hsv);
}

float4 Color(float r, float g, float b, float a)
{
	return float4(r / 255.f, g / 255.f, b / 255.f, a / 255.f);
}

float3 aces_input_matrix[3] =
{
		float3(0.59719f, 0.35458f, 0.04823f),
		float3(0.07600f, 0.90834f, 0.01566f),
		float3(0.02840f, 0.13383f, 0.83777f)
};

float3 aces_output_matrix[3] =
{
	float3(1.60475f, -0.53108f, -0.07367f),
	float3(-0.10208f,  1.10813f, -0.00605f),
	float3(-0.00327f, -0.07276f, 1.07602f)
};

float3 rtt_and_odt_fit(float3 v)
{
    float3 a = v * (v + 0.0245786f) - 0.000090537f;
    float3 b = v * (0.983729f * v + 0.4329510f) + 0.238081f;
    return a / b;
}

float3 Aces_fitted(float3 v)
{
    //float x = aces_input_matrix[0][0] * v[0] + aces_input_matrix[0][1] * v[1] + aces_input_matrix[0][2] * v[2];
    //float y = aces_input_matrix[1][0] * v[1] + aces_input_matrix[1][1] * v[1] + aces_input_matrix[1][2] * v[2];
    //float z = aces_input_matrix[2][0] * v[1] + aces_input_matrix[2][1] * v[1] + aces_input_matrix[2][2] * v[2];
    
    //v = float3(x, y, z);
    //v = rtt_and_odt_fit(v);
	
    //x = aces_output_matrix[0][0] * v[0] + aces_output_matrix[0][1] * v[1] + aces_output_matrix[0][2] * v[2];
    //y = aces_output_matrix[1][0] * v[1] + aces_output_matrix[1][1] * v[1] + aces_output_matrix[1][2] * v[2];
    //z = aces_output_matrix[2][0] * v[1] + aces_output_matrix[2][1] * v[1] + aces_output_matrix[2][2] * v[2];
    //return float3(x, y, z);
    
    float a = 2.51f;
    float b = 0.03f;
    float c = 2.43f;
    float d = 0.59f;
    float e = 0.14f;
    return saturate((v * (a * v + b)) / (v * (c * v + d) + e));
}


float3 CommerceToneMapping(float3 color)
{
    float startCompression = 0.8 - 0.04;
    float desaturation = 0.15;
    
    float x = min(color.r, min(color.g, color.b));
    float offset = x < 0.08 ? x - 6.25 * x * x : 0.04;
    color -= offset;

    float peak = max(color.r, max(color.g, color.b));
    if (peak < startCompression)
        return color;

    float d = 1. - startCompression;
    float newPeak = 1. - d * d / (peak + d - startCompression);
    color *= newPeak / peak;

    float g = 1. - 1. / (desaturation * (peak - newPeak) + 1.);
    return lerp(color, newPeak * float3(1, 1, 1), g);
}


//For PBR///////////

const static float PI = 3.14159265359;
float RadicalInverse_VdC(uint bits)
{
    bits = (bits << 16u) | (bits >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
    return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}
// ----------------------------------------------------------------------------
//�� ����� Hammersley �ݱ����� ������ �����ϸ�, �̴� �̼��� ���� ��꿡 ������ �����Դϴ�.
//���� �ε����� ���� ������ �Է����� ����ϰ� �ݱ��� ���� ������ ��Ÿ���� 2D ���͸� ��ȯ�մϴ�.
float2 Hammersley(uint i, uint N)
{
    return float2(float(i) / float(N), RadicalInverse_VdC(i));
}

//Microfacet Sampling:
//�� ����� GGX(anisotropic Trowbridge-Reitz) ������ ������� �̼��� ������ ���ø��մϴ�. 2D ������ ����( Xi),
//�̼��� ����( N) �� ��ĥ��( roughness)�� �Է����� ����ϰ� ���� �������� ���ø��� �̼��� ������ ��ȯ�մϴ�.
float3 ImportanceSampleGGX(float2 Xi, float3 N, float roughness)
{
    float a = roughness * roughness;
	
    float phi = 2.0 * PI * Xi.x;
    float cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (a * a - 1.0) * Xi.y));
    float sinTheta = sqrt(1.0 - cosTheta * cosTheta);
	
    // from spherical coordinates to cartesian coordinates
    float3 H;
    H.x = cos(phi) * sinTheta;
    H.y = sin(phi) * sinTheta;
    H.z = cosTheta;
	
    // from tangent-space vector to world-space sample vector
    float3 up = abs(N.z) < 0.999 ? float3(0.0, 0.0, 1.0) : float3(1.0, 0.0, 0.0);
    float3 tangent = normalize(cross(up, N));
    float3 bitangent = cross(N, tangent);
	
    float3 sampleVec = tangent * H.x + bitangent * H.y + N * H.z;
    return normalize(sampleVec);
}

//Microfacet BRDF (Bidirectional Reflectance Distribution Function) Calculations
//�� �Լ��� �־��� �þ� ���⿡�� �̼��� ���ü��� Ȯ���� ��Ÿ���� GGX �̼��� BRDF�� �������� ���� ����մϴ�.
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float a = roughness;
    float k = (a * a) / 2.0;

    float nom = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}
// ----------------------------------------------------------------------------
//�� ����� ���� �þ� ���⿡ ���� GGX ���ü� �� �����Ͽ� �׸��� �� ��ü �׸��� ȿ���� �����մϴ�.
float GeometrySmith(float3 N, float3 V, float3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

//�� ����� ������ ��� �̼��� ������ �����ϱ� ���� �ݱ��� ���� �̼��� BRDF�� ��ġ������ �����մϴ�. 
//�̴� �� ���� ����( NdotV)�� ��ĥ��( roughness)�� 
//�Է����� ���ϰ� A�� Ȯ�� ���� ��Ҹ� ��Ÿ���� B�� BRDF�� �ݻ� ���� ��Ҹ� ��Ÿ���� 2D ���͸� ��ȯ�մϴ�.
float2 IntegrateBRDF(float NdotV, float roughness)
{
    float3 V;
    V.x = sqrt(1.0 - NdotV * NdotV);
    V.y = 0.0;
    V.z = NdotV;

    float A = 0.0;
    float B = 0.0;

    float3 N = float3(0.0, 0.0, 1.0);

    const uint SAMPLE_COUNT = 1024u;
    for (uint i = 0u; i < SAMPLE_COUNT; ++i)
    {
        float2 Xi = Hammersley(i, SAMPLE_COUNT);
        float3 H = ImportanceSampleGGX(Xi, N, roughness);
        float3 L = normalize((2.0 * dot(V, H) * H) - V);

        float NdotL = max(L.z, 0.0);
        float NdotH = max(H.z, 0.0);
        float VdotH = max(dot(V, H), 0.0);

        if (NdotL > 0.0)
        {
            float G = GeometrySmith(N, V, L, roughness);
            float G_Vis = (G * VdotH) / (NdotH * NdotV);
            float Fc = pow(1.0 - VdotH, 5.0);

            A += (1.0 - Fc) * G_Vis;
            B += Fc * G_Vis;
        }
    }
    A /= float(SAMPLE_COUNT);
    B /= float(SAMPLE_COUNT);
    return float2(A, B);
}

//NDF
// GGX �̼��� �𵨿� ���� ���Ժ����Լ�(NDF)�� ����մϴ�. �̼��� ����( n), ���� ����( h) �� ��ĥ��( a)�� 
//�Է����� ����ϰ� NDF ���� ��ȯ�մϴ�.
float NormalDistributionGGXTR(float3 n, float3 h, float a)
{
    float a2 = a * a;
    float NdotH = saturate(dot(n, h));
    float NdotH2 = NdotH * NdotH;

    float nom = a2;
    float denom = (NdotH2 * (a2 - 1.0f) + 1.0f);
    denom = PI * denom * denom;

    return nom / denom;
}

//////////////////////////////////////////////////////
//Trowbridge-Reitz ���� ������� �� �̼��� NDF�� �� �ٸ� �����Դϴ�. 
//NdotH�ݺ���( )�� ��ĥ��( )�� �Է����� ����Ͽ� �̼��� ���� ������ roughness�Է��ϰ� NDF ���� ��ȯ�մϴ�.
float trowbridgeReitzNDF(float NdotH, float roughness) //DistributionGGX
{
    float alpha = roughness * roughness;
    float alpha2 = alpha * alpha;
    float NdotH2 = NdotH * NdotH;
    float denom = ((alpha2 - 1) * NdotH2 + 1);
    float denominator = PI * denom * denom;
    return alpha2 / denominator;
}

//�� �Լ��� ������ ���� ����մϴ�. �̴� ǥ���� ��ǥ������ �ݻ�Ǵ� ������ ������ ��Ĩ�ϴ�. �׵��� ������ ����( )�� 
//�پ��� ������ ���ϰ� F0�Ϲ� ����( NdotV)�� ��ĥ��( roughness)�� �Է����� ���� ������ ������ ���ڸ� ��ȯ�մϴ�.
float3 fresnel(float3 F0, float NdotV, float roughness)
{
    return F0 + (max(1.0 - roughness, F0) - F0) * pow(1.0 - NdotV, 5.0);
}

float3 fresnel(float3 F0, float NdotV)
{
    return F0 + (1 - F0) * pow(1 - NdotV, 5);
}

//Schlick-Beckmann �ٻ縦 ������� �� ������ ���� �� �ٸ� �����Դϴ�.
float schlickBeckmannGAF(float dotProduct, float roughness)
{
    float fRoughness = (roughness + 1.f);
    float k = (fRoughness * fRoughness) / 8.f;
    
    return dotProduct / (dotProduct * (1 - k) + k);
}

//������ ����, �þ߰��� �ڻ���, ��ĥ�⸦ ����� �� �ٸ� ������ �� ���� �� ��� ����.
float3 fresnelSchlickRoughness(float cosTheta, float3 F0, float roughness)
{
    return F0 + (max(1.0 - roughness, F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}
///////////////////


static float3 GetViewSpacePosition(Texture2D DepthTexture, float2 vTexcoord,matrix ProjMatrixInv)
{
    float4 vDepthDesc = DepthTexture.Sample(PointSampler, vTexcoord);
    float fViewZ = vDepthDesc.y * 1000.f;
    float4 vWorldPos;
    vWorldPos.x = vTexcoord.x * 2.f - 1.f;
    vWorldPos.y = vTexcoord.y * -2.f + 1.f;
    vWorldPos.z = vDepthDesc.x;
    vWorldPos.w = 1.0f;

    vWorldPos *= fViewZ;

    return vWorldPos = mul(vWorldPos, ProjMatrixInv);
}


/*For.Fog*/


float ExponentialFog(float dist,float fogStart, float fogDensity)
{
    float fogDistance = max(dist - fogStart, 0.0);
    
    float fog = exp(-fogDistance * fogDensity);
    return 1 - fog;
}

float ExponentialFog(float4 worldSpacePosition, float4 viewSpacePosition, float4 cameraPosition, float fogStart, float fogDensity)
{
    worldSpacePosition /= worldSpacePosition.w;
    float3 worldToCamera = (cameraPosition - worldSpacePosition).xyz;

    float distance = length(worldToCamera);
    float fogDistance = max(distance - fogStart, 0.0);
    
    float fog = exp(-fogDistance * fogDensity);
    return 1 - fog;
}

float ExponentialHeightFog(float4 worldSpacePosition, float4 viewSpacePosition, float4 cameraPosition, float fogStart, float fogDensity, float fogFalloff)
{
    worldSpacePosition /= worldSpacePosition.w;
    float3 cameraToWorld = (worldSpacePosition - cameraPosition).xyz;

    float distance = length(cameraToWorld);
    float fogDist = max(distance - fogStart, 0.0);

    float fogHeightDensityAtViewer = exp(-fogFalloff * cameraPosition.y);
    float fogDistInt = fogDist * fogHeightDensityAtViewer;

	// Height based fog intensity
    float eyeToPixelY = cameraToWorld.y * (fogDist / distance);
    float t = -fogFalloff * eyeToPixelY;
    const float thresholdT = 0.01;
    float fogHeightInt = abs(t) > thresholdT ? (1.0 - exp(-t)) / t : 1.0;

    float fog = exp(-fogDensity * fogDistInt * fogHeightInt);
    return 1 - fog;
}
