#!/usr/bin/env perl

use strict;
use warnings;
use File::Basename qw(dirname basename);
use File::Path qw(make_path);
use List::Util 'uniqstr';

my $missingtypes = qr(^(Min|Max|String|Undirected|Directed|Symmetric|NonSymmetric)$);
my %added_types;
my %needed_types;
my $type_tuples = [];
my $wrap_calls = [];
my $core_calls = [];
my $extra_calls = [];

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

sub add_types {
   foreach (@_) {
      add_type($_);
   }
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

sub def { [$_[0], "pm::$_[0]", $_[0], lc($_[0])] }
sub Min { def("Min"); }
sub Max { def("Max"); }
sub Symmetric    { def("Symmetric"); }
sub NonSymmetric { def("NonSymmetric"); }
sub Rational  { def("Rational"); }
sub Integer   { def("Integer"); }

sub double { return ["double", "double", "Float64", "double"]; }
sub Int { return ["Int","pm::Int","CxxWrap.CxxLong","int"]; }
sub Directed     { return ["Directed","pm::graph::Directed","Directed","directed"]; }
sub Undirected   { return ["Undirected","pm::graph::Undirected","Undirected","undirected"]; }
sub String    { return ["String","std::string","CxxWrap.StdString","string"]; }
sub BigObject { return ["BigObject", "pm::perl::BigObject", "BigObject", "bigobject"]; }
sub BigObjectType { return ["BigObjectType", "pm::perl::BigObjectType", "BigObjectType", "bigobjecttype"]; }
sub BasicDecoration { return [ "BasicDecoration", "polymake::graph::lattice::BasicDecoration", "BasicDecoration", "basicdecoration"]; }

sub QuadraticExtension {
   return template("QuadraticExtension", @_);
}
sub TropicalNumber {
   return template("TropicalNumber", @_);
}

sub Vector {
   return () if @_ == 0;
   my $p = $_[0] eq "Sparse" ? shift : "";
   my $e = shift;
   push @$wrap_calls, [lc("wrap_${p}vector"), [$e->[1]]];
   return (template("${p}Vector", $e), Vector(@_));
}
sub Matrix {
   return () if @_ == 0;
   my $p = $_[0] eq "Sparse" ? shift : "";
   my $e = shift;
   push @$wrap_calls, [lc("wrap_${p}matrix"), [$e->[1]]];
   return (template("${p}Matrix", $e), Matrix(@_));
}
sub Array {
   return () if @_ == 0;
   my $e = shift;
   push @$wrap_calls, ["wrap_array", [$e->[1]]];
   return (template("Array", $e), Array(@_));
}
sub Set {
   return () if @_ == 0;
   my $e = shift;
   push @$wrap_calls, ["wrap_set", [$e->[1]]];
   return (template("Set", $e), Set(@_));
}
sub Map {
   return () if @_ == 0;
   my $k = shift; my $v = shift;
   push @$wrap_calls, ["wrap_map", [$k->[1], $v->[1]]];
   return (template("Map", $k, $v), Map(@_));
}
sub Pair {
   return () if @_ == 0;
   my $f = shift; my $s = shift;
   push @$wrap_calls, ["wrap_pair", [$f->[1], $s->[1]]];
   return (template(["Pair", "std::pair", "StdPair", "pair"], $f, $s), Pair(@_));
}
sub List {
   return () if @_ == 0;
   my $e = shift;
   push @$wrap_calls, ["wrap_list", [$e->[1]]];
   return (template(["List", "std::list", "StdList", "list"], $e), List(@_));
}
sub UniPolynomial {
   return () if @_ == 0;
   my $c = shift; my $e = shift;
   push @$wrap_calls, ["wrap_unipolynomial", [$c->[1], $e->[1]]];
   return (template("UniPolynomial", $c, $e), UniPolynomial(@_));
}
sub Polynomial {
   return () if @_ == 0;
   my $c = shift; my $e = shift;
   push @$wrap_calls, ["wrap_polynomial", [$c->[1], $e->[1]]];
   return (template("Polynomial", $c, $e), Polynomial(@_));
}
sub IncidenceMatrix {
   my $i = def("IncidenceMatrix");
   # just one fixed call for a custom file
   push @$wrap_calls, ["add_incidencematrix", []];
   return (template($i, Symmetric),
           template($i, NonSymmetric));
}
sub Graph {
   my $g = ["Graph", "graph::Graph", "Graph", "graph"];
   # just one fixed call for a custom file
   push @$wrap_calls, ["add_graph", []];
   return (template($g, Directed),
           template($g, Undirected));
}
sub NodeMap {
   return () if @_ == 0;
   my $e = shift;
   my $nm = ["NodeMap", "pm::graph::NodeMap", "NodeMap", "nodemap"];
   push @$wrap_calls, ["wrap_nodemap", ["pm::graph::Undirected", $e->[1]]];
   push @$wrap_calls, ["wrap_nodemap", ["pm::graph::Directed"  , $e->[1]]];
   return (template($nm, Directed, $e),
           template($nm, Undirected, $e),
           NodeMap(@_));
}
sub EdgeMap {
   return () if @_ == 0;
   my $e = shift;
   my $em = ["EdgeMap", "pm::graph::EdgeMap", "EdgeMap", "edgemap"];
   push @$wrap_calls, ["wrap_edgemap", ["pm::graph::Undirected", $e->[1]]];
   push @$wrap_calls, ["wrap_edgemap", ["pm::graph::Directed"  , $e->[1]]];
   return (template($em, Directed, $e),
           template($em, Undirected, $e),
           EdgeMap(@_));
}

# mapped name, C++, CxxWrap, helper (to_...)
my $scalars = [ Int, Integer, Rational, double,
                QuadraticExtension(Rational),
                TropicalNumber(Min,Rational),
                TropicalNumber(Max,Rational)
              ];

my $simplecontainers = [ \&Matrix, \&Vector, \&Array, ];

add_types(@$scalars);

for my $c (@$simplecontainers) {
   add_types(map {$c->($_)} @$scalars);
}

# these must be sorted, i.e. any member template must appear before using it
# simple containers with just a scalar are done above
# sparse containers are added at the end (since they need sets)
#
# for supported types this will also generate the `wrap_type<Members>(jlpoymake)` call
# otherwise it will only produce the necessary type-mappings for the julia bindings

add_types(
        ["PropertyValue", "pm::perl::PropertyValue", "PropertyValue", undef],
        # ListResult cannot be used like other scalar based types so this is
        # commented on purpose, also as a reminder
        # ["ListResult", "pm::perl::ListResult", "ListResult", undef],
        ["OptionSet", "pm::perl::OptionSet", "OptionSet", undef],

        BigObject,
        BigObjectType,

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

        Pair(Vector(Int),Array(Int)),
        Pair(Vector(Integer),Array(Int)),
        Pair(Vector(Rational),Array(Int)),

        Set(Int),
        Set(Set(Int)),
        Array(Set(Int)),
        Array(Set(Set(Int))),

        Pair(Set(Int),Int),
        Array(Pair(Set(Int),Int)),

        Array(String),
        Array(Array(Int)),
        Array(Array(Integer)),
        Array(Array(Rational)),
        Array(Array(Set(Int))),
        Array(Matrix(Integer)),
        Array(Matrix(Rational)),
        Array(Vector(Integer)),
        Array(Vector(Rational)),
        Array(BigObject),

        Pair(Matrix(TropicalNumber(Max,Rational)),Matrix(TropicalNumber(Max,Rational))),
        Pair(Matrix(TropicalNumber(Min,Rational)),Matrix(TropicalNumber(Min,Rational))),
     );

# must be after Set{Int}
add_types(map { Matrix("Sparse", $_) } @$scalars);
add_types(map { Vector("Sparse", $_) } @$scalars);

add_types(
        UniPolynomial(Int,Int),
        UniPolynomial(Integer,Int),
        UniPolynomial(Rational,Int),
        UniPolynomial(Rational,Rational),
        UniPolynomial(QuadraticExtension(Rational),Int),
        Polynomial(Int,Int),
        Polynomial(Integer,Int),
        Polynomial(Rational,Int),
        Polynomial(Rational,Rational),
        Polynomial(double,Int),
        Polynomial(QuadraticExtension(Rational),Int),

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
        Map(Pair(Int,Int),Int),
        Map(Pair(Int,Int),Vector(Integer)),

        IncidenceMatrix,
        Array(IncidenceMatrix),

        Graph,

        NodeMap(Int),
        NodeMap(double),
        NodeMap(Integer),
        NodeMap(Rational),
        NodeMap(String),
        NodeMap(Set(Int)),
        NodeMap(Array(Set(Int))),
        NodeMap(Vector(double)),
        NodeMap(Vector(Rational)),
        NodeMap(IncidenceMatrix),

        EdgeMap(Int),
        EdgeMap(double),
        EdgeMap(Integer),
        EdgeMap(Rational),
        EdgeMap(String),
        EdgeMap(Vector(double)),
        EdgeMap(Vector(Rational)),
        EdgeMap(Array(Array(Int))),

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
        [
            "SwitchTable",
            "polymake::group::SwitchTable",
            "SwitchTable",
            "switchtable",
        ],
     );

# core calls done
$core_calls = $wrap_calls;
$wrap_calls = [];

add_types(
        BasicDecoration,
        NodeMap(BasicDecoration),
        Array(BasicDecoration),
     );

$extra_calls = $wrap_calls;

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

my $header = <<"---";
#include "jlpolymake/jlpolymake.h"

#include "jlpolymake/tools.h"

#include "jlpolymake/functions.h"

#include "jlpolymake/type_modules.h"

#include "jlpolymake/containers.h"

namespace jlpolymake {

---

my $footer = <<"---";
}

}
---
my $wrapperlimit = 20;

