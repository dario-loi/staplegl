#type vertex
#version 420 core

layout(location = 0) in vec3 position;

layout(std140, binding = 0) uniform u_matrices
{
    mat4 projection;
    mat4 view;
    mat4 model;
    vec4 camera_pos;
};

out vec3 texCoords;

void main()
{
    texCoords = position;
    gl_Position = projection * view * model * vec4(position, 1.0);
}

#type fragment
#version 420 core

in vec3 texCoords;
out vec4 FragColor;

uniform samplerCube skybox;

void main()
{
    vec3 skybox_color = texture(skybox, texCoords).rgb;

    FragColor = vec4(skybox_color.rgb, 1.0);
}
