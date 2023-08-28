/**
 * @file sandbox.cpp
 * @author Dario Loi
 * @brief Minimal example of how to use glcore.
 *
 * @copyright MIT License
 *
 */

#include "glad.h"
#include "glcore.hpp"
#include <GLFW/glfw3.h>
#include <algorithm>
#include <array>
#include <iostream>
#include <span>
#include <utility>

#include "teapot.h"

// glm linear algebra library, NOT needed for glcore, just for the example.
#include "glm.hpp"
#include "gtc/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"

/*

        IMPORTANT: in order for the program to correctly compile, go to
        include/glcore/modules/gl_functions.hpp and replace the placeholder
        with the provided OpenGL loader ("glad.h").

*/

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

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
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Utah Teapot", nullptr, nullptr);
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

    glEnable(GL_DEPTH_TEST);

    glcore::shader_program teapot { "teapot_shader", "./shaders/teapot_shader.glsl" };

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------

    std::vector<unsigned int> teapot_indices(TEAPOT_VERTEX_COUNT / 3);

    std::generate(teapot_indices.begin(), teapot_indices.end(), [n = 0]() mutable { return n++; });

    glcore::vertex_buffer VBO { { teapot_vertices, TEAPOT_VERTEX_COUNT }, glcore::driver_draw_hint::STATIC_DRAW };
    glcore::index_buffer EBO { teapot_indices };

    using namespace glcore::shader_data_type;

    glcore::vertex_buffer_layout layout { { shader_type::vec3, "aPos" } };

    VBO.set_layout(layout);

    glcore::vertex_array VAO;

    VAO.add_vertex_buffer(std::move(VBO));
    VAO.set_index_buffer(std::move(EBO));

    teapot.bind();

    glm::vec4 light_pos { 0.0F, 0.0F, 1.0F, 1.0F };

    glm::vec4 look_dir { 0.0F, 0.0F, -1.0F, 1.0F };

    // get projection matrix
    glm::mat4 model = glm::mat4(1.0F);

    // invert the y axis
    glm::mat4 flip_vert = glm::scale(model, glm::vec3(1.0F, -1.0F, 1.0F));

    glm::mat4 projection = glm::perspective(glm::radians(75.0F), static_cast<float>(SCR_WIDTH) / static_cast<float>(SCR_HEIGHT), 0.01F, 100.0F);

    glcore::vertex_buffer_layout uniform_block_layout {
        { shader_type::mat4, "projection" },
        { shader_type::mat4, "view" },
        { shader_type::mat4, "model" },
        { shader_type::vec4, "light_pos" },
        { shader_type::vec4, "camera_pos" }
    };

    glcore::uniform_buffer uniform_block { uniform_block_layout, 0 };

    uniform_block.bind();

    uniform_block.set_attribute_data(std::span { glm::value_ptr(projection), 16 }, "projection", 0);
    uniform_block.set_attribute_data(std::span { glm::value_ptr(flip_vert), 16 }, "model", 0);
    uniform_block.set_attribute_data(std::span { glm::value_ptr(light_pos), 4 }, "light_pos", 0);

    while (glfwWindowShouldClose(window) == 0) {
        // input
        // -----
        processInput(window);

        float time = static_cast<float>(glfwGetTime());

        float camX = sin(time) * 3.0F;
        float camZ = cos(time) * 3.0F;
        float camY = sin(time) * 1.2F + 0.6F; // slightly over the teapot.

        glm::vec4 camera_pos { camX, camY, camZ, 1.0F };
        const glm::vec4 camera_target { 0.0F, 0.0F, -.0F, 1.0F };

        glm::mat4 view = glm::lookAt(glm::vec3(camera_pos), glm::vec3(0.0F, 0.0F, 0.0F),
            glm::vec3(0.0F, 1.0F, 0.0F));

        uniform_block.set_attribute_data(std::span { glm::value_ptr(camera_pos), 4 }, "camera_pos", 0);
        uniform_block.set_attribute_data(std::span { glm::value_ptr(view), 16 }, "view", 0);

        // render
        // ------

        // clear sky blue
        glClearColor(0.529F, 0.808F, 0.922F, 1.0F);
        glClear(GL_COLOR_BUFFER_BIT);
        glClear(GL_DEPTH_BUFFER_BIT);

        // draw our first triangle
        VAO.bind(); // seeing as we only have a single VAO there's
                    // no need to bind it every time, but we'll do
                    // so to keep things a bit more organized
        // glDrawArrays(GL_TRIANGLES, 0, 6);
        glDrawElements(GL_TRIANGLES, VAO.index_data().count(), GL_UNSIGNED_INT, nullptr);
        // glBindVertexArray(0); // no need to unbind it every time

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved
        // etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------

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
