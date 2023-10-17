#include "jlpolymake/jlpolymake.h"

#include "jlpolymake/tools.h"

#include "jlpolymake/functions.h"

#include "jlpolymake/type_modules.h"

namespace jlpolymake {

void add_switchtable(jlcxx::Module& jlpolymake)
{
    using ST = polymake::group::SwitchTable;
    jlpolymake.add_type<ST>("SwitchTable")
       .constructor<const pm::Array<pm::Array<Int>>&>()
       .method("lex_maximize_vector", [](const ST& st, const Vector<Int>& v) { return st.lex_maximize_vector(v); })
       .method("lex_maximize_vector", [](const ST& st, const Vector<Integer>& v) { return st.lex_maximize_vector(v); })
       .method("lex_maximize_vector", [](const ST& st, const Vector<Rational>& v) { return st.lex_maximize_vector(v); })
       .method("lex_minimize_vector", [](const ST& st, const Vector<Int>& v) { return st.lex_minimize_vector(v); })
       .method("lex_minimize_vector", [](const ST& st, const Vector<Integer>& v) { return st.lex_minimize_vector(v); })
       .method("lex_minimize_vector", [](const ST& st, const Vector<Rational>& v) { return st.lex_minimize_vector(v); })
            
       .method("show_small_obj", [](const ST& st) {
                return show_small_object<ST>(st);
            });

}

}
