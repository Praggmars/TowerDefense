Texture2D texture_diffuse;
Texture2D texture_normal;
Texture2D texture_shadowMap;
SamplerState textureSampler;
SamplerComparisonState shadowMapSampler;

cbuffer LightBuffer
{
	float4 light_diffuseColor;
	float4 light_ambientColor;
	float3 light_sourcePosition;
	float  light_padding1;
	float3 light_eyePosition;
	float  light_padding2;
};

cbuffer MaterialBuffer
{
	float4 material_diffuseColor;
	float4 material_specularColor;
	float  material_textureWeight;
	float  material_specularPower;
	float  material_padding[2];
};

struct PixelInputType
{
	float4 wndpos   : SV_POSITION;
	float3 position : POSITION;
	float2 texcoord : TEXCOORD0;
	float3 normal   : NORMAL;
	float3 tangent  : TANGENT;
	float4 lightTex : TEXCOORD1;
};

float4 PixelColor(float2 texCoord)
{
	return texture_diffuse.Sample(textureSampler, texCoord) * material_textureWeight + material_diffuseColor * (1.0f - material_textureWeight);
}
float3 PixelNormal(float3 normal, float3 tangent, float2 texCoord)
{
	tangent = normalize(tangent - dot(tangent, normal) * normal);
	normal = normalize(normal);
	float3 bitangent = cross(tangent, normal);
	float4 bumpmap = texture_normal.Sample(textureSampler, texCoord) * 2.0f - 1.0f;
	return normalize(mul(bumpmap.xyz, float3x3(tangent, bitangent, normal)));
}
float ShadowFactor(float4 lightTex)
{
	lightTex.xyz /= lightTex.w;
	const float d = 1.0f / 2048.0f;
	float percentLit = 0.0f;
	const float2 offsets[] = {
		float2(-d, -d), float2(0.0f, -d), float2(d, -d),
		float2(-d, 0.0f), float2(0.0f, 0.0f), float2(d, 0.0f),
		float2(-d, d), float2(0.0f, d), float2(d, d)
	};
	percentLit += texture_shadowMap.SampleCmpLevelZero(shadowMapSampler, lightTex.xy + offsets[0], lightTex.z).r;
	percentLit += texture_shadowMap.SampleCmpLevelZero(shadowMapSampler, lightTex.xy + offsets[1], lightTex.z).r;
	percentLit += texture_shadowMap.SampleCmpLevelZero(shadowMapSampler, lightTex.xy + offsets[2], lightTex.z).r;
	percentLit += texture_shadowMap.SampleCmpLevelZero(shadowMapSampler, lightTex.xy + offsets[3], lightTex.z).r;
	percentLit += texture_shadowMap.SampleCmpLevelZero(shadowMapSampler, lightTex.xy + offsets[4], lightTex.z).r;
	percentLit += texture_shadowMap.SampleCmpLevelZero(shadowMapSampler, lightTex.xy + offsets[5], lightTex.z).r;
	percentLit += texture_shadowMap.SampleCmpLevelZero(shadowMapSampler, lightTex.xy + offsets[6], lightTex.z).r;
	percentLit += texture_shadowMap.SampleCmpLevelZero(shadowMapSampler, lightTex.xy + offsets[7], lightTex.z).r;
	percentLit += texture_shadowMap.SampleCmpLevelZero(shadowMapSampler, lightTex.xy + offsets[8], lightTex.z).r;
	return percentLit / 9.0f;
}

float4 main(PixelInputType input) : SV_TARGET
{
	float4 specular = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 pixelColor = PixelColor(input.texcoord);
	float3 pixelNormal = PixelNormal(input.normal, input.tangent, input.texcoord);

	float3 viewDirection = normalize(light_eyePosition - input.position);
	float3 lightDirection = normalize(light_sourcePosition - input.position);

	input.lightTex.x = input.lightTex.x / input.lightTex.w * 0.5f + 0.5f;
	input.lightTex.y = input.lightTex.y / input.lightTex.w * (-0.5f) + 0.5f;
	input.lightTex.z = input.lightTex.z / input.lightTex.w;
	float intensity = (texture_shadowMap.SampleCmpLevelZero(shadowMapSampler, input.lightTex.xy, input.lightTex.z).r < input.lightTex.z) ? 0.0f : saturate(dot(pixelNormal, lightDirection));

	float4 color = light_ambientColor;

	if (intensity > 0.0f)
	{
		color = saturate(color + light_diffuseColor * intensity);
		float3 reflection = normalize(2.0f * intensity * pixelNormal - lightDirection);
		reflection = reflect(-lightDirection, pixelNormal);
		specular = pow(saturate(dot(reflection, viewDirection)), material_specularPower);
		specular *= material_specularColor;
	}
	color = color * pixelColor;

	color = saturate(color + specular);
	color.w = pixelColor.w;
	return color;
}