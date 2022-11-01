#include "jlpolymake/jlpolymake.h"

#include "jlpolymake/tools.h"

#include "jlpolymake/functions.h"

#include "jlpolymake/type_modules.h"

namespace jlpolymake {

pm::Integer new_integer_from_bigint(jl_value_t* integer)
{
    pm::Integer* p;
    p = reinterpret_cast<pm::Integer*>(integer);
    return *p;
}

pm::Integer new_integer_from_fmpz(jl_value_t* integer)
{
    mpz_t z_mp;
    mpz_init(z_mp);
    fmpz_get_mpz(z_mp, *reinterpret_cast<fmpz_t*>(integer));
    return pm::Integer(std::move(z_mp));
}

void new_fmpz_from_integer(const pm::Integer& integer, void* p_fmpz)
{
    if (isinf(integer)) throw pm::GMP::BadCast();
    fmpz_set_mpz(*reinterpret_cast<fmpz_t*>(p_fmpz), integer.get_rep());
}

void new_fmpq_from_integer(const pm::Integer& integer, void* p_fmpq)
{
    if (isinf(integer)) throw pm::GMP::BadCast();
    fmpz_t z_fmp, z_one;
    fmpz_init(z_fmp);
    fmpz_set_mpz(z_fmp, integer.get_rep());
    fmpz_init(z_one);
    fmpz_one(z_one);
    fmpq_set_fmpz_frac(*reinterpret_cast<fmpq_t*>(p_fmpq), z_fmp, z_one);
    fmpz_clear(z_fmp);
    fmpz_clear(z_one);
}

pm::Integer new_integer_from_fmpq(jl_value_t* rational)
{
    mpz_t z_one, z_mp;
    mpz_init(z_mp);
    mpz_init(z_one);
    fmpq_get_mpz_frac(z_mp, z_one, *reinterpret_cast<fmpq_t*>(rational));
    if (mpz_cmp_si(z_one, 1) != 0)
        throw pm::GMP::BadCast("non-integral number");
    mpz_clear(z_one);
    return pm::Integer(std::move(z_mp));
}

pm::Int new_int_from_integer(const pm::Integer& integer)
{
    if (isinf(integer)) throw pm::GMP::BadCast();
    return static_cast<pm::Int>(integer);
}

void add_integer(jlcxx::Module& jlpolymake)
{
    jlpolymake
        .add_type<pm::Integer>("Integer",
                               jlcxx::julia_type("Integer", "Base"))
        .constructor<int64_t>()
        .method("<", [](pm::Integer& a, pm::Integer& b) { return a < b; })
        .method("<", [](pm::Integer& a,
                        int64_t      b) { return a < static_cast<pm::Int>(b); })
        .method("<", [](int64_t      a,
                        pm::Integer& b) { return static_cast<pm::Int>(a) < b; })
        .method("<=", [](pm::Integer& a, pm::Integer& b) { return a <= b; })
        .method("<=", [](pm::Integer& a,
                         int64_t b) { return a <= static_cast<pm::Int>(b); })
        .method("<=",
                [](int64_t a, pm::Integer& b) {
                    return static_cast<pm::Int>(a) <= b;
                })

        .method("show_small_obj",
                [](pm::Integer& i) {
                    return show_small_object<pm::Integer>(i, false);
                })
        .method("Float64", [](pm::Integer& a) { return double(a); })
        .method("-", [](pm::Integer& a, pm::Integer& b) { return a - b; })
        .method("-", [](pm::Integer& a,
                        int64_t      b) { return a - static_cast<pm::Int>(b); })
        .method("-", [](int64_t      a,
                        pm::Integer& b) { return static_cast<pm::Int>(a) - b; })
        // unary minus
        .method("-", [](pm::Integer& a) { return -a; })

        .method("div", [](pm::Integer& a, pm::Integer& b) { return a / b; })
        .method("div", [](pm::Integer& a,
                          int64_t b) { return a / static_cast<pm::Int>(b); })
        .method("div",
                [](int64_t a, pm::Integer& b) {
                    return static_cast<pm::Int>(a) / b;
                })

        .method("rem", [](pm::Integer& a, pm::Integer& b) { return a % b; })
        .method("rem", [](pm::Integer& a,
                          int64_t b) { return a % static_cast<pm::Int>(b); })
        .method("rem",
                [](int64_t a, pm::Integer& b) {
                    return static_cast<pm::Int>(a) % b;
                });

        jlpolymake.set_override_module(jlpolymake.julia_module());
        jlpolymake.method("==", [](pm::Integer& a, pm::Integer& b) {
            return a == b; });
        jlpolymake.method("==", [](pm::Integer& a, int64_t b) {
            return a == static_cast<pm::Int>(b); });
        jlpolymake.method("==", [](int64_t a, pm::Integer& b) {
            return static_cast<pm::Int>(a) == b; });

        // the symmetric definitions are on the julia side
        jlpolymake.method("+", [](pm::Integer& a, pm::Integer& b) {
            return a + b; });
        jlpolymake.method("+", [](pm::Integer& a, int64_t b) {
            return a + static_cast<pm::Int>(b); });
        jlpolymake.method("+", [](int64_t a, pm::Integer& b) {
            return static_cast<pm::Int>(a) + b; });
        jlpolymake.method("*", [](pm::Integer& a, pm::Integer& b) {
            return a * b; });
        jlpolymake.method("*", [](pm::Integer& a, int64_t b) {
            return a * static_cast<pm::Int>(b); });
        jlpolymake.method("*", [](int64_t a, pm::Integer& b) {
            return static_cast<pm::Int>(a) * b; });
        jlpolymake.unset_override_module();

    jlpolymake.method("new_integer_from_bigint", new_integer_from_bigint);
    jlpolymake.method("new_integer_from_fmpz", new_integer_from_fmpz);
    jlpolymake.method("new_fmpz_from_integer", new_fmpz_from_integer);
    jlpolymake.method("new_fmpq_from_integer", new_fmpq_from_integer);
    jlpolymake.method("new_integer_from_fmpq", new_integer_from_fmpq);
    jlpolymake.method("new_int_from_integer", new_int_from_integer);
    jlpolymake.method("to_integer", [](pm::perl::PropertyValue pv) {
        return to_SmallObject<pm::Integer>(pv);
    });
}

}
