#version 450

layout(push_constant) uniform PushConstants
{
    vec4 color;
} pc;

layout(location = 0) out vec4 frag_color;

vec2 positions[3] = vec2[](
    vec2(0.0, -0.05),
    vec2(0.05, 0.05),
    vec2(-0.05, 0.05)
);

void main()
{
    vec2 position = positions[gl_VertexIndex];
    gl_Position = vec4(position.x, position.y, 0.0, 1.0);
    frag_color = pc.color;
}