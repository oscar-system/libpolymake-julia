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
    .add_type<jlcxx::Parametric<jlcxx::TypeVar<1>>, jlcxx::ParameterList<bool>>(
        "Graph")
        .apply_combination<pm::graph::Graph, jlcxx::ParameterList<pm::graph::Undirected,pm::graph::Directed>>(
            [](auto wrapped) {
        typedef typename decltype(wrapped)::type WrappedT;
        wrapped.method("_nv", &WrappedT::nodes);
        wrapped.method("_ne", &WrappedT::edges);
        wrapped.method("_inneighbors", [](WrappedT& M, int64_t i) { return pm::Set<pm::Int>(M.in_adjacent_nodes(i)); });
        wrapped.method("_outneighbors", [](WrappedT& M, int64_t i) { return pm::Set<pm::Int>(M.out_adjacent_nodes(i)); });
        wrapped.method("_has_vertex", [](WrappedT& M, int64_t i) { return M.node_exists(i); });
        wrapped.method("_has_edge", [](WrappedT& M, int64_t tail, int64_t head) { return M.edge_exists(tail, head); });
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
