#type vertex
#version 440 core

layout(std140, binding = 0) uniform u_matrices
{
    mat4 projection;
    mat4 view;
    mat4 model;
    vec4 camera_pos;
};

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;

out float depth;
out vec3 normal;

void main()
{
    vec4 viewPos = view * model * vec4(aPos, 1.0);
    normal = aNormal;

    depth = length(viewPos.xyz);
    gl_Position = projection * viewPos;
}

#type fragment

#version 440 core

in float depth;
in vec3 normal;
out vec4 FragColor;

// Controls how much to shade the teapot based on depth
const float shade_control = 0.40F;
const vec3 porcelain_white = vec3(0.61F);

void main()
{
    const float teapotDistance = 4.0F;
    const float depthFactor = depth / teapotDistance; // Normalize depth

    // Calculate shading color based on depth
    vec3 shadeColor = porcelain_white - shade_control * vec3(depthFactor); // Make it slightly darker
    shadeColor = clamp(shadeColor, 0.0, 1.0); // Clamp to [0, 1]

    float gamma = 2.2;
    FragColor = vec4(pow(shadeColor.rgb, vec3(1.0 / gamma)), 1.0);
}