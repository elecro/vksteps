#version 450
#extension GL_EXT_nonuniform_qualifier : enable

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
    uint textureIdx;
    if (in_uv.x < 0.5f) {
        textureIdx = constants.textureIdx;
    } else {
        textureIdx = (constants.textureIdx == 1) ? 0 : 1;
    }

    vec4 pixel = texture(images[nonuniformEXT(textureIdx)], in_uv);
    out_color = vec4(pixel.rgb, 1.0f);
}
