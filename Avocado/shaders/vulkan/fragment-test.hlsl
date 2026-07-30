struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float3 frag_color : TEXCOORD0;
};


float4 main(VS_OUTPUT Input) : SV_TARGET
{
    return float4(Input.frag_color, 1.0);
}