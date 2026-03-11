#version 450
#extension GL_EXT_buffer_reference : require


// TASK: draw another triangle
const vec3 positions[] = vec3[](
        vec3(-0.5f, -0.5f, 0.0f),
        vec3(-0.5f, 0.5f, 0.0f),
        vec3(0.5f, -0.5f, 0.0f)
    );

const vec3 colors[3] = vec3[](
        vec3(1.0f, 0.0f, 0.0f),
        vec3(0.0f, 1.0f, 0.0f),
        vec3(0.0f, 0.0f, 1.0f)
    );

layout(location = 0) out vec3 out_color;
layout(location = 1) out vec2 out_uv;

layout(buffer_reference) buffer VertexDataPtr {
    vec4 positions[3];
};

layout(push_constant) uniform PushConstants {
    layout(offset = 0) mat4 mvp;
    layout(offset = 4*4*4) VertexDataPtr vertexData;
} constants;

void main() {
    vec3 current_pos = constants.vertexData.positions[gl_VertexIndex].xyz;
    vec2 uv = (current_pos.xy + vec2(1.0f, 1.0f)) / 2.0f;

    gl_Position = constants.mvp * vec4(current_pos, 1.0f);
    out_color = colors[gl_VertexIndex % 3];
    out_uv = uv;
}
