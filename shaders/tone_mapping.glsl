#type vertex
#version 420 core
#line 3

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

void main()
{
    TexCoord = aTexCoord;
    gl_Position = vec4(aPos, 1.0);
}

#type fragment
#version 420 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D scene;
uniform sampler2D bloom;

void main()
{
    const float exposure = 0.5F;
    const float gamma = 2.2F;
    vec3 hdrColor = texture(scene, TexCoord).rgb;
    vec3 bloomColor = texture(bloom, TexCoord).rgb;

    // exposure tone mapping and gamma correction

    vec3 result = mix(hdrColor, bloomColor, 0.04F);

    result = vec3(1.0) - exp(-result * exposure);
    result = pow(result, vec3(1.0F / gamma));

    FragColor = vec4(result, 1.0F);
}