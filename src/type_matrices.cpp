#include "jlpolymake/jlpolymake.h"

#include "jlpolymake/tools.h"

#include "jlpolymake/functions.h"

#include "jlpolymake/type_modules.h"

namespace jlpolymake {

void add_matrix(jlcxx::Module& jlpolymake)
{

    jlpolymake
        .add_type<jlcxx::Parametric<jlcxx::TypeVar<1>>>(
            "Matrix", jlcxx::julia_type("AbstractMatrix", "Base"))
        .apply_combination<pm::Matrix, VecOrMat_supported::value_type>(
            [](auto wrapped) {
                typedef typename decltype(wrapped)::type             WrappedT;
                typedef typename decltype(wrapped)::type::value_type elemType;
                wrapped.template constructor<int64_t, int64_t>();

                wrapped.method("_getindex",
                               [](WrappedT& f, int64_t i, int64_t j) {
                                   return elemType(f(i - 1, j - 1));
                               });
                wrapped.method("_setindex!",
                               [](WrappedT& M, elemType r, int64_t i,
                                  int64_t j) { M(i - 1, j - 1) = r; });
                wrapped.method("nrows", &WrappedT::rows);
                wrapped.method("ncols", &WrappedT::cols);
                wrapped.method("resize!", [](WrappedT& M, int64_t i,
                                            int64_t j) { M.resize(i, j); });

                wrapped.method("take",
                               [](pm::perl::BigObject p, const std::string& s,
                                  WrappedT& M) { p.take(s) << M; });
                wrapped.method("show_small_obj", [](WrappedT& M) {
                    return show_small_object<WrappedT>(M);
                });
            });
    jlpolymake.method("to_matrix_int", [](pm::perl::PropertyValue pv) {
        return to_SmallObject<pm::Matrix<pm::Int>>(pv);
    });
    jlpolymake.method("to_matrix_integer", [](pm::perl::PropertyValue pv) {
        return to_SmallObject<pm::Matrix<pm::Integer>>(pv);
    });
    jlpolymake.method("to_matrix_rational", [](pm::perl::PropertyValue pv) {
        return to_SmallObject<pm::Matrix<pm::Rational>>(pv);
    });
    jlpolymake.method("to_matrix_double", [](pm::perl::PropertyValue pv) {
        return to_SmallObject<pm::Matrix<double>>(pv);
    });
    jlpolymake.method("to_matrix_quadraticextension_rational", [](pm::perl::PropertyValue pv) {
        return to_SmallObject<pm::Matrix<pm::QuadraticExtension<pm::Rational>>>(pv);
    });
}

}
