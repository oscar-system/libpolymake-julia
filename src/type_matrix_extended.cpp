#include "jlpolymake/jlpolymake.h"

#include "jlpolymake/tools.h"

#include "jlpolymake/functions.h"

#include "jlpolymake/type_modules.h"

namespace jlpolymake {

void add_matrix_extended(jlcxx::Module& jlpolymake, tparametric1 matrix_type)
{
    matrix_type
        .apply_combination<pm::Matrix, VecOrMat_supported_limited::value_type>(
            [](auto wrapped) {
                typedef typename decltype(wrapped)::type             WrappedT;
                typedef typename decltype(wrapped)::type::value_type elemType;

                wrapped.method("_same_element_matrix_polynomial", [](const elemType& e, int64_t i, int64_t j) {
                                    return WrappedT(same_element_matrix(e, i, j));
                                });
                wrapped.method("_getindex",
                               [](const WrappedT& f, int64_t i, int64_t j) {
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
                                  const WrappedT& M) { p.take(s) << M; });
                wrapped.method("show_small_obj", [](const WrappedT& M) {
                    return show_small_object<WrappedT>(M);
                });
            });
}

}
