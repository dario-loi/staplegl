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
out float depth;

void main()
{
    vec4 viewPos = view * model * vec4(aPos, 1.0);

    depth = length(viewPos.xyz);
    gl_Position = projection * viewPos;
}

#type fragment

#version 440 core

in float depth;
out vec4 FragColor;

void main()
{

    float teapotDistance = 4.0F;
    float depthFactor = depth / teapotDistance; // Normalize depth

    // Calculate shading color based on depth
    vec3 shadeColor = clamp(vec3(0.61F) - 0.25F * vec3(depthFactor), 0.F, 1.F); // Make it slightly darker

    FragColor = vec4(shadeColor, 1.0);
}