#!/usr/bin/env perl

use strict;
use warnings;
use File::Basename qw(dirname);
use File::Path qw(make_path);

my $type_tuples = [
        ["PropertyValue", "pm::perl::PropertyValue", "PropertyValue", undef],
        # ListResult cannot be used like other scalar based types so this is
        # commented on purpose, also as a reminder
        # ["ListResult", "pm::perl::ListResult", "ListResult", undef],
        ["OptionSet", "pm::perl::OptionSet", "OptionSet", undef],
        ["BigObject", "pm::perl::BigObject", "BigObject", "to_bigobject"],
        ["Integer", "pm::Integer", "Integer", "to_integer"],
        ["Rational", "pm::Rational", "Rational", "to_rational"],
        [
            "QuadraticExtension_Rational",
            "pm::QuadraticExtension<pm::Rational>",
            "QuadraticExtension{Rational}",
            "to_quadraticextension_rational",
        ],
        [
            "Matrix_Polynomial_Rational_Int",
            "pm::Matrix<pm::Polynomial<pm::Rational,long>>",
            "Matrix{Polynomial{Rational,CxxWrap.CxxLong}}",
            "to_matrix_polynomial_rational_int",
        ],
        [
            "Vector_Polynomial_Rational_Int",
            "pm::Vector<pm::Polynomial<pm::Rational,long>>",
            "Vector{Polynomial{Rational,CxxWrap.CxxLong}}",
            "to_vector_polynomial_rational_int",
        ],
        [
            "Pair_Int_Int",
            "std::pair<pm::Int, pm::Int>",
            "StdPair{CxxWrap.CxxLong,CxxWrap.CxxLong}",
            "to_pair_int_int",
        ],
        [
            "Pair_Array_Int_Array_Int",
            "std::pair<pm::Array<pm::Int>, pm::Array<pm::Int>>",
            "StdPair{Array{CxxWrap.CxxLong},Array{CxxWrap.CxxLong}}",
            "to_pair_array_int_array_int",
        ],
        [
            "Array_Pair_Array_Int_Array_Int",
            "pm::Array<std::pair<pm::Array<pm::Int>, pm::Array<pm::Int>>>",
            "Array{StdPair{Array{CxxWrap.CxxLong},Array{CxxWrap.CxxLong}}}",
            "to_array_pair_array_int_array_int",
        ],
        [
            "Pair_Int_List_List_Pair_Int_Int",
            "std::pair<pm::Int, std::list<std::list<std::pair<pm::Int, pm::Int>>>>",
            "StdPair{CxxWrap.CxxLong, StdList{StdList{StdPair{CxxWrap.CxxLong,CxxWrap.CxxLong}}}}",
            "to_pair_int_list_list_pair_int_int",
        ],
        [
            "Pair_Integer_Int",
            "std::pair<pm::Integer, pm::Int>",
            "StdPair{Integer,CxxWrap.CxxLong}",
            "to_pair_integer_int",
        ],
        [
            "List_Pair_Int_Int",
            "std::list<std::pair<pm::Int, pm::Int>>",
            "StdList{StdPair{CxxWrap.CxxLong,CxxWrap.CxxLong}}",
            "to_list_pair_int_int",
        ],
        [
            "List_List_Pair_Int_Int",
            "std::list<std::list<std::pair<pm::Int, pm::Int>>>",
            "StdList{StdList{StdPair{CxxWrap.CxxLong,CxxWrap.CxxLong}}}",
            "to_list_list_pair_int_int",
        ],
        [
            "List_Pair_Integer_Int",
            "std::list<std::pair<pm::Integer, pm::Int>>",
            "StdList{StdPair{Integer,CxxWrap.CxxLong}}",
            "to_list_pair_integer_int",
        ],
        ["Set_Int", "pm::Set<long>", "Set{CxxWrap.CxxLong}", "to_set_int"],
        [
            "Array_String",
            "pm::Array<std::string>",
            "Array{CxxWrap.StdString}",
            "to_array_string",
        ],
        [
            "Array_Set_Int",
            "pm::Array<pm::Set<long>>",
            "Array{Set{CxxWrap.CxxLong}}",
            "to_array_set_int",
        ],
        [
            "Array_Array_Int",
            "pm::Array<pm::Array<long>>",
            "Array{Array{CxxWrap.CxxLong}}",
            "to_array_array_int",
        ],
        [
            "Array_Array_Integer",
            "pm::Array<pm::Array<pm::Integer>>",
            "Array{Array{Integer}}",
            "to_array_array_integer",
        ],
        [
            "Array_Array_Set_Int",
            "pm::Array<pm::Array<pm::Set<long>>>",
            "Array{Array{Set{CxxWrap.CxxLong}}}",
            "to_array_array_set_int",
        ],
        [
            "Array_Array_Rational",
            "pm::Array<pm::Array<pm::Rational>>",
            "Array{Array{Rational}}",
            "to_array_array_rational",
        ],
        [
            "Array_Pair_Int_Int",
            "pm::Array<std::pair<pm::Int, pm::Int>>",
            "Array{StdPair{CxxWrap.CxxLong,CxxWrap.CxxLong}}",
            "to_array_pair_int_int",
        ],
        [
            "Array_List_Pair_Int_Int",
            "pm::Array<std::list<std::pair<pm::Int, pm::Int>>>",
            "Array{StdList{StdPair{CxxWrap.CxxLong,CxxWrap.CxxLong}}}",
            "to_array_list_pair_int_int",
        ],
        [
            "Array_Matrix_Integer",
            "pm::Array<pm::Matrix<pm::Integer>>",
            "Array{Matrix{Integer}}",
            "to_array_matrix_integer",
        ],
        [
            "Array_HomologyGroup_Integer",
            "pm::Array<polymake::topaz::HomologyGroup<pm::Integer>>",
            "Array{HomologyGroup{Integer}}",
            "to_array_homologygroup_integer",
        ],
        [
            "Array_BigObject",
            "pm::Array<pm::perl::BigObject>",
            "Array{BigObject}",
            "to_array_bigobject",
        ],
        [
            "Array_Polynomial_Integer_Int",
            "pm::Array<pm::Polynomial<pm::Integer,long>>",
            "Array{Polynomial{Integer,CxxWrap.CxxLong}}",
            "to_array_polynomial_integer_int",
        ],
        [
            "Array_Polynomial_Rational_Int",
            "pm::Array<pm::Polynomial<pm::Rational,long>>",
            "Array{Polynomial{Rational,CxxWrap.CxxLong}}",
            "to_array_polynomial_rational_int",
        ],
        [
            "IncidenceMatrix_NonSymmetric",
            "pm::IncidenceMatrix<pm::NonSymmetric>",
            "IncidenceMatrix{NonSymmetric}",
            "to_incidencematrix_nonsymmetric",
        ],
        [
            "IncidenceMatrix_Symmetric",
            "pm::IncidenceMatrix<pm::Symmetric>",
            "IncidenceMatrix{Symmetric}",
            "to_incidencematrix_symmetric",
        ],
        [
            "TropicalNumber_Max_Rational",
            "pm::TropicalNumber<pm::Max,pm::Rational>",
            "TropicalNumber{Max,Rational}",
            "to_tropicalnumber_max_rational",
        ],
        [
            "TropicalNumber_Min_Rational",
            "pm::TropicalNumber<pm::Min,pm::Rational>",
            "TropicalNumber{Min,Rational}",
            "to_tropicalnumber_min_rational",
        ],
        # [
        #     "TropicalNumber_Max_Integer",
        #     "pm::TropicalNumber<pm::Max,pm::Integer>",
        #     "TropicalNumber{Max,Integer}",
        #     "to_tropicalnumber_max_Integer",
        # ],
        # [
        #     "TropicalNumber_Min_Integer",
        #     "pm::TropicalNumber<pm::Min,pm::Integer>",
        #     "TropicalNumber{Min,Integer}",
        #     "to_tropicalnumber_min_Integer",
        # ],
        [
            "UniPolynomial_Int_Int",
            "pm::UniPolynomial<long,long>",
            "UniPolynomial{CxxWrap.CxxLong,CxxWrap.CxxLong}",
            "to_unipolynomial_int_int",
        ],
        [
            "UniPolynomial_Integer_Int",
            "pm::UniPolynomial<pm::Integer,long>",
            "UniPolynomial{Integer,CxxWrap.CxxLong}",
            "to_unipolynomial_integer_int",
        ],
        [
            "UniPolynomial_Rational_Int",
            "pm::UniPolynomial<pm::Rational,long>",
            "UniPolynomial{Rational,CxxWrap.CxxLong}",
            "to_unipolynomial_rational_int",
        ],
        [
            "UniPolynomial_QuadraticExtension_Rational_Int",
            "pm::UniPolynomial<pm::QuadraticExtension<pm::Rational>,long>",
            "UniPolynomial{QuadraticExtension{Rational},CxxWrap.CxxLong}",
            "to_unipolynomial_quadraticextension_rational_int",
        ],
        [
            "Polynomial_Int_Int",
            "pm::Polynomial<long,long>",
            "Polynomial{CxxWrap.CxxLong,CxxWrap.CxxLong}",
            "to_polynomial_int_int",
        ],
        [
            "Polynomial_Integer_Int",
            "pm::Polynomial<pm::Integer,long>",
            "Polynomial{Integer,CxxWrap.CxxLong}",
            "to_polynomial_integer_int",
        ],
        [
            "Polynomial_Rational_Int",
            "pm::Polynomial<pm::Rational,long>",
            "Polynomial{Rational,CxxWrap.CxxLong}",
            "to_polynomial_rational_int",
        ],
        [
            "Polynomial_double_Int",
            "pm::Polynomial<double,long>",
            "Polynomial{Float64,CxxWrap.CxxLong}",
            "to_polynomial_double_int",
        ],
        [
            "Polynomial_QuadraticExtension_Rational_Int",
            "pm::Polynomial<pm::QuadraticExtension<pm::Rational>,long>",
            "Polynomial{QuadraticExtension{Rational},CxxWrap.CxxLong}",
            "to_polynomial_quadraticextension_rational_int",
        ],
        [
            "Map_String_String",
            "pm::Map<std::string,std::string>",
            "Map{CxxWrap.StdString,CxxWrap.StdString}",
            "to_map_string_string",
        ],
        [
            "HomologyGroup_Integer",
            "polymake::topaz::HomologyGroup<pm::Integer>",
            "HomologyGroup{Integer}",
            "to_homologygroup_integer",
        ],
        [
            "Graph_Undirected",
            "pm::graph::Graph<pm::graph::Undirected>",
            "Graph{Undirected}",
            "to_graph_undirected",
        ],
        [
            "Graph_Directed",
            "pm::graph::Graph<pm::graph::Directed>",
            "Graph{Directed}",
            "to_graph_directed",
        ],
        [
            "EdgeMap_Directed_Int",
            "pm::graph::EdgeMap<pm::graph::Directed, pm::Int>",
            "EdgeMap{Directed, CxxWrap.CxxLong}",
            "to_edgemap_directed_int",
        ],
        [
            "EdgeMap_Undirected_Int",
            "pm::graph::EdgeMap<pm::graph::Undirected, pm::Int>",
            "EdgeMap{Undirected, CxxWrap.CxxLong}",
            "to_edgemap_undirected_int",
        ],
        [
            "NodeMap_Directed_Int",
            "pm::graph::NodeMap<pm::graph::Directed, pm::Int>",
            "NodeMap{Directed, CxxWrap.CxxLong}",
            "to_nodemap_directed_int",
        ],
        [
            "NodeMap_Undirected_Int",
            "pm::graph::NodeMap<pm::graph::Undirected, pm::Int>",
            "NodeMap{Undirected, CxxWrap.CxxLong}",
            "to_nodemap_undirected_int",
        ],
        [
            "NodeMap_Directed_Set_Int",
            "pm::graph::NodeMap<pm::graph::Directed, pm::Set<pm::Int>>",
            "NodeMap{Directed, Set{CxxWrap.CxxLong}}",
            "to_nodemap_directed_set_int",
        ],
        [
            "NodeMap_Undirected_Set_Int",
            "pm::graph::NodeMap<pm::graph::Undirected, pm::Set<pm::Int>>",
            "NodeMap{Undirected, Set{CxxWrap.CxxLong}}",
            "to_nodemap_undirected_set_int",
        ],
    ];