sub gen_limited_files {
   my ($path, @calls) = @_;
   $path =~ s/\.cpp$//;
   my $addfun = basename($path);

   my $funsig = "(jlcxx::Module& jlpolymake)";

   my $files = {};
   my $i = 0;
   do {
      my $filename = $path."_$i.cpp";
      my $function = $header .
                     "void ${addfun}_$i$funsig\n" .
                     "{\n    " .
                     join ("\n    ",splice(@calls, 0, $wrapperlimit)) .
                     "\n" . $footer;

      $files->{$filename} = $function;
      $i += 1;
   } while (@calls > 0);

   my $main = $header;
   $main .= "void ${addfun}_$_$funsig;\n" for (0..$i-1);

   $main .= "\nvoid $addfun$funsig\n{\n";
   $main .= "    ${addfun}_$_(jlpolymake);\n" for (0..$i-1);
   $main .= "$footer";

   $files->{"$path.cpp"} = $main;
   return $files;
}

sub unbox_propertyvalue_src {
   my ($type_hashes, $calls, $path) = @_;
   my @functions = map {
    "jlpolymake.method(\"to_$_->{convert_f}\", [](pm::perl::PropertyValue pv) {
        return to_SmallObject<$_->{ctype}>(pv);
    });"
   } grep {defined($_->{convert_f}) && $_->{type_string} ne "BigObject"} @$type_hashes;
   return gen_limited_files($path, @functions);
}

sub wrap_types_src {
   my ($type_hashes, $calls, $path) = @_;
   my @calls = uniqstr(map {
       "    $_->[0]".(@{$_->[1]} > 0 ? "<".join(",",@{$_->[1]}).">" : "")."(jlpolymake);"
   } @$calls);

   return gen_limited_files($path, @calls);
}

sub wrap_types_extra_src {
   my ($type_hashes, $calls, $path) = @_;
   return wrap_types_src($type_hashes, $extra_calls, $path);
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
                  "$src/wrap_types_extra.cpp" => \&wrap_types_extra_src,
                 );


foreach (keys %generated) {
   my $files = $generated{$_}->($type_hashes, $core_calls, $_);
   $files = {$_ => $files} if ref($files) ne "HASH";
   while(my($k, $v) = each %$files) {
      open my $f, ">", "$k";
      print $f $v,"\n";
      close $f;
   }
}

foreach my $k (keys(%needed_types)) {
   print "WARNING: type missing: $k\n" unless $k =~ $missingtypes;
}
