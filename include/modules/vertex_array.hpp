/**
 * @file vertex_array.hpp
 * @authors Christian Panov, Dario Loi
 * @brief Vertex Array Object (VAO) wrapper.
 *
 * @date 2023-04-28
 *
 * @copyright MIT License
 *
 * @details Vertex Array Object Wrapper. <br>
 *
 * Vertex Array Objects (VAOs) are OpenGL objects that store all of the state needed to supply vertex data
 * to the GPU for a draw call. <br>
 *
 * This includes the format of the vertex data as well as the buffers containing the vertices and
 * the indices specifying the order in which to render them. <br>
 *
 * @see vertex_buffer
 * @see index_buffer
 * @see vertex_buffer_layout
 * @see https://www.khronos.org/opengl/wiki/Vertex_Specification#Vertex_Array_Object
 *
 */

#pragma once
#include "gl_functions.hpp"
#include "index_buffer.hpp"
#include "vertex_buffer.hpp"
#include "vertex_buffer_inst.hpp"

#include <cstdint>
#include <list>
#include <optional>

namespace staplegl {

/**
 * @brief Vertex Array Object (VAO) wrapper.
 *
 * @details Vertex Array Objects are OpenGL objects that store all of the state needed to supply vertex data,
 * that is, an index buffer (also called an Element Buffer Object) and one or more vertex buffers.
 *
 * Rendering using VAOs is advised as it simplifies the process of rendering multiple objects with different
 * vertex data and different rendering modes.
 *
 * @see https://www.khronos.org/opengl/wiki/Vertex_Specification#Vertex_Array_Object
 * @see vertex_buffer.hpp
 * @see index_buffer.hpp
 * @see vertex_buffer_layout.hpp
 */
class vertex_array {
public:
    /**
     * @brief Construct a new vertex array object
     *
     * @details Constructs a VAO and generates an id for it, VBOs and EBOs can be added later.
     *
     * @see add_vertex_buffer
     * @see set_instance_buffer
     *
     */
    vertex_array() noexcept;
    ~vertex_array();

    vertex_array(const vertex_array&) = delete;
    auto operator=(const vertex_array&) -> vertex_array& = delete;

    /**
     * @brief Construct a new vertex array object from another one.
     *
     * @details constructs a VAO by stealing the ID and the attached VBOs and EBOs from another VAO.
     *
     * @param other the vertex array object to copy from.
     *
     */
    vertex_array(vertex_array&& other) noexcept
        : m_id(other.m_id)
        , m_vertex_buffers(std::move(other.m_vertex_buffers))
        , m_instanced_vbo(std::move(other.m_instanced_vbo))
        , m_index_buffer(std::move(other.m_index_buffer))
        , attrib_index(other.attrib_index)
    {
        other.m_id = 0;
    }

    /**
     * @brief Move assignment operator.
     *
     *
     * @param other the vertex array object to move from.
     * @return vertex_array& a reference to the moved vertex array object.
     */
    auto operator=(vertex_array&& other) noexcept -> vertex_array&
    {
        if (this != &other) {
            m_id = other.m_id;
            m_vertex_buffers = std::move(other.m_vertex_buffers);
            m_instanced_vbo = std::move(other.m_instanced_vbo);
            m_index_buffer = std::move(other.m_index_buffer);
            attrib_index = other.attrib_index;

            other.m_id = 0;
        }
        return *this;
    }

    /**
     * @brief Iterator type returned from add_vertex_buffer.
     *
     */
    using iterator_t = std::list<vertex_buffer>::iterator;

    /**
     * @brief Bind the vertex array object.
     *
     */
    void bind() const;

    /**
     * @brief Unbind the vertex array object.
     *
     */
    static void unbind();

    /**
     * @brief Add a vertex buffer to the vertex array object.
     * @param vbo the vertex buffer object to add.
     *
     * @return iterator_t an iterator to the newly added vertex buffer, it is guaranteed
     * to be valid for the lifetime of the vertex array object, useful to keep track of
     * the individual VBOs.
     *
     * @see vertex_buffer.hpp
     */
    auto add_vertex_buffer(vertex_buffer&& vbo) -> vertex_array::iterator_t;

