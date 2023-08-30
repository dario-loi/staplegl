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

out vec3 normal;

void main()
{

    normal = normalize(aNormal);
    gl_Position = projection * view * model * vec4(aPos, 1.0F);
}

#type fragment

#version 420 core
#line 87

layout(std140, binding = 0) uniform u_matrices
{
    mat4 projection;
    mat4 view;
    mat4 model;
    vec4 camera_pos;
};

in vec3 normal;

out vec4 color;

uniform samplerCube environment;

const float refraction_ratio = 0.15F;
const float ambient_light = 0.70F; // brightness of the environment

void main()
{

    vec3 N = normalize(normal);
    vec3 V = normalize(camera_pos.xyz);
    vec3 R = reflect(V, N);

    vec4 env_color = texture(environment, R);
    vec4 teapot_porcelain_color = vec4(0.51F, 0.55F, 0.66F, 1.0F);

    vec4 diffuse = ambient_light * teapot_porcelain_color + refraction_ratio * env_color;

    color = pow(diffuse, vec4(1.0F / 2.2F));
}