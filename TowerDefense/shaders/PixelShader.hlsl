struct PixelInputType
{
	float4 wndpos : SV_POSITION;
	float  depth : DEPTH;
	float3 normal : NORMAL;
};

float4 main(PixelInputType input) : SV_TARGET
{
	return float4(normalize(input.normal), input.depth);
}