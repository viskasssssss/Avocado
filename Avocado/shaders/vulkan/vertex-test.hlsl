static const float2 positions[3] =
{
    float2(0.0, -0.05),
	float2(0.05, 0.05),
	float2(-0.05, 0.05)
};

static const float3 colors[3] =
{
    float3(1.0, 0.0, 0.0),
	float3(0.0, 1.0, 0.0),
	float3(0.0, 0.0, 1.0)
};

//struct Constants
//{
//    float4x4 model;
//};
//
//[[vk::push_constant]] Constants constants;

struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float3 frag_color : TEXCOORD0;
};
VS_OUTPUT main(uint vertex_id : SV_VertexID)
{
    VS_OUTPUT Output;
    Output.position = float4(positions[vertex_id].x, positions[vertex_id].y, 0.0, 1.0);
    Output.frag_color = colors[vertex_id];
    
    return Output;
}