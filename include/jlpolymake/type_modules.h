#ifndef JLPOLYMAKE_TYPE_MODULES
#define JLPOLYMAKE_TYPE_MODULES

#include <jlcxx/jlcxx.hpp>

namespace jlpolymake {

void unbox_pv(jlcxx::Module& jlpolymake);

using tparametric1 = jlcxx::TypeWrapper<jlcxx::Parametric<jlcxx::TypeVar<1>>>;
using tparametric2 = jlcxx::TypeWrapper<jlcxx::Parametric<jlcxx::TypeVar<1>, jlcxx::TypeVar<2>>>;

void add_array(jlcxx::Module& jlpolymake);
void prepare_containers(jlcxx::Module& jlpolymake);

void wrap_types(jlcxx::Module& jlpolymake);
void wrap_types_extra(jlcxx::Module& jlpolymake);

void add_bigobject(jlcxx::Module& jlpolymake);
void add_direct_calls(jlcxx::Module&);

void add_integer(jlcxx::Module& jlpolymake);
void add_rational(jlcxx::Module& jlpolymake);
void add_tropicalnumber(jlcxx::Module& jlpolymake);
void add_quadraticextension(jlcxx::Module& jlpolymake);

void add_homologygroup(jlcxx::Module& jlpolymake);
void add_incidencematrix(jlcxx::Module& jlpolymake);
void add_switchtable(jlcxx::Module& jlpolymake);
void add_decoration(jlcxx::Module& jlpolymake);

void add_graph(jlcxx::Module& jlpolymake);

void add_edgemap(jlcxx::Module& jlpolymake);
void add_nodemap(jlcxx::Module& jlpolymake);

void add_matrix_extended(jlcxx::Module& jlpolymake, tparametric1 matrixt);
void add_vector_extended(jlcxx::Module& jlpolymake, tparametric1 vectort);

}

#endif
