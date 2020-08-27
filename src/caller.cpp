#define INCLUDED_FROM_CALLER

#include "jlpolymake/caller.h"

#include "jlpolymake/tools.h"

#include <vector>

namespace jlpolymake {

static auto type_map_translator = new std::map<std::string, jl_value_t**>();

void insert_type_in_map(std::string&& ptr_name, jl_value_t** var_space)
{
    type_map_translator->emplace(std::make_pair(ptr_name, var_space));
}

void set_julia_type(std::string name, void* type_address)
{
    jl_value_t** address;
    try {
        address = (*type_map_translator)[name];
    }
    catch (std::exception& e) {
        std::cerr << "In 'set_julia_type': type translation failed for " << name << std::endl;
        return;
    }
    memcpy(address, &type_address, sizeof(jl_value_t*));
}

#include "jlpolymake/generated/call_function_feed_argument.h"

struct context {
   using list = std::optional<pm::perl::ListResult>;
   using scalar = pm::perl::PropertyValue;
};

// Visualization in polymake only works if the function is called and
// then immediately released,i.e. not converted to a property value
template<typename return_type=typename context::scalar>
auto polymake_call_function(
    const std::string&                     function_name,
    const std::vector<std::string>&        template_vector,
    const jlcxx::ArrayRef<jl_value_t*, 1> arguments)
    -> return_type
{
    auto   function = polymake::prepare_call_function(function_name, template_vector);
    for (auto arg : arguments)
        call_function_feed_argument(function, arg);
    return static_cast<return_type>(function());
}

// Visualization in polymake only works if the method is called and
// then immediately released,i.e. not converted to a property value
template<typename return_type=typename context::scalar>
auto polymake_call_method(
    const std::string&                     function_name,
    pm::perl::BigObject             object,
    const jlcxx::ArrayRef<jl_value_t*, 1> arguments)
    -> return_type
{
    auto   function = object.prepare_call_method(function_name);
    for (auto arg : arguments)
        call_function_feed_argument(function, arg);
    return static_cast<return_type>(function());
}

void add_caller(jlcxx::Module& jlpolymake)
{
    jlpolymake.method("internal_call_function",
                      &polymake_call_function<>);
    jlpolymake.method("internal_call_function_list",
                      &polymake_call_function<typename context::list>);
    jlpolymake.method("internal_call_function_void",
                      &polymake_call_function<void>);
    jlpolymake.method("internal_call_method",
                      &polymake_call_method<>);
    jlpolymake.method("internal_call_method_list",
                      &polymake_call_method<typename context::list>);
    jlpolymake.method("internal_call_method_void",
                      &polymake_call_method<void>);
    jlpolymake.method("set_julia_type", &set_julia_type);
}

}
