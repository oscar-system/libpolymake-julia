#ifndef JLPOLYMAKE_TYPE_MODULES
#define JLPOLYMAKE_TYPE_MODULES

#include <jlcxx/jlcxx.hpp>

namespace jlpolymake {

using tparametric1 = jlcxx::TypeWrapper<jlcxx::Parametric<jlcxx::TypeVar<1>>>;
using tparametric2 = jlcxx::TypeWrapper<jlcxx::Parametric<jlcxx::TypeVar<1>, jlcxx::TypeVar<2>>>;

tparametric1 add_array(jlcxx::Module& jlpolymake);
void add_array_polynomial(jlcxx::Module& jlpolymake, tparametric1 arrayt);

void add_bigobject(jlcxx::Module& jlpolymake);
void add_direct_calls(jlcxx::Module&);
void add_graph(jlcxx::Module& jlpolymake);
void add_edgemap(jlcxx::Module& jlpolymake);
void add_nodemap(jlcxx::Module& jlpolymake);
void add_homologygroup(jlcxx::Module& jlpolymake);
void add_incidencematrix(jlcxx::Module& jlpolymake);
void add_integer(jlcxx::Module& jlpolymake);
void add_lists(jlcxx::Module& jlpolymake);
void add_map(jlcxx::Module& jlpolymake);
void add_matrix(jlcxx::Module& jlpolymake);
void add_polynomial(jlcxx::Module& jlpolymake);
void add_unipolynomial(jlcxx::Module& jlpolymake);
void add_rational(jlcxx::Module& jlpolymake);
void add_quadraticextension(jlcxx::Module& jlpolymake);
tparametric2 add_pairs(jlcxx::Module& jlpolymake);
void add_pair_list(jlcxx::Module& jlpolymake, tparametric2 pairt);
void add_set(jlcxx::Module& jlpolymake);
void add_sparsematrix(jlcxx::Module& jlpolymake);
void add_sparsevector(jlcxx::Module& jlpolymake);
void add_tropicalnumber(jlcxx::Module& jlpolymake);
void add_type_translations(jlcxx::Module&);
void add_vector(jlcxx::Module& jlpolymake);

}

#endif
