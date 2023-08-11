#include "glad.h"
#include "glcore.hpp"

#include <GLFW/glfw3.h>
#include <algorithm>
#include <array>
#include <filesystem>
#include <iostream>
#include <span>
#include <utility>

/*

        IMPORTANT: in order for the program to correctly compile, go to
        include/glcore/modules/gl_functions.hpp and replace the placeholder
        with the provided OpenGL loader ("glad.h").

*/

auto lerp(float a, float b, float f) -> float
{
    return a * (1.0F - f) + b * f;
}

struct vec3 {
    float x;
    float y;
    float z;
};

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

void GLAPIENTRY
MessageCallback(GLenum source [[maybe_unused]],
    GLenum type,
    GLuint id [[maybe_unused]],
    GLenum severity,
    GLsizei length [[maybe_unused]],
    const GLchar* message,
    const void* userParam [[maybe_unused]])
{
    if (type == GL_DEBUG_TYPE_OTHER || type == GL_DEBUG_TYPE_PERFORMANCE)
        return;
    fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x,\nmessage = %s\n",
        (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
        type, severity, message);
}
// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

auto main() -> int
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", nullptr, nullptr);
    if (window == nullptr) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)) == 0) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // During init, enable debug output
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(MessageCallback, nullptr);

    // antialiasing and other nice things
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glEnable(GL_POLYGON_SMOOTH);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);

    glcore::shader_program basic { "batched_shader", "./shaders/batched_shader.glsl" };

    basic.bind();

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    std::array<float, 12> vertices {
        0.001F, 0.001F, 0.0F, // top right
        0.001F, -0.001F, 0.0F, // bottom right
        -0.001F, -0.001F, 0.0F, // bottom left
        -0.001F, 0.001F, 0.0F, // top left
    };

    std::array<unsigned int, 6> indices {
        // note that we start from 0!
        0, 1, 3, // first Triangle
        1, 2, 3 // second Triangle
    };

    glcore::vertex_buffer_layout layout {
        { glcore::shader_data_type::vec3, "aPos" }
    };

    glcore::vertex_buffer_layout instance_layout {
        { glcore::shader_data_type::vec3, "instancePos" }
    };

    glcore::vertex_buffer VBO(vertices, glcore::driver_draw_hint::STATIC_DRAW);
    glcore::vertex_buffer_inst VBO_inst({}); // empty for now

    VBO_inst.set_layout(instance_layout);
    VBO.set_layout(layout);

    glcore::index_buffer EBO { indices };

    glcore::vertex_array VAO;

    VAO.add_vertex_buffer(std::move(VBO));
    VAO.set_instance_buffer(std::move(VBO_inst));
    VAO.set_index_buffer(std::move(EBO));

    VAO.bind();

    const float START = -0.95F;
    const float END = 0.95F;

    const float Z_START = 0.01F;
    const float Z_END = 1.00F;

    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    for (int i = 0; i < 2048; i++) {
        std::array<float, 3> offset = {
            lerp(START, END,
                static_cast<float>(rand()) / static_cast<float>(RAND_MAX)),
            lerp(START, END,
                static_cast<float>(rand()) / static_cast<float>(RAND_MAX)),
            lerp(Z_START, Z_END,
                static_cast<float>(rand()) / static_cast<float>(RAND_MAX))
        };

        // I know the instance is set since it is in the optional, so I go for direct access
        VAO.instanced_data()->add_instance(offset);
    }

    while (glfwWindowShouldClose(window) == 0) {

        processInput(window);

        glClearColor(0.2F, 0.3F, 0.3F, 1.0F);
        glClear(GL_COLOR_BUFFER_BIT);

        glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr,
            VAO.instanced_data()->instance_count());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, 1);
    }
}

void framebuffer_size_callback(GLFWwindow* /*window*/, int width, int height)
{
    glViewport(0, 0, width, height);
}
