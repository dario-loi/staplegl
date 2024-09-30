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

const mat3 aces_input = transpose(mat3(
    vec3(0.59719f, 0.35458f, 0.04823f),
    vec3(0.07600f, 0.90834f, 0.01566f),
    vec3(0.02840f, 0.13383f, 0.83777f)));

const mat3 aces_output = transpose(mat3(
    vec3(1.60475f, -0.53108f, -0.07367f),
    vec3(-0.10208f, 1.10813f, -0.00605f),
    vec3(-0.00327f, -0.07276f, 1.07602f)));

vec3 rtt_and_odt_fit(vec3 v)
{
    vec3 a = v * (v + 0.0245786f) - 0.000090537f;
    vec3 b = v * (0.983729f * v + 0.4329510f) + 0.238081f;
    return a / b;
}

vec3 aces(vec3 v)
{
    return aces_output * rtt_and_odt_fit(aces_input * v);
}
void main()
{
    const float gamma = 2.2F;
    vec3 hdrColor = texture(scene, TexCoord).rgb;
    vec3 bloomColor = texture(bloom, TexCoord).rgb;

    // exposure tone mapping and gamma correction

    vec3 result = mix(hdrColor, bloomColor, 0.05F);

    // Vignette effect
    vec2 center = vec2(0.5F, 0.5F);
    float radius = 0.95F;

    float vignette = 1.0F - smoothstep(0.0F, radius, length(TexCoord - center));
    result *= vignette;

    result = aces(result);
    result = pow(result, vec3(1.0F / gamma));

    FragColor = vec4(result, 1.0F);
}