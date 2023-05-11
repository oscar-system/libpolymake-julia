#include "jlpolymake/jlpolymake.h"

#include "jlpolymake/tools.h"

#include "jlpolymake/functions.h"

#include "jlpolymake/type_modules.h"

namespace jlpolymake {

void add_lists(jlcxx::Module& jlpolymake)
{
    auto type = jlpolymake
        .add_type<jlcxx::Parametric<jlcxx::TypeVar<1>>>(
            "StdList");

        type.apply<std::list<std::pair<pm::Int,pm::Int>>, std::list<std::list<std::pair<pm::Int,pm::Int>>>, std::list<std::pair<pm::Integer, pm::Int>>>([&jlpolymake](auto wrapped) {
            typedef typename decltype(wrapped)::type WrappedT;
            typedef typename decltype(wrapped)::type::value_type elemType;

            wrapped.template constructor();
            wrapped.template constructor<WrappedT>();

            //Pattern to overwrite function in Base
            jlpolymake.set_override_module(jl_base_module);

            wrapped.method("isempty", &WrappedT::empty);

            wrapped.method("empty!", [](WrappedT& L) {
                L.clear();
                return L;
            });

            wrapped.method("push!", [](WrappedT& L, elemType i) {
                L.push_back(i);
                return L;
            });

            wrapped.method("pushfirst!", [](WrappedT& L, elemType i) {
                L.push_front(i);
                return L;
            });

            wrapped.method("length", &WrappedT::size);

            jlpolymake.unset_override_module();

            wrapped.method("show_small_obj", [](const WrappedT& S) {
                return show_small_object<WrappedT>(S);
            });
        });


    jlpolymake.add_type<jlcxx::Parametric<jlcxx::TypeVar<1>>>("ListIterator")
            .apply<WrappedStdListIterator<std::pair<pm::Int, pm::Int>>, WrappedStdListIterator<std::list<std::pair<pm::Int, pm::Int>>>, WrappedStdListIterator<std::pair<pm::Integer, pm::Int>>>(
            [](auto wrapped) {
                typedef typename decltype(wrapped)::type WrappedT;
                typedef typename decltype(wrapped)::type::value_type elemType;

                wrapped.method("beginiterator", [](const std::list<elemType>& L) {
                    return WrappedT(L);
                });

                wrapped.method("increment", [](WrappedT& state) {
                    state.iterator++;
                });
                wrapped.method("get_element", [](WrappedT& state) {
                    auto elt = *(state.iterator);
                    return elt;
                });
                wrapped.method("isdone", [](const std::list<elemType>& L,
                                            WrappedT&    state) {
                    return L.end() == state.iterator;
                });
            });

}

}
