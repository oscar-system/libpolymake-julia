#include "jlpolymake/jlpolymake.h"

#include "jlpolymake/tools.h"

#include "jlpolymake/functions.h"

#include "jlpolymake/type_modules.h"

template<> struct jlcxx::IsMirroredType<pm::graph::Directed> : std::false_type { };
template<> struct jlcxx::IsMirroredType<pm::graph::Undirected> : std::false_type { };

namespace jlpolymake {

void add_graph(jlcxx::Module& jlpolymake)
{
    jlpolymake.add_type<pm::graph::Undirected>("Undirected");
    jlpolymake.add_type<pm::graph::Directed>("Directed");
    jlpolymake
    .add_type<jlcxx::Parametric<jlcxx::TypeVar<1>>, jlcxx::ParameterList<bool,int>>(
        "Graph", jlcxx::julia_type("AbstractSparseMatrix", "SparseArrays"))
        .apply_combination<pm::graph::Graph, jlcxx::ParameterList<pm::graph::Undirected,pm::graph::Directed>>(
            [](auto wrapped) {
        typedef typename decltype(wrapped)::type WrappedT;
        // wrapped.template constructor<int64_t, int64_t>();
        // wrapped.method("_getindex",
        //     [](WrappedT& M, int64_t i, int64_t j) {
        //         return bool(M(i - 1, j - 1));
        // });
        // wrapped.method("_setindex!",
        //     [](WrappedT& M, bool r, int64_t i,
        //     int64_t j) {
        //         M(i - 1, j - 1) = r;
        // });
        wrapped.method("nnodes", &WrappedT::nodes);
        wrapped.method("nedges", &WrappedT::edges);
        // wrapped.method("nrows", &WrappedT::rows);
        // wrapped.method("_row", [](WrappedT& M, int64_t i) { return pm::Set<pm::Int>(M.row(i - 1)); });
        // wrapped.method("ncols", &WrappedT::cols);
        // wrapped.method("_col", [](WrappedT& M, int64_t i) { return pm::Set<pm::Int>(M.col(i - 1)); });
        // wrapped.method("_resize!", [](WrappedT& M, int64_t i,
        //                             int64_t j) { M.resize(i, j); });
        // wrapped.method("take",
        //                [](pm::perl::BigObject p, const std::string& s,
        //                   WrappedT& M) { p.take(s) << M; });

        wrapped.method("show_small_obj", [](WrappedT& S) {
            return show_small_object<WrappedT>(S);
        });
    });
    jlpolymake.method("to_graph_undirected", [](pm::perl::PropertyValue pv) {
        return to_SmallObject<pm::graph::Graph<pm::graph::Undirected>>(pv);
    });
    jlpolymake.method("to_graph_directed", [](pm::perl::PropertyValue pv) {
        return to_SmallObject<pm::graph::Graph<pm::graph::Directed>>(pv);
    });
}

}
