#include "jlpolymake/jlpolymake.h"

#include "jlpolymake/type_modules.h"

#include "jlpolymake/generated/get_type_names.h"

namespace jlpolymake {

void add_type_translations(jlcxx::Module& jlpolymake)
{
    jlpolymake.method("get_type_names", &get_type_names);
}

}
