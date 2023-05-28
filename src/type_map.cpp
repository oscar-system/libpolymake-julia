#include "jlpolymake/jlpolymake.h"

#include "jlpolymake/tools.h"

#include "jlpolymake/functions.h"

#include "jlpolymake/type_modules.h"

namespace jlpolymake {

void add_map(jlcxx::Module& jlpolymake)
{

    auto type = jlpolymake
        .add_type<jlcxx::Parametric<jlcxx::TypeVar<1>, jlcxx::TypeVar<2>>>(
            "Map", jlcxx::julia_type("AbstractDict", "Base" ));

        type.apply<pm::Map<std::string, std::string>>([&jlpolymake](auto wrapped) {

            typedef typename decltype(wrapped)::type WrappedT;
            typedef typename decltype(wrapped)::type::key_type keyT;
            typedef typename decltype(wrapped)::type::mapped_type valT;

            wrapped.method("_getindex", [](const WrappedT& M, keyT& key) {
                return M[key];
            });

            wrapped.method("_setindex!", [](WrappedT& M, valT& val, keyT& key) {
                M[key] = val;
            });

            wrapped.method("isempty", &WrappedT::empty);
            wrapped.method("length", &WrappedT::size);

            wrapped.method("_isequal", [](const WrappedT& S, const WrappedT& T) { return S == T; });

            wrapped.method("show_small_obj", [](const WrappedT& S) {
                return show_small_object<WrappedT>(S);
            });
        });

    jlpolymake.add_type<jlcxx::Parametric<jlcxx::TypeVar<1>>>("MapIterator")
        .apply<WrappedMapIterator<std::string, std::string>>(
            [](auto wrapped) {
                typedef typename decltype(wrapped)::type WrappedMapIter;
                typedef typename decltype(wrapped)::type::key_type keyT;
                typedef typename decltype(wrapped)::type::mapped_type valT;
                wrapped.method("beginiterator", [](const pm::Map<keyT, valT>& M) {
                    auto result = WrappedMapIterator<keyT, valT>{M};
                    return result;
                });

                wrapped.method("increment", [](WrappedMapIter& state) {
                    state.iterator++;
                });
                wrapped.method("get_element", [](const WrappedMapIter& state) {
                    auto elt = *(state.iterator);
                    return std::tuple<keyT, valT>(elt.first, elt.second);
                });
                wrapped.method("isdone", [](const pm::Map<keyT, valT>& M,
                                            WrappedMapIter&    state) {
                    return M.end() == state.iterator;
                });
            });

}

}
