#include "jlpolymake/jlpolymake.h"

#include "jlpolymake/tools.h"

#include "jlpolymake/functions.h"

#include "jlpolymake/type_modules.h"

namespace jlpolymake {

void add_vector_extended(jlcxx::Module& jlpolymake, tparametric1 vector_type)
{
    vector_type
        .apply_combination<pm::Vector, VecOrMat_supported_limited::value_type>(
            [](auto wrapped) {
                typedef typename decltype(wrapped)::type             WrappedT;
                typedef typename decltype(wrapped)::type::value_type elemType;
                wrapped.template constructor<int64_t, elemType>();
                wrapped.method("_getindex", [](const WrappedT& V, int64_t n) {
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
                                  const WrappedT& V) { p.take(s) << V; });
                wrapped.method("show_small_obj", [](const WrappedT& V) {
                    return show_small_object<WrappedT>(V);
                });
            });
    
    jlpolymake.method(
        "to_vector_polynomial_rational_int", [](const pm::perl::PropertyValue& pv) {
            return to_SmallObject<pm::Vector<pm::Polynomial<pm::Rational,pm::Int>>>(pv);
        });
}

}
