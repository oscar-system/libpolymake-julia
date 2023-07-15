#!/usr/bin/env perl

use strict;
use warnings;
use File::Basename qw(dirname);
use File::Path qw(make_path);
use List::Util 'uniqstr';

my %added_types;
my %needed_types;
my $type_tuples = [];
my $wrap_calls = [];

sub check_type {
   foreach my $a (@_) {
      if (defined($a) && !exists($added_types{$a->[0]})) {
         $needed_types{$a->[0]} = 1;
      }
   }
}

sub add_type {
   $added_types{$_[0]->[0]} = 1;
   delete $needed_types{$_[0]->[0]};
   push @$type_tuples, $_[0];
}

sub joinname { return join("_",@_); }
sub joincxxt {
   my $name = shift;
   my $prefix = $name =~ /^(std::|polymake::|double|pm::)/ ? "" : "pm::";
   return "$prefix$name<".join(",",@_).">";
}
sub joinjlt  { return shift."{".join(",",@_)."}"; }
sub jointo   { return lc(join("_",@_)); }

sub template {
   my ($name, $arr1, $arr2) = @_;
   check_type($arr1, $arr2);
   my @names = ref($name) eq "ARRAY" ? @$name : (($name) x 4);
   if (defined($arr2)) {
      return [ joinname($names[0],$arr1->[0],$arr2->[0]),
               joincxxt($names[1],$arr1->[1],$arr2->[1]),
               joinjlt ($names[2],$arr1->[2],$arr2->[2]),
               jointo  ($names[3],$arr1->[3],$arr2->[3])];
   } else {
      return [ joinname($names[0],$arr1->[0]),
               joincxxt($names[1],$arr1->[1]),
               joinjlt ($names[2],$arr1->[2]),
               jointo  ($names[3],$arr1->[3])];
   }
}

sub double { return ["double", "double", "Float64", "double"]; }
sub Int { return ["Int","pm::Int","CxxWrap.CxxLong","int"]; }
sub Min { return ["Min","pm::Min","Min","min"]; }
sub Max { return ["Max","pm::Max","Max","max"]; }
sub Directed   { return ["Directed","pm::graph::Directed","Directed","directed"]; }
sub Undirected { return ["Undirected","pm::graph::Undirected","Undirected","undirected"]; }
sub Rational { return ["Rational","pm::Rational","Rational","rational"]; }
sub Integer { return ["Integer","pm::Integer","Integer","integer"]; }
sub String { return ["String","std::string","CxxWrap.StdString","string"]; }
sub BigObject { return ["BigObject", "pm::perl::BigObject", "BigObject", "bigobject"]; }

sub QuadraticExtension {
   return template("QuadraticExtension", @_);
}
sub TropicalNumber {
   return template("TropicalNumber", @_);
}
sub Vector {
   my $p = $_[0] eq "Sparse" ? shift : "";
   push @$wrap_calls, [lc("wrap_${p}vector"), [$_[0]->[1]]]
       if $_[0]->[1] !~ /Polynomial/;
   return template("${p}Vector", @_);
}
sub Matrix {
   my $p = $_[0] eq "Sparse" ? shift : "";
   push @$wrap_calls, [lc("wrap_${p}matrix"), [$_[0]->[1]]]
       if $_[0]->[1] !~ /Polynomial/;
   return template("${p}Matrix", @_);
}
sub Set {
   push @$wrap_calls, ["wrap_set", [$_[0]->[1]]];
   return template("Set", @_);
}
sub Map {
   push @$wrap_calls, ["wrap_map", [$_[0]->[1], $_[1]->[1]]];
   return template("Map", @_);
}
sub Array {
   push @$wrap_calls, ["wrap_array", [$_[0]->[1]]]
       if $_[0]->[1] !~ /Polynomial/;
   return template("Array", @_);
}
sub Pair {
   push @$wrap_calls, ["wrap_pair", [$_[0]->[1], $_[1]->[1]]];
   return template(["Pair", "std::pair", "StdPair", "pair"], @_);
}
sub List {
   push @$wrap_calls, ["wrap_list", [$_[0]->[1]]];
   return template(["List", "std::list", "StdList", "list"], @_);
}
sub UniPolynomial {
   return template("UniPolynomial", @_);
}
sub Polynomial {
   return template("Polynomial", @_);
}
sub NodeMap {
   my $nm = ["NodeMap", "graph::NodeMap", "NodeMap", "nodemap"];
   return (template($nm, Directed, @_),
           template($nm, Undirected, @_));
}
sub EdgeMap {
   my $em = ["EdgeMap", "graph::EdgeMap", "EdgeMap", "edgemap"];
   return (template($em, Directed, @_),
           template($em, Undirected, @_));
}
sub Graph {
   my $g = ["Graph", "graph::Graph", "Graph", "graph"];
   return (template($g, Directed),
           template($g, Undirected));
}

# mapped name, C++, CxxWrap, helper (to_...)
my $scalars = [ Int, Integer, Rational, double,
                QuadraticExtension(Rational),
                TropicalNumber(Min,Rational),
                TropicalNumber(Max,Rational)
              ];

