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

void main()
{
    const float exposure = 1.0F;
    const float gamma = 2.2F;
    vec3 hdrColor = texture(scene, TexCoord).rgb;

    // exposure tone mapping and gamma correction

    hdrColor = vec3(1.0) - exp(-hdrColor * exposure);
    hdrColor = pow(hdrColor, vec3(1.0F / gamma));

    FragColor = vec4(hdrColor, 1.0F);
}