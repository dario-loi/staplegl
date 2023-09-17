#version 420 core
layout(location = 0) out vec4 FragColor;

layout(location = 0) in vec2 TexCoord;

layout(binding = 4) uniform sampler2D scene;
layout(binding = 5) uniform sampler2D bloom;

void main()
{
    const float exposure = 0.5F;
    const float gamma = 2.2F;
    vec3 hdrColor = texture(scene, TexCoord).rgb;
    vec3 bloomColor = texture(bloom, TexCoord).rgb;

    // exposure tone mapping and gamma correction

    vec3 result = mix(hdrColor, bloomColor, 0.04F);

    result = vec3(1.0) - exp(-hdrColor * exposure);
    result = pow(result, vec3(1.0F / gamma));

    FragColor = vec4(result, 1.0F);
}