# mapped name, C++, CxxWrap, helper (to_...)
my $scalars = [["Int", "long", "CxxWrap.CxxLong", "int"],
               ["Integer", "pm::Integer", "Integer", "integer"],
               ["Rational", "pm::Rational", "Rational", "rational"],
               ["double", "double", "Float64", "double"],
               ["QuadraticExtension_Rational", "pm::QuadraticExtension<pm::Rational>", 
                  "QuadraticExtension{Rational}", "quadraticextension_rational"]
              ];
my $simplecontainers = [["Matrix","pm::Matrix","Matrix","matrix"],
                        ["SparseMatrix","pm::SparseMatrix","SparseMatrix","sparsematrix"],
                        ["Vector","pm::Vector","Vector","vector"],
                        ["SparseVector","pm::SparseVector","SparseVector","sparsevector"],
                        ["Array","pm::Array","Array","array"],
                       ];
for my $c (@$simplecontainers) { 
   for my $s (@$scalars) {
      push @$type_tuples, ["$c->[0]_$s->[0]",
                           "$c->[1]<$s->[1]>",
                           "$c->[2]\{$s->[2]\}",
                           "to_$c->[3]_$s->[3]"];
   }
}

my @keys = qw(type_string ctype jltype convert_f);

my $type_hashes = [];
foreach (@$type_tuples) {
   my %hash;
   @hash{@keys} = @$_;
   push @$type_hashes, \%hash;
}

