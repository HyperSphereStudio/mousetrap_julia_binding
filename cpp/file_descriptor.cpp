#include "../mousetrap_julia_binding.hpp"

// ### FILE DESCRIPTOR

void implement_file_descriptor(jlcxx::Module& module)
{
    auto file = module.add_type(FileDescriptor)
    .constructor<>(USE_FINALIZERS)
    .constructor([](void* internal){
        return new FileDescriptor(G_FILE(internal));
    })
    .constructor<const std::string&>(USE_FINALIZERS)
    .add_type_method(FileDescriptor, create_from_path, !)
    .add_type_method(FileDescriptor, create_from_uri, !)
    .add_type_method(FileDescriptor, get_name)
    .add_type_method(FileDescriptor, get_path)
    .add_type_method(FileDescriptor, get_uri)
    .add_type_method(FileDescriptor, get_path_relative_to)
    .add_type_method(FileDescriptor, get_parent)
    .add_type_method(FileDescriptor, get_file_extension)
    .add_type_method(FileDescriptor, exists)
    .add_type_method(FileDescriptor, is_folder)
    .add_type_method(FileDescriptor, is_file)
    .add_type_method(FileDescriptor, is_symlink)
    .add_type_method(FileDescriptor, read_symlink)
    .add_type_method(FileDescriptor, is_executable)
    .add_type_method(FileDescriptor, get_content_type)
    .method("query_info", [](FileDescriptor& file, const std::string& query) {
        return file.query_info(query.c_str());
    })
    .add_type_method(FileDescriptor, hash)
    .method("create_monitor", [](FileDescriptor& file) -> void* {
        return file.create_monitor().get_internal();
    })
    .method("get_children", [](FileDescriptor& file, bool recursive) -> jl_value_t* {

        auto children = file.get_children(recursive);
        static auto* ctor = jl_eval_string("Vector{Ptr{Cvoid}}");
        static auto* resize = jl_get_function(jl_base_module, "resize!");

        auto* out = jl_calln(ctor);
        jl_calln(resize, out, jl_box_int64(children.size()));

        for (uint64_t i = 0; i < children.size(); ++i)
            jl_arrayset((jl_array_t*) out, jl_box_voidpointer(children.at(i).get_internal()), i);

        return out;
    })
    ;

    module.method("file_descriptor_equal", [](FileDescriptor& a, FileDescriptor& b) -> bool {
        return a == b;
    });
}