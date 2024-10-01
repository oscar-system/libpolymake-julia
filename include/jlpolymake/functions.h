#ifndef JLPOLYMAKE_FUNCTIONS
#define JLPOLYMAKE_FUNCTIONS

#include "jlpolymake/jlpolymake.h"

namespace jlpolymake {

void initialize_polymake(bool interactive);

void initialize_polymake_with_dir(const std::string& userdir, bool interactive);

void initialize_polymake_with_dir_and_prefix(const std::string& userdir, const std::string& top, const std::string& arch, bool interactive);

pm::perl::BigObject to_bigobject(const pm::perl::PropertyValue&);

jl_sym_t* typeinfo_symbol_helper(const pm::perl::PropertyValue& p, bool demangle);

template <typename T> T to_SmallObject(const pm::perl::PropertyValue& pv)
{
    T obj;
    pv >> obj;
    return obj;
};

pm::Integer new_integer_from_bigint(jl_value_t*);

// We can do better templating here
template <typename T>
std::string show_small_object(const T& obj, bool print_typename = true)
{
    std::ostringstream buffer("");
    auto               wrapped_buffer = wrap(buffer);
    if (print_typename) {
        wrapped_buffer << polymake::legible_typename(typeid(obj)) << pm::endl;
    }
    wrapped_buffer << obj;
    return buffer.str();
}

struct TropicalNumberTypes
{
   typedef jlcxx::ParameterList<pm::TropicalNumber<pm::Min>, pm::TropicalNumber<pm::Max>> value_type;
};

}

#endif
