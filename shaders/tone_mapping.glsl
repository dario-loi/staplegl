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
layout(location = 0) out vec4 FragColor;


layout(binding = 1) uniform sampler2D scene;
layout(binding = 2) uniform sampler2D bloom;

float luminance(vec3 v)
{
    return dot(v, vec3(0.2126f, 0.7152f, 0.0722f));
}


vec3 change_luminance(vec3 c_in, float l_out)
{
    float l_in = luminance(c_in);
    return c_in * (l_out / l_in);
}

vec3 reinhard(vec3 v, float max_white_l)
{
    float l_old = luminance(v);
    float numerator = l_old * (1.0f + (l_old / (max_white_l * max_white_l)));
    float l_new = numerator / (1.0f + l_old);
    return change_luminance(v, l_new);
}

void main()
{
    const float gamma = 2.2F;
    vec3 hdrColor = texture(scene, TexCoord).rgb;
    vec3 bloomColor = texture(bloom, TexCoord).rgb;

    // exposure tone mapping and gamma correction

    vec3 result = mix(hdrColor, bloomColor, 0.04F);

    result = reinhard(result, 1.0F);
    result = pow(result, vec3(1.0F / gamma));

    FragColor = vec4(result, 1.0F);
}