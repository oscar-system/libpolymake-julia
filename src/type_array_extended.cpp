#include "jlpolymake/jlpolymake.h"

#include "jlpolymake/tools.h"

#include "jlpolymake/functions.h"

#include "jlpolymake/type_modules.h"

namespace jlpolymake {

void add_array_extended(jlcxx::Module& jlpolymake, tparametric1 array_type)
{
    array_type
        .apply<
                pm::Array<pm::Polynomial<pm::Rational,long>>,
                pm::Array<pm::Polynomial<pm::Integer,long>>,
                pm::Array<std::pair<pm::Array<pm::Int>, pm::Array<pm::Int>>>
            >([](auto wrapped) {
            typedef typename decltype(wrapped)::type             WrappedT;
            typedef typename decltype(wrapped)::type::value_type elemType;

            wrapped.template constructor<int64_t>();
            wrapped.template constructor<int64_t, elemType>();

            wrapped.method("_getindex", [](const WrappedT& A, int64_t n) {
                return elemType(A[static_cast<pm::Int>(n) - 1]);
            });
            wrapped.method("_setindex!",
                           [](WrappedT& A, const elemType& val, int64_t n) {
                               A[static_cast<pm::Int>(n) - 1] = val;
                           });
            wrapped.method("length", &WrappedT::size);
            wrapped.method("resize!", [](WrappedT& A, int64_t newsz) {
                A.resize(static_cast<pm::Int>(newsz));
                return A;
            });

            wrapped.method("append!", [](WrappedT& A, WrappedT& B) {
                A.append(B);
                return A;
            });
            wrapped.method("fill!", [](WrappedT& A, const elemType& x) {
                A.fill(x);
                return A;
            });
            wrapped.method("show_small_obj", [](const WrappedT& A) {
                return show_small_object<WrappedT>(A);
            });
            wrapped.method("take",
                           [](pm::perl::BigObject p, const std::string& s,
                              WrappedT& A) { p.take(s) << A; });
        });
    jlpolymake.method(
        "to_array_polynomial_integer_int", [](const pm::perl::PropertyValue& pv) {
            return to_SmallObject<pm::Array<pm::Polynomial<pm::Integer,long>>>(pv);
        });
    jlpolymake.method(
        "to_array_polynomial_rational_int", [](const pm::perl::PropertyValue& pv) {
            return to_SmallObject<pm::Array<pm::Polynomial<pm::Rational,long>>>(pv);
        });
    jlpolymake.method(
        "to_array_pair_array_int_array_int", [](const pm::perl::PropertyValue& pv) {
            return to_SmallObject<pm::Array<std::pair<pm::Array<pm::Int>, pm::Array<pm::Int>>>>(pv);
        });
}

}
