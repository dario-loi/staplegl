#type vertex
#version 420 core
#line 3

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

void main() {
    TexCoord = aTexCoord;
    gl_Position = vec4(aPos, 1.0);
}

#type fragment
#version 420 core
#line 17

in vec2 TexCoord;

uniform sampler2D scene;
uniform vec2 uResolution;

layout(location = 0) out vec3 downsample;

void main() {

    vec2 src_uv = 1.0F / uResolution;
    float x = src_uv.x;
    float y = src_uv.y;

    vec3 a = texture(scene, vec2(TexCoord.x - 2 * x, TexCoord.y + 2 * y)).rgb;
    vec3 b = texture(scene, vec2(TexCoord.x, TexCoord.y + 2 * y)).rgb;
    vec3 c = texture(scene, vec2(TexCoord.x + 2 * x, TexCoord.y + 2 * y)).rgb;

    vec3 d = texture(scene, vec2(TexCoord.x - 2 * x, TexCoord.y)).rgb;
    vec3 e = texture(scene, vec2(TexCoord.x, TexCoord.y)).rgb;
    vec3 f = texture(scene, vec2(TexCoord.x + 2 * x, TexCoord.y)).rgb;

    vec3 g = texture(scene, vec2(TexCoord.x - 2 * x, TexCoord.y - 2 * y)).rgb;
    vec3 h = texture(scene, vec2(TexCoord.x, TexCoord.y - 2 * y)).rgb;
    vec3 i = texture(scene, vec2(TexCoord.x + 2 * x, TexCoord.y - 2 * y)).rgb;

    vec3 j = texture(scene, vec2(TexCoord.x - x, TexCoord.y + y)).rgb;
    vec3 k = texture(scene, vec2(TexCoord.x + x, TexCoord.y + y)).rgb;
    vec3 l = texture(scene, vec2(TexCoord.x - x, TexCoord.y - y)).rgb;
    vec3 m = texture(scene, vec2(TexCoord.x + x, TexCoord.y - y)).rgb;

    downsample = e * 0.125;
    downsample += (a + c + g + i) * 0.03125;
    downsample += (b + d + f + h) * 0.0625;
    downsample += (j + k + l + m) * 0.125; 
}