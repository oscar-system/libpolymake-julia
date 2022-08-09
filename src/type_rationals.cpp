#include "jlpolymake/jlpolymake.h"

#include "jlpolymake/tools.h"

#include "jlpolymake/functions.h"

#include "jlpolymake/type_modules.h"

namespace jlpolymake {
    
pm::Rational new_rational_from_fmpq(jl_value_t* rational)
{
    mpq_t x;
    mpq_init(x);
    fmpq_t* r;
    r = reinterpret_cast<fmpq_t*>(rational);
    fmpq_get_mpq(x, *r);
    pm::Rational res(x);
    mpq_clear(x);
    return res;
}

void add_rational(jlcxx::Module& jlpolymake)
{

    jlpolymake
        .add_type<pm::Rational>("Rational",
                                jlcxx::julia_type("Real", "Base"))
        .constructor<pm::Integer, pm::Integer>()
        .method("rational_si_si", [](
            const jlcxx::StrictlyTypedNumber<long> num,
            const jlcxx::StrictlyTypedNumber<long> den) {
            return pm::Rational(num.value, den.value);
        })
        .method("<", [](pm::Rational& a, pm::Rational& b) { return a < b; })
        .method("<", [](pm::Rational& a, pm::Integer& b) { return a < b; })
        .method("<", [](pm::Rational& a,
                        int64_t       b) { return a < static_cast<pm::Int>(b); })
        .method("<", [](pm::Integer& a, pm::Rational& b) { return a < b; })
        .method("<", [](int64_t       a,
                        pm::Rational& b) { return static_cast<pm::Int>(a) < b; })

        .method("<=", [](pm::Rational& a, pm::Rational& b) { return a <= b; })
        .method("<=", [](pm::Rational& a, pm::Integer& b) { return a <= b; })
        .method("<=", [](pm::Rational& a,
                         int64_t b) { return a <= static_cast<pm::Int>(b); })
        .method("<=", [](pm::Integer& a, pm::Rational& b) { return a <= b; })
        .method("<=", [](int64_t a, pm::Rational& b) {
                    return static_cast<pm::Int>(a) <= b;
                })

        .method(
            "numerator",
            [](const pm::Rational& r) { return pm::Integer(numerator(r)); })
        .method(
            "denominator",
            [](const pm::Rational& r) { return pm::Integer(denominator(r)); })
        .method("show_small_obj",
                [](const pm::Rational& r) {
                    return show_small_object<pm::Rational>(r, false);
                })
        .method("Float64", [](pm::Rational& a) { return double(a); })
        .method("-", [](pm::Rational& a, pm::Rational& b) { return a - b; })
        .method("-", [](pm::Rational& a, pm::Integer& b) { return a - b; })
        .method("-", [](pm::Rational& a,
                        int64_t       b) { return a - static_cast<pm::Int>(b); })
        .method("-", [](pm::Integer& a, pm::Rational& b) { return a - b; })
        .method("-", [](int64_t       a,
                        pm::Rational& b) { return static_cast<pm::Int>(a) - b; })
        // unary minus
        .method("-", [](pm::Rational& a) { return -a; })

        .method("//", [](pm::Rational& a, pm::Rational& b) { return a / b; })
        .method("//", [](pm::Rational& a, pm::Integer&  b) { return a / b; })
        .method("//", [](pm::Rational& a, int64_t       b) {
            return a / static_cast<pm::Int>(b); })
        .method("//", [](pm::Integer&  a, pm::Rational& b) { return a / b; })
        .method("//", [](int64_t       a, pm::Rational& b) {
            return static_cast<pm::Int>(a) / b; });

        jlpolymake.set_override_module(jlpolymake.julia_module());
        jlpolymake.method("==", [](pm::Rational& a, pm::Rational& b) {
            return a == b; });
        jlpolymake.method("==", [](pm::Rational& a, pm::Integer& b) {
            return a == b; });
        jlpolymake.method("==", [](pm::Integer& a, pm::Rational& b) {
            return a == b; });
        jlpolymake.method("==", [](pm::Rational& a, int64_t b) {
            return a == static_cast<pm::Int>(b); });
        jlpolymake.method("==", [](int64_t a, pm::Rational& b) {
            return static_cast<pm::Int>(a) == b; });
        // the symmetric definitions are on the julia side
        jlpolymake.method("+", [](pm::Rational& a, pm::Rational& b) {
            return a + b; });
        jlpolymake.method("+", [](pm::Rational& a, pm::Integer& b) {
            return a + b; });
        jlpolymake.method("+", [](pm::Integer& a, pm::Rational& b) {
            return a + b; });
        jlpolymake.method("+", [](pm::Rational& a, int64_t b) {
            return a + static_cast<pm::Int>(b); });
        jlpolymake.method("+", [](int64_t a, pm::Rational& b) {
            return static_cast<pm::Int>(a) + b; });
        jlpolymake.method("*", [](pm::Rational& a, pm::Rational& b) {
            return a * b; });
        jlpolymake.method("*", [](pm::Rational& a, pm::Integer& b) {
            return a * b; });
        jlpolymake.method("*", [](pm::Integer& a, pm::Rational& b) {
            return a * b; });
        jlpolymake.method("*", [](pm::Rational& a, int64_t b) {
            return a * static_cast<pm::Int>(b); });
        jlpolymake.method("*", [](int64_t a, pm::Rational& b) {
            return static_cast<pm::Int>(a) * b; });
        jlpolymake.unset_override_module();

    jlpolymake.method("new_rational_from_fmpq", new_rational_from_fmpq);
    jlpolymake.method("to_rational", [](pm::perl::PropertyValue pv) {
        return to_SmallObject<pm::Rational>(pv);
    });
}

}
