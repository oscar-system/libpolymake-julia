#include "jlpolymake/jlpolymake.h"

#include "jlpolymake/tools.h"

#include "jlpolymake/functions.h"

#include "jlpolymake/type_modules.h"

#include "jlpolymake/containers.h"

namespace jlpolymake {

std::unique_ptr<pmwrappers> pmwrappers::m_instance = std::unique_ptr<pmwrappers>();

void pmwrappers::instantiate(jlcxx::Module& mod)
{
  m_instance.reset(new pmwrappers(mod));
}

pmwrappers& pmwrappers::instance()
{
  if(m_instance == nullptr)
  {
    throw std::runtime_error("polymake container wrappers was not instantiated");
  }
  return *m_instance;
}

pmwrappers& wrappers()
{
  return pmwrappers::instance();
}

pmwrappers::pmwrappers(jlcxx::Module& jlpolymake) :
  m_pmw_mod(jlpolymake),
  pmarray(jlpolymake.add_type<jlcxx::Parametric<jlcxx::TypeVar<1>>>("Array", jlcxx::julia_type("AbstractVector", "Base"))),
  pmset(jlpolymake.add_type<jlcxx::Parametric<jlcxx::TypeVar<1>>>("Set", jlcxx::julia_type("AbstractSet", "Base"))),
  pmsetiterator(jlpolymake.add_type<jlcxx::Parametric<jlcxx::TypeVar<1>>>("SetIterator")),
  pmvector(jlpolymake.add_type<jlcxx::Parametric<jlcxx::TypeVar<1>>>("Vector", jlcxx::julia_type("AbstractVector", "Base"))),
  pmmatrix(jlpolymake.add_type<jlcxx::Parametric<jlcxx::TypeVar<1>>>("Matrix", jlcxx::julia_type("AbstractMatrix", "Base"))),
  pmsparsevector(jlpolymake.add_type<jlcxx::Parametric<jlcxx::TypeVar<1>>,jlcxx::ParameterList<jlcxx::TypeVar<1>,int>>("SparseVector", jlcxx::julia_type("AbstractSparseVector", "SparseArrays"))),
  pmsparsematrix(jlpolymake.add_type<jlcxx::Parametric<jlcxx::TypeVar<1>>,jlcxx::ParameterList<jlcxx::TypeVar<1>,int>>("SparseMatrix", jlcxx::julia_type("AbstractSparseMatrix", "SparseArrays"))),
  pmpair(jlpolymake.add_type<jlcxx::Parametric<jlcxx::TypeVar<1>, jlcxx::TypeVar<2>>>("StdPair"))
{
}

void prepare_containers(jlcxx::Module& jlpolymake) {
   pmwrappers::instantiate(jlpolymake);
}

}
