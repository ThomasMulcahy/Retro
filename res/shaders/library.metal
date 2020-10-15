#include <metal_stdlib>

using namespace metal;

struct VertexIn
{
    float2 position;
    float4 color;
};

struct UniformsIn
{
    float4x4 orthoMatrix;
};

struct VertexOut
{
    float4 position [[position]];
    float4 color;
};

vertex VertexOut vertexFunction(device VertexIn *vertices [[buffer(0)]], 
                                device UniformsIn &uniforms [[buffer(1)]],
                                uint vid [[vertex_id]])
{
    VertexOut out;
    VertexIn vin = vertices[vid];
    float4 pos = vector_float4(vin.position.x, vin.position.y, 0, 1);
    float4 projPos = uniforms.orthoMatrix * pos;
    out.position = projPos;
    out.color = vertices[vid].color;
    return out;
}

fragment float4 fragmentFunction(VertexOut in [[stage_in]])
{
    return in.color;
}