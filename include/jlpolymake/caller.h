#ifndef POLYMAKE_WRAP_CALLER
#define POLYMAKE_WRAP_CALLER

#include "jlpolymake/jlpolymake.h"

namespace jlpolymake {

#include "jlpolymake/generated/type_declarations_extern.h"

template <typename T>
using value_feeder = std::function<bool(T, jl_value_t*)>;

template <typename T>
struct feeder_list {
   using list_type = std::list<value_feeder<T>>;
   static list_type& get()
   {
      static list_type list;
      return list;
   }
};

// this needs to be called three times, with:
// value_feeder<VarFunCall&>
// value_feeder<PropertyOut&>
// value_feeder<Value> (for OptionSet[key]
// (careful with the refs &)
template <typename T>
void register_value_feeder(value_feeder<T>&& f) {
   feeder_list<T>::get().emplace_back(f);
}

void insert_type_in_map(std::string&&, jl_value_t**);

void set_julia_type(const std::string&, void*);

void add_caller(jlcxx::Module&);

}

#endif
