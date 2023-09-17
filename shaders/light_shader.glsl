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

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}

#type fragment
#version 420 core
#line 22

layout(std140, binding = 1) uniform u_light
{
    vec4 light_pos;
    vec4 light_color;
    vec4 light_attenuation;
    vec2 light_intensities;
};

layout(location = 0) out vec4 FragColor;

void main()
{ // make the light glow with intensity proportional to the diffuse intensity
    FragColor = light_color * light_intensities.x;
}