#type vertex

#version 440 core

layout(std140, binding = 0) uniform u_matrices
{
    mat4 projection;
    mat4 view;
    mat4 model;
    vec4 light_pos;
    vec4 camera_pos;
};

layout(location = 0) in vec3 aPos;
out float depth;

void main()
{
    vec4 viewPos = view * model * vec4(aPos, 1.0);

    depth = viewPos.z;

    gl_Position = projection * viewPos;
}

#type fragment

#version 440 core

in float depth;
out vec4 FragColor;

void main()
{
    float maxDepth = 100.0; // Far clip plane distance
    float depthFactor = depth / maxDepth; // Normalize depth

    // Calculate shading color based on depth
    vec3 shadeColor = vec3(1.0) - depthFactor * 0.2; // Make it slightly darker

    FragColor = vec4(shadeColor, 1.0);
}