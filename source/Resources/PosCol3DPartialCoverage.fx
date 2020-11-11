//----------------------------------------------------------------
//		GLOBAL VARIABLES
//----------------------------------------------------------------
float4x4 gWorldViewProj : WORLDVIEWPROJECTION;
float4x4 gWorld;
float4x4 gViewInverse;

Texture2D gDiffuse;
bool gUseTransparency;

float3 gLightDirection = float3(0.577f, -0.577f, 0.577f);
float gPI = 3.1415f;
float gLightIntensity = 2.f;
float gShininess = 25.f;

//----------------------------------------------------------------
//		INPUT/OUTPUT Structs
//----------------------------------------------------------------
struct VS_INPUT
{
    float4 Position : POSITION;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float2 TexCoord : TEXCOORD;
};

struct VS_OUTPUT
{
    float4 Position : SV_POSITION;
    float4 WorldPosition : COLOR;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float2 TexCoord : TEXCOORD;
};

SamplerState samPoint
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = Border;
    AddressV = Clamp;
    BorderColor = float4(0.f, 0.f, 1.f, 1.f);
};

SamplerState samLinear
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Border;
    AddressV = Clamp;
    BorderColor = float4(0.f, 0.f, 1.f, 1.f);
};

SamplerState samAnisotropic
{
    Filter = ANISOTROPIC;
    AddressU = Border;
    AddressV = Clamp;
    BorderColor = float4(0.f, 0.f, 1.f, 1.f);
};

RasterizerState FrontCulling
{
    CullMode = none;
    FrontCounterClockwise = true;
};

BlendState gBlendState
{
    BlendEnable[0] = true;
    SrcBlend = src_alpha;
    DestBlend = inv_src_alpha;
    BlendOp = add;
    SrcBlendAlpha = zero;
    DestBlendAlpha = zero;
    BlendOpAlpha = add;
    RenderTargetWriteMask[0] = 0x0F;
};

DepthStencilState gDepthStencilState
{
    DepthEnable = true;
    DepthWriteMask = zero;
    DepthFunc = less;
    StencilEnable = false;

    StencilReadMask = 0x0F;
    StencilWriteMask = 0x0F;

    FrontFaceStencilFunc = always;
    BackFaceStencilFunc = always;

    FrontFaceStencilDepthFail = keep;
    BackFaceStencilDepthFail = keep;

    FrontFaceStencilPass = keep;
    BackFaceStencilPass = keep;

    FrontFaceStencilFail = keep;
    BackFaceStencilFail = keep;
};

//----------------------------------------------------------------
//		Vertex Shader
//----------------------------------------------------------------
VS_OUTPUT VS(VS_INPUT input)
{
    VS_OUTPUT output = (VS_OUTPUT) 0;
    output.Position = mul(input.Position, gWorldViewProj);
    output.WorldPosition = mul(input.Position, gWorld);
    output.normal = mul(input.normal, (float3x3) gWorld);
    output.tangent = mul(input.tangent, (float3x3) gWorld);
    output.TexCoord = input.TexCoord;
    return output;
}

float4 PS_POINT(VS_OUTPUT input) : SV_TARGET
{
   
    //Calculating the diffuse color
    float4 diffuseColor = (float4) 0;
    diffuseColor = gDiffuse.Sample(samPoint, input.TexCoord);
    
    if (!gUseTransparency)
        diffuseColor.a = 1.f;
    
    return diffuseColor;
}

float4 PS_LINEAR(VS_OUTPUT input) : SV_TARGET
{
    //Calculating the diffuse color
    float4 diffuseColor = (float4) 0;
    diffuseColor = gDiffuse.Sample(samLinear, input.TexCoord);
    
    if (!gUseTransparency)
        diffuseColor.a = 1.f;
    
    return diffuseColor;
}

float4 PS_ANISOTROPIC(VS_OUTPUT input) : SV_TARGET
{
    //Calculating the diffuse color
    float4 diffuseColor = (float4) 0;
    diffuseColor = gDiffuse.Sample(samAnisotropic, input.TexCoord);
    
    if (!gUseTransparency)
        diffuseColor.a = 1.f;
    
    return diffuseColor;
}

//----------------------------------------------------------------
//		Technique
//----------------------------------------------------------------
technique11 DefaultTechnique
{
    pass P0
    {
        SetRasterizerState(FrontCulling);
        SetDepthStencilState(gDepthStencilState, 0);
        SetBlendState(gBlendState, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS_POINT()));
    }
}

technique11 LinearTechnique
{
    pass P0
    {
        SetRasterizerState(FrontCulling);
        SetDepthStencilState(gDepthStencilState, 0);
        SetBlendState(gBlendState, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS_LINEAR()));
    }
}

technique11 AnisotropicTechnique
{
    pass P0
    {
        SetRasterizerState(FrontCulling);
        SetDepthStencilState(gDepthStencilState, 0);
        SetBlendState(gBlendState, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS_ANISOTROPIC()));
    }
}