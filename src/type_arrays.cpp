#include "jlpolymake/jlpolymake.h"

#include "jlpolymake/tools.h"

#include "jlpolymake/functions.h"

#include "jlpolymake/type_modules.h"

namespace jlpolymake {

tparametric1 add_array(jlcxx::Module& jlpolymake)
{

    auto type = jlpolymake
        .add_type<jlcxx::Parametric<jlcxx::TypeVar<1>>>(
            "Array", jlcxx::julia_type("AbstractVector", "Base"));

        type.apply<pm::Array<pm::Int>, pm::Array<pm::Integer>,
               pm::Array<pm::Rational>,
               pm::Array<std::string>, pm::Array<pm::Set<pm::Int>>,
               pm::Array<pm::Array<pm::Int>>,
               pm::Array<pm::Array<pm::Integer>>,
               pm::Array<pm::Array<pm::Rational>>,
               pm::Array<std::pair<pm::Int, pm::Int>>,
               pm::Array<std::list<std::pair<pm::Int, pm::Int>>>,
               pm::Array<pm::Matrix<pm::Integer>>>([](auto wrapped) {
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
        })
        .apply<pm::Array<pm::perl::BigObject>>([](auto wrapped) {
            typedef typename decltype(wrapped)::type WrappedT;
            typedef pm::perl::BigObject elemType;

            wrapped.template constructor<int64_t>();

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
        });

    // this must be here instead of type_bigobject to have the array available
    jlpolymake.method("_lookup_multi", [](pm::perl::BigObject p, const std::string& name) -> pm::Array<pm::perl::BigObject> {
        return p.lookup_multi(name, All);
    });

    jlpolymake.method("to_array_int", [](const pm::perl::PropertyValue& pv) {
        return to_SmallObject<pm::Array<pm::Int>>(pv);
    });
    jlpolymake.method("to_array_integer",
                    [](const pm::perl::PropertyValue& pv) {
                        return to_SmallObject<pm::Array<pm::Integer>>(pv);
                    });
    jlpolymake.method("to_array_string", [](const pm::perl::PropertyValue& pv) {
        return to_SmallObject<pm::Array<std::string>>(pv);
    });
    jlpolymake.method("to_array_string",[](std::optional<pm::perl::ListResult>& l) {
        if (!l)
           throw std::runtime_error("ListResult can be unpacked only once.");
        pm::Array<std::string> arr;
        *l >> pm::perl::unroll(arr);
        l.reset();
        return arr;
    });

    jlpolymake.method("to_array_array_int",
                    [](const pm::perl::PropertyValue& pv) {
                        return to_SmallObject<pm::Array<pm::Array<pm::Int>>>(pv);
                    });
    jlpolymake.method(
        "to_array_array_integer", [](const pm::perl::PropertyValue& pv) {
            return to_SmallObject<pm::Array<pm::Array<pm::Integer>>>(pv);
        });
    jlpolymake.method(
        "to_array_array_rational", [](const pm::perl::PropertyValue& pv) {
            return to_SmallObject<pm::Array<pm::Array<pm::Rational>>>(pv);
        });
    jlpolymake.method(
        "to_array_set_int", [](const pm::perl::PropertyValue& pv) {
            return to_SmallObject<pm::Array<pm::Set<pm::Int>>>(pv);
        });
    jlpolymake.method(
        "to_array_pair_int_int", [](const pm::perl::PropertyValue& pv){
                return to_SmallObject<pm::Array<std::pair<pm::Int, pm::Int>>>(pv);
        });
    jlpolymake.method(
            "to_array_list_pair_int_int", [](const pm::perl::PropertyValue& pv){
                return to_SmallObject<pm::Array<std::list<std::pair<pm::Int, pm::Int>>>>(pv);
            });
    jlpolymake.method(
        "to_array_matrix_integer", [](const pm::perl::PropertyValue& pv) {
            return to_SmallObject<pm::Array<pm::Matrix<pm::Integer>>>(pv);
        });
    jlpolymake.method(
        "to_array_bigobject", [](const pm::perl::PropertyValue& pv) {
            return to_SmallObject<pm::Array<pm::perl::BigObject>>(pv);
        });
    return type;
}

}
