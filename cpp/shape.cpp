#include "../mousetrap_julia_binding.hpp"

// ### SHAPE

void implement_shape(jlcxx::Module& module)
{
    #if MOUSETRAP_ENABLE_OPENGL_COMPONENT

    auto vertex = module.add_type(Vertex)
    .constructor([](float x, float y, jl_value_t* rgba){
        return new Vertex(x, y, RGBA(
        jl_unbox_float32(jl_get_property(rgba, "r")),
        jl_unbox_float32(jl_get_property(rgba, "g")),
        jl_unbox_float32(jl_get_property(rgba, "b")),
        jl_unbox_float32(jl_get_property(rgba, "a"))
        ));
    }, USE_FINALIZERS)
    .method("get_position", [](Vertex& vertex) -> jl_value_t* {
        return box_vector3f(vertex.position);
    })
    .method("set_position!", [](Vertex& vertex, jl_value_t* vec3){
        vertex.position = unbox_vector3f(vec3);
    })
    .method("get_color", [](Vertex& vertex) -> jl_value_t* {
        return box_rgba(vertex.color);
    })
    .method("set_color!", [](Vertex& vertex, jl_value_t* rgba){
        vertex.color = unbox_rgba(rgba);
    })
    .method("get_texture_coordinates", [](Vertex& vertex) -> jl_value_t* {
        return box_vector2f(vertex.position);
    })
    .method("set_texture_coordinates!", [](Vertex& vertex, jl_value_t* vec3){
        vertex.position = unbox_vector3f(vec3);
    })
    ;

    static auto unbox_vector_of_vec2 = [](jl_value_t* in) -> std::vector<Vector2f> {

        std::vector<Vector2f> out;

        for (uint64_t i = 0; i < jl_array_len(in); ++i)
            out.push_back(unbox_vector2f(jl_arrayref((jl_array_t*) in, i)));
        return out;
    };

    auto shape = module.add_type(Shape)
    .constructor<>(USE_FINALIZERS)
    .add_type_method(Shape, get_native_handle)
    .method("as_point!", [](Shape& shape, jl_value_t* vec2){
        shape.as_point(unbox_vector2f(vec2));
    })
    .method("as_points!", [](Shape& shape, jl_value_t* vector_of_vec2_in){
        shape.as_points(unbox_vector_of_vec2(vector_of_vec2_in));
    })
    .method("as_triangle!", [](Shape& shape, jl_value_t* a, jl_value_t* b, jl_value_t* c){
        shape.as_triangle(
        unbox_vector2f(a),
        unbox_vector2f(b),
        unbox_vector2f(c)
        );
    })
    .method("as_rectangle!", [](Shape& shape, jl_value_t* top_left, jl_value_t* size){
        shape.as_rectangle(
        unbox_vector2f(top_left),
        unbox_vector2f(size)
        );
    })
    .method("as_circle!", [](Shape& shape, jl_value_t* center, float radius, uint64_t n_outer_vertices){
        shape.as_circle(unbox_vector2f(center), radius, n_outer_vertices);
    })
    .method("as_ellipse!", [](Shape& shape, jl_value_t* center, float x_radius, float y_radius, uint64_t n_outer_vertices){
        shape.as_ellipse(unbox_vector2f(center), x_radius, y_radius, n_outer_vertices);
    })
    .method("as_line!", [](Shape& shape, jl_value_t* a, jl_value_t* b){
        shape.as_line(unbox_vector2f(a), unbox_vector2f(b));
    })
    .method("as_lines!", [](Shape& shape, jl_value_t* vec_of_tuple){
        std::vector<std::pair<Vector2f, Vector2f>> vec;
        vec.resize(jl_array_len(vec_of_tuple));

        for (uint64_t i = 0; i < jl_array_len(vec_of_tuple); ++i)
        {
            static auto* getindex = jl_get_function(jl_base_module, "getindex");

            auto* tuple = jl_arrayref((jl_array_t*) vec_of_tuple, i);
            vec.emplace_back(
            unbox_vector2f(jl_calln(getindex, tuple, jl_box_int64(1))),
            unbox_vector2f(jl_calln(getindex, tuple, jl_box_int64(2)))
            );
        }

        shape.as_lines(vec);
    })
    .method("as_line_strip!", [](Shape& shape, jl_value_t* vector_of_vec2_in)
    {
        shape.as_line_strip(unbox_vector_of_vec2(vector_of_vec2_in));
    })
    .method("as_polygon!", [](Shape& shape, jl_value_t* vector_of_vec2_in){
        shape.as_polygon(unbox_vector_of_vec2(vector_of_vec2_in));
    })
    .method("as_rectangular_frame!", [](Shape& shape, jl_value_t* top_left, jl_value_t* outer_size, float x_width, float y_width){
        shape.as_rectangular_frame(unbox_vector2f(top_left), unbox_vector2f(outer_size), x_width, y_width);
    })
    .method("as_circular_ring!", [](Shape& shape, jl_value_t* center, float outer_radius, float thickness, uint64_t n_outer_vertices){
        shape.as_circular_ring(unbox_vector2f(center), outer_radius, thickness, n_outer_vertices);
    })
    .method("as_elliptical_ring!", [](Shape& shape, jl_value_t* center, float x_radius, float y_radius, float x_thickness, float y_thickness, uint64_t n_outer_vertices){
        shape.as_elliptical_ring(unbox_vector2f(center), x_radius, y_radius, x_thickness, y_thickness, n_outer_vertices);
    })
    .method("as_wireframe!", [](Shape& shape, jl_value_t* vector_of_vec2_in){
        shape.as_wireframe(unbox_vector_of_vec2(vector_of_vec2_in));
    })
    .method("as_outline!", [](Shape& shape, Shape& other){
        shape.as_outline(other);
    })
    .method("render", [](Shape& shape, const Shader& shader, GLTransform& transform){
        shape.render(shader, transform);
    })
    .method("get_vertex_color", [](Shape& shape, uint64_t index) -> jl_value_t* {
        return box_rgba(shape.get_vertex_color(index));
    })
    .method("set_vertex_color!", [](Shape& shape, uint64_t index, jl_value_t* rgba){
        shape.set_vertex_color(index, unbox_rgba(rgba));
    })
    .method("get_vertex_texture_coordinate", [](Shape& shape, uint64_t index) -> jl_value_t* {
        return box_vector2f(shape.get_vertex_texture_coordinate(index));
    })
    .method("set_vertex_texture_coordinate!", [](Shape& shape, uint64_t index, jl_value_t* vec2){
        shape.set_vertex_texture_coordinate(index, unbox_vector2f(vec2));
    })
    .method("get_vertex_position", [](Shape& shape, uint64_t index) -> jl_value_t* {
        return box_vector3f(shape.get_vertex_position(index));
    })
    .method("set_vertex_position!", [](Shape& shape, uint64_t index, jl_value_t* vec){
        shape.set_vertex_position(index, unbox_vector3f(vec));
    })
    .add_type_method(Shape, get_n_vertices)
    .method("set_color!", [](Shape& shape, jl_value_t* rgba){
        shape.set_color(unbox_rgba(rgba));
    })
    .add_type_method(Shape, set_is_visible, !)
    .add_type_method(Shape, get_is_visible)
    .method("get_bounding_box", [](Shape& shape) -> jl_value_t* {
        static auto* rectangle_ctor = jl_eval_string("Mousetrap.AxisAlignedRectangle");
        auto bounds = shape.get_bounding_box();
        return jl_calln(rectangle_ctor, box_vector2f(bounds.top_left), box_vector2f(bounds.size));
    })
    .method("get_size", [](Shape& shape) -> jl_value_t* {
        return box_vector2f(shape.get_size());
    })
    .method("set_centroid!", [](Shape& shape, jl_value_t* vec){
        shape.set_centroid(unbox_vector2f(vec));
    })
    .method("get_centroid", [](Shape& shape) -> jl_value_t* {
        return box_vector2f(shape.get_centroid());
    })
    .method("set_top_left!", [](Shape& shape, jl_value_t* pos){
        shape.set_top_left(unbox_vector2f(pos));
    })
    .method("get_top_left", [](Shape& shape) -> jl_value_t* {
        return box_vector2f(shape.get_top_left());
    })
    .method("rotate!", [](Shape& shape, float angle_rad, float origin_x, float origin_y){
        shape.rotate(radians(angle_rad), Vector2f(origin_x, origin_y));
    })
    .method("set_texture!", [](Shape& shape, void* texture){
        shape.set_texture((TextureObject*) texture);
    })
    ;

    #endif // MOUSETRAP_ENABLE_OPENGL_COMPONENT
}