sub insert_type_map {
   my $type_string = shift;
   "insert_type_in_map(\"$type_string\", &POLYMAKETYPE_$type_string);"
}

sub map_inserts_code {
   my $type_hashes = shift;
   return join("\n", map {insert_type_map($_->{type_string})} @$type_hashes);
}

sub call_function_feed_argument_if {
   my ($juliatype, $ctype) = @_;
    return <<"---";
    else if (jl_subtype(current_type, POLYMAKETYPE_$juliatype)) {
        function << jlcxx::unbox<const $ctype&>(value);
    }
---
 }

sub call_function_feed_argument_code {
   my ($type_hashes) = @_;
   my $feeding_ifs = join("",
        map {call_function_feed_argument_if($_->{type_string}, $_->{ctype})} @$type_hashes);

   return <<"---";
template <typename T>
void call_function_feed_argument(T& function, jl_value_t* value)
{
    jl_value_t* current_type = jl_typeof(value);
    if (jl_is_int64(value)) {
        function << static_cast<long>(jl_unbox_int64(value));
    } else if (jl_is_bool(value)) {
        function << jl_unbox_bool(value);
    } else if (jl_is_string(value)) {
        function << std::string(jl_string_data(value));
    } else if (jl_typeis(value, jl_float64_type)){
        function << jl_unbox_float64(value);
    } $feeding_ifs
    else {
        for (const auto& feeder : feeder_list<T&>::get()) {
            if (feeder(function, value))
                return;
        }
        throw std::runtime_error(
            "Cannot pass function value: conversion failed for argument of type " + std::string(jl_typeof_str(value))
        );
    }
    return;
}
---
}

