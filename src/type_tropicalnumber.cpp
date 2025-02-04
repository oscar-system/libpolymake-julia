#include "jlpolymake/jlpolymake.h"

#include "jlpolymake/tools.h"

#include "jlpolymake/functions.h"

#include "jlpolymake/type_modules.h"

namespace jlpolymake {

void add_tropicalnumber(jlcxx::Module& jlpolymake)
{
    jlpolymake.add_type<pm::Max>("Max");
    jlpolymake.add_type<pm::Min>("Min");

    jlpolymake
        .add_type<jlcxx::Parametric<jlcxx::TypeVar<1>, jlcxx::TypeVar<2>>>(
            "TropicalNumber", jlcxx::julia_type("Number", "Base"))
            .apply_combination<pm::TropicalNumber,
            jlcxx::ParameterList<pm::Min,pm::Max>,
                jlcxx::ParameterList<pm::Rational>>(
                [](auto wrapped) {
                    typedef typename decltype(wrapped)::type tropType;
                    wrapped.template constructor<const pm::Rational&>();
                    wrapped.template constructor<const tropType&>();
                    wrapped.method("zero", [](const tropType& a) { return a.zero(); });
                    wrapped.method("dual_zero", [](const tropType& a) { return a.dual_zero(); });
                    wrapped.method("one", [](const tropType& a) { return a.one(); });
                    wrapped.method("_add", [](const tropType& a, const tropType& b) { return a + b; });
                    wrapped.method("_mul", [](const tropType& a, const tropType& b) { return a * b; });
                    wrapped.method("//", [](const tropType& a, const tropType& b) { return a / b; });
                    wrapped.method("_isequal", [](const tropType& a,
                            const tropType& b) { return a == b; });
                    wrapped.method("<", [](const tropType& a,
                            const tropType& b) { return a < b; });
                    wrapped.method("scalar", [](const tropType& a) { return pm::Rational(a); });
                    wrapped.method("show_small_obj", [](const tropType& S) {
                        return show_small_object<tropType>(S);
                    });
                    wrapped.method("take", [](pm::perl::BigObject& p, const std::string& s,
                            const tropType& v) { p.take(s) << v; });
            });
}

}
