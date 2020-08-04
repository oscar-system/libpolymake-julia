#include "jlpolymake/jlpolymake.h"

#include "jlpolymake/tools.h"

#include "jlpolymake/functions.h"

#include "jlpolymake/type_modules.h"

namespace jlpolymake {

void add_vector(jlcxx::Module& jlpolymake)
{

    jlpolymake
        .add_type<jlcxx::Parametric<jlcxx::TypeVar<1>>>(
            "Vector", jlcxx::julia_type("AbstractVector", "Base"))
        .apply_combination<pm::Vector, VecOrMat_supported::value_type>(
            [](auto wrapped) {
                typedef typename decltype(wrapped)::type             WrappedT;
                typedef typename decltype(wrapped)::type::value_type elemType;
                wrapped.template constructor<int64_t>();
                wrapped.method("_getindex", [](WrappedT& V, int64_t n) {
                    return elemType(V[n - 1]);
                });
                wrapped.method("_setindex!",
                               [](WrappedT& V, elemType val, int64_t n) {
                                   V[n - 1] = val;
                               });
                wrapped.method("length", &WrappedT::size);
                wrapped.method("resize!",
                               [](WrappedT& V, int64_t sz) { V.resize(sz); });

                wrapped.method("take",
                               [](pm::perl::BigObject p, const std::string& s,
                                  WrappedT& V) { p.take(s) << V; });
                wrapped.method("show_small_obj", [](WrappedT& V) {
                    return show_small_object<WrappedT>(V);
                });
            });

    jlpolymake.method("to_vector_int", [](pm::perl::PropertyValue pv) {
        return to_SmallObject<pm::Vector<pm::Int>>(pv);
    });
    jlpolymake.method("to_vector_integer", [](pm::perl::PropertyValue pv) {
        return to_SmallObject<pm::Vector<pm::Integer>>(pv);
    });
    jlpolymake.method("to_vector_rational", [](pm::perl::PropertyValue pv) {
        return to_SmallObject<pm::Vector<pm::Rational>>(pv);
    });
    jlpolymake.method("to_vector_double", [](pm::perl::PropertyValue pv) {
        return to_SmallObject<pm::Vector<double>>(pv);
    });
}

}
