#include "jlpolymake/jlpolymake.h"

#include "jlpolymake/tools.h"

#include "jlpolymake/functions.h"

#include "jlpolymake/type_modules.h"

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

}

}