sub option_set_take_if {
   my ($type_string, $ctype) = @_;
    return <<"---";
    else if (jl_subtype(current_type, POLYMAKETYPE_$type_string)) {
        optset[key] << jlcxx::unbox<const $ctype&>(value);
    }
---
}

sub option_set_take_code {
   my ($type_hashes) = @_;
   my $option_set_ifs = join("", map {option_set_take_if($_->{type_string}, $_->{ctype}) } @$type_hashes);

    return <<"---";
void option_set_take(pm::perl::OptionSet optset,
                     std::string         key,
                     jl_value_t*         value)
{
    jl_value_t* current_type = jl_typeof(value);
    if (jl_is_int64(value)) {
        optset[key] << static_cast<long>(jl_unbox_int64(value));
    } else if (jl_is_bool(value)) {
        optset[key] << jl_unbox_bool(value);
    } else if (jl_is_string(value)) {
        optset[key] << std::string(jl_string_data(value));
    } else if (jl_typeis(value, jl_float64_type)){
        optset[key] << jl_unbox_float64(value);
    } $option_set_ifs
    else {
        for (const auto& feeder : feeder_list<pm::perl::Value>::get()) {
            if (feeder(optset[key], value))
                return;
        }
        throw std::runtime_error(
            "Cannot create OptionSet: conversion failed for (key, value) = (" +
            key +
            ", ::" +
            std::string(jl_typeof_str(value)) +
            ")"
        );
    }
    return;
}
---
}

