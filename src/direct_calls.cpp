#include "jlpolymake/jlpolymake.h"

#include "jlpolymake/type_modules.h"

namespace jlpolymake {

template<typename Scalar>
pm::Vector<Scalar> direct_call_solve_LP(
    const pm::Matrix<Scalar>& inequalities,
    const pm::Matrix<Scalar>& equalities,
    const pm::Vector<Scalar>& objective,
    bool                      maximize)
{
    try {
        auto solution = polymake::polytope::solve_LP(inequalities, equalities, objective, maximize);
        return solution.solution;
    } catch (...) {
        return pm::Vector<Scalar>();
    }
}


void add_direct_calls(jlcxx::Module& jlpolymake)
{
    jlpolymake.method("direct_call_solve_LP", &direct_call_solve_LP<pm::Rational>);
    jlpolymake.method("direct_call_solve_LP_float", &direct_call_solve_LP<double>);
}

}
