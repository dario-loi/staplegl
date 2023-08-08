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

void GLAPIENTRY
MessageCallback(GLenum source,
    GLenum type,
    GLuint id,
    GLenum severity,
    GLsizei length,
    const GLchar* message,
    const void* userParam)
{
    if (type == GL_DEBUG_TYPE_OTHER)
        return;
    fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x,\nmessage = %s\n",
        (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
        type, severity, message);
}
// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

float lerp(float a, float b, float f)
{
    return a + f * (b - a);
}

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

    glcore::shader_program basic { "batched_shader", "./shaders/batched_shader.glsl" };

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
        0.001F, 0.001F, 0.0F, // top right
        0.001F, -0.001F, 0.0F, // bottom right
        -0.001F, -0.001F, 0.0F, // bottom left
        -0.001F, 0.001F, 0.0F, // top left
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

    float start = -0.9F;
    float end = 0.9F;

    VAO.bind();
    for(int i = 0; i < 2000; ++i)
    {
        float offset[3] = {
            lerp(start, end,
                 static_cast<float>(rand()) / static_cast<float>(RAND_MAX)),
            lerp(start, end,
                 static_cast<float>(rand()) / static_cast<float>(RAND_MAX)),
            lerp(start, end,
                 static_cast<float>(rand()) / static_cast<float>(RAND_MAX))};

        
        VAO.m_vbo.add_instance(offset, 3 * sizeof(float));

        // print current VAO state


    }
    VAO.unbind();

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

        // map vbo so I can use the debug output

        float* data { static_cast<float*>(glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE)) };

        std::cout << "data: " << data << std::endl;


        glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, 3);

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
