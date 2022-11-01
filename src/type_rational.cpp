#include "jlpolymake/jlpolymake.h"

#include "jlpolymake/tools.h"

#include "jlpolymake/functions.h"

#include "jlpolymake/type_modules.h"

namespace jlpolymake {

pm::Rational new_rational_from_fmpq(jl_value_t* rational)
{
    mpz_t n, d;
    mpz_init(n);
    mpz_init(d);
    fmpq_get_mpz_frac(n, d, *reinterpret_cast<fmpq_t*>(rational));
    return pm::Rational(std::move(n), std::move(d));
}

pm::Rational new_rational_from_fmpz(jl_value_t* integer)
{
    mpz_t z_mp;
    mpz_init(z_mp);
    fmpz_get_mpz(z_mp, *reinterpret_cast<fmpz_t*>(integer));
    return pm::Rational(std::move(z_mp));
}

void new_fmpq_from_rational(const pm::Rational& rational, void* p_fmpq)
{
    if (isinf(rational)) throw pm::GMP::BadCast();
    fmpq_set_mpq(*reinterpret_cast<fmpq_t*>(p_fmpq), rational.get_rep());
}

void new_fmpz_from_rational(const pm::Rational& rational, void* p_fmpz)
{
    if (!rational.is_integral() || isinf(rational)) throw pm::GMP::BadCast();
    fmpz_set_mpz(*reinterpret_cast<fmpz_t*>(p_fmpz), numerator(rational).get_rep());
}

pm::Int new_int_from_rational(const pm::Rational& rational)
{
    if (!rational.is_integral() || isinf(rational)) throw pm::GMP::BadCast();
    return static_cast<pm::Int>(rational);
}

pm::Integer new_integer_from_rational(const pm::Rational& rational)
{
    if (!rational.is_integral()) throw pm::GMP::BadCast();
    return static_cast<pm::Integer>(rational);
}

pm::Rational new_rational_from_integer(const pm::Integer& integer)
{
    return static_cast<pm::Rational>(integer);
}

void new_baserational_from_rational(const pm::Rational& rational, void* num, void* den)
{
    if (isinf(rational)) throw pm::GMP::BadCast();
    mpz_set(*reinterpret_cast<mpz_t*>(num), numerator(rational).get_rep());
    mpz_set(*reinterpret_cast<mpz_t*>(den), denominator(rational).get_rep());
}

pm::Rational new_rational_from_baserational(const void* num, const void* den)
{
    return pm::Rational(*reinterpret_cast<const mpz_t*>(num), *reinterpret_cast<const mpz_t*>(den));
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
    jlpolymake.method("new_rational_from_fmpz", new_rational_from_fmpz);
    jlpolymake.method("new_fmpq_from_rational", new_fmpq_from_rational);
    jlpolymake.method("new_fmpz_from_rational", new_fmpz_from_rational);
    jlpolymake.method("new_int_from_rational", new_int_from_rational);
    jlpolymake.method("new_integer_from_rational", new_integer_from_rational);
    jlpolymake.method("new_rational_from_integer", new_rational_from_integer);
    jlpolymake.method("new_baserational_from_rational", new_baserational_from_rational);
    jlpolymake.method("new_rational_from_baserational", new_rational_from_baserational);
    jlpolymake.method("to_rational", [](pm::perl::PropertyValue pv) {
        return to_SmallObject<pm::Rational>(pv);
    });
}

}
