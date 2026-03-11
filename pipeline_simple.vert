#version 450

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

layout(set = 0, binding = 0) uniform Input {
    vec4 positions[3];
} srcBuffer;

layout(push_constant) uniform PushConstants {
    layout(offset = 0) mat4 mvp;
} constants;

void main() {
    vec3 current_pos = srcBuffer.positions[gl_VertexIndex].xyz;

    gl_Position = constants.mvp * vec4(current_pos, 1.0f);
    out_color = colors[gl_VertexIndex % 3];
}
