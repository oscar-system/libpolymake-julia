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

//fmpz_get_mpz

pm::Integer new_integer_from_fmpz(jl_value_t* integer)
{
    fmpz_t* z;
    z = reinterpret_cast<fmpz_t*>(integer);
    mpz_t z_mp;
    mpz_init(z_mp);
    fmpz_get_mpz(z_mp, *z);
    return pm::Integer(std::move(z_mp));
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
    jlpolymake.method("to_integer", [](pm::perl::PropertyValue pv) {
        return to_SmallObject<pm::Integer>(pv);
    });
}

}
