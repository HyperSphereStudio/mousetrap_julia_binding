#include "../mousetrap_julia_binding.hpp"

// ### KEY FILE

void implement_key_file(jlcxx::Module& module)
{
    auto key_file = module.add_type(KeyFile)
    .constructor<>(USE_FINALIZERS)
    .constructor<const std::string&>(USE_FINALIZERS)
    .add_type_method(KeyFile, as_string)
    .add_type_method(KeyFile, create_from_file, !)
    .add_type_method(KeyFile, create_from_string, !)
    .add_type_method(KeyFile, save_to_file)
    .add_type_method(KeyFile, get_groups)
    .add_type_method(KeyFile, get_keys)
    .add_type_method(KeyFile, has_key)
    .add_type_method(KeyFile, has_group)
    .add_type_method(KeyFile, set_comment_above_group, !)
    .add_type_method(KeyFile, set_comment_above_key, !)
    .add_type_method(KeyFile, get_comment_above_group)
    .add_type_method(KeyFile, get_comment_above_key)
    ;

    #define define_get_value_as(type, name) \
        key_file.method("get_value_as_" + std::string(name), [](KeyFile& file, KeyFile::GroupID group_id, KeyFile::KeyID key_id) { \
            return file.get_value_as<type>(group_id, key_id); \
        });

    #define define_set_value_as(type, name) \
        key_file.method("set_value_as_" + std::string(name) + "!", [](KeyFile& file, KeyFile::GroupID group_id, KeyFile::KeyID key_id, type value) { \
            return file.set_value_as<type>(group_id, key_id, value); \
        });

    #define define_get_value_as_list(type, name)\
        key_file.method("get_value_as_" + std::string(name) + "_list", [](KeyFile& file, KeyFile::GroupID group_id, KeyFile::KeyID key_id) { \
            return file.get_value_as<std::vector<type>>(group_id, key_id); \
        });

    #define define_set_value_as_list(type, name)\
        key_file.method("set_value_as_" + std::string(name) + "_list!", [](KeyFile& file, KeyFile::GroupID group_id, KeyFile::KeyID key_id, jl_value_t* in) { \
            std::vector<type> vec; \
            for (uint64_t i = 0; i < jl_array_len(in); ++i) \
                vec.push_back(jlcxx::unbox<type>(jl_arrayref((jl_array_t*) in, i))); \
            file.set_value_as<std::vector<type>>(group_id, key_id, vec); \
        });

    define_get_value_as(std::string, "string");
    define_set_value_as(std::string, "string");

    define_get_value_as(bool, "bool");
    define_set_value_as(bool, "bool");

    define_get_value_as(int, "int");
    define_set_value_as(int, "int");

    define_get_value_as(uint64_t, "uint");
    define_set_value_as(uint64_t, "uint");

    define_get_value_as(float, "float");
    define_set_value_as(float, "float");

    define_get_value_as(double, "double");
    define_set_value_as(double, "double");

    define_set_value_as(Image, "image");
    define_get_value_as(Image, "image");

    key_file.method("get_value_as_rgba", [](KeyFile& file, KeyFile::GroupID group, KeyFile::KeyID key) -> jl_value_t*{
       return box_rgba(file.get_value_as<RGBA>(group, key));
    });

    key_file.method("set_value_as_rgba!", [](KeyFile& file, KeyFile::GroupID group, KeyFile::KeyID key, jl_value_t* rgba){
        file.set_value_as<RGBA>(group, key, unbox_rgba(rgba));
    });

    key_file.method("get_value_as_hsva", [](KeyFile& file, KeyFile::GroupID group, KeyFile::KeyID key) -> jl_value_t*{
        return box_hsva(file.get_value_as<HSVA>(group, key));
    });

    key_file.method("set_value_as_hsva!", [](KeyFile& file, KeyFile::GroupID group, KeyFile::KeyID key, jl_value_t* hsva){
        file.set_value_as<HSVA>(group, key, unbox_hsva(hsva));
    });

    define_get_value_as_list(std::string, "string");

    key_file.method("set_value_as_string_list!", [](KeyFile& file, KeyFile::GroupID group, KeyFile::KeyID key, jl_value_t* list){

        auto vec = std::vector<std::string>();
        for (uint64_t i = 0; i < jl_array_len(list); ++i)
        {
            auto* ptr = jl_string_ptr(jl_arrayref((jl_array_t*) list, i));
            if (ptr != nullptr)
                vec.emplace_back(ptr);
        }

        file.set_value_as<std::vector<std::string>>(group, key, vec);
    });

    define_get_value_as_list(bool, "bool");
    define_set_value_as_list(bool, "bool");

    define_get_value_as_list(int, "int");
    define_set_value_as_list(int, "int");

    define_get_value_as_list(uint64_t, "uint");
    define_set_value_as_list(uint64_t, "uint");

    define_get_value_as_list(float, "float");
    define_set_value_as_list(float, "float");

    define_get_value_as_list(double, "double");
    define_set_value_as_list(double, "double");
}