#ifndef JLPOLYMAKE_H
#define JLPOLYMAKE_H

#include <jlcxx/jlcxx.hpp>
#include <jlcxx/functions.hpp>

#include <string>
#include <iostream>

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wlogical-op-parentheses"
#pragma clang diagnostic ignored "-Wshift-op-parentheses"
#endif

#include <polymake/Main.h>
#include <polymake/Matrix.h>
#include <polymake/SparseMatrix.h>
#include <polymake/Vector.h>
#include <polymake/Set.h>
#include <polymake/Array.h>
#include <polymake/Rational.h>
#include <polymake/QuadraticExtension.h>
#include <polymake/TropicalNumber.h>
#include <polymake/IncidenceMatrix.h>
#include <polymake/Polynomial.h>
#include <polymake/polytope/solve_LP.h>
#include <polymake/SparseVector.h>
#include <polymake/Map.h>
#include <polymake/Graph.h>
#include <polymake/topaz/HomologyComplex.h>
#include <polymake/group/switch_table.h>
#include <polymake/RandomGenerators.h>

#include <polymake/perl/calls.h>

#include <polymake/perl/macros.h>
#include <polymake/perl/wrappers.h>

#if defined(__clang__)
#pragma clang diagnostic pop
#endif

#define JLPOLYMAKE_VERSION_MAJOR 0
#define JLPOLYMAKE_VERSION_MINOR 11
#define JLPOLYMAKE_VERSION_PATCH 1

#define __JLPOLYMAKE_STR_HELPER(x) #x
#define __JLPOLYMAKE_STR(x) __JLPOLYMAKE_STR_HELPER(x)
#define JLPOLYMAKE_VERSION_STRING __JLPOLYMAKE_STR(JLPOLYMAKE_VERSION_MAJOR) "." __JLPOLYMAKE_STR(JLPOLYMAKE_VERSION_MINOR) "." __JLPOLYMAKE_STR(JLPOLYMAKE_VERSION_PATCH)

#endif
