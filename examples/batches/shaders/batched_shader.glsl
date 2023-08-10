#type vertex

#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 instancePos;
void main()
{
    float z = aPos.z + instancePos.z;
    float x = (aPos.x + instancePos.x);
    float y = (aPos.y + instancePos.y);

    gl_Position = vec4(x, y, z, z);
}

#type fragment

#version 330 core
out vec4 FragColor;


void main()
{
    vec4 color = vec4(1.0f, 0.5f, 0.2f, 1.0f);

    FragColor = color;
}