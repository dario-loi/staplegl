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

// quick and dirty models as C headers.
#include "box.h"
#include "screen_quad.h"
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
const unsigned int SCR_WIDTH = 1200;
const unsigned int SCR_HEIGHT = 800;

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
    if (type == GL_DEBUG_TYPE_PERFORMANCE)
        return;
    fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x,\nmessage = %s\n",
        (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
        type, severity, message);

    // print location of error
    fprintf(stderr, "source = 0x%x, id = %d\n", source, id);
}

constexpr uint32_t calc_pyramid_levels(uint32_t width, uint32_t height)
{
    uint32_t levels = 1;
    while (width > 16 && height > 16) {
        width /= 2;
        height /= 2;
        levels++;
    }
    return levels;
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
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);

    glfwWindowHint(GLFW_SAMPLES, 4); // MSAA
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

    // enabe OpenGL features: depth test, MSAA, debug output, face culling
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_CULL_FACE);

    // set up tessellation
    glPatchParameteri(GL_PATCH_VERTICES, 3);

    glDebugMessageCallback(MessageCallback, nullptr);

    // load our shaders, one for the teapot, one for the skybox, also set up the
    // skybox shader to use the cubemap texture.
    glcore::shader_program teapot_shader { "teapot_shader", "./shaders/teapot_shader.glsl" };
    glcore::shader_program skybox_shader { "skybox_shader", "./shaders/skybox_shader.glsl" };
    glcore::shader_program light_shader { "light_shader", "./shaders/light_shader.glsl" };
    glcore::shader_program tonemap_shader { "tone_mapping", "./shaders/tone_mapping.glsl" };

    skybox_shader.bind();
    skybox_shader.upload_uniform1i("skybox", 0);

    teapot_shader.bind(); // we also set the skybox as the environment map for the teapot.
    teapot_shader.upload_uniform1i("environment", 0);

    tonemap_shader.bind();
    tonemap_shader.upload_uniform1i("scene", 4);

    // set up framebuffers for bloom effect

    glcore::framebuffer hdr_fbo {
        glcore::ATTACH_DEPTH_STENCIL_BUFFER,
        { SCR_WIDTH, SCR_HEIGHT },
        { GL_RGBA16F, GL_RGBA, GL_FLOAT }
    };

    hdr_fbo.bind();
    hdr_fbo.get_texture(0)->set_unit(4);

    if (!hdr_fbo.assert_completeness()) {
        std::cout << "HDR framebuffer not complete!" << std::endl;
        return -1;
    }

    hdr_fbo.unbind();
    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------

    // Teapot model
    glcore::vertex_buffer_layout layout_3P_3N { { shader_type::vec3, "aPos" }, { shader_type::vec3, "aNormal" } };

    glcore::vertex_buffer VBO { { teapot_vertices, TEAPOT_VERTEX_COMPONENTS * TEAPOT_VERTICES }, glcore::driver_draw_hint::STATIC_DRAW };
    VBO.set_layout(layout_3P_3N);

    glcore::index_buffer EBO {
        { teapot_indices, TEAPOT_INDICES }
    };

    glcore::vertex_array VAO;

    VAO.add_vertex_buffer(std::move(VBO));
    VAO.set_index_buffer(std::move(EBO));
    VAO.unbind(); // unbind to ensure that the VAO is not modified by accident.

    // Cube model used for the skybox

    glcore::vertex_buffer_layout layout_3P { { shader_type::vec3, "aPos" } };

    glcore::vertex_buffer skybox_VBO { { skybox_vertices, SKYBOX_VERTS },
        glcore::driver_draw_hint::STATIC_DRAW };
    skybox_VBO.set_layout(layout_3P); // we reuse the same layout as the teapot.

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

    // screen quad for post-processing

    glcore::vertex_buffer_layout layout_3P_2UV { { shader_type::vec3, "aPos" }, { shader_type::vec2, "aTexCoord" } };

    glcore::vertex_buffer quad_VBO { { quadVertices, GLCORE_QUAD_VERTICES },
        glcore::driver_draw_hint::STATIC_DRAW };

    quad_VBO.set_layout(layout_3P_2UV);
    glcore::vertex_array quad_VAO;

    quad_VAO.add_vertex_buffer(std::move(quad_VBO));
    quad_VAO.unbind();

    /*

        UNIFORMS:

        we have three uniform blocks, one holding the camera data and the various matrices,
        one holding the light data, and one holding the material data.

    */

    glcore::vertex_buffer_layout camera_block_layout {
        { shader_type::mat4, "projection" },
        { shader_type::mat4, "view" },
        { shader_type::mat4, "model" },
        { shader_type::vec4, "camera_pos" }
    };

    glcore::uniform_buffer camera_block { camera_block_layout, 0 };

    glcore::vertex_buffer_layout light_block_layout {
        { shader_type::vec4, "light_pos" },
        { shader_type::vec4, "light_color" },
        { shader_type::vec4, "light_attenuation" }, // 0 : constant, 1 : linear, 2 : quadratic, 3 : padding
        { shader_type::vec2, "light_intensities" } // 0 : diffuse, 1 : specular
    };

    glcore::uniform_buffer light_block { light_block_layout, 1 };

    const glm::vec4 light_pos { 1.0F, 1.0F, 8.0F, 1.0F };
    const glm::vec4 light_color { 0.0F, 0.0F, 1.0F, 1.0F };

    light_block.bind();
    light_block.set_attribute_data(std::span { glm::value_ptr(light_pos), 4 }, "light_pos");
    light_block.set_attribute_data(std::span { glm::value_ptr(light_color), 4 }, "light_color");
    light_block.set_attribute_data(std::span { glm::value_ptr(glm::vec4(1.0F, 0.14F, 0.07F, 0.0F)), 4 }, "light_attenuation");
    light_block.set_attribute_data(std::span { glm::value_ptr(glm::vec2(5.0F, 1.2F)), 2 }, "light_intensities");
    light_block.unbind();

    glcore::vertex_buffer_layout material_block_layout {
        { shader_type::vec4, "material_color" },
        { shader_type::float32, "material_shininess" },
        { shader_type::float32, "material_roughness" }
    };
    glcore::uniform_buffer material_block { material_block_layout, 2 };

    // teapot materials

    const glm::vec4 teapot_color { 0.51F, 0.55F, 0.66F, 1.0F };
    const glm::vec4 teapot_specular { 0.5F, 0.5F, 0.5F, 1.0F };
    const float teapot_shininess = 16.0F;
    const float teapot_roughness = 0.25F;

    material_block.bind();
    material_block.set_attribute_data(std::span { glm::value_ptr(teapot_color), 4 }, "material_color");
    material_block.set_attribute_data(std::span { &teapot_shininess, 1 }, "material_shininess");
    material_block.set_attribute_data(std::span { &teapot_roughness, 1 }, "material_roughness");
    material_block.unbind();

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
    camera_block.bind();
    glClearColor(0.F, 0.F, 0.F, 1.0F);

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
        camera_block.set_attribute_data(std::span { glm::value_ptr(camera_pos), 4 }, "camera_pos");
        camera_block.set_attribute_data(std::span { glm::value_ptr(view), 16 }, "view");
        camera_block.set_attribute_data(std::span { glm::value_ptr(projection), 16 }, "projection");

        // Draw calls start here

        // clear total black

        hdr_fbo.bind();

        if (!hdr_fbo.assert_completeness()) {
            std::cout << "Framebuffer not complete!" << std::endl;
            return -1;
        }
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // ------

        // prep to render the skybox
        skybox_VAO.bind();
        skybox_shader.bind();

        // make the skybox huge and centered on the camera.
        // in a more complex scene you would have it follow the camera.
        glm::mat4 skybox_mat = glm::scale(glm::mat4(1.0F), glm::vec3(50.0F, 50.0F, 50.0F));
        camera_block.set_attribute_data(std::span { glm::value_ptr(skybox_mat), 16 }, "model");

        // disable depth writing so that the skybox is always drawn behind everything else.
        // we do not need to invert culling due to the skybox's indices being wound in the opposite direction.
        glDepthMask(GL_FALSE);
        glDrawElements(GL_TRIANGLES, skybox_VAO.index_data().count(), GL_UNSIGNED_INT, nullptr);
        glDepthMask(GL_TRUE);

        // draw the blue light source
        // we use the same VAO as the skybox, but we change the shader and the model matrix.

        glm::mat4 light_mat = glm::translate(glm::mat4(1.0F), glm::vec3(light_pos));
        camera_block.set_attribute_data(std::span { glm::value_ptr(light_mat), 16 }, "model");

        light_shader.bind();
        // since the skybox has an inverted winding order, we need to invert culling
        // (we want to see the cube from outside now), so the effect is not cancelled out.
        glFrontFace(GL_CW);
        glDrawElements(GL_TRIANGLES, skybox_VAO.index_data().count(), GL_UNSIGNED_INT, nullptr);
        glFrontFace(GL_CCW); // on a real program, use better models :)

        // draw the teapot

        // The teapot model matrix flips it upside-down, so that it is rendered correctly.
        glm::mat4 model_mat = glm::scale(model, glm::vec3(1.0F, -1.0F, 1.0F));
        camera_block.set_attribute_data(std::span { glm::value_ptr(model_mat), 16 }, "model");

        VAO.bind();
        teapot_shader.bind();

        glDrawElements(GL_TRIANGLES, VAO.index_data().count(), GL_UNSIGNED_INT, nullptr);

        // HDR post-processing
        glcore::framebuffer::bind_default();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        tonemap_shader.bind();
        quad_VAO.bind();

        auto const& hdr_texture = hdr_fbo.get_texture(0);

        hdr_texture->bind();
        hdr_texture->set_unit(4);

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // no need to de-allocate anything as glcore handles all the OpenGL objects in a RAII fashion.

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