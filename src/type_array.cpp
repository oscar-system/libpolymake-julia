#include "jlpolymake/jlpolymake.h"

#include "jlpolymake/tools.h"

#include "jlpolymake/functions.h"

#include "jlpolymake/type_modules.h"

#include "jlpolymake/containers.h"

namespace jlpolymake {

void add_array(jlcxx::Module& jlpolymake)
{
   wrap_array<std::string>(jlpolymake);
   wrap_array<pm::Array<pm::Set<pm::Int>>>(jlpolymake);
   wrap_array<pm::Array<pm::Int>>(jlpolymake);
   wrap_array<pm::Array<pm::Integer>>(jlpolymake);
   wrap_array<pm::Array<pm::Rational>>(jlpolymake);
   wrap_array<std::pair<pm::Int, pm::Int>>(jlpolymake);
   wrap_array<std::list<std::pair<pm::Int, pm::Int>>>(jlpolymake);
   wrap_array<pm::Matrix<pm::Integer>>(jlpolymake);
   wrap_array<polymake::topaz::HomologyGroup<pm::Integer>>(jlpolymake);

   wrap_array<pm::perl::BigObject>(jlpolymake);

   jlpolymake.method("to_array_string",[] (std::optional<pm::perl::ListResult>& l)
         {
            if (!l)
               throw std::runtime_error("ListResult can be unpacked only once.");
            pm::Array<std::string> arr;
            *l >> pm::perl::unroll(arr);
            l.reset();
            return arr;
         });
}

}
