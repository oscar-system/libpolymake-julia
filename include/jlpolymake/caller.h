#ifndef POLYMAKE_WRAP_CALLER
#define POLYMAKE_WRAP_CALLER

#include "jlpolymake/jlpolymake.h"

namespace jlpolymake {

#include "jlpolymake/generated/type_declarations_extern.h"

void insert_type_in_map(std::string&&, jl_value_t**);

void set_julia_type(const std::string&, void*);

void add_caller(jlcxx::Module&);

}

#endif
