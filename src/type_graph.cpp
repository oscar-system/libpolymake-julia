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
        
        wrapped.method("nv", &WrappedT::nodes);
        wrapped.method("ne", &WrappedT::edges);
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

    jlpolymake.add_type<jlcxx::Parametric<jlcxx::TypeVar<1>>>("GraphEdgeIterator")
      .apply<WrappedGraphEdgeIterator<pm::graph::Directed>>(
         [](auto wrapped) {
            typedef typename decltype(wrapped)::type WrappedGraphEdgeIter;
            typedef typename decltype(wrapped)::type::dir TDir;
            wrapped.method("edgeiterator", [](pm::graph::Graph<TDir>& G) {
               auto result = WrappedGraphEdgeIterator<TDir>{G};
               return result;
            });
            wrapped.method("increment", [](WrappedGraphEdgeIter& state) {
               ++state.iterator;
            });
            wrapped.method("get_element", [](WrappedGraphEdgeIter& state) {
               return std::pair<Int, Int>(state.iterator.from_node(), state.iterator.to_node());
            });
            wrapped.method("isdone", [](WrappedGraphEdgeIter& state) {
               return state.iterator.at_end();
            });
         }
      );

    jlpolymake.add_type<jlcxx::Parametric<jlcxx::TypeVar<1>>>("GraphNodeIterator")
      .apply<WrappedGraphNodeIterator<pm::graph::Directed>>(
         [](auto wrapped) {
            typedef typename decltype(wrapped)::type WrappedGraphNodeIter;
            typedef typename decltype(wrapped)::type::dir TDir;
            wrapped.method("nodeiterator", [](pm::graph::Graph<TDir>& G) {
               auto result = WrappedGraphNodeIterator<TDir>{G};
               return result;
            });
            wrapped.method("increment", [](WrappedGraphNodeIter& state) {
               ++state.iterator;
            });
            wrapped.method("get_element", [](WrappedGraphNodeIter& state) {
               auto elt = *(state.iterator);
               return elt;
            });
            wrapped.method("isdone", [](WrappedGraphNodeIter& state) {
               return state.iterator.at_end();
            });
         }
      );
}

}
