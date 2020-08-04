#!/usr/bin/env perl

use strict;
use warnings;
use File::Basename qw(dirname);
use File::Path qw(make_path);

my $type_tuples = [
        ["PropertyValue", "pm::perl::PropertyValue", "PropertyValue", undef],
        ["OptionSet", "pm::perl::OptionSet", "OptionSet", undef],
        ["BigObject", "pm::perl::BigObject", "BigObject", "to_bigobject"],
        ["Integer", "pm::Integer", "Integer", "to_integer"],
        ["Rational", "pm::Rational", "Rational", "to_rational"],
        [
            "Matrix_Int",
            "pm::Matrix<long>",
            "Matrix{CxxWrap.CxxLong}",
            "to_matrix_int",
        ],
        [
            "Matrix_Integer",
            "pm::Matrix<pm::Integer>",
            "Matrix{Integer}",
            "to_matrix_integer",
        ],
        [
            "Matrix_Rational",
            "pm::Matrix<pm::Rational>",
            "Matrix{Rational}",
            "to_matrix_rational",
        ],
        [
            "Matrix_double",
            "pm::Matrix<double>",
            "Matrix{Float64}",
            "to_matrix_double",
        ],
        [
            "Vector_Int",
            "pm::Vector<long>",
            "Vector{CxxWrap.CxxLong}",
            "to_vector_int",
        ],
        [
            "Vector_Integer",
            "pm::Vector<pm::Integer>",
            "Vector{Integer}",
            "to_vector_integer",
        ],
        [
            "Vector_Rational",
            "pm::Vector<pm::Rational>",
            "Vector{Rational}",
            "to_vector_rational",
        ],
        [
            "Vector_double",
            "pm::Vector<double>",
            "Vector{Float64}",
            "to_vector_double",
        ],
        [
            "Pair_Int_Int",
            "std::pair<pm::Int, pm::Int>",
            "StdPair{CxxWrap.CxxLong,CxxWrap.CxxLong}",
            "to_pair_int_int",
        ],
        [
            "List_Pair_Int_Int",
            "std::list<std::pair<pm::Int, pm::Int>>",
            "StdList{StdPair{CxxWrap.CxxLong,CxxWrap.CxxLong}}",
            "to_list_pair_int_int",
        ],
        ["Set_Int", "pm::Set<long>", "Set{CxxWrap.CxxLong}", "to_set_int"],
        [
            "Array_Int",
            "pm::Array<long>",
            "Array{CxxWrap.CxxLong}",
            "to_array_int",
        ],
        [
            "Array_Integer",
            "pm::Array<pm::Integer>",
            "Array{Integer}",
            "to_array_integer",
        ],
        [
            "Array_String",
            "pm::Array<std::string>",
            "Array{String}",
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
            "SparseMatrix_Integer",
            "pm::SparseMatrix<pm::Integer>",
            "SparseMatrix{Integer}",
            "to_sparsematrix_integer",
        ],
        [
            "SparseMatrix_Rational",
            "pm::SparseMatrix<pm::Rational>",
            "SparseMatrix{Rational}",
            "to_sparsematrix_rational",
        ],
        [
            "SparseMatrix_Int",
            "pm::SparseMatrix<long>",
            "SparseMatrix{CxxWrap.CxxLong}",
            "to_sparsematrix_int",
        ],
        [
            "SparseMatrix_double",
            "pm::SparseMatrix<double>",
            "SparseMatrix{Float64}",
            "to_sparsematrix_double",
        ],
        [
            "SparseVector_Integer",
            "pm::SparseVector<pm::Integer>",
            "SparseVector{Integer}",
            "to_sparsevector_integer",
        ],
        [
            "SparseVector_Rational",
            "pm::SparseVector<pm::Rational>",
            "SparseVector{Rational}",
            "to_sparsevector_rational",
        ],
        [
            "SparseVector_Int",
            "pm::SparseVector<long>",
            "SparseVector{CxxWrap.CxxLong}",
            "to_sparsevector_int",
        ],
        [
            "SparseVector_double",
            "pm::SparseVector<double>",
            "SparseVector{Float64}",
            "to_sparsevector_double",
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
    ];

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
        // check size of long, to be sure
        static_assert(sizeof(long) == 8, "long must be 64 bit");
        function << static_cast<long>(jl_unbox_int64(value));
    } else if (jl_is_bool(value)) {
        function << jl_unbox_bool(value);
    } else if (jl_is_string(value)) {
        function << std::string(jl_string_data(value));
    } else if (jl_typeis(value, jl_float64_type)){
        function << jl_unbox_float64(value);
    } $feeding_ifs
    else {
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
        // check size of long, to be sure
        static_assert(sizeof(long) == 8, "long must be 64 bit");
        optset[key] << static_cast<long>(jl_unbox_int64(value));
    } else if (jl_is_bool(value)) {
        optset[key] << jl_unbox_bool(value);
    } else if (jl_is_string(value)) {
        optset[key] << std::string(jl_string_data(value));
    } else if (jl_typeis(value, jl_float64_type)){
        optset[key] << jl_unbox_float64(value);
    } $option_set_ifs
    else {
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

my $target = @ARGV > 0 ? $ARGV[0] : dirname(__FILE__);
my $cpp = "$target/include/jlpolymake/generated";
my $jl = "$target/jl";

make_path($cpp, $jl);

my %generated = ( 
                  "$cpp/map_inserts.h" => \&map_inserts_code,
                  "$cpp/call_function_feed_argument.h" => \&call_function_feed_argument_code,
                  "$cpp/option_set_take.h" => \&option_set_take_code,
                  "$cpp/get_type_names.h" => \&get_type_names_code,
                  "$cpp/type_declarations.h" => \&get_type_declarations,
                  "$cpp/type_declarations_extern.h" => \&get_type_declarations_extern,
                  "$jl/type_translator.jl" => \&type_translator_code_jl,
                 );


foreach (keys %generated) {
   open my $f, ">", "$_";
   print $f $generated{$_}->($type_hashes),"\n";
   close $f;
}
