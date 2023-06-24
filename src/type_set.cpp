#include "jlpolymake/jlpolymake.h"

#include "jlpolymake/tools.h"

#include "jlpolymake/functions.h"

#include "jlpolymake/type_modules.h"

#include "jlpolymake/containers.h"

template<> struct jlcxx::IsMirroredType<pm::operations::cmp> : std::false_type { };

namespace jlpolymake {

void add_set(jlcxx::Module& jlpolymake)
{
    jlpolymake.add_type<pm::operations::cmp>("operations_cmp");

    wrap_set<pm::Int>(jlpolymake);
    wrap_set<pm::Set<pm::Int>>(jlpolymake);
            
    jlpolymake.method("range", [](const pm::Int a, const pm::Int b) {
            return pm::Set<pm::Int>{pm::range(a, b)};
            });
    jlpolymake.method("sequence", [](const pm::Int a, const pm::Int c) {
            return pm::Set<pm::Int>{pm::sequence(a, c)};
            });
    jlpolymake.method("scalar2set", [](const pm::Int s) {
            return pm::Set<pm::Int>{pm::scalar2set(s)};
            });
}

}
