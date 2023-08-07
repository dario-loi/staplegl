#include "glad.h"
#include "glcore.hpp"
#include <GLFW/glfw3.h>
#include <iostream>

#include "extensions/instanced_vbo.hpp"
#include "extensions/instanced_vao.hpp"

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

    glcore::shader_program basic { "batched_shader", "./shaders/batched_shader.glsl" };

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
        0.5F, 0.5F, 0.0F, // top right
        0.5F, -0.5F, 0.0F, // bottom right
        -0.5F, -0.5F, 0.0F, // bottom left
        -0.5F, 0.5F, 0.0F, // top left
    };
    
    unsigned int indices[] = {
        // note that we start from 0!
        0, 1, 3, // first Triangle
        1, 2, 3 // second Triangle
    };

    //----- INSTANCE DATA -----


    glcore::vertex_buffer_layout layout{
        {glcore::shader_data_type::vec3, "aPos"}};

    glcore::vertex_buffer_layout instance_layout{
        {glcore::shader_data_type::vec3, "instancePos"}};
    

    glcore::instanced_vbo VBO{vertices, sizeof(vertices), instance_layout, layout};
    glcore::index_buffer EBO{indices, 6};

    glcore::instanced_vao VAO;

    VAO.set_vertex_buffer(std::move(VBO));
    VAO.set_index_buffer(std::move(EBO));

    float offsets[] = {
        // four triangles, four translat
        0.0, 0.0, 0.0F, // top right
    };

    for(int i = 0; i < 1; ++i)
    {
        VBO.add_instance(offsets + i * 3, 3 * sizeof(float));
    }

    basic.bind();

    while (glfwWindowShouldClose(window) == 0) {
        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.2F, 0.3F, 0.3F, 1.0F);
        glClear(GL_COLOR_BUFFER_BIT);

        // draw our first triangle
        VAO.bind(); // seeing as we only have a single VAO there's
                    // no need to bind it every time, but we'll do
                    // so to keep things a bit more organized
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // glDrawArraysInstanced(GL_TRIANGLES, 0, 6, 1);
        // glBindVertexArray(0); // no need to unbind it every time

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse
        // moved etc.)
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
