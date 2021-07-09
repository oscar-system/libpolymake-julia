#include "jlpolymake/jlpolymake.h"

#include "jlpolymake/tools.h"

#include "jlpolymake/functions.h"

#include "jlpolymake/type_modules.h"

namespace jlpolymake {

void add_polynomial(jlcxx::Module& jlpolymake)
{
    jlpolymake
        .add_type<jlcxx::Parametric<jlcxx::TypeVar<1>, jlcxx::TypeVar<2>>>(
            "Polynomial")
        .apply_combination<pm::Polynomial, VecOrMat_supported::value_type, jlcxx::ParameterList<pm::Int>>(
            [](auto wrapped) {
                typedef typename decltype(wrapped)::type polyT;
                typedef typename decltype(wrapped)::type::coefficient_type coeffT;
                typedef typename decltype(wrapped)::type::monomial_type::value_type expT;

                wrapped.template constructor<pm::Vector<coeffT>, pm::Matrix<expT>>();

                wrapped.method("_isequal", [](polyT& a, polyT& b) { return a == b; });
                wrapped.method("_add", [](polyT& a, polyT& b) { return a + b; });
                wrapped.method("_sub", [](polyT& a, polyT& b) { return a - b; });
                wrapped.method("_mul", [](polyT& a, polyT& b) { return a * b; });
                wrapped.method("^", [](polyT& a, int64_t b) { return a ^ b; });
                wrapped.method("/", [](polyT& a, coeffT c) { return a / c; });
                wrapped.method("coefficients_as_vector", &polyT::coefficients_as_vector);
                wrapped.method("monomials_as_matrix", [](polyT& a) { return a.monomials_as_matrix(); });
                wrapped.method("set_var_names", [](polyT& a, Array<std::string>& names) { a.set_var_names(names); });
                wrapped.method("get_var_names", [](polyT& a) { return a.get_var_names(); });
                wrapped.method("nvars", [] (polyT& a) -> pm::Int { return a.n_vars(); });

                wrapped.method("show_small_obj", [](polyT& P) {
                    return show_small_object<polyT>(P);
                });
                wrapped.method("take",
                    [](pm::perl::BigObject p, const std::string& s,
                        polyT& P){ p.take(s) << P; });
        });

    jlpolymake.method("to_polynomial_int_int", [](pm::perl::PropertyValue v) {
            return to_SmallObject<pm::Polynomial<pm::Int,pm::Int>>(v);
        });
    jlpolymake.method("to_polynomial_integer_int", [](pm::perl::PropertyValue v) {
            return to_SmallObject<pm::Polynomial<pm::Integer,pm::Int>>(v);
        });
    jlpolymake.method("to_polynomial_rational_int", [](pm::perl::PropertyValue v) {
            return to_SmallObject<pm::Polynomial<pm::Rational,pm::Int>>(v);
        });
    jlpolymake.method("to_polynomial_double_int", [](pm::perl::PropertyValue v) {
            return to_SmallObject<pm::Polynomial<double,pm::Int>>(v);
        });
}

}
