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

std::string typeinfo_helper(const pm::perl::PropertyValue& p, bool demangle)
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

}
