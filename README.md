# glcore
No-dependency OpenGL support library, which abstracts the processes of creating buffers and shaders
# Install
```
git clone --recursive https://github.com/ChristianPanov/glcore
```
# Design Highlights
- Very lightweight - glcore is merely a thin wrapper around the OpenGL functions, except the shader class, which is more than a wrapper. It may be thin, but it's very useful for abstracting the OpenGL state machine logic to a more object-oriented logic
- No dependencies - glcore does not enforce any dependencies such as function loaders, by using placeholder functions, which can be replaced with the function loader of choice simply by putting the needed includes in a specific file
# Features
- Vertex buffers
- Vertex buffer layout
- Index buffers
- Vertex arrays
- Shaders
# Setup
Because of the no-dependency nature of glcore, you will need to provide the OpenGL function loader.\
To do that, you will need to locate the ```src/glcore/tweakme``` folder, and open the ```gl_functions.h``` header file.\
From there on, it is pretty self explanatory with the comments that are provided in the file.
# Usage
## Basic Usage
```cpp
#include "glcore.h"

int main()
{
	float vertices[4][7] = {
		{ -0.5f, -0.5f, 0.0f, 0.8f, 0.2f, 0.8f, 1.0f },
		{  0.5f, -0.5f, 0.0f, 0.2f, 0.3f, 0.8f, 1.0f },
		{  0.5f,  0.5f, 0.0f, 0.8f, 0.8f, 0.2f, 1.0f },
		{ -0.5f,  0.5f, 0.0f, 0.8f, 0.8f, 0.2f, 1.0f }
	};

	unsigned int indices[2][3] = {
		{ 0, 1, 2 },
		{ 0, 2, 3 }
	};

	glcore::vertex_buffer vbo(*vertices, sizeof(vertices));
	glcore::index_buffer ibo(*indices, sizeof(indices) / sizeof(int));
	glcore::vertex_buffer_layout layout =
	{
		{ glcore::shader_data_type::type::vec3, "position" },
		{ glcore::shader_data_type::type::vec4, "color" }
	};
	vbo.set_layout(layout);

	glcore::vertex_array vao;
	vao.add_vertex_buffer(vbo);
	vao.set_index_buffer(ibo);

	glcore::shader_program shaders("Basic", {
		{ glcore::shader_type::vertex, "shader_examples/vert.glsl" },
		{ glcore::shader_type::fragment, "shader_examples/frag.glsl" }
		});
	shaders.bind();

	shaders.upload_uniform4f("u_Color", 0.2f, 0.3f, 0.8f, 1.0f);

	return 0;
}
```
## Vertex buffer layout
The vertex buffer layout is declared in a very intuitive way. You provide a shader data type and an identifier name. The shader data types reside in the ```glcore::shader_data_type``` namespace.
```cpp
glcore::vertex_buffer_layout layout =
{
	{ glcore::shader_data_type::type::vec3, "position" },
	{ glcore::shader_data_type::type::vec4, "color" }
};
```
A vertex buffer layout can either be declared on it's own like in the example above, or can be created as an rvalue directly in the constructor of ```glcore::vertex_buffer```
```cpp
glcore::vertex_buffer vbo(*vertices, sizeof(vertices), {
	{ glcore::shader_data_type::type::vec3, "position" },
	{ glcore::shader_data_type::type::vec4, "color" }
});
```
## Shaders
A shader program can be handled in two different ways. You can have separate shader files for each type of shader, or you can have one single shader file.
### Single file
With the single file approach, you only need to provide the file path and a name for the shader program. If you don't provide a name for the shader program, glcore will automatically set the name to be the name of the file.
```cpp
glcore::shader_program shaders_single("Basic", "shader_examples/basic.glsl");

// the name of the shader program will be set to 'basic'
glcore::shader_program shader_single_noname("shader_examples/basic.glsl");
```
For the shader parser to differentiate between the different shaders in the file, the shader code needs to start with a specific command line - ```#type [shader type]```
#### Example
```glsl
#type vertex
#version 330 core

layout(location = 0) in vec3 a_Position;

out vec3 v_Position;

void main()
{
	v_Position = a_Position;
	gl_Position = vec4(a_Position, 1.0);
};

#type fragment
#version 330 core

layout(location = 0) out vec4 color;

uniform vec4 u_Color;

void main()
{
	color = u_Color;
};
```
### Multiple files
With this approach, you will need to spefify the type of the shader with an enum value, which resides in the ```glcore::shader_type``` namespace, and a file path.
```cpp
glcore::shader_program shaders("Basic", {
		{ glcore::shader_type::vertex, "shader_examples/vert.glsl" },
		{ glcore::shader_type::fragment, "shader_examples/frag.glsl" }
		});
```
### Supported types of shaders
Shader Type | Enum Value | GLSL Command
------------ | ------------- | -------------
Vertex | ```glcore::shader_type::vertex``` | ```#type vertex```
Fragment | ```glcore::shader_type::fragment``` | ```#type fragment```
Tessellation Control | ```glcore::shader_type::tess_control``` | ```#type tess_control```
Tessellation Evaluation | ```glcore::shader_type::tess_eval``` | ```#type tess_eval```
Geometry | ```glcore::shader_type::geometry``` | ```#type geometry```
