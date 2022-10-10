#include "jlpolymake/jlpolymake.h"
#include "jlpolymake/tools.h"
#include "jlpolymake/caller.h"
#include "jlpolymake/functions.h"

#include <typeinfo>
#include <cxxabi.h>

namespace jlpolymake {

void initialize_polymake(bool interactive = true)
{
    try {
        if (data.main_polymake_session == nullptr) {
            data.main_polymake_session = new polymake::Main;
            data.main_polymake_session->shell_enable();
            if (interactive){
                std::cout << data.main_polymake_session->greeting() << std::endl;
            };
        };
    }
    catch (const std::exception& e) {
        jl_error(e.what());
    }
}

pm::perl::BigObject to_bigobject(const pm::perl::PropertyValue& v)
{
    pm::perl::BigObject obj;
    v >> obj;
    return v;
}

std::string filter_spaces(const std::string str)
{
    std::stringstream res;
    for(std::string::const_iterator i = str.begin(); i != str.end(); i++) {
         if(*i != ' ') res << *i;
    }
    return res.str();
}

std::string typeinfo_string_helper(const pm::perl::PropertyValue& p, bool demangle)
{
    PropertyValueHelper ph(p);

    if (!ph.is_defined()) {
        return "undefined";
    }
    if (ph.is_boolean()) {
        return "bool";
    }
    switch (ph.classify_number()) {
        // primitives
        case PropertyValueHelper::number_is_zero:
        case PropertyValueHelper::number_is_int:
            return "Int";
        case PropertyValueHelper::number_is_float:
            return "double";

        // with typeinfo ptr (nullptr for Objects)
        case PropertyValueHelper::number_is_object:
            // some non-primitive Scalar type with typeinfo (e.g. Rational)
        case PropertyValueHelper::not_a_number:
            // a c++ type with typeinfo or a perl Object
            {
                const std::type_info* ti = ph.get_canned_typeinfo();
                if (ti == nullptr) {
                    // check some perl based types via custom perl code
                    return call_function("classify_perl_pv", p);
                }
                // demangle:
                int status = -1;
                std::unique_ptr<char, void (*)(void*)> res{
                    abi::__cxa_demangle(ti->name(), nullptr, nullptr, &status),
                    std::free};
                return (status == 0 && demangle) ? res.get() : ti->name();
            }
    }
    return "unknown";
}

jl_sym_t* typeinfo_symbol_helper(const pm::perl::PropertyValue& p, bool demangle)
{
    PropertyValueHelper ph(p);

    if (!ph.is_defined()) {
        return jl_symbol("undefined");
    }
    if (ph.is_boolean()) {
        return jl_symbol("bool");
    }
    switch (ph.classify_number()) {
        // primitives
        case PropertyValueHelper::number_is_zero:
        case PropertyValueHelper::number_is_int:
            return jl_symbol("Int");
        case PropertyValueHelper::number_is_float:
            return jl_symbol("double");

        // with typeinfo ptr (nullptr for Objects)
        case PropertyValueHelper::number_is_object:
            // some non-primitive Scalar type with typeinfo (e.g. Rational)
        case PropertyValueHelper::not_a_number:
            // a c++ type with typeinfo or a perl Object
            {
                const std::type_info* ti = ph.get_canned_typeinfo();
                if (ti == nullptr) {
                    // check some perl based types via custom perl code
                    return jl_symbol(filter_spaces(call_function("classify_perl_pv", p)).c_str());
                }
                // demangle:
                int status = -1;
                std::unique_ptr<char, void (*)(void*)> res{
                    abi::__cxa_demangle(ti->name(), nullptr, nullptr, &status),
                    std::free};
                return (status == 0 && demangle) ? jl_symbol(filter_spaces(res.get()).c_str()) : jl_symbol(filter_spaces(ti->name()).c_str());
            }
    }
    return jl_symbol("unknown");
}

}
