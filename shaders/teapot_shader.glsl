#type vertex
#version 420 core
#line 3

layout(std140, binding = 0) uniform u_matrices
{
    mat4 projection;
    mat4 view;
    mat4 model;
    vec4 camera_pos;
};

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;

layout(location = 0) out vec3 normal;
layout(location = 1) out vec3 frag_pos_view;

void main()
{

    normal = normalize(aNormal);
    frag_pos_view = vec3(view * model * vec4(aPos, 1.0F));
    gl_Position = projection * vec4(frag_pos_view, 1.0F);
}

#type fragment

#version 420 core
#line 30

layout(std140, binding = 0) uniform u_matrices
{
    mat4 projection;
    mat4 view;
    mat4 model;
    vec4 camera_pos;
};

layout(std140, binding = 1) uniform u_light
{
    vec4 light_pos;
    vec4 light_color;
    vec4 light_attenuation;
    vec2 light_intensity;
};

layout(std140, binding = 2) uniform u_material
{
    vec4 material_color;
    float material_shininess;
    float material_roughness;
};

layout(location = 0) in vec3 normal;
layout(location = 1) in vec3 frag_pos_view;

layout(location = 0) out vec4 color;

layout(binding = 0) uniform samplerCube environment;

const float refraction_ratio = 0.20F;
const float ambient_light = 0.10F; // brightness of the environment

void main()
{

    vec3 N = normalize(normal);
    vec3 V = normalize(camera_pos.xyz);
    vec3 R = reflect(V, N);

    vec4 env_color = texture(environment, R);
    vec4 teapot_porcelain_color = vec4(0.51F, 0.55F, 0.66F, 1.0F);

    vec4 ambient = ambient_light * teapot_porcelain_color + refraction_ratio * env_color;

    float dist_to_light = length((view * light_pos).xyz - frag_pos_view);
    float lum = 1.0F / (light_attenuation.x + light_attenuation.y * dist_to_light + light_attenuation.z * dist_to_light * dist_to_light);

    vec4 diffuse = max(dot(N, light_pos.xyz), 0.0F) * light_color;
    diffuse /= dist_to_light;

    vec3 H = normalize(V + light_pos.xyz);
    float spec = pow(max(dot(N, H), 0.0F), material_shininess);
    spec /= dist_to_light;

    vec4 specular = spec * (1.0F - material_roughness) * light_color;

    if (dot(N, V) < 0.0F) {
        specular = vec4(0.0F);
    }

    color = ambient + lum * diffuse * light_intensity.x + lum * specular * light_intensity.y;
}