#include "glad.h"
#include "glcore.hpp"

#include <GLFW/glfw3.h>
#include <algorithm>
#include <iostream>
#include <span>
#include <utility>

/*

        IMPORTANT: in order for the program to correctly compile, go to
        include/glcore/modules/gl_functions.hpp and replace the placeholder
        with the provided OpenGL loader ("glad.h").

*/

float lerp(float a, float b, float f)
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
MessageCallback(GLenum source,
    GLenum type,
    GLuint id,
    GLenum severity,
    GLsizei length,
    const GLchar* message,
    const void* userParam)
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
    glDebugMessageCallback(MessageCallback, 0);

    // antialiasing and other nice things
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

    glcore::shader_program basic { "batched_shader", "./shaders/batched_shader.glsl" };

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    const float vertices[] = {
        0.1F, 0.1F, 0.0F, // top right
        0.1F, -0.1F, 0.0F, // bottom right
        -0.1F, -0.1F, 0.0F, // bottom left
        -0.1F, 0.1F, 0.0F, // top left
    };

    const unsigned int indices[] = {
        // note that we start from 0!
        0, 1, 3, // first Triangle
        1, 2, 3 // second Triangle
    };

    //----- INSTANCE DATA -----

    float offsets[15] = {
        .5F, .5F, .0F, // top right
        .5F, -.5F, .0F, // bottom right
        -.5F, -.5F, .0F, // bottom left
        -.5F, .5F, .0F, // top left
        .0F, .0F, .0F // center
    };

    glcore::vertex_buffer_layout layout {
        { glcore::shader_data_type::vec3, "aPos" }
    };

    glcore::vertex_buffer_layout instance_layout {
        { glcore::shader_data_type::vec3, "instancePos" }
    };

    glcore::vertex_buffer VBO(std::span<const float>(vertices, 12), glcore::driver_draw_hint::STATIC_DRAW);

    glcore::vertex_buffer_inst VBO_inst(std::span<const float>(offsets, 15));

    VBO_inst.set_layout(instance_layout);
    VBO.set_layout(layout);

    glcore::index_buffer EBO { indices, 6 };

    glcore::vertex_array VAO;

    VAO.add_vertex_buffer(std::move(VBO));

    auto inst_it = VAO.add_vertex_buffer(std::move(VBO_inst));

    VAO.set_index_buffer(std::move(EBO));

    const float start = -0.95F;
    const float end = 0.95F;

    const float z_start = 0.01F;
    const float z_end = 1.00F;

    VAO.bind();
    /*
    for (int i = 0; i < 1; i++) {
        float offset[3] = {
            lerp(start, end,
                static_cast<float>(rand()) / static_cast<float>(RAND_MAX)),
            lerp(start, end,
                static_cast<float>(rand()) / static_cast<float>(RAND_MAX)),
            lerp(z_start, z_end,
                static_cast<float>(rand()) / static_cast<float>(RAND_MAX))
        };


    }*/

    basic.bind();

    while (glfwWindowShouldClose(window) == 0) {
        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.2F, 0.3F, 0.3F, 1.0F);
        glClear(GL_COLOR_BUFFER_BIT);

        // draw all quads in a single draw call
        glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, 5);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse
        // moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this
// frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, 1);
    }
}

// glfw: whenever the window size changed (by OS or user resize) this callback
// function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* /*window*/, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width
    // and height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}
