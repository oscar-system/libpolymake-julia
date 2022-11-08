#include "jlpolymake/jlpolymake.h"

#include "jlpolymake/tools.h"

#include "jlpolymake/functions.h"

#include "jlpolymake/type_modules.h"

namespace jlpolymake {

void add_pair_extended(jlcxx::Module& jlpolymake, tparametric2 pair_type)
{
    pair_type
        .apply<
                std::pair<pm::Int, std::list<std::list<std::pair<pm::Int, pm::Int>>>>,
                std::pair<pm::Array<pm::Int>, pm::Array<pm::Int>>
            >([&jlpolymake](auto wrapped) {
            typedef typename decltype(wrapped)::type WrappedT;
            typedef typename decltype(wrapped)::type::first_type firstT;
            typedef typename decltype(wrapped)::type::second_type secondT;

            wrapped.template constructor();
            wrapped.template constructor<firstT, secondT>();

            //Pattern to overwrite function in Base
            jlpolymake.set_override_module(jl_base_module);

            wrapped.method("first", [](const WrappedT& P) {
                return P.first;
            });

            wrapped.method("last", [](const WrappedT& P) {
                return P.second;
            });

            jlpolymake.unset_override_module();

            wrapped.method("show_small_obj", [](const WrappedT& S) {
                return show_small_object<WrappedT>(S);
            });
        });
    
    jlpolymake.method("to_pair_int_list_list_pair_int_int", [](const pm::perl::PropertyValue& pv) {
        return to_SmallObject<std::pair<pm::Int, std::list<std::list<std::pair<pm::Int, pm::Int>>>>>(pv);
    });
    jlpolymake.method("to_pair_array_int_array_int", [](const pm::perl::PropertyValue& pv) {
        return to_SmallObject<std::pair<pm::Array<pm::Int>, pm::Array<pm::Int>>>(pv);
    });

}

}
