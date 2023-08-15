#type vertex

#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 instancePos;

out vec3 pos;

void main()
{
    float z = aPos.z;
    float x = (aPos.x + instancePos.x);
    float y = (aPos.y + instancePos.y);

    pos = vec3(x, y, z);
    gl_Position = vec4(x, y, z, 1.0);

}

#type fragment

#version 330 core
out vec4 FragColor;
in vec3 pos;

void main()
{
    const vec4 whiteColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
    vec4 color = vec4(1.0f, 0.5f, 0.2f, 1.0f);

    float x = pos.x;

    float dist_center = abs(x) / 2.0f;

    color = mix(color, whiteColor, dist_center);

    FragColor = color;
}