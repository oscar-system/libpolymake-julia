#include "jlpolymake/jlpolymake.h"

#include "jlpolymake/tools.h"

#include "jlpolymake/functions.h"

#include "jlpolymake/type_modules.h"

#include "polymake/graph/DijkstraShortestPath.h"
#include "polymake/graph/DijkstraShortestPathWithScalarWeights.h"

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

        wrapped.template constructor<int64_t>();

        wrapped.method("nv", &WrappedT::nodes);
        wrapped.method("ne", &WrappedT::edges);
        wrapped.method("_inneighbors", [](const WrappedT& G, int64_t i) { return pm::Set<pm::Int>(G.in_adjacent_nodes(i)); });
        wrapped.method("_outneighbors", [](const WrappedT& G, int64_t i) { return pm::Set<pm::Int>(G.out_adjacent_nodes(i)); });
        wrapped.method("_has_vertex", [](const WrappedT& G, int64_t i) { return !G.invalid_node(i) && G.node_exists(i); });
        wrapped.method("_has_edge", [](const WrappedT& G, int64_t tail, int64_t head) { return G.edge_exists(tail, head); });
        wrapped.method("_add_edge", [](WrappedT& G, int64_t tail, int64_t head) { return G.add_edge(tail, head); });
        wrapped.method("_add_vertex", [](WrappedT& G) { return G.add_node(); });
        wrapped.method("_squeeze", [](WrappedT& G) { return G.squeeze(); });
        wrapped.method("_rem_vertex", [](WrappedT& G, int64_t i) { return G.delete_node(i); });
        wrapped.method("_rem_edge", [](WrappedT& G, int64_t tail, int64_t head) { return G.delete_edge(tail, head); });

        wrapped.method("show_small_obj", [](const WrappedT& S) {
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


void add_edgemap(jlcxx::Module& jlpolymake)
{
    auto type = jlpolymake
        .add_type<jlcxx::Parametric<jlcxx::TypeVar<1>, jlcxx::TypeVar<2>>>(
                "EdgeMap");

    type.apply<pm::graph::EdgeMap<pm::graph::Undirected, pm::Int>,
        pm::graph::EdgeMap<pm::graph::Directed, pm::Int>>
            ([](auto wrapped){
                typedef typename decltype(wrapped)::type WrappedT;
                typedef typename decltype(wrapped)::type::graph_type GType;
                typedef typename decltype(wrapped)::type::graph_type::dir TDir;
                typedef typename decltype(wrapped)::type::value_type E;
                wrapped.template constructor<GType>();


                wrapped.method("_set_entry", [](WrappedT& EM, int64_t tail, int64_t head, const E& val) { EM(tail, head) = val; });
                wrapped.method("_get_entry", [](const WrappedT& EM, int64_t tail, int64_t head) { return EM(tail, head); });
                wrapped.method("show_small_obj", [](const WrappedT& S) {
                    return show_small_object<WrappedT>(S);
                });

                wrapped.method("_shortest_path_dijkstra", [](const GType& G, const WrappedT& S, int64_t start, int64_t end, bool backward){
                    polymake::graph::DijkstraShortestPath<polymake::graph::DijkstraShortestPathWithScalarWeights<TDir, E>> DSP(G, S);
                    auto path_it = DSP.solve(static_cast<pm::Int>(start), static_cast<pm::Int>(end), !backward);
                    std::vector<pm::Int> rev_path;
                    if (!path_it.at_end()) {
                        do
                            rev_path.push_back(path_it.cur_node());
                        while (!(++path_it).at_end());
                    }
                    return pm::Array<pm::Int>(rev_path.rbegin(), rev_path.rend());
                });
                
            });
    jlpolymake.method("to_edgemap_undirected_int", [](pm::perl::PropertyValue pv) {
        return to_SmallObject<pm::graph::EdgeMap<pm::graph::Undirected, pm::Int>>(pv);
    });
    jlpolymake.method("to_edgemap_directed_int", [](pm::perl::PropertyValue pv) {
        return to_SmallObject<pm::graph::EdgeMap<pm::graph::Directed, pm::Int>>(pv);
    });
}



void add_nodemap(jlcxx::Module& jlpolymake)
{
    auto type = jlpolymake
        .add_type<jlcxx::Parametric<jlcxx::TypeVar<1>, jlcxx::TypeVar<2>>>(
                "NodeMap");

    type.apply<pm::graph::NodeMap<pm::graph::Undirected, pm::Int>,
        pm::graph::NodeMap<pm::graph::Directed, pm::Int>,
        pm::graph::NodeMap<pm::graph::Undirected, pm::Set<pm::Int>>,
        pm::graph::NodeMap<pm::graph::Directed, pm::Set<pm::Int>>>
            ([](auto wrapped){
                typedef typename decltype(wrapped)::type WrappedT;
                typedef typename decltype(wrapped)::type::graph_type GType;
                typedef typename decltype(wrapped)::type::graph_type::dir TDir;
                typedef typename decltype(wrapped)::type::value_type E;
                wrapped.template constructor<GType>();


                wrapped.method("_set_entry", [](WrappedT& EM, int64_t node, const E& val) { EM[node] = val; });
                wrapped.method("_get_entry", [](const WrappedT& EM, int64_t node) { return EM[node]; });
                wrapped.method("show_small_obj", [](const WrappedT& S) {
                    return show_small_object<WrappedT>(S);
                });
                
            });
    jlpolymake.method("to_nodemap_undirected_int", [](pm::perl::PropertyValue pv) {
        return to_SmallObject<pm::graph::NodeMap<pm::graph::Undirected, pm::Int>>(pv);
    });
    jlpolymake.method("to_nodemap_directed_int", [](pm::perl::PropertyValue pv) {
        return to_SmallObject<pm::graph::NodeMap<pm::graph::Directed, pm::Int>>(pv);
    });
    jlpolymake.method("to_nodemap_undirected_set_int", [](pm::perl::PropertyValue pv) {
        return to_SmallObject<pm::graph::NodeMap<pm::graph::Undirected, pm::Set<pm::Int>>>(pv);
    });
    jlpolymake.method("to_nodemap_directed_set_int", [](pm::perl::PropertyValue pv) {
        return to_SmallObject<pm::graph::NodeMap<pm::graph::Directed, pm::Set<pm::Int>>>(pv);
    });
}

}
