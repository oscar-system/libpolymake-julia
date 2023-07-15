#include "jlpolymake/jlpolymake.h"

#include "jlpolymake/tools.h"

#include "jlpolymake/functions.h"

#include "jlpolymake/type_modules.h"

template<> struct jlcxx::IsMirroredType<pm::NonSymmetric> : std::false_type { };
template<> struct jlcxx::IsMirroredType<pm::Symmetric> : std::false_type { };

namespace jlpolymake {

void add_incidencematrix(jlcxx::Module& jlpolymake)
{
    jlpolymake.add_type<pm::NonSymmetric>("NonSymmetric");
    jlpolymake.add_type<pm::Symmetric>("Symmetric");
    jlpolymake
    .add_type<jlcxx::Parametric<jlcxx::TypeVar<1>>, jlcxx::ParameterList<bool,int>>(
        "IncidenceMatrix", jlcxx::julia_type("AbstractSparseMatrix", "SparseArrays"))
        .apply_combination<pm::IncidenceMatrix, jlcxx::ParameterList<pm::NonSymmetric,pm::Symmetric>>(
            [](auto wrapped) {
        typedef typename decltype(wrapped)::type WrappedT;
        wrapped.template constructor<int64_t, int64_t>();
        wrapped.method("_getindex",
            [](const WrappedT& M, int64_t i, int64_t j) {
                return bool(M(i - 1, j - 1));
        });
        wrapped.method("_setindex!",
            [](WrappedT& M, bool r, int64_t i,
            int64_t j) {
                M(i - 1, j - 1) = r;
        });
        wrapped.method("nrows", [](const WrappedT& M) { return static_cast<int64_t>(M.rows()); });
        wrapped.method("_row", [](const WrappedT& M, int64_t i) { return pm::Set<pm::Int>(M.row(i - 1)); });
        wrapped.method("ncols", [](const WrappedT& M) { return static_cast<int64_t>(M.cols()); });
        wrapped.method("_col", [](const WrappedT& M, int64_t i) { return pm::Set<pm::Int>(M.col(i - 1)); });
        wrapped.method("_vcat", [](const WrappedT& M, const WrappedT& N) {
              return IncidenceMatrix<NonSymmetric>(M/N);
            });
        wrapped.method("_hcat", [](const WrappedT& M, const WrappedT& N) {
              return IncidenceMatrix<NonSymmetric>(M|N);
            });
        wrapped.method("_resize!", [](WrappedT& M, int64_t i,
                                    int64_t j) { M.resize(i, j); });
        wrapped.method("take",
                       [](pm::perl::BigObject p, const std::string& s,
                          const WrappedT& M) { p.take(s) << M; });

        wrapped.method("show_small_obj", [](const WrappedT& S) {
            return show_small_object<WrappedT>(S);
        });
    });
}

}
