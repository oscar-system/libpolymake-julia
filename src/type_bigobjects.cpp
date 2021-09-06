#include "jlpolymake/jlpolymake.h"

#include "jlpolymake/tools.h"

#include "jlpolymake/functions.h"

#include "jlpolymake/type_modules.h"

#include "jlpolymake/caller.h"


namespace jlpolymake {

#include "jlpolymake/generated/option_set_take.h"

#include "jlpolymake/generated/call_function_feed_argument.h"

void add_bigobject(jlcxx::Module& jlpolymake)
{

    jlpolymake.add_type<pm::perl::PropertyValue>("PropertyValue");
    jlpolymake.add_type<pm::perl::ListResult>("ListResult_internal");
    jlpolymake.add_type<std::optional<pm::perl::ListResult>>("ListResult");
    jlpolymake.add_type<pm::perl::OptionSet>("OptionSet");

    jlpolymake.method("option_set_take", option_set_take);

    jlpolymake.add_type<pm::perl::BigObjectType>("BigObjectType")
        .constructor<const std::string&>()
        .method("type_name", [](pm::perl::BigObjectType p) { return p.name(); });

    jlpolymake.add_type<pm::perl::BigObject>("BigObject")
        .constructor<const pm::perl::BigObjectType&>()
        .constructor<const pm::perl::BigObjectType&, const pm::perl::BigObject&>()
        .method("save_bigobject",
                [](pm::perl::BigObject p, const std::string& s) {
                    return p.save(s);
                })
        .method(
            "load_bigobject",
            [](const std::string& s) { return pm::perl::BigObject::load(s); })
        .method("internal_give",
                [](pm::perl::BigObject p, const std::string& s) {
                    return p.give(s);
                })
        .method("exists", [](pm::perl::BigObject& p,
                             const std::string& s) { return p.exists(s); })
        .method("_isa", [](const pm::perl::BigObject& p,
                          const pm::perl::BigObjectType& t) { return p.isa(t); })
        .method("cast!", [](pm::perl::BigObject& p,
                            const pm::perl::BigObjectType& t) { return p.cast(t); })
        .method("bigobject_type", [](pm::perl::BigObject p) { return p.type(); })
        .method("type_name",
                [](pm::perl::BigObject p) { return p.type().name(); })
        .method("properties", [](pm::perl::BigObject p) {
            std::string x = p.call_method("properties");
            return x;
        })
        .method("_get_attachment", [](pm::perl::BigObject p, const std::string& s) {
            return p.get_attachment(s);
        })
        .method("remove_attachment", [](pm::perl::BigObject p, const std::string& s) {
            return p.remove_attachment(s);
        })
        .method("_lookup_multi", [](const pm::perl::BigObject& p, const std::string& name, const std::string subobj_name) {
            auto obj = p.lookup_multi(name, subobj_name);
            if (!obj.valid())
                throw std::runtime_error("BigObject: no such subobject");
            return obj;
        })
        .method("_lookup_multi", [](const pm::perl::BigObject& p, const std::string& name, int64_t subobj_index) {
            auto arr = p.lookup_multi(name, All);
            if (subobj_index < 0 || subobj_index >= arr.size())
                throw std::runtime_error("BigObject: no such subobject");
            return BigObject(arr[subobj_index]);
        })
        .method("attach", [](pm::perl::BigObject p, const std::string& s,
                              jl_value_t* v) {
            auto pv_helper = p.attach(s);
            call_function_feed_argument(pv_helper, v);
        })
        ;

    jlpolymake.method("to_bool", [](pm::perl::PropertyValue p) {
        return static_cast<bool>(p);
    });
    jlpolymake.method("to_int", [](pm::perl::PropertyValue p) {
        return static_cast<pm::Int>(p);
    });
    jlpolymake.method("to_double", [](pm::perl::PropertyValue p) {
        return static_cast<double>(p);
    });
    jlpolymake.method("to_string", [](pm::perl::PropertyValue p) {
        return to_SmallObject<std::string>(p);
    });
    jlpolymake.method("to_bigobject", &to_bigobject);

    jlpolymake.method("setname!", [](pm::perl::BigObject p, const std::string& s){
        p.set_name(s);
    });
    jlpolymake.method("take", [](pm::perl::BigObject p, const std::string& s,
                               const std::string& t) { p.take(s) << t; });
    jlpolymake.method("take",
                    [](pm::perl::BigObject p, const std::string& s,
                       const pm::perl::PropertyValue& v) { p.take(s) << v; });
    jlpolymake.method("take",
                    [](pm::perl::BigObject p, const std::string& s,
                       const pm::perl::BigObject& v) { p.take(s) << v; });
    jlpolymake.method("add", [](pm::perl::BigObject p, const std::string& s,
                              const pm::perl::BigObject& v) { p.add(s, v); });
    jlpolymake.method("add", [](pm::perl::BigObject p, const std::string& name,
                              const std::string& sub_name, pm::perl::BigObject& v) 
                              { 
                                 v.set_name(sub_name);
                                 p.add(name, v); });

    jlpolymake.method("typeinfo_string",
                    [](pm::perl::PropertyValue p, bool demangle) {
                        return typeinfo_helper(p, demangle);
                    });
    jlpolymake.method("check_defined", [](pm::perl::PropertyValue v) {
        return PropertyValueHelper(v).is_defined();
    });
}

}
