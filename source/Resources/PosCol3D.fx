//----------------------------------------------------------------
//		GLOBAL VARIABLES
//----------------------------------------------------------------
float4x4 gWorldViewProj : WORLDVIEWPROJECTION;
float4x4 gWorld;
float4x4 gViewInverse;

Texture2D gDiffuse;
Texture2D gNormal;
Texture2D gSpecular;
Texture2D gGlossiness;

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
    CullMode = front;
    FrontCounterClockwise = false;
};

BlendState gBlendState
{
    BlendEnable[0] = false;
};

DepthStencilState gDepthStencilState
{
    DepthEnable = true;
    DepthWriteMask = all;
};

float3 CalculateSpecularPhong(float3 viewDirection, float3 normal, float2 texCoord)
{
    float3 specularColor = float3(0.0f, 0.0f, 0.0f);
	
    float3 reflectedLightDirection = reflect(gLightDirection, normal);
    float specularStrength = dot(-viewDirection, reflectedLightDirection);
    float shininess = gGlossiness.Sample(samLinear, texCoord);
    specularStrength = saturate(specularStrength);
    specularStrength = pow(specularStrength, gShininess * shininess);
    specularColor = gSpecular.Sample(samLinear, texCoord) * specularStrength;
   
		
    return specularColor;
}

float3 CalculateNewNormal(float3 tangent, float3 normal, float2 texCoord)
{
    float3 newNormal = normal;

    float3 biNormal = normalize(cross(tangent, normal));
	
    float3x3 localAxis = float3x3(tangent, biNormal, normal);
	
    float3 sampledNormal = gNormal.Sample(samLinear, texCoord);
    sampledNormal = sampledNormal * 2.f - 1.f;
	
    newNormal = normalize(mul(sampledNormal, localAxis));
    
    return newNormal;
}

float3 CalculateDiffuseColor(float3 normal, float2 texCoord)
{
    float3 diffuseColor = (float3)0;
    float diffuseLightValue = max(dot(-normal, gLightDirection) * gLightIntensity, 0);
    diffuseColor = gDiffuse.Sample(samLinear, texCoord) * diffuseLightValue;
    
    return diffuseColor;
}

//----------------------------------------------------------------
//		Vertex Shader
//----------------------------------------------------------------
VS_OUTPUT VS(VS_INPUT input)
{
	VS_OUTPUT output = (VS_OUTPUT)0;
	output.Position = mul(input.Position, gWorldViewProj);
    output.WorldPosition = mul(input.Position, gWorld);
    output.normal = mul(input.normal, (float3x3) gWorld);
    output.tangent = mul(input.tangent, (float3x3) gWorld);
	output.TexCoord = input.TexCoord;
	return output;
}

float4 PS_POINT(VS_OUTPUT input) : SV_TARGET
{
    float3 viewDirection = normalize(input.WorldPosition.xyz - gViewInverse[3].xyz);
    
    //Creating the new normal
    float3 newNormal = input.normal;

    float3 biNormal = normalize(cross(input.tangent, input.normal));
	
    float3x3 localAxis = float3x3(input.tangent, biNormal, input.normal);
	
    float3 sampledNormal = gNormal.Sample(samPoint, input.TexCoord);
    sampledNormal = sampledNormal * 2.f - 1.f;
	
    newNormal = normalize(mul(sampledNormal, localAxis));
    
    //Calculating the specular phong
    float3 specularColor = float3(0.0f, 0.0f, 0.0f);
	
    float3 reflectedLightDirection = reflect(gLightDirection, newNormal);
    float specularStrength = dot(-viewDirection, reflectedLightDirection);
    float shininess = gGlossiness.Sample(samPoint, input.TexCoord);
    specularStrength = saturate(specularStrength);
    specularStrength = pow(specularStrength, gShininess * shininess);
    specularColor = gSpecular.Sample(samPoint, input.TexCoord) * specularStrength;
	
    //Calculating the diffuse color
    float3 diffuseColor = (float3) 0;
    float diffuseLightValue = max(dot(-newNormal, gLightDirection) * gLightIntensity, 0);
    diffuseColor = gDiffuse.Sample(samPoint, input.TexCoord) * diffuseLightValue;
    
    return float4(diffuseColor + specularColor, 1.f);
}

float4 PS_LINEAR(VS_OUTPUT input) : SV_TARGET
{
    float3 viewDirection = normalize(input.WorldPosition.xyz - gViewInverse[3].xyz);
    
    //Creating the new normal
    float3 newNormal = input.normal;

    float3 biNormal = normalize(cross(input.tangent, input.normal));
	
    float3x3 localAxis = float3x3(input.tangent, biNormal, input.normal);
	
    float3 sampledNormal = gNormal.Sample(samLinear, input.TexCoord);
    sampledNormal = sampledNormal * 2.f - 1.f;
	
    newNormal = normalize(mul(sampledNormal, localAxis));
    
    //Calculating the specular phong
    float3 specularColor = float3(0.0f, 0.0f, 0.0f);
	
    float3 reflectedLightDirection = reflect(gLightDirection, newNormal);
    float specularStrength = dot(-viewDirection, reflectedLightDirection);
    float shininess = gGlossiness.Sample(samLinear, input.TexCoord);
    specularStrength = saturate(specularStrength);
    specularStrength = pow(specularStrength, gShininess * shininess);
    specularColor = gSpecular.Sample(samLinear, input.TexCoord) * specularStrength;
	
    //Calculating the diffuse color
    float3 diffuseColor = (float3) 0;
    float diffuseLightValue = max(dot(-newNormal, gLightDirection) * gLightIntensity, 0);
    diffuseColor = gDiffuse.Sample(samLinear, input.TexCoord) * diffuseLightValue;
    
    return float4(diffuseColor + specularColor, 1.f);
}

float4 PS_ANISOTROPIC(VS_OUTPUT input) : SV_TARGET
{
    float3 viewDirection = normalize(input.WorldPosition.xyz - gViewInverse[3].xyz);
    
    //Creating the new normal
    float3 newNormal = input.normal;

    float3 biNormal = normalize(cross(input.tangent, input.normal));
	
    float3x3 localAxis = float3x3(input.tangent, biNormal, input.normal);
	
    float3 sampledNormal = gNormal.Sample(samAnisotropic, input.TexCoord);
    sampledNormal = sampledNormal * 2.f - 1.f;
	
    newNormal = normalize(mul(sampledNormal, localAxis));
    
    //Calculating the specular phong
    float3 specularColor = float3(0.0f, 0.0f, 0.0f);
	
    float3 reflectedLightDirection = reflect(gLightDirection, newNormal);
    float specularStrength = dot(-viewDirection, reflectedLightDirection);
    float shininess = gGlossiness.Sample(samAnisotropic, input.TexCoord);
    specularStrength = saturate(specularStrength);
    specularStrength = pow(specularStrength, gShininess * shininess);
    specularColor = gSpecular.Sample(samAnisotropic, input.TexCoord) * specularStrength;
	
    //Calculating the diffuse color
    float3 diffuseColor = (float3) 0;
    float diffuseLightValue = max(dot(-newNormal, gLightDirection) * gLightIntensity, 0);
    diffuseColor = gDiffuse.Sample(samAnisotropic, input.TexCoord) * diffuseLightValue;
    
    return float4(diffuseColor + specularColor, 1.f);
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