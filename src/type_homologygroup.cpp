#include "jlpolymake/jlpolymake.h"

#include "jlpolymake/tools.h"

#include "jlpolymake/functions.h"

#include "jlpolymake/type_modules.h"

namespace jlpolymake {

void add_homologygroup(jlcxx::Module& jlpolymake)
{

    auto type = jlpolymake
        .add_type<jlcxx::Parametric<jlcxx::TypeVar<1>>>(
            "HomologyGroup")

        .apply<polymake::topaz::HomologyGroup<pm::Integer>>([&jlpolymake](auto wrapped) {

            typedef typename decltype(wrapped)::type WrappedT;

            wrapped.method("betti_number", [](const WrappedT& a) { return a.betti_number; });
            wrapped.method("torsion", [](const WrappedT& a) { return a.torsion; });
            
            wrapped.method("show_small_obj", [](const WrappedT& S) {
                return show_small_object<WrappedT>(S);
            });
        });

    auto cgtype = jlpolymake
        .add_type<jlcxx::Parametric<jlcxx::TypeVar<1>>>(
            "CycleGroup")

        .apply<polymake::topaz::CycleGroup<pm::Integer>>([&jlpolymake](auto wrapped) {

            typedef typename decltype(wrapped)::type WrappedT;

            wrapped.method("coeffs", [](const WrappedT& a) { return a.coeffs; });
            wrapped.method("faces", [](const WrappedT& a) { return a.faces; });

            wrapped.method("show_small_obj", [](const WrappedT& S) {
                return show_small_object<WrappedT>(S);
            });
        });

}

}
