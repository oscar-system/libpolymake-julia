#include "jlpolymake/jlpolymake.h"
#include "jlpolymake/tools.h"
#include "jlpolymake/caller.h"
#include "jlpolymake/functions.h"

#include <typeinfo>
#include <cxxabi.h>
#include <unordered_map>

namespace jlpolymake {

void initialize_polymake_with_dir_and_prefix(const std::string& userdir, const std::string& top, const std::string& arch, bool interactive = true)
{
    try {
        if (data.main_polymake_session == nullptr) {
            data.main_polymake_session = new polymake::Main(userdir, top, arch);
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

void initialize_polymake_with_dir(const std::string& userdir, bool interactive = true) {
   initialize_polymake_with_dir_and_prefix(userdir, "", "", interactive);
}

void initialize_polymake(bool interactive = true)
{
   initialize_polymake_with_dir("user", interactive);
}

pm::perl::BigObject to_bigobject(const pm::perl::PropertyValue& v)
{
    pm::perl::BigObject obj;
    v >> obj;
    return v;
}

std::string filter_spaces(const std::string& str)
{
    std::stringstream res;
    for(std::string::const_iterator i = str.begin(); i != str.end(); i++) {
         if(*i != ' ') res << *i;
    }
    return res.str();
}

// Demangling and interning cost about 1 µs per call, so cache the symbol per
// type. type_info objects live as long as their library, which polymake never
// unloads, and julia symbols are never freed. Not locked: calls into the perl
// interpreter are not thread-safe anyway.
jl_sym_t* canned_typeinfo_symbol(const std::type_info* ti, bool demangle)
{
    static std::unordered_map<const std::type_info*, jl_sym_t*> cache[2];
    auto& symbols = cache[demangle];

    auto it = symbols.find(ti);
    if (it != symbols.end()) {
        return it->second;
    }

    int status = -1;
    std::unique_ptr<char, void (*)(void*)> res{
        abi::__cxa_demangle(ti->name(), nullptr, nullptr, &status),
        std::free};
    jl_sym_t* sym = (status == 0 && demangle) ? jl_symbol(filter_spaces(res.get()).c_str()) : jl_symbol(filter_spaces(ti->name()).c_str());
    symbols.emplace(ti, sym);
    return sym;
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
                return canned_typeinfo_symbol(ti, demangle);
            }
    }
    return jl_symbol("unknown");
}

}
