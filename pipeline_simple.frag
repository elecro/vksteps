#version 450
#extension GL_ARB_gpu_shader_int64 : enable

layout(set = 0, binding = 0) uniform sampler2D images[8];

layout(location = 0) in vec3 in_color;
layout(location = 1) in vec2 in_uv;

layout(location = 0) out vec4 out_color;

layout(push_constant) uniform PushConstants {
    /*layout(offset = 0) mat4 mvp;
    layout(offset = 4*4*4) uint64_t vertexData;*/
    layout(offset = 4*4*4 + 8) uint textureIdx;
} constants;


void main() {
    vec4 pixel = texture(images[constants.textureIdx], in_uv);
    out_color = vec4(pixel.rgb, 1.0f);
}
