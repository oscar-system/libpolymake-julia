#include "jlpolymake/jlpolymake.h"

#include "jlpolymake/tools.h"

#include "jlpolymake/functions.h"

#include "jlpolymake/type_modules.h"

namespace jlpolymake {

void add_puiseuxfraction(jlcxx::Module& jlpolymake)
{

   using unipoly = pm::UniPolynomial<Rational,Rational>;
    jlpolymake
        .add_type<jlcxx::Parametric<jlcxx::TypeVar<1>,jlcxx::TypeVar<2>,jlcxx::TypeVar<3>>>("PuiseuxFraction")
        .apply_combination<pm::PuiseuxFraction, jlcxx::ParameterList<pm::Min,pm::Max>, jlcxx::ParameterList<pm::Rational>, jlcxx::ParameterList<pm::Rational>>([&jlpolymake](auto wrapped){
              using pf = typename decltype(wrapped)::type;
        wrapped.template constructor<const unipoly&, const unipoly&>();
        wrapped.template constructor<const unipoly&>();
        wrapped.template constructor<const Rational&>();

        wrapped.method(
            "numerator",
            [](const pf& r) { return unipoly(numerator(r)); });
        wrapped.method(
            "denominator",
            [](const pf& r) { return unipoly(denominator(r)); });
        wrapped.method("show_small_obj",
                [](const pf& r) {
                    return show_small_object<pf>(r, true);
                });
        wrapped.method("take",
                [](pm::perl::BigObject& p, const std::string& s, const pf& r) {
                   p.take(s) << r;
                });
        jlpolymake.set_override_module(jlpolymake.julia_module());
        wrapped.method("<", [](const pf& a, const pf& b) { return a < b; });
        wrapped.method("<", [](const pm::Rational& a, const pf& b) { return a < b; });
        wrapped.method("<", [](const pf& a, const pm::Rational& b) { return a < b; });
        wrapped.method("<=", [](const pf& a, const pf& b) { return a <= b; });
        wrapped.method("<=", [](const pf& a, const pm::Rational& b) { return a <= b; });
        wrapped.method("<=", [](const pm::Rational& a, const pf& b) { return a <= b; });
        wrapped.method("-", [](const pf& a, const pf& b) { return a - b; });
        wrapped.method("-", [](const pm::Rational& a, const pf& b) { return a - b; });
        wrapped.method("-", [](const pf& a, const pm::Rational& b) { return a - b; });
        // unary minus
        wrapped.method("-", [](const pf& a) { return -a; });

        wrapped.method("//", [](const pf& a, const pf& b) { return a / b; });

        wrapped.method("==", [](const pf& a, const pf& b) {
            return a == b; });
        wrapped.method("==", [](const pf& a, const pm::Rational& b) {
            return a == b; });
        wrapped.method("==", [](const pm::Rational& a, const pf& b) {
            return a == b; });
        // the symmetric definitions are on the julia side
        wrapped.method("+", [](const pf& a, const pf& b) {
            return a + b; });
        wrapped.method("+", [](const pf& a, const pm::Rational& b) {
            return a + b; });
        wrapped.method("+", [](const pm::Rational& a, const pf& b) {
            return a + b; });
        wrapped.method("*", [](const pf& a, const pf& b) {
            return a * b; });
        wrapped.method("*", [](const pf& a, const pm::Rational& b) {
            return a * b; });
        wrapped.method("*", [](const pm::Rational& a, const pf& b) {
            return a * b; });
        jlpolymake.unset_override_module();
        wrapped.method("cmp", [](const pf& a, const pf& b) {
            return a.compare(b); });
        wrapped.method("val", [](const pf& a) {
              return a.val();
              });
        wrapped.method("get_hash", [](const pf& a) {
              return a.get_hash();
              });
        wrapped.method("set_var_names", [](const pf& a, const Array<std::string>& names) { a.set_var_names(names); });
        wrapped.method("get_var_names", [](const pf& a) { return a.get_var_names(); });
      });

}

}
