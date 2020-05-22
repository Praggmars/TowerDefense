cbuffer MatrixBuffer {
    matrix projectionMatrix;
};
struct VertexInputType {
    float3 position : POSITION;
    /*...*/
};
struct PixelInputType {
    float4 screen : SV_POSITION;
    float3 position : POSITION;
};

PixelInputType main(VertexInputType input) {
    PixelInputType output;
    output.position = input.position;
    output.screen = mul(float4(input.position, 1.0f), projectionMatrix);
    return output;
}