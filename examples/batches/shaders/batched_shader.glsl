#type vertex

#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 instancePos;
void main()
{
    gl_Position = vec4(aPos.x + instancePos.x, aPos.y + instancePos.y, aPos.z + instancePos.z, 1.0);
}

#type fragment

#version 330 core
out vec4 FragColor;
void main()
{
    FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
}