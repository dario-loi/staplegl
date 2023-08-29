/**
 * @file sandbox.cpp
 * @author Dario Loi
 * @brief More complex example of glcore usage with the Utah Teapot model.
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

// quick and dirty teapot and skybox models as C headers.
#include "box.h"
#include "teapot_data.h"

// glm linear algebra library, NOT needed for glcore, just for the example.
#include "glm.hpp"
#include "gtc/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"

// stb_image in order to load up textures.
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

/*

        IMPORTANT: in order for the program to correctly compile, go to
        include/glcore/modules/gl_functions.hpp and replace the placeholder
        with the provided OpenGL loader ("glad.h").

*/

// glfw callbacks
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

// initial window size
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// OpenGL debug callback
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

    fprintf(stderr, "source = 0x%x, id = %d\n", source, id);
}

// global aspect ratio, used for the projection matrix, please don't do this in
// a real program.
float aspect_ratio = static_cast<double>(SCR_WIDTH) / static_cast<double>(SCR_HEIGHT);

auto main() -> int
{

    // shorten layout declarations a bit.
    using namespace glcore::shader_data_type;

    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    glfwWindowHint(GLFW_SAMPLES, 4); // MSAA
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

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

    // enabe OpenGL features: depth test, MSAA, debug output, face culling
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_CULL_FACE);

    glDebugMessageCallback(MessageCallback, nullptr);

    // load our shaders, one for the teapot, one for the skybox, also set up the
    // skybox shader to use the cubemap texture.
    glcore::shader_program teapot_shader { "teapot_shader", "./shaders/teapot_shader.glsl" };
    glcore::shader_program skybox_shader { "skybox_shader", "./shaders/skybox_shader.glsl" };

    skybox_shader.bind();
    skybox_shader.upload_uniform1i("skybox", 0);

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------

    // Teapot model
    glcore::vertex_buffer_layout teapot_layout { { shader_type::vec3, "aPos" }, { shader_type::vec3, "aNormal" } };

    glcore::vertex_buffer VBO { { teapot_vertices, TEAPOT_VERTEX_COMPONENTS * TEAPOT_VERTICES }, glcore::driver_draw_hint::STATIC_DRAW };
    VBO.set_layout(teapot_layout);

    glcore::index_buffer EBO {
        { teapot_indices, TEAPOT_INDICES }
    };

    glcore::vertex_array VAO;

    VAO.add_vertex_buffer(std::move(VBO));
    VAO.set_index_buffer(std::move(EBO));
    VAO.unbind(); // unbind to ensure that the VAO is not modified by accident.

    // Cube model used for the skybox

    glcore::vertex_buffer_layout cube_layout { { shader_type::vec3, "aPos" } };

    glcore::vertex_buffer skybox_VBO { { skybox_vertices, SKYBOX_VERTS },
        glcore::driver_draw_hint::STATIC_DRAW };
    skybox_VBO.set_layout(cube_layout); // we reuse the same layout as the teapot.

    // the skybox has no indices per se, we generate them on the fly.
    std::vector<unsigned int> skybox_indices(SKYBOX_VERTS / 3);
    std::generate(skybox_indices.begin(), skybox_indices.end(),
        [n = 0]() mutable { return n++; });
    glcore::index_buffer skybox_EBO { skybox_indices };

    glcore::vertex_array skybox_VAO;

    skybox_VAO.add_vertex_buffer(std::move(skybox_VBO));
    skybox_VAO.set_index_buffer(std::move(skybox_EBO));
    skybox_VAO.unbind(); // another unbind for good measure.

    // declare a simple model matrix, to be modified in the render loop.
    glm::mat4 model = glm::mat4(1.0F);

    // Declare the layout of the uniform block, and create the uniform block
    // that is going to hold the various matrices.
    glcore::vertex_buffer_layout uniform_block_layout {
        { shader_type::mat4, "projection" },
        { shader_type::mat4, "view" },
        { shader_type::mat4, "model" },
        { shader_type::vec4, "camera_pos" }
    };

    glcore::uniform_buffer uniform_block { uniform_block_layout, 0 };

    // load up cubemap texture
    // goes in order: right, left, top, bottom, front, back
    std::array<std::string, 6> faces {
        "./assets/skybox/right.jpg", "./assets/skybox/left.jpg",
        "./assets/skybox/top.jpg", "./assets/skybox/bottom.jpg",
        "./assets/skybox/front.jpg", "./assets/skybox/back.jpg"
    };

    // prep for data transfer
    std::array<std::byte*, 6> cube_data;

    std::int32_t width, height, nrChannels;
    int i = 0;
    for (auto& face : faces) {
        std::byte* data = reinterpret_cast<std::byte*>(
            stbi_load(face.c_str(), &width, &height, &nrChannels, 0));
        if (data == nullptr) {
            std::cout << "Failed to load cubemap texture" << std::endl;
            return -1;
        }
        // pack data in array
        cube_data[i++] = data;
    }

    // create the cubemap texture
    // srgb8 is used to ensure that the texture is correctly gamma corrected.
    // the static casts are necessary to specify that a narrowing conversion is
    // intended.
    glcore::cubemap skybox {
        cube_data, { static_cast<uint32_t>(width), static_cast<uint32_t>(height) },
        { .internal_format = GL_SRGB8, .format = GL_RGB, .datatype = GL_UNSIGNED_BYTE }, true
    };

    // bind the texture and set it to unit 0 so that it can be sampled from the shader.
    skybox.bind();
    skybox.set_unit(0);

    // bind the uniform block
    uniform_block.bind();
    while (glfwWindowShouldClose(window) == 0) {
        // input
        // -----
        processInput(window);

        // Rotate the camera around the teapot.
        const float time = static_cast<float>(glfwGetTime());
        const float radius = 4.0F;
        const float slow_factor = 0.5F;

        const float camX = sin(time * slow_factor) * radius;
        const float camZ = cos(time * slow_factor) * radius;
        const float camY = sin(time * slow_factor) + 0.5F; // slightly over the teapot.

        // gen camera vector and make it look at the teapot (center of the screen).
        glm::vec4 camera_pos { camX, camY, camZ, 1.0F };
        const glm::vec4 camera_target { 0.0F, 0.0F, 0.0F, 1.0F };

        // gen the view and projection matrices.
        glm::mat4 view = glm::lookAt(glm::vec3(camera_pos), glm::vec3(0.0F, 0.0F, 0.0F),
            glm::vec3(0.0F, 1.0F, 0.0F));
        glm::mat4 projection = glm::perspective(glm::radians(75.0F), aspect_ratio, 0.01F, 100.0F);

        // send the data to the GPU through the uniform block.
        uniform_block.set_attribute_data(std::span { glm::value_ptr(camera_pos), 4 }, "camera_pos");
        uniform_block.set_attribute_data(std::span { glm::value_ptr(view), 16 }, "view");
        uniform_block.set_attribute_data(std::span { glm::value_ptr(projection), 16 }, "projection");

        // Draw calls start here

        // clear total black
        glClearColor(0.F, 0.F, 0.F, 1.0F);
        glClear(GL_COLOR_BUFFER_BIT);
        glClear(GL_DEPTH_BUFFER_BIT);
        // ------

        // prep to render the skybox
        skybox_VAO.bind();
        skybox_shader.bind();

        // make the skybox huge and centered on the camera.
        // in a more complex scene you would have it follow the camera.
        glm::mat4 skybox_mat = glm::scale(glm::mat4(1.0F), glm::vec3(50.0F, 50.0F, 50.0F));
        uniform_block.set_attribute_data(std::span { glm::value_ptr(skybox_mat), 16 }, "model");

        // disable depth writing so that the skybox is always drawn behind everything else.
        // we do not need to invert culling due to the skybox's indices being wound in the opposite direction.
        glDepthMask(GL_FALSE);
        glDrawElements(GL_TRIANGLES, skybox_VAO.index_data().count(), GL_UNSIGNED_INT, nullptr);
        glDepthMask(GL_TRUE);

        // The teapot model matrix flips it upside-down, so that it is rendered correctly.
        glm::mat4 model_mat = glm::scale(model, glm::vec3(1.0F, -1.0F, 1.0F));
        uniform_block.set_attribute_data(std::span { glm::value_ptr(model_mat), 16 }, "model");

        VAO.bind();
        teapot_shader.bind();

        glDrawElements(GL_TRIANGLES, VAO.index_data().count(), GL_UNSIGNED_INT, nullptr);

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
    aspect_ratio = static_cast<double>(width) / static_cast<double>(height);
}