my $simplecontainers = [ \&Matrix, \&Vector, \&Array, ];

foreach my $s (@$scalars) {
   add_type($s);
}

for my $c (@$simplecontainers) {
   for my $s (@$scalars) {
      add_type($c->($s));
   }
}

# these must be sorted, i.e. any member template must appear before using it
# simple containers with just a scalar are done above
# sparse containers are added at the end (since they need sets)
#
# for supported types this will also generate the `wrap_type<Members>(jlpoymake)` call
# otherwise it will only produce the necessary type-mappings for the julia bindings

foreach my $typearr (
        ["PropertyValue", "pm::perl::PropertyValue", "PropertyValue", undef],
        # ListResult cannot be used like other scalar based types so this is
        # commented on purpose, also as a reminder
        # ["ListResult", "pm::perl::ListResult", "ListResult", undef],
        ["OptionSet", "pm::perl::OptionSet", "OptionSet", undef],

        BigObject,

        Pair(Array(Int),Array(Int)),
        Array(Pair(Array(Int),Array(Int))),

        Pair(Int,Int),
        List(Pair(Int,Int)),
        List(List(Pair(Int,Int))),
        Pair(Int, List(List(Pair(Int,Int)))),
        Array(Pair(Int,Int)),
        Array(List(Pair(Int,Int))),

        Pair(Integer,Int),
        List(Pair(Integer,Int)),

        Set(Int),
        Set(Set(Int)),
        Array(Set(Int)),
        Array(Set(Set(Int))),

        Array(String),
        Array(Array(Int)),
        Array(Array(Integer)),
        Array(Array(Rational)),
        Array(Array(Set(Int))),
        Array(Matrix(Integer)),
        Array(BigObject),

        UniPolynomial(Int,Int),
        UniPolynomial(Integer,Int),
        UniPolynomial(Rational,Int),
        UniPolynomial(QuadraticExtension(Rational),Int),
        Polynomial(Int,Int),
        Polynomial(Integer,Int),
        Polynomial(Rational,Int),
        Polynomial(double,Int),
        Polynomial(QuadraticExtension(Rational),Int),

        # the wrap calls are done separately, via add_*_extended
        Matrix(Polynomial(Rational,Int)),
        Vector(Polynomial(Rational,Int)),
        Array(Polynomial(Integer,Int)),
        Array(Polynomial(Rational,Int)),

        Map(String,String),
        Map(String,Int),
        Map(Int,Int),
        Map(Integer,Int),
        Map(Rational,Rational),
        Map(Set(Int),Integer),
        Map(Set(Int),Rational),
        Map(Set(Int),Vector(Rational)),
        Map(Vector(Int),Integer),

        [
            "IncidenceMatrix_NonSymmetric",
            "pm::IncidenceMatrix<pm::NonSymmetric>",
            "IncidenceMatrix{NonSymmetric}",
            "incidencematrix_nonsymmetric",
        ],
        [
            "IncidenceMatrix_Symmetric",
            "pm::IncidenceMatrix<pm::Symmetric>",
            "IncidenceMatrix{Symmetric}",
            "incidencematrix_symmetric",
        ],

        Graph,
        NodeMap(Int),
        NodeMap(Set(Int)),
        EdgeMap(Int),

        [
            "HomologyGroup_Integer",
            "polymake::topaz::HomologyGroup<pm::Integer>",
            "HomologyGroup{Integer}",
            "homologygroup_integer",
        ],
        [
            "Array_HomologyGroup_Integer",
            "pm::Array<polymake::topaz::HomologyGroup<pm::Integer>>",
            "Array{HomologyGroup{Integer}}",
            "array_homologygroup_integer",
        ],
     ) {
   add_type($typearr);
}

# must be after Set{Int}
foreach my $s (@$scalars) {
   add_type(Matrix("Sparse", $s));
   add_type(Vector("Sparse", $s));
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
    type_name_tuples[i] = jl_cstr_to_string(\"to_$convert_f\");
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
    "jlpolymake.method(\"to_$_->{convert_f}\", [](pm::perl::PropertyValue pv) {
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

sub wrap_types_src {
   my $calls = join("\n", uniqstr(map {
       "    $_->[0]<".join(",",@{$_->[1]}).">(jlpolymake);"
   } @$wrap_calls));

   return <<"---";
#include "jlpolymake/jlpolymake.h"

#include "jlpolymake/tools.h"

#include "jlpolymake/functions.h"

#include "jlpolymake/type_modules.h"

#include "jlpolymake/containers.h"

namespace jlpolymake {

void wrap_types(jlcxx::Module& jlpolymake)
{
$calls
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
                  "$src/wrap_types.cpp" => \&wrap_types_src,
                 );


foreach (keys %generated) {
   open my $f, ">", "$_";
   print $f $generated{$_}->($type_hashes),"\n";
   close $f;
}

foreach my $k (keys(%needed_types)) {
   print "Type missing: $k\n";
}
