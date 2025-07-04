#include "jlpolymake/jlpolymake.h"

#include "jlpolymake/tools.h"

#include "jlpolymake/functions.h"

#include "jlpolymake/type_modules.h"

#include <polymake/tropical/covectors.h>

namespace jlpolymake {

void add_decoration(jlcxx::Module& jlpolymake)
{
    using DecT = polymake::graph::lattice::BasicDecoration;
    auto type = jlpolymake.add_type<DecT>("BasicDecoration")
        .constructor<const pm::Set<Int>&, Int>()
        .method("decoration_face", [](const DecT& a) { return a.face; })
        .method("decoration_rank", [](const DecT& a) { return a.rank; })

        .method("show_small_obj", [](const DecT& S) {
            return show_small_object<DecT>(S);
        });

    using CovDecT = polymake::tropical::CovectorDecoration;
    auto covtype = jlpolymake.add_type<CovDecT>("CovectorDecoration")
        .constructor<const pm::Set<Int>&, Int, const IncidenceMatrix<>&>()
        .method("decoration_face", [](const CovDecT& a) { return a.face; })
        .method("decoration_rank", [](const CovDecT& a) { return a.rank; })
        .method("decoration_covector", [](const CovDecT& a) { return a.covector; })

        .method("show_small_obj", [](const CovDecT& S) {
            return show_small_object<CovDecT>(S);
        });
}

}
