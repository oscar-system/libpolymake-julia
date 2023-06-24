#pragma once

#include "jlpolymake/jlpolymake.h"

#include "jlpolymake/tools.h"

#include "jlpolymake/functions.h"

#include "jlpolymake/type_modules.h"

namespace jlpolymake {

using TypeWrapper2 = jlcxx::TypeWrapper<jlcxx::Parametric<jlcxx::TypeVar<1>, jlcxx::TypeVar<2>>>;
using TypeWrapper1 = jlcxx::TypeWrapper1;

class pmwrappers
{
private:
  pmwrappers(jlcxx::Module& mod);
  static std::unique_ptr<pmwrappers> m_instance;
  jlcxx::Module& m_pmw_mod;
public:
  TypeWrapper1 pmarray;
  TypeWrapper1 pmset;
  TypeWrapper1 pmsetiterator;
  TypeWrapper1 pmvector;
  TypeWrapper1 pmmatrix;
  TypeWrapper1 pmsparsevector;
  TypeWrapper1 pmsparsematrix;
  TypeWrapper2 pmpair;

  static void instantiate(jlcxx::Module& mod);
  static pmwrappers& instance();

  inline jl_module_t* module() const
  {
    return m_pmw_mod.julia_module();
  }
};

pmwrappers& wrappers();

template<typename TypeWrapperT>
void wrap_common(TypeWrapperT& wrapped)
{
  using WrappedT = typename TypeWrapperT::type;
  wrapped.module().set_override_module(pmwrappers::instance().module());
  wrapped.method("take",
        [](pm::perl::BigObject p, const std::string& s,
           const WrappedT& M) { p.take(s) << M; });
  wrapped.method("show_small_obj", [](const WrappedT& M) {
        return show_small_object<WrappedT>(M);
        });
  wrapped.module().unset_override_module();
}


struct WrapMatrix
{
   template <typename TypeWrapperT>
   static void wrap(TypeWrapperT& wrapped)
   {
      using WrappedT = typename TypeWrapperT::type;
      using elemType = typename TypeWrapperT::type::value_type;

      wrapped.template constructor<int64_t, int64_t>();

      wrapped.module().set_override_module(pmwrappers::instance().module());
      wrapped.method("_getindex",
            [](const WrappedT& f, int64_t i, int64_t j) {
            return elemType(f(i - 1, j - 1));
            });
      wrapped.method("_setindex!",
            [](WrappedT& M, const elemType& r, int64_t i,
               int64_t j) { M(i - 1, j - 1) = r; });
      wrapped.method("nrows", &WrappedT::rows);
      wrapped.method("ncols", &WrappedT::cols);
      wrapped.method("resize!", [](WrappedT& M, int64_t i,
               int64_t j) { M.resize(i, j); });
      wrapped.module().unset_override_module();
      wrap_common(wrapped);
   }
   template <typename TypeWrapperT>
   void operator()(TypeWrapperT&& wrapped)
   {
      WrapMatrix::wrap(wrapped);
   }
};

struct WrapSparseMatrix
{
   template <typename TypeWrapperT>
   void operator()(TypeWrapperT&& wrapped)
   {
      using WrappedT = typename TypeWrapperT::type;
      using elemType = typename TypeWrapperT::type::value_type;
      WrapMatrix::wrap(wrapped);
      wrapped.module().set_override_module(pmwrappers::instance().module());
      wrapped.method("nzindices", [](const WrappedT& S) {
            return Array<Set<pm::Int>>(pm::rows(pm::index_matrix(S)));
            });
      wrapped.module().unset_override_module();
   }
};

struct WrapVector
{
   template <typename TypeWrapperT>
   static void wrap(TypeWrapperT& wrapped)
   {
      using WrappedT = typename TypeWrapperT::type;
      using elemType = typename TypeWrapperT::type::value_type;

      wrapped.template constructor<int64_t>();

      wrapped.module().set_override_module(pmwrappers::instance().module());
      wrapped.method("_getindex", [](const WrappedT& V, int64_t n) {
            return elemType(V[n - 1]);
            });
      wrapped.method("_setindex!",
            [](WrappedT& V, elemType val, int64_t n) {
            V[n - 1] = val;
            });
      wrapped.method("length", &WrappedT::dim);
      wrapped.method("resize!",
            [](WrappedT& V, int64_t sz) { V.resize(sz); });

      wrapped.module().unset_override_module();
      wrap_common(wrapped);
   }
   template <typename TypeWrapperT>
   void operator()(TypeWrapperT&& wrapped)
   {
      WrapVector::wrap(wrapped);
   }
};

struct WrapPair
{
   template <typename TypeWrapperT>
   void operator()(TypeWrapperT&& wrapped)
   {
      using WrappedT = typename TypeWrapperT::type;
      using firstT = typename TypeWrapperT::type::first_type;
      using secondT = typename TypeWrapperT::type::second_type;

      wrapped.template constructor();
      wrapped.template constructor<firstT, secondT>();

      //Pattern to overwrite function in Base
      wrapped.module().set_override_module(jl_base_module);
      wrapped.method("first", [](const WrappedT& P) {
            return P.first;
            });
      wrapped.method("last", [](const WrappedT& P) {
            return P.second;
            });
      wrapped.module().unset_override_module();
      wrap_common(wrapped);
   }
};

struct WrapSparseVector
{
   template <typename TypeWrapperT>
   void operator()(TypeWrapperT&& wrapped)
   {
      using WrappedT = typename TypeWrapperT::type;
      using elemType = typename TypeWrapperT::type::value_type;

      WrapVector::wrap(wrapped);

      wrapped.module().set_override_module(pmwrappers::instance().module());
      wrapped.method("_nzindices", [](const WrappedT& S) {
            return Set<pm::Int>(pm::indices(S));
            });
      wrapped.method("*", [](const elemType& s, const WrappedT& V) { return WrappedT(s * V); });
      wrapped.method("/", [](const WrappedT& V, const elemType& s) { return WrappedT(V / s); });
      wrapped.module().unset_override_module();
   }
};

template <typename elem>
struct WrapArrayImpl
{
   template <typename TypeWrapperT>
   static void wrap(TypeWrapperT& wrapped)
   {
      using WrappedT = typename TypeWrapperT::type;
      using elemType = typename TypeWrapperT::type::value_type;

      wrapped.template constructor<int64_t>();
      wrapped.template constructor<int64_t, elemType>();

      wrapped.module().set_override_module(pmwrappers::instance().module());
      wrapped.method("_getindex", [](const WrappedT& V, int64_t n) {
            return elemType(V[static_cast<pm::Int>(n) - 1]);
            });
      wrapped.method("_setindex!",
            [](WrappedT& V, elemType val, int64_t n) {
            V[static_cast<pm::Int>(n) - 1] = val;
            });
      wrapped.method("length", &WrappedT::size);
      wrapped.method("resize!", [](WrappedT& V, int64_t sz) {
            V.resize(static_cast<pm::Int>(sz));
            return V;
            });
      wrapped.method("append!", [](WrappedT& A, const WrappedT& B) {
            A.append(B);
            return A;
            });
      wrapped.method("fill!", [](WrappedT& A, const elemType& x) {
            A.fill(x);
            return A;
            });
      wrapped.module().unset_override_module();
      wrap_common(wrapped);
   }
};

template <bool readonly>
struct WrapArrayImpl<pm::perl::BigObject::Array_element<readonly>>
{
   template <typename TypeWrapperT>
   static void wrap(TypeWrapperT& wrapped)
   {
      using WrappedT = typename TypeWrapperT::type;
      using elemType = pm::perl::BigObject;

      wrapped.template constructor<int64_t>();
      wrapped.template constructor<pm::perl::BigObjectType, int64_t>();

      wrapped.module().set_override_module(pmwrappers::instance().module());
      wrapped.method("_getindex", [](const WrappedT& V, int64_t n) {
            return elemType(V[static_cast<pm::Int>(n) - 1]);
            });
      wrapped.method("_setindex!",
            [](WrappedT& V, elemType val, int64_t n) {
            V[static_cast<pm::Int>(n) - 1] = val;
            });
      wrapped.method("length", &WrappedT::size);
      wrapped.method("resize!", [](WrappedT& V, int64_t sz) {
            V.resize(static_cast<pm::Int>(sz));
            });
      // no show small object for bigobjects
      //wrap_common(wrapped);
      wrapped.method("take",
            [](pm::perl::BigObject p, const std::string& s,
               const WrappedT& M) { p.take(s) << M; });
      wrapped.module().unset_override_module();
   }
};
struct WrapArray
{
   template <typename TypeWrapperT>
   void operator()(TypeWrapperT&& wrapped)
   {
      using elemType = typename TypeWrapperT::type::value_type;
      WrapArrayImpl<elemType>::wrap(wrapped);
   }
};

template <typename elem>
struct WrapSetImpl
{
   template <typename TypeWrapperT>
   static void wrap(TypeWrapperT& wrapped)
   {
      using WrappedT = typename TypeWrapperT::type;
      using elemType = typename TypeWrapperT::type::value_type;

      wrapped.template constructor<pm::Set<elemType>>();


      wrapped.module().set_override_module(pmwrappers::instance().module());
      wrapped.method("_new_set", [](jlcxx::ArrayRef<elemType> A) {
         pm::Set<elemType> s{A.begin(), A.end()};
         return s;
      });

      wrapped.method("swap", &WrappedT::swap);

      wrapped.method("isempty", &WrappedT::empty);
      wrapped.method("length", &WrappedT::size);

      wrapped.method("empty!", [](WrappedT& S) {
          S.clear();
          return S;
      });
      wrapped.method("_isequal", [](const WrappedT& S, const WrappedT& T) { return S == 
T; });
      wrapped.method(
          "in", [](const elemType& i, const WrappedT& S) { return S.contains(i); });

      wrapped.method("push!", [](WrappedT& S, const elemType& i) {
          S += i;
          return S;
      });

      wrapped.method("delete!", [](WrappedT& S, const elemType& i) {
          S -= i;
          return S;
      });

      wrapped.method("union!",
                     [](WrappedT& S, const WrappedT& T) { return S += T; });
      wrapped.method("intersect!",
                     [](WrappedT& S, const WrappedT& T) { return S *= T; });
      wrapped.method("setdiff!",
                     [](WrappedT& S, const WrappedT& T) { return S -= T; });
      wrapped.method("symdiff!",
                     [](WrappedT& S, const WrappedT& T) { return S ^= T; });

      wrapped.method(
          "union", [](const WrappedT& S, const WrappedT& T) { return WrappedT{S + T}; })
;
      wrapped.method("intersect", [](const WrappedT& S, const WrappedT& T) {
          return WrappedT{S * T};
      });
      wrapped.method("setdiff", [](const WrappedT& S, const WrappedT& T) {
          return WrappedT{S - T};
      });
      wrapped.method("symdiff", [](const WrappedT& S, const WrappedT& T) {
          return WrappedT{S ^ T};
      });

      wrapped.method("incl", [](const WrappedT& s1, const WrappedT& s2) {
              return pm::incl(s1, s2);
              });

      wrapped.module().unset_override_module();
      wrap_common(wrapped);
   }
};
struct WrapSet
{
   template <typename TypeWrapperT>
   void operator()(TypeWrapperT&& wrapped)
   {
      using elemType = typename TypeWrapperT::type::value_type;
      WrapSetImpl<elemType>::wrap(wrapped);
   }
};

template <typename elem>
struct WrapSetIteratorImpl
{
   template <typename TypeWrapperT>
   static void wrap(TypeWrapperT& wrapped)
   {
      using WrappedSetIter = typename TypeWrapperT::type;
      using elemType = typename TypeWrapperT::type::value_type;
      wrapped.module().set_override_module(pmwrappers::instance().module());
      wrapped.method("beginiterator", [](const pm::Set<elemType>& S) {
          auto result = WrappedSetIterator<elemType>{S};
          return result;
      });

      wrapped.method("increment", [](WrappedSetIter& state) {
          state.iterator++;
      });
      wrapped.method("get_element", [](const WrappedSetIter& state) {
          auto elt = *(state.iterator);
          return elt;
      });
      wrapped.method("isdone", [](const pm::Set<elemType>& S,
                                  const WrappedSetIter&    state) {
          return S.end() == state.iterator;
      });
      wrapped.module().unset_override_module();
   }
};
struct WrapSetIterator
{
   template <typename TypeWrapperT>
   void operator()(TypeWrapperT&& wrapped)
   {
      using elemType = typename TypeWrapperT::type::value_type;
      WrapSetIteratorImpl<elemType>::wrap(wrapped);
   }
};


template<typename T>
inline void wrap_vector(jlcxx::Module& mod)
{
   TypeWrapper1(mod, pmwrappers::instance().pmvector).apply<pm::Vector<T>>(WrapVector());
}
template<typename T>
inline void wrap_vector_for_types(jlcxx::Module& mod)
{
   TypeWrapper1(mod, pmwrappers::instance().pmvector).apply_combination<pm::Vector, T>(WrapVector());
}

template<typename T>
inline void wrap_matrix(jlcxx::Module& mod)
{
   TypeWrapper1(mod, pmwrappers::instance().pmmatrix).apply<pm::Matrix<T>>(WrapMatrix());
}
template<typename T>
inline void wrap_matrix_for_types(jlcxx::Module& mod)
{
   TypeWrapper1(mod, pmwrappers::instance().pmmatrix).apply_combination<pm::Matrix, T>(WrapMatrix());
}

template<typename T>
inline void wrap_sparsevector(jlcxx::Module& mod)
{
   TypeWrapper1(mod, pmwrappers::instance().pmsparsevector).apply<pm::SparseVector<T>>(WrapSparseVector());
}
template<typename T>
inline void wrap_sparsevector_for_types(jlcxx::Module& mod)
{
   TypeWrapper1(mod, pmwrappers::instance().pmsparsevector).apply_combination<pm::SparseVector, T>(WrapSparseVector());
}

template<typename T>
inline void wrap_sparsematrix(jlcxx::Module& mod)
{
   TypeWrapper1(mod, pmwrappers::instance().pmsparsematrix).apply<pm::SparseMatrix<T>>(WrapSparseMatrix());
}
template<typename T>
inline void wrap_sparsematrix_for_types(jlcxx::Module& mod)
{
   TypeWrapper1(mod, pmwrappers::instance().pmsparsematrix).apply_combination<pm::SparseMatrix, T>(WrapSparseMatrix());
}

template<typename T>
inline void wrap_array(jlcxx::Module& mod)
{
   TypeWrapper1(mod, pmwrappers::instance().pmarray).apply<pm::Array<T>>(WrapArray());
}
template<typename T>
inline void wrap_array_for_types(jlcxx::Module& mod)
{
   TypeWrapper1(mod, pmwrappers::instance().pmarray).apply_combination<pm::Array, T>(WrapArray());
}

template<typename T>
inline void wrap_set(jlcxx::Module& mod)
{
   TypeWrapper1(mod, pmwrappers::instance().pmset).apply<pm::Set<T>>(WrapSet());
   TypeWrapper1(mod, pmwrappers::instance().pmsetiterator).apply<WrappedSetIterator<T>>(WrapSetIterator());
}

template<typename T1, typename T2>
inline void wrap_pair(jlcxx::Module& mod)
{
   TypeWrapper2(mod, pmwrappers::instance().pmpair).apply<std::pair<T1,T2>>(WrapPair());
}

}
