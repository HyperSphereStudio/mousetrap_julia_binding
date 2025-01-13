#include "../mousetrap_julia_binding.hpp"

// ### FILE CHOOSER

void implement_file_chooser(jlcxx::Module& module)
{
    define_enum_in(module, FileChooserAction);
    module.add_enum_value(FileChooserAction, FILE_CHOOSER_ACTION, OPEN_FILE);
    module.add_enum_value(FileChooserAction, FILE_CHOOSER_ACTION, OPEN_MULTIPLE_FILES);
    module.add_enum_value(FileChooserAction, FILE_CHOOSER_ACTION, SAVE);
    module.add_enum_value(FileChooserAction, FILE_CHOOSER_ACTION, SELECT_FOLDER);
    module.add_enum_value(FileChooserAction, FILE_CHOOSER_ACTION, SELECT_MULTIPLE_FOLDERS);

    module.add_type(FileFilter)
    .constructor<const std::string&>(USE_FINALIZERS)
    .add_type_method(FileFilter, get_name)
    .add_type_method(FileFilter, add_allowed_pattern, !)
    .add_type_method(FileFilter, add_allow_all_supported_image_formats, !)
    .add_type_method(FileFilter, add_allowed_suffix, !)
    .add_type_method(FileFilter, add_allowed_mime_type, !)
    ;

    auto chooser = module.add_type(FileChooser)
    .constructor<FileChooserAction, const std::string&>(USE_FINALIZERS)
    .add_type_method(FileChooser, set_accept_label, !)
    .add_type_method(FileChooser, get_accept_label)
    .add_type_method(FileChooser, present, !)
    .add_type_method(FileChooser, cancel, !)
    .add_type_method(FileChooser, set_is_modal, !)
    .add_type_method(FileChooser, get_is_modal)
    .add_type_method(FileChooser, set_title, !)
    .add_type_method(FileChooser, get_title)
    .add_type_method(FileChooser, set_file_chooser_action, !)
    .add_type_method(FileChooser, get_file_chooser_action)
    .add_type_method(FileChooser, add_filter, !)
    .add_type_method(FileChooser, clear_filters, !)
    .add_type_method(FileChooser, set_initial_filter, !)
    .add_type_method(FileChooser, set_initial_file, !)
    .add_type_method(FileChooser, set_initial_folder, !)
    .add_type_method(FileChooser, set_initial_name, !)
    ;

    chooser.method("on_accept!", [](FileChooser& self, jl_value_t* task){
        self.on_accept([](FileChooser& self, const std::vector<FileDescriptor>& files, jl_value_t* task){

            static auto* ctor = jl_eval_string("Vector{Ptr{Cvoid}}");
            static auto* resize = jl_get_function(jl_base_module, "resize!");

            auto* out = jl_calln(ctor);
            jl_calln(resize, out, jl_box_int64(files.size()));

            for (uint64_t i = 0; i < files.size(); ++i)
                jl_arrayset((jl_array_t*) out, jl_box_voidpointer(files.at(i).get_internal()), i);

            jl_safe_call("FileChooser::on_accept", task, jlcxx::box<FileChooser&>(self), out);
        }, gc_protect(self, task));
    });

    chooser.method("on_cancel!", [](FileChooser& self, jl_value_t* task){
        self.on_cancel([](FileChooser& self, jl_value_t* task) {
            jl_safe_call("FileChooser::on_cancel", task, jlcxx::box<FileChooser&>(self));
        }, gc_protect(self, task));
    });
}