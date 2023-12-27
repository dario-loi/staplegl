/**
 * @file teapot.cpp
 * @author Dario Loi
 * @brief More complex example of staplegl usage with the Utah Teapot model.
 *
 * @copyright MIT License
 *
 * @example teapot.cpp
 */

#include "glad.h"
#include "staplegl.hpp"
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

// glm linear algebra library, NOT needed for staplegl, just for the example.
#include "glm.hpp"
#include "gtc/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"

// stb_image in order to load up textures.
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

/*

        IMPORTANT: in order for the program to correctly compile, go to
        include/staplegl/modules/gl_functions.hpp and replace the placeholder
        with the provided OpenGL loader ("glad.h").

*/

#define STAPLEGL_TEAPOT_LOW_MSAA

#ifdef STAPLEGL_TEAPOT_LOW_MSAA
constexpr auto MSAA = staplegl::tex_samples::MSAA_X1;
constexpr auto MSAA_SAMPLES = 1;
#else
constexpr auto MSAA = staplegl::tex_samples::MSAA_X8;
constexpr auto MSAA_SAMPLES = 8;
#endif

// glfw callbacks
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

// initial window size
const int32_t SCR_WIDTH = 1600;
const int32_t SCR_HEIGHT = 900;

// global luminosity to be updated by keypresses
float luminosity = 10.0F; // NOLINT

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
    // if (type == GL_DEBUG_TYPE_PERFORMANCE || type == GL_DEBUG_TYPE_OTHER) {
    //     return;
    // }

    fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x,\nmessage = %s\n", // NOLINT
        (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
        type, severity, message);

    // print location of error
    fprintf(stderr, "source = 0x%x, id = %d\n", source, id); // NOLINT
}

constexpr auto calc_pyramid_levels(uint32_t width, uint32_t height) -> uint32_t
{
    uint32_t levels = 1;
    while (width > 2 && height > 2) {
        width /= 2;
        height /= 2;
        levels++;
    }
    return levels;
}

// global aspect ratio, used for the projection matrix, please don't do this in
// a real program.
float aspect_ratio = static_cast<double>(SCR_WIDTH) / static_cast<double>(SCR_HEIGHT); // NOLINT

