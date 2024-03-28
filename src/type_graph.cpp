#include "jlpolymake/jlpolymake.h"

#include "jlpolymake/tools.h"

#include "jlpolymake/functions.h"

#include "jlpolymake/type_modules.h"

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

        wrapped.template constructor<int64_t>();
        wrapped.template constructor<const WrappedT&>();
        wrapped.template constructor<const IncidenceMatrix<NonSymmetric>&>();
        wrapped.template constructor<const IncidenceMatrix<Symmetric>&>();


        wrapped.method("nv", &WrappedT::nodes);
        wrapped.method("ne", &WrappedT::edges);
        wrapped.method("_inneighbors", [](const WrappedT& G, int64_t i) { return pm::Set<pm::Int>(wary(G).in_adjacent_nodes(i)); });
        wrapped.method("_outneighbors", [](const WrappedT& G, int64_t i) { return pm::Set<pm::Int>(wary(G).out_adjacent_nodes(i)); });
        wrapped.method("_has_vertex", [](const WrappedT& G, int64_t i) { return !G.invalid_node(i) && G.node_exists(i); });
        wrapped.method("_has_edge", [](const WrappedT& G, int64_t tail, int64_t head) { return wary(G).edge_exists(tail, head); });
        wrapped.method("_add_edge", [](WrappedT& G, int64_t tail, int64_t head) { return wary(G).add_edge(tail, head); });
        wrapped.method("_add_vertex", [](WrappedT& G) { return G.add_node(); });
        wrapped.method("_squeeze", [](WrappedT& G) { return G.squeeze(); });
        wrapped.method("_rem_vertex", [](WrappedT& G, int64_t i) { return wary(G).delete_node(i); });
        wrapped.method("_rem_edge", [](WrappedT& G, int64_t tail, int64_t head) { return wary(G).delete_edge(tail, head); });
        wrapped.method("_degree", [](const WrappedT& G, int64_t node) { return G.degree(static_cast<Int>(node)); });
        wrapped.method("_indegree", [](const WrappedT& G, int64_t node) { return G.in_degree(static_cast<Int>(node)); });
        wrapped.method("_outdegree", [](const WrappedT& G, int64_t node) { return G.out_degree(static_cast<Int>(node)); });
        wrapped.method("_contract_edge", [](WrappedT& G, int64_t tail, int64_t head) { G.contract_edge(static_cast<Int>(tail), static_cast<Int>(head)); });

        wrapped.method("show_small_obj", [](const WrappedT& S) {
            return show_small_object<WrappedT>(S);
        });
    });

    jlpolymake.add_type<jlcxx::Parametric<jlcxx::TypeVar<1>>>("GraphEdgeIterator")
      .apply<WrappedGraphEdgeIterator<pm::graph::Directed>, WrappedGraphEdgeIterator<pm::graph::Undirected>>(
         [](auto wrapped) {
            typedef typename decltype(wrapped)::type WrappedGraphEdgeIter;
            typedef typename decltype(wrapped)::type::dir TDir;
            wrapped.method("edgeiterator", [](const pm::graph::Graph<TDir>& G) {
               auto result = WrappedGraphEdgeIterator<TDir>{G};
               return result;
            });
            wrapped.method("increment", [](WrappedGraphEdgeIter& state) {
               ++state.iterator;
            });
            wrapped.method("get_element", [](const WrappedGraphEdgeIter& state) {
               return std::pair<Int, Int>(state.iterator.from_node(), state.iterator.to_node());
            });
            wrapped.method("isdone", [](const WrappedGraphEdgeIter& state) {
               return state.iterator.at_end();
            });
         }
      );

    jlpolymake.add_type<jlcxx::Parametric<jlcxx::TypeVar<1>>>("GraphNodeIterator")
      .apply<WrappedGraphNodeIterator<pm::graph::Directed>, WrappedGraphNodeIterator<pm::graph::Undirected>>(
         [](auto wrapped) {
            typedef typename decltype(wrapped)::type WrappedGraphNodeIter;
            typedef typename decltype(wrapped)::type::dir TDir;
            wrapped.method("nodeiterator", [](const pm::graph::Graph<TDir>& G) {
               auto result = WrappedGraphNodeIterator<TDir>{G};
               return result;
            });
            wrapped.method("increment", [](WrappedGraphNodeIter& state) {
               ++state.iterator;
            });
            wrapped.method("get_element", [](const WrappedGraphNodeIter& state) {
               auto elt = *(state.iterator);
               return elt;
            });
            wrapped.method("isdone", [](const WrappedGraphNodeIter& state) {
               return state.iterator.at_end();
            });
         }
      );
    jlpolymake.method("take",
                    [](pm::perl::BigObject& p, const std::string& s,
                       const pm::graph::Graph<pm::graph::Undirected>& G) { p.take(s) << G; });
    jlpolymake.method("take",
                    [](pm::perl::BigObject& p, const std::string& s,
                       const pm::graph::Graph<pm::graph::Directed>& G) { p.take(s) << G; });
}

}
