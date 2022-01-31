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
            
            wrapped.method("<", [](WrappedT& a, WrappedT& b) { return a < b; });
            wrapped.method("==", [](WrappedT& a, WrappedT& b) { return a == b; });
            
            wrapped.method("-", [](WrappedT& a) { return -a; });
            
            wrapped.method("+", [](WrappedT& a, WrappedT& b) { return a + b; });
            wrapped.method("-", [](WrappedT& a, WrappedT& b) { return a - b; });
            wrapped.method("*", [](WrappedT& a, WrappedT& b) { return a * b; });
            wrapped.method("//", [](WrappedT& a, WrappedT& b) { return a / b; });
            
            wrapped.method("_a", [](WrappedT& a) { return a.a(); });
            wrapped.method("_b", [](WrappedT& a) { return a.b(); });
            wrapped.method("_r", [](WrappedT& a) { return a.r(); });
            
            wrapped.method("conjugate", [](WrappedT& a) { return conjugate(a); });
            
            wrapped.method("show_small_obj", [](WrappedT& S) {
                return show_small_object<WrappedT>(S);
            });
        });

    jlpolymake.method("to_quadraticextension_rational", [](pm::perl::PropertyValue v) {
        return to_SmallObject<pm::QuadraticExtension<pm::Rational>>(v);
    });

}

}