auto main() -> int
{

    std::string_view const hello_message {
        "Hello! This is a more complex example of staplegl usage, featuring the Utah Teapot model.\n"
        "Press the U and D keys to increase (U) and decrease (D) the luminosity of the light source.\n"
        "Play around with them to observe how the bloom effect changes."
    };

    std::cout << hello_message << std::endl;

    // shorten layout declarations a bit.
    using namespace staplegl::shader_data_type;

    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);

    glfwWindowHint(GLFW_SAMPLES, MSAA_SAMPLES); // MSAA
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
    if (gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)) == 0) { // NOLINT
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // enabe OpenGL features: depth test, MSAA, debug output, face culling
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

#ifdef STAPLEGL_DEBUG
    glDebugMessageCallback(MessageCallback, nullptr);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
    std::clog << "Printing OpenGL version info:" << std::endl;
    std::clog << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;
    std::clog << "OpenGL vendor: " << glGetString(GL_VENDOR) << std::endl;
    std::clog << "OpenGL renderer: " << glGetString(GL_RENDERER) << std::endl;
#endif

    // Set up all the shaders
    staplegl::shader_program teapot_shader { "teapot_shader", "./shaders/teapot_shader.glsl" };
    staplegl::shader_program skybox_shader { "skybox_shader", "./shaders/skybox_shader.glsl" };
    staplegl::shader_program light_shader { "light_shader", "./shaders/light_shader.glsl" };
    staplegl::shader_program tonemap_shader { "tone_mapping", "./shaders/tone_mapping.glsl" };
    staplegl::shader_program downsample_shader { "downsample", "./shaders/downsample_shader.glsl" };
    staplegl::shader_program upsample_shader { "upsample", "./shaders/upsample_shader.glsl" };
    staplegl::shader_program passthrough_shader { "passthrough", "./shaders/passthrough_shader.glsl" };

    skybox_shader.bind();
    skybox_shader.upload_uniform1i("skybox", 0);

    teapot_shader.bind();
    teapot_shader.upload_uniform1i("environment", 0);

    passthrough_shader.bind();
    passthrough_shader.upload_uniform1i("scene", 1);

    tonemap_shader.bind();
    tonemap_shader.upload_uniform1i("scene", 1);
    tonemap_shader.upload_uniform1i("bloom", 2);

    downsample_shader.bind();
    downsample_shader.upload_uniform1i("scene", 1);

    upsample_shader.bind();
    upsample_shader.upload_uniform1i("scene", 1);

    // set up framebuffers and textures for HDR and bloom effect
    std::array<staplegl::texture_2d, calc_pyramid_levels(SCR_WIDTH, SCR_HEIGHT)> pyramid_textures {};

    staplegl::texture_2d const msaa_color {
        std::span<const float> {},
        staplegl::resolution { SCR_WIDTH, SCR_HEIGHT },
        staplegl::texture_color {
            .internal_format = GL_RGBA16F, .format = GL_RGBA, .datatype = GL_FLOAT },
        staplegl::texture_filter {
            .min_filter = GL_LINEAR, .mag_filter = GL_LINEAR, .clamping = GL_CLAMP_TO_EDGE },
        MSAA
    };

    staplegl::texture_2d hdr_color {
        std::span<const float> {},
        staplegl::resolution { SCR_WIDTH, SCR_HEIGHT },
        staplegl::texture_color {
            .internal_format = GL_RGBA16F, .format = GL_RGBA, .datatype = GL_FLOAT },
        staplegl::texture_filter {
            .min_filter = GL_LINEAR, .mag_filter = GL_LINEAR, .clamping = GL_CLAMP_TO_EDGE }
    };

    // construct a texture pyramid, with each texture being half the size of the previous one.
    // iteratively downsampling from screen resolution towards the bottom will allow us to
    // spread the bloom effect over a wide area, with more details.
    for (int i = 0; auto& tex : pyramid_textures) {
        tex = staplegl::texture_2d {
            std::span<const float> {},
            staplegl::resolution { SCR_WIDTH >> i, SCR_HEIGHT >> i },
            staplegl::texture_color {
                .internal_format = GL_RGBA16F, .format = GL_RGBA, .datatype = GL_FLOAT },
            staplegl::texture_filter {
                .min_filter = GL_LINEAR, .mag_filter = GL_LINEAR, .clamping = GL_CLAMP_TO_EDGE }
        };
        i++;
    }

    staplegl::framebuffer msaa_fbo {};
    staplegl::framebuffer post_fbo {};

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------

    // Teapot model
    staplegl::vertex_buffer_layout const layout_3P_3N { { u_type::vec3, "aPos" }, { u_type::vec3, "aNormal" } };

    staplegl::vertex_buffer VBO { { teapot_vertices }, // std::span extracts the size from the C array, preventing decay.
        staplegl::driver_draw_hint::STATIC_DRAW };
    VBO.set_layout(layout_3P_3N);

    staplegl::index_buffer EBO {
        { teapot_indices } // capture the array again.
    };

    staplegl::vertex_array VAO;

    VAO.add_vertex_buffer(std::move(VBO));
    VAO.set_index_buffer(std::move(EBO));
    VAO.unbind(); // unbind to ensure that the VAO is not modified by accident.

    // Cube model used for the skybox

    staplegl::vertex_buffer_layout const layout_3P { { u_type::vec3, "aPos" } };

    staplegl::vertex_buffer skybox_VBO { { skybox_vertices }, // std::span extracts the size from the C array, preventing decay.
        staplegl::driver_draw_hint::STATIC_DRAW };
    skybox_VBO.set_layout(layout_3P); // we reuse the same layout as the teapot.

    // the skybox has no indices per se, we generate them on the fly.
    std::vector<unsigned int> skybox_indices(SKYBOX_VERTS / 3);
    std::generate(skybox_indices.begin(), skybox_indices.end(),
        [n = 0]() mutable { return n++; });
    staplegl::index_buffer skybox_EBO { skybox_indices };

    staplegl::vertex_array skybox_VAO;

    skybox_VAO.add_vertex_buffer(std::move(skybox_VBO));
    skybox_VAO.set_index_buffer(std::move(skybox_EBO));
    skybox_VAO.unbind(); // another unbind for good measure.

    // declare a simple model matrix, to be modified in the render loop.
    glm::mat4 const model = glm::mat4(1.0F);

    // screen quad for post-processing

    staplegl::vertex_buffer_layout const layout_3P_2UV { { u_type::vec3, "aPos" }, { u_type::vec2, "aTexCoord" } };

    staplegl::vertex_buffer quad_VBO { { quadVertices }, // std::span extracts the size from the C array, preventing decay.
        staplegl::driver_draw_hint::STATIC_DRAW };

    quad_VBO.set_layout(layout_3P_2UV);
    staplegl::vertex_array quad_VAO;

    quad_VAO.add_vertex_buffer(std::move(quad_VBO));
    quad_VAO.unbind();

    /*

        UNIFORMS:

        we have three uniform blocks, one holding the camera data and the various matrices,
        one holding the light data, and one holding the material data.

    */

    staplegl::vertex_buffer_layout const camera_block_layout {
        { u_type::mat4, "projection" },
        { u_type::mat4, "view" },
        { u_type::mat4, "model" },
        { u_type::vec4, "camera_pos" }
    };

    staplegl::uniform_buffer camera_block { camera_block_layout, 0 };

    staplegl::vertex_buffer_layout const light_block_layout {
        { u_type::vec4, "light_pos" },
        { u_type::vec4, "light_color" },
        { u_type::vec4, "light_attenuation" }, // 0 : constant, 1 : linear, 2 : quadratic, 3 : padding
        { u_type::vec2, "light_intensities" } // 0 : diffuse, 1 : specular
    };

    staplegl::uniform_buffer light_block { light_block_layout, 1 };

    const glm::vec4 light_pos { 1.0F, 1.0F, 10.0F, 1.0F };
    const glm::vec4 light_color { 0.9333F, 0.5098, 0.9333F, 1.0F };

    light_block.bind();
    light_block.set_attribute_data(std::span { glm::value_ptr(light_pos), 4 }, "light_pos");
    light_block.set_attribute_data(std::span { glm::value_ptr(light_color), 4 }, "light_color");
    light_block.set_attribute_data(std::span { glm::value_ptr(glm::vec4(1.0F, 0.045F, 0.0075F, 0.0F)), 4 }, "light_attenuation");
    light_block.set_attribute_data(std::span { glm::value_ptr(glm::vec2(luminosity, 1.2F)), 2 }, "light_intensities");
    light_block.unbind();

    staplegl::vertex_buffer_layout const material_block_layout {
        { u_type::vec4, "material_color" },
        { u_type::float32, "material_shininess" },
        { u_type::float32, "material_roughness" }
    };
    staplegl::uniform_buffer material_block { material_block_layout, 2 };

    // teapot materials
    const glm::vec4 teapot_color { 0.51F, 0.55F, 0.66F, 1.0F };
    const float teapot_shininess = 32.0F;
    const float teapot_roughness = 0.80F;

    material_block.bind();
    material_block.set_attribute_data(std::span { glm::value_ptr(teapot_color), 4 }, "material_color");
    material_block.set_attribute_data(std::span { &teapot_shininess, 1 }, "material_shininess");
    material_block.set_attribute_data(std::span { &teapot_roughness, 1 }, "material_roughness");
    material_block.unbind();

    // load up cubemap texture
    // goes in order: right, left, top, bottom, front, back
    std::array<std::string, 6> const faces {
        "./assets/skybox/right.jpg", "./assets/skybox/left.jpg",
        "./assets/skybox/top.jpg", "./assets/skybox/bottom.jpg",
        "./assets/skybox/front.jpg", "./assets/skybox/back.jpg"
    };

    // prep for data transfer
    std::array<std::span<std::byte>, 6> cube_data;

    std::int32_t width = 0, height = 0, nrChannels = 0;
    int i = 0;
    for (auto& face : faces) {
        auto* data = reinterpret_cast<std::byte*>( // NOLINT
            stbi_load(face.c_str(), &width, &height, &nrChannels, 0));
        if (data == nullptr) {
            std::cout << "Failed to load texture" << std::endl;
            return -1;
        }
        // pack data in array
        cube_data[i++] = std::span<std::byte> { data, static_cast<size_t>(width * height * nrChannels) };
    }

    // create the cubemap texture
    // srgb8 is used to ensure that the texture is correctly gamma corrected.
    // the static casts are necessary to specify that a narrowing conversion is
    // intended.
    staplegl::cubemap const skybox {
        cube_data, { width, height },
        { .internal_format = GL_SRGB8, .format = GL_RGB, .datatype = GL_UNSIGNED_BYTE },
        { .min_filter = GL_LINEAR, .mag_filter = GL_LINEAR, .clamping = GL_CLAMP_TO_EDGE },
        true
    };

    for (auto& face : cube_data) {
        stbi_image_free(face.data());
    }

    // set the texture to unit 0, so that the shader can find it.
    skybox.set_unit(0);

    glClearColor(0.F, 0.F, 0.F, 1.0F);

    while (glfwWindowShouldClose(window) == 0) {
        // input
        // -----
        processInput(window);

        light_block.bind();
        light_block.set_attribute_data(std::span { glm::value_ptr(glm::vec2(luminosity, 1.2F)), 2 }, "light_intensities");

        // Rotate the camera around the teapot.
        const auto time = static_cast<float>(glfwGetTime());
        const float radius = 4.0F;
        const float slow_factor = 0.25F;

        const float camX = sin(time * slow_factor) * radius;
        const float camZ = cos(time * slow_factor) * radius;
        const float camY = sin(time * slow_factor) + 0.5F; // slightly over the teapot.

        // gen camera vector and make it look at the teapot (center of the screen).
        glm::vec4 camera_pos { camX, camY, camZ, 1.0F };

        // gen the view and projection matrices.
        glm::mat4 view = glm::lookAt(glm::vec3(camera_pos), glm::vec3(0.0F, 0.0F, 0.0F),
            glm::vec3(0.0F, 1.0F, 0.0F));
        glm::mat4 projection = glm::perspective(glm::radians(75.0F), aspect_ratio, 0.01F, 100.0F);

        // send the data to the GPU through the uniform block.
        camera_block.bind();
        camera_block.set_attribute_data(
            std::span { glm::value_ptr(camera_pos), 4 }, "camera_pos");
        camera_block.set_attribute_data(std::span { glm::value_ptr(view), 16 }, "view");
        camera_block.set_attribute_data(std::span { glm::value_ptr(projection), 16 }, "projection");

        // first off, we bind the framebuffer and start drawing to our HDR texture.
        msaa_fbo.bind();
        msaa_fbo.set_texture(msaa_color);
        msaa_fbo.set_renderbuffer(
            { SCR_WIDTH,
                SCR_HEIGHT }, // get a renderbuffer of the same size as the screen.
            staplegl::fbo_attachment::ATTACH_DEPTH_STENCIL_BUFFER,
            MSAA); // set the same samples as the color texture.

        if (!msaa_fbo.assert_completeness()) [[unlikely]] {
            std::cerr << "Framebuffer not complete, line: " << __LINE__ << std::endl;
            return EXIT_FAILURE; // check for completeness
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear the framebuffer

        /*

            Rendering with MSAA

        */

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

        /*

            Resolve MSAA to HDR framebuffer

        */

        post_fbo.bind();
        post_fbo.set_texture(hdr_color);

        staplegl::framebuffer::transfer_data(msaa_fbo, post_fbo, { SCR_WIDTH, SCR_HEIGHT });

        post_fbo.bind();
        post_fbo.set_renderbuffer({ 0, 0 }, staplegl::fbo_attachment::NONE);

        /*

            Post-processing

        */

        // bind screen-quad, every draw call will now simply
        // transfer content from one texture to another.

        quad_VAO.bind();

        // copy stuff to the bloom pyramid
        passthrough_shader.bind();
        hdr_color.set_unit(1);
        post_fbo.set_texture(pyramid_textures[0], 0);

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4); // copy the scene to the lowest level of the pyramid

        downsample_shader.bind();

        // work our way down the levels of the pyramid, downsampling the texture each time.
        for (size_t i = 0; i < pyramid_textures.size() - 1; i++) {
            // downsample from texture i to texture i + 1

            auto& draw_source = pyramid_textures[i];
            auto& draw_target = pyramid_textures[i + 1];
            auto const& t_res = draw_target.get_resolution();

            draw_source.set_unit(1);

            post_fbo.set_texture(draw_target, 0);
            post_fbo.set_viewport(
                { t_res.width,
                    t_res.height });

            downsample_shader.upload_uniform2f(
                "uResolution", static_cast<float>(t_res.width),
                static_cast<float>(t_res.height));

            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        }

        // now every level of the pyramid is filled with a downsampled version of the scene.
        // we can now upsample the pyramid and add the results together to get the bloom effect.

        // blending for Bloom
        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE);
        glBlendEquation(GL_FUNC_ADD);

        upsample_shader.bind();

        for (int i = pyramid_textures.size() - 1; i > 0; i--) {
            // upsample from texture i to texture i - 1

            auto& draw_source = pyramid_textures[i];
            auto& draw_target = pyramid_textures[i - 1];

            draw_source.set_unit(1);

            post_fbo.set_texture(draw_target, 0);
            post_fbo.set_viewport(
                { draw_target.get_resolution().width,
                    draw_target.get_resolution().height });
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        }
        glDisable(GL_BLEND);

        // After upscaling the pyramid, we have the bloom effect in the lowest level of the pyramid.
        // we can combine it with the original scene to get the final result.

        // HDR post-processing
        staplegl::framebuffer::bind_default();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        tonemap_shader.bind();
        // bind the pyramid texture and the original scene
        hdr_color.set_unit(1);
        pyramid_textures[0].set_unit(2);

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // no need to de-allocate anything as staplegl handles all the OpenGL objects in a RAII fashion.

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
    if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS) {
        luminosity += 0.1F;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        luminosity = std::max(0.0F, luminosity - 0.1F);
    }
    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
        // toggle mesh wireframe
        static bool wireframe = false;
        wireframe = !wireframe;
        glPolygonMode(GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);
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
    aspect_ratio = static_cast<float>(width) / static_cast<float>(height);
}
