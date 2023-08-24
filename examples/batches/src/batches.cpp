#include "glad.h"
#include "glcore.hpp"

#include <GLFW/glfw3.h>
#include <algorithm>
#include <array>
#include <cmath>
#include <ctime>
#include <execution>
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
    const std::array<const float, 12> vertices {
        0.001F, 0.001F, 0.0F, // top right
        0.001F, -0.001F, 0.0F, // bottom right
        -0.001F, -0.001F, 0.0F, // bottom left
        -0.001F, 0.001F, 0.0F, // top left
    };

    const std::array<const unsigned int, 6> indices {
        // note that we start from 0!
        0, 1, 3, // first Triangle
        1, 2, 3 // second Triangle
    };

    using namespace glcore::shader_data_type;

    glcore::vertex_buffer_layout layout {
        { shader_type::vec3, "aPos" }
    };

    glcore::vertex_buffer_layout instance_layout {
        { shader_type::vec3, "instancePos" }
    };

    glcore::vertex_buffer VBO(vertices, glcore::driver_draw_hint::STATIC_DRAW);
    glcore::vertex_buffer_inst VBO_inst({}); // empty for now

    VBO_inst.set_layout(instance_layout);
    VBO.set_layout(layout);

    glcore::index_buffer EBO { indices };
    glcore::vertex_array VAO;

    glcore::vertex_buffer_layout UBO_block_layout {
        { shader_array_type::float32_arr, "u_color", 4 }
    };

    glcore::uniform_buffer UBO_block { UBO_block_layout, 1 };

    UBO_block.bind();

    std::array<float, 4> color {};

    VAO.add_vertex_buffer(std::move(VBO));
    VAO.set_instance_buffer(std::move(VBO_inst));
    VAO.set_index_buffer(std::move(EBO));

    VAO.bind();

    const float START = -0.95F;
    const float END = 0.95F;

    const float Z_START = 0.01F;
    const float Z_END = 1.00F;

    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    for (int i = 0; i < 65535; i++) {
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

        float timeNow = static_cast<float>(glfwGetTime());

        for (float& col : color) {
            col = std::sin(timeNow) / 2.0F + 0.5F;
        }
        color[3] = 1.0F;

        for (int i = 0; i < 4; ++i) {
            UBO_block.set_attribute_data(std::span { &color[i], 1 }, "u_color", i);
        }

        VAO.instanced_data()->apply(
            [](std::span<float> data,
                glcore::vertex_buffer_layout const& layout [[maybe_unused]]) {
                // reinterpret the data as a 3D vector
                struct vec3 {
                    float x;
                    float y;
                    float z;
                };

                static_assert(sizeof(vec3) == 3 * sizeof(float));
                std::span<vec3> vec_data { reinterpret_cast<vec3*>(data.data()),
                    data.size() / 3 };

                std::for_each(std::execution::par_unseq, vec_data.begin(), vec_data.end(),
                    [](vec3& v) {
                        const float speed = ((static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX)) - 0.5F) / 1000.0F;
                        v.x += speed;
                    });
            });

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
