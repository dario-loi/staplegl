#type vertex
#version 420 core
#line 3

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoord;

layout(location = 0) out vec2 TexCoord;

void main()
{
    TexCoord = aTexCoord;
    gl_Position = vec4(aPos, 1.0);
}

#type fragment
#version 420 core
#line 18

layout(location = 0) in vec2 TexCoord;

layout(binding = 1) uniform sampler2D scene;

layout(location = 0) out vec3 outColor;

void main()
{
    outColor = vec3(texture(scene, TexCoord));
}