    /**
     * @brief Set the instance buffer object
     *
     * @param vbo the instance buffer object to set.
     * @see vertex_buffer_inst.hpp
     */
    void set_instance_buffer(vertex_buffer_inst&& vbo);

    /**
     * @brief Clear the instance buffer object
     *
     */
    void clear_instance_buffer() { m_instanced_vbo.reset(); }

    /**
     * @brief Set the index buffer object
     *
     * @param ibo the index buffer object to set.
     * @see index_buffer.hpp
     */
    void set_index_buffer(index_buffer&& ibo);

    // utility functions

    /** @brief Get the vertex array object id.
     *
     * @return uint32_t the vertex array object id.
     */
    [[nodiscard]] constexpr auto id() const -> uint32_t { return m_id; }

    /**
     * @brief Get the vertex buffer object at the specified index.
     *
     * @return std::list<vertex_buffer>&  the vertex buffer object at the specified index.
     */
    [[nodiscard]] constexpr auto buffers_data() -> std::list<vertex_buffer>& { return m_vertex_buffers; }

    /**
     * @brief Get the instance buffer object.
     *
     * @return std::optional<vertex_buffer_inst>& the instance buffer object.
     */
    [[nodiscard]] constexpr auto instanced_data() -> std::optional<vertex_buffer_inst>& { return m_instanced_vbo; }

    /**
     * @brief Get the index buffer object.
     *
     * @return index_buffer& the index buffer object.
     */
    [[nodiscard]] constexpr auto index_data() -> index_buffer& { return m_index_buffer; }

private:
    std::uint32_t m_id {};
    std::list<vertex_buffer> m_vertex_buffers;
    std::optional<vertex_buffer_inst> m_instanced_vbo;
    index_buffer m_index_buffer;

    uint32_t attrib_index {};
};

/*

        IMPLEMENTATIONS

*/

inline vertex_array::vertex_array() noexcept
{
    glGenVertexArrays(1, &m_id);
}

inline vertex_array::~vertex_array()
{
    glDeleteVertexArrays(1, &m_id);
}

inline void vertex_array::bind() const
{
    glBindVertexArray(m_id);
}

inline void vertex_array::unbind()
{
    glBindVertexArray(0);
}

inline auto vertex_array::add_vertex_buffer(vertex_buffer&& vbo) -> vertex_array::iterator_t
{
    m_vertex_buffers.push_back(std::move(vbo));
    glBindVertexArray(m_id);

    // get a reference to the newly added vertex buffer from the variant in the vector
    vertex_buffer const& vbo_ref = m_vertex_buffers.back();

    vbo_ref.bind();

    for (const auto& [type, name, offset, element_count] : vbo_ref.layout().get_attributes()) {
        glEnableVertexAttribArray(attrib_index);
        glVertexAttribPointer(
            attrib_index++,
            static_cast<int32_t>(shader_data_type::component_count(type) * element_count),
            shader_data_type::to_opengl_underlying_type(type),
            GL_FALSE,
            static_cast<int32_t>(vbo_ref.layout().stride()),
            reinterpret_cast<const void*>(offset)); // NOLINT (reinterpret-cast)
    }

    return std::prev(m_vertex_buffers.end());
}

inline void vertex_array::set_instance_buffer(vertex_buffer_inst&& vbo)
{
    m_instanced_vbo = std::move(vbo);

    glBindVertexArray(m_id);
    m_instanced_vbo->bind();

    for (const auto& [type, name, offset, element_count] : m_instanced_vbo->layout().get_attributes()) {
        glEnableVertexAttribArray(attrib_index);
        glVertexAttribPointer(
            attrib_index++,
            static_cast<int32_t>(shader_data_type::component_count(type) * element_count),
            shader_data_type::to_opengl_underlying_type(type),
            GL_FALSE,
            static_cast<int32_t>(m_instanced_vbo->layout().stride()),
            reinterpret_cast<const void*>(offset)); // NOLINT (reinterpret-cast)
        glVertexAttribDivisor(attrib_index - 1, 1);
    }
}

inline void vertex_array::set_index_buffer(index_buffer&& ibo)
{
    m_index_buffer = std::move(ibo);

    glBindVertexArray(m_id);
    m_index_buffer.bind();
}
}