cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix cameraMatrix;
};

struct VertexInputType
{
	float3 position : POSITION;
	float2 texcoord : TEXCOORD;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
};

struct PixelInputType
{
	float4 wndpos: SV_POSITION;
	float3 position : POSITION;
	float2 texcoord : TEXCOORD;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
};

PixelInputType main(VertexInputType input)
{
	PixelInputType output;
	output.wndpos = mul(float4(input.position, 1.0f), worldMatrix);
	output.position = output.wndpos.xyz;
	output.wndpos = mul(output.wndpos, cameraMatrix);
	output.texcoord = input.texcoord;
	output.normal = mul(input.normal, (float3x3)worldMatrix);
	output.tangent = mul(input.tangent, (float3x3)worldMatrix);
	return output;
}