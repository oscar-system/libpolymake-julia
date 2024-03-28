#ifndef POLYMAKE_TOOLS
#define POLYMAKE_TOOLS

#include "jlpolymake/jlpolymake.h"

template<> struct jlcxx::IsMirroredType<pm::operations::cmp> : std::false_type { };
template<> struct jlcxx::IsMirroredType<pm::NonSymmetric> : std::false_type { };
template<> struct jlcxx::IsMirroredType<pm::Symmetric> : std::false_type { };
template<> struct jlcxx::IsMirroredType<pm::Max> : std::false_type { };
template<> struct jlcxx::IsMirroredType<pm::Min> : std::false_type { };
template<> struct jlcxx::IsMirroredType<pm::graph::Directed> : std::false_type { };
template<> struct jlcxx::IsMirroredType<pm::graph::Undirected> : std::false_type { };

namespace pm {
template <typename PointedT, typename CppT>
struct iterator_cross_const_helper<jlcxx::array_iterator_base<PointedT, CppT>,
                                   true> {
    typedef jlcxx::array_iterator_base<std::remove_const_t<PointedT>,
                                       std::remove_const_t<CppT>>
        iterator;
    typedef jlcxx::array_iterator_base<std::add_const_t<PointedT>,
                                       std::add_const_t<CppT>>
        const_iterator;
};
}    // namespace pm

namespace jlpolymake {

using namespace polymake;

class PropertyValueHelper : public pm::perl::PropertyValue {
  public:
    PropertyValueHelper(const pm::perl::PropertyValue& pv)
        : pm::perl::PropertyValue(pv){};

    // in some form these will be moved to the polymake code
    bool is_boolean() const
    {
        return call_function("is_boolean_wrapper", *this);
    };

    using Value::classify_number;
    using Value::get_canned_typeinfo;
    using Value::is_defined;
    using Value::not_a_number;
    using Value::number_is_float;
    using Value::number_is_int;
    using Value::number_is_object;
    using Value::number_is_zero;
};

struct Polymake_Data {
    polymake::Main*        main_polymake_session;
    polymake::Scope* main_polymake_scope;
};

extern Polymake_Data data;

template <typename T> struct WrappedSetIterator {
    const pm::Set<T>& set;
    typename pm::Set<T>::const_iterator iterator;
    using value_type = T;
    WrappedSetIterator<T>(const pm::Set<T>& S) : set(S), iterator(pm::entire(set)) { }
};

template<typename TDir>
struct WrappedGraphNodeIterator {
   const pm::graph::Graph<TDir>& graph;
   typename pm::Nodes<pm::graph::Graph<TDir>>::const_iterator iterator;
   using dir = TDir;
   WrappedGraphNodeIterator<TDir>(const pm::graph::Graph<TDir>& G) : graph(G), iterator(pm::entire(pm::nodes(graph))) { }
};

template<typename TDir>
struct WrappedGraphEdgeIterator {
   const pm::graph::Graph<TDir>& graph;
   typename pm::Edges<pm::graph::Graph<TDir>>::const_iterator iterator;
   using dir = TDir;
   WrappedGraphEdgeIterator<TDir>(const pm::graph::Graph<TDir>& G) : graph(G), iterator(pm::entire(pm::edges(graph))) { }
};

template <typename T> struct WrappedStdListIterator {
    const std::list<T>& list;
    typename std::list<T>::const_iterator iterator;
    using value_type = T;
    WrappedStdListIterator<T>(const std::list<T>& L) : list(L), iterator(list.begin()) { }
};

template <typename S, typename T> struct WrappedMapIterator {
    const pm::Map<S,T>& map;
    typename pm::Map<S,T>::const_iterator iterator;
    using key_type = S;
    using mapped_type = T;
    WrappedMapIterator<S,T>(const pm::Map<S,T>& M) : map(M), iterator(map.begin()) { }
};

}

#endif
