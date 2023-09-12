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
uniform int pyramid_level;

layout(location = 0) out vec3 outColor;

void main() {
    vec3 upsample = vec3(0.F);

    float x = 0.005F;
    float y = 0.005F;

    vec3 a = texture(scene, vec2(TexCoord.x - x, TexCoord.y + y)).rgb;
    vec3 b = texture(scene, vec2(TexCoord.x, TexCoord.y + y)).rgb;
    vec3 c = texture(scene, vec2(TexCoord.x + x, TexCoord.y + y)).rgb;

    vec3 d = texture(scene, vec2(TexCoord.x - x, TexCoord.y)).rgb;
    vec3 e = texture(scene, vec2(TexCoord.x, TexCoord.y)).rgb;
    vec3 f = texture(scene, vec2(TexCoord.x + x, TexCoord.y)).rgb;

    vec3 g = texture(scene, vec2(TexCoord.x - x, TexCoord.y - y)).rgb;
    vec3 h = texture(scene, vec2(TexCoord.x, TexCoord.y - y)).rgb;
    vec3 i = texture(scene, vec2(TexCoord.x + x, TexCoord.y - y)).rgb;

    upsample = e * 4.0;
    upsample += (b + d + f + h) * 2.0;
    upsample += (a + c + g + i);
    upsample *= 1.0 / 16.0;

    outColor += upsample;
}

