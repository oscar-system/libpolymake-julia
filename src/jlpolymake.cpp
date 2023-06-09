#include "jlpolymake/jlpolymake.h"

#include "jlpolymake/tools.h"

#include "jlpolymake/functions.h"

#include "jlpolymake/caller.h"

#include "jlpolymake/type_modules.h"

#include "jlpolymake/containers.h"


namespace jlpolymake {

#include "jlpolymake/generated/type_declarations.h"

Polymake_Data data{nullptr, nullptr};

}

// to allow calling this before init
extern "C"
{

// build-time libcxxwrap-julia version
// use CxxWrap.libcxxwrapversion() for run-time library version
const char* jlpolymake_libcxxwrap_build_version()
{
   return JLCXX_VERSION_STRING;
}

const char* jlpolymake_version()
{
   return JLPOLYMAKE_VERSION_STRING;
}

}

using namespace jlpolymake;

JLCXX_MODULE define_module_polymake(jlcxx::Module& jlpolymake)
{
    add_bigobject(jlpolymake);

    add_integer(jlpolymake);

    add_rational(jlpolymake);
    
    add_tropicalnumber(jlpolymake);

    add_quadraticextension(jlpolymake);

    prepare_containers(jlpolymake);

    wrap_array_for_types<VecOrMat_supported::value_type>(jlpolymake);

    // needed for sparse containers
    add_set(jlpolymake);
    wrap_array<pm::Set<pm::Int>>(jlpolymake);

    wrap_vector_for_types<VecOrMat_supported::value_type>(jlpolymake);
    wrap_matrix_for_types<VecOrMat_supported::value_type>(jlpolymake);
    wrap_sparsevector_for_types<VecOrMat_supported::value_type>(jlpolymake);
    wrap_sparsematrix_for_types<VecOrMat_supported::value_type>(jlpolymake);

    wrap_array_for_types<TropicalNumberTypes::value_type>(jlpolymake);
    wrap_vector_for_types<TropicalNumberTypes::value_type>(jlpolymake);
    wrap_matrix_for_types<TropicalNumberTypes::value_type>(jlpolymake);
    wrap_sparsevector_for_types<TropicalNumberTypes::value_type>(jlpolymake);
    wrap_sparsematrix_for_types<TropicalNumberTypes::value_type>(jlpolymake);

    wrap_pair<pm::Int, pm::Int>(jlpolymake);
    wrap_pair<pm::Integer, pm::Int>(jlpolymake);

    add_lists(jlpolymake);

    add_graph(jlpolymake);

    add_homologygroup(jlpolymake);

    add_array(jlpolymake);

    wrap_pair<pm::Int, std::list<std::list<std::pair<pm::Int, pm::Int>>>>(jlpolymake);
    wrap_pair<pm::Array<pm::Int>, pm::Array<pm::Int>>(jlpolymake);

    add_edgemap(jlpolymake);
    add_nodemap(jlpolymake);

    add_incidencematrix(jlpolymake);

    add_polynomial(jlpolymake);
    add_unipolynomial(jlpolymake);

    wrap_array<pm::Polynomial<pm::Rational,long>>(jlpolymake);
    wrap_array<pm::Polynomial<pm::Integer,long>>(jlpolymake);
    wrap_array<std::pair<pm::Array<pm::Int>, pm::Array<pm::Int>>>(jlpolymake);
    
    add_direct_calls(jlpolymake);

    auto matrix_type = jlcxx::TypeWrapper1(jlpolymake, pmwrappers::instance().pmmatrix);
    auto vector_type = jlcxx::TypeWrapper1(jlpolymake, pmwrappers::instance().pmvector);

    add_matrix_extended(jlpolymake, matrix_type);
    add_vector_extended(jlpolymake, vector_type);

    add_map(jlpolymake);

    add_unbox_pv(jlpolymake);
    
    // this must be here instead of type_bigobject to have the array available
    jlpolymake.method("_lookup_multi", [](pm::perl::BigObject p, const std::string& name) -> pm::Array<pm::perl::BigObject> {
        return p.lookup_multi(name, All);
    });

    jlpolymake.method("_get_global_epsilon", []() { return pm::spec_object_traits<double>::global_epsilon; });
    jlpolymake.method("_set_global_epsilon", [](double e) { pm::spec_object_traits<double>::global_epsilon = e; });

    jlpolymake.method("initialize_polymake", &initialize_polymake);
    jlpolymake.method("initialize_polymake_with_dir", &initialize_polymake_with_dir);
    jlpolymake.method("initialize_polymake_with_dir", &initialize_polymake_with_dir_and_prefix);
    jlpolymake.method("application", [](const std::string x) {
        data.main_polymake_session->set_application(x);
    });

    jlpolymake.method("_shell_execute", [](const std::string x) {
        return data.main_polymake_session->shell_execute(x);
    });

    jlpolymake.method("shell_complete", [](const std::string x) {
        auto res = data.main_polymake_session->shell_complete(x);
        return std::tuple<int64_t, std::vector<std::string>>{
            std::get<0>(res),
            std::get<2>(res)
        };
    });

    jlpolymake.method("shell_context_help", [](
        const std::string& input,
        size_t pos=std::string::npos,
        bool full=false,
        bool html=false){
        std::vector<std::string> ctx_help =
            data.main_polymake_session->shell_context_help(input, pos, full, html);
        return ctx_help;
    });

    jlpolymake.add_type<polymake::Scope>("Scope_internal");
    jlpolymake.add_type<std::optional<polymake::Scope>>("Scope");

    jlpolymake.method("set_preference", [](const std::string x) {
        return data.main_polymake_session->set_preference(x);
    });

    jlpolymake.method("scope_begin", []() {
        return std::optional<polymake::Scope>(
                // allocate new scope object
                std::move(data.main_polymake_session->newScope())
            );
    });
    jlpolymake.method("scope_end", [](std::optional<polymake::Scope>& scope) {
        // destroy scope object to force immediate reset of any temporary changes
        scope.reset();
    });
    jlpolymake.method("internal_prefer_now", [](const std::optional<polymake::Scope>& scope, const std::string& label) {
        if (!scope)
           throw std::runtime_error("attempt to use polymake::Scope after destruction");
        scope->prefer_now(label);
    });

#include "jlpolymake/generated/map_inserts.h"

    add_caller(jlpolymake);

    add_type_translations(jlpolymake);

}
