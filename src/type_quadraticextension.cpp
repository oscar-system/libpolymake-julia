#include "jlpolymake/jlpolymake.h"

#include "jlpolymake/tools.h"

#include "jlpolymake/functions.h"

#include "jlpolymake/type_modules.h"

namespace jlpolymake {

void add_quadraticextension(jlcxx::Module& jlpolymake)
{

    jlpolymake
        .add_type<jlcxx::Parametric<jlcxx::TypeVar<1>>>(
            "QuadraticExtension", jlcxx::julia_type("Real", "Base"))
        .apply<pm::QuadraticExtension<pm::Rational>>([](auto wrapped) {
            typedef typename decltype(wrapped)::type             WrappedT;
            typedef typename decltype(wrapped)::type::field_type fieldType;

            wrapped.template constructor<fieldType, fieldType, fieldType>();
            
            wrapped.method("<", [](const WrappedT& a, const WrappedT& b) { return a < b; });
            wrapped.method("==", [](const WrappedT& a, const WrappedT& b) { return a == b; });
            
            wrapped.method("-", [](const WrappedT& a) { return -a; });
            
            wrapped.method("+", [](const WrappedT& a, const WrappedT& b) { return a + b; });
            wrapped.method("-", [](const WrappedT& a, const WrappedT& b) { return a - b; });
            wrapped.method("*", [](const WrappedT& a, const WrappedT& b) { return a * b; });
            wrapped.method("//", [](const WrappedT& a, const WrappedT& b) { return a / b; });
            
            wrapped.method("_a", [](const WrappedT& a) { return a.a(); });
            wrapped.method("_b", [](const WrappedT& a) { return a.b(); });
            wrapped.method("_r", [](const WrappedT& a) { return a.r(); });
            
            wrapped.method("conjugate", [](const WrappedT& a) { return conjugate(a); });
            
            wrapped.method("show_small_obj", [](const WrappedT& S) {
                return show_small_object<WrappedT>(S);
            });
            wrapped.method("take",
                [](pm::perl::BigObject& p, const std::string& s, const WrappedT& q) {
                   p.take(s) << q;
                });
        });

}

}
