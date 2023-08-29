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

out vec3 normal;
out float depth;

void main()
{
    // Transform vertex position to camera space
    vec4 viewPos = view * model * vec4(aPos, 1.0);

    // Pass values to tessellation and fragment shaders
    normal = normalize(aNormal);
    depth = length(viewPos.xyz);

    // Output transformed vertex position
    gl_Position = projection * viewPos;
}

#type tess_control
#version 440 core

layout(vertices = 3) out;

layout(std140, binding = 0) uniform u_matrices
{
    mat4 projection;
    mat4 view;
    mat4 model;
    vec4 camera_pos;
};

in vec3 position[];
in vec3 normal[];
in float depth[];

out vec3 tcPosition[];
out vec3 tcNormal[];
out float tcDepth[];

// Tessellation level for teapot
const int TEAPOT_TESSELLATION_LEVEL = 3;

void main()
{
    // Set tessellation levels for each edge
    gl_TessLevelOuter[0] = TEAPOT_TESSELLATION_LEVEL;
    gl_TessLevelOuter[1] = TEAPOT_TESSELLATION_LEVEL;
    gl_TessLevelOuter[2] = TEAPOT_TESSELLATION_LEVEL;

    // Inner tessellation level (used for Phong tessellation)
    gl_TessLevelInner[0] = TEAPOT_TESSELLATION_LEVEL;

    // Pass data to the tessellation evaluation shader
    tcPosition[gl_InvocationID] = position[gl_InvocationID];
    tcNormal[gl_InvocationID] = normal[gl_InvocationID];
    tcDepth[gl_InvocationID] = depth[gl_InvocationID];

    barrier();
}

#type tess_eval
#version 440 core

layout(triangles, fractional_even_spacing, ccw) in;

in vec3 tcPosition[];
in vec3 tcNormal[];
in float tcDepth[];

out vec3 position;
out vec3 normal;
out float depth;

layout(std140, binding = 0) uniform u_matrices
{
    mat4 projection;
    mat4 view;
    mat4 model;
    vec4 camera_pos;
};

void main()
{
    // Calculate barycentric coordinates
    vec3 barycentric = gl_TessCoord;

    // Interpolate position, normal, and depth using barycentric coordinates
    position = barycentric.x * tcPosition[0] + barycentric.y * tcPosition[1] + barycentric.z * tcPosition[2];
    normal = barycentric.x * tcNormal[0] + barycentric.y * tcNormal[1] + barycentric.z * tcNormal[2];
    depth = barycentric.x * tcDepth[0] + barycentric.y * tcDepth[1] + barycentric.z * tcDepth[2];

    // Transform position to camera space
    vec4 viewPosition = view * model * vec4(position, 1.0);
    gl_Position = projection * viewPosition;
}

#type fragment
#version 440 core

in float depth;
in vec3 normal;
in vec3 position;

out vec4 FragColor;

layout(std140, binding = 0) uniform u_matrices
{
    mat4 projection;
    mat4 view;
    mat4 model;
    vec4 camera_pos;
};

// Controls how much to shade the teapot based on depth
const float shade_control = 0.40F;
const vec3 porcelain_white = vec3(0.61F, 0.61F, 0.61F);

const float reflectivity = 0.05F;

uniform samplerCube environment;

void main()
{
    // Normalize depth for shading
    const float teapotDistance = 4.0F;
    const float depthFactor = depth / teapotDistance;

    // Calculate shading color based on depth
    vec3 shadeColor = porcelain_white - shade_control * vec3(depthFactor);
    shadeColor = clamp(shadeColor, 0.0, 1.0);

    // Calculate reflection vector and color
    vec3 look_vec = normalize(position - camera_pos.xyz);
    vec3 reflect_vec = reflect(look_vec, normalize(normal));
    vec4 reflect_color = vec4(texture(environment, reflect_vec).rgb, 1.0F);

    // Mix shading and reflection for final color
    vec4 diffuse = mix(vec4(shadeColor, 1.0F), reflect_color, reflectivity);

    // Apply gamma correction
    float gamma = 2.2;
    FragColor = vec4(pow(diffuse.rgb, vec3(1.0 / gamma)), 1.0);
}