sub type_translator_code_jl {
   my ($type_hashes) = @_;
   my $content = join("\n",map {"(\"$_->{type_string}\", $_->{jltype}),"} @$type_hashes);
   return "const C_TYPES=[$content]";
}

sub type_translator {
   my ($ctype, $convert_f) = @_;
    return <<"---";
    type_name_tuples[i] = jl_cstr_to_string(\"$convert_f\");
    realname = abi::__cxa_demangle(typeid($ctype).name(), nullptr, nullptr, &status);
    type_name_tuples[i + 1] = jl_cstr_to_string(realname);
    free(realname);
    i += 2;
---
}

sub get_type_names_code {
   my ($type_hashes) = @_;
   my @non_nothing_types = grep { defined $_->{convert_f} } @$type_hashes;


   my $type_translations = join("\n",
        map { type_translator($_->{ctype}, $_->{convert_f}) } @non_nothing_types);

    return <<"---";
#include <cxxabi.h>
#include <typeinfo>

namespace jlpolymake {

jlcxx::ArrayRef<jl_value_t*> get_type_names() {
    int          status;
    char*        realname;
    int          number_of_types = $#non_nothing_types+1;
    jl_value_t** type_name_tuples = new jl_value_t*[2 * number_of_types];
    int          i = 0;
$type_translations
    return jlcxx::make_julia_array(type_name_tuples, 2 * number_of_types);
}

}
---
}

sub decl {
   my ($type_hashes) = @_;
   return [map {"jl_value_t* POLYMAKETYPE_$_->{type_string};"} @$type_hashes];
}


sub get_type_declarations {
   my ($type_hashes) = @_;
   return join("\n",@{decl($type_hashes)});
}
sub get_type_declarations_extern {
   my ($type_hashes) = @_;
   return "extern ".join("\nextern ",@{decl($type_hashes)});
}

sub unbox_propertyvalue_src {
   my ($type_hashes) = @_;
   my $functions = join("\n", map {
    "jlpolymake.method(\"$_->{convert_f}\", [](pm::perl::PropertyValue pv) {
        return to_SmallObject<$_->{ctype}>(pv);
    });" 
   } grep {defined($_->{convert_f}) && $_->{type_string} ne "BigObject"} @$type_hashes);
   return <<"---";
#include "jlpolymake/jlpolymake.h"

#include "jlpolymake/tools.h"

#include "jlpolymake/functions.h"

#include "jlpolymake/type_modules.h"

namespace jlpolymake {

void add_unbox_pv(jlcxx::Module& jlpolymake)
{
$functions
}

}
---
}


my $target = @ARGV > 0 ? $ARGV[0] : dirname(__FILE__);
my $cpp = "$target/include/jlpolymake/generated";
my $src = "$target/src";
my $jl = "$target/jl";

make_path($cpp, $jl, $src);

my %generated = (
                  "$cpp/map_inserts.h" => \&map_inserts_code,
                  "$cpp/call_function_feed_argument.h" => \&call_function_feed_argument_code,
                  "$cpp/option_set_take.h" => \&option_set_take_code,
                  "$cpp/get_type_names.h" => \&get_type_names_code,
                  "$cpp/type_declarations.h" => \&get_type_declarations,
                  "$cpp/type_declarations_extern.h" => \&get_type_declarations_extern,
                  "$jl/type_translator.jl" => \&type_translator_code_jl,
                  "$src/unbox_pv.cpp" => \&unbox_propertyvalue_src,
                 );


foreach (keys %generated) {
   open my $f, ">", "$_";
   print $f $generated{$_}->($type_hashes),"\n";
   close $f;
}
