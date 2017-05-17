#pragma once
#ifndef _cig_mapper_model_h_
#define _cig_mapper_model_h_

#include <cig_common.h>

#include <cinttypes>
#include <string>

using namespace std;

namespace cig {
	namespace mapper {

		struct location {
			string      file;
			uint32_t    line   {0},
						column {0};

			inline bool is_empty () const { return file.empty(); }
		};

		enum class type_kind : uint32_t {
			type_kind_invalid,
			type_kind_unhandled,
			type_kind_void,
			type_kind_bool,
			type_kind_char,
			type_kind_uchar,
			type_kind_char16,
			type_kind_char32,
			type_kind_ushort,
			type_kind_uint,
			type_kind_ulong,
			type_kind_ulonglong,
			type_kind_char_s,
			type_kind_schar,
			type_kind_wchar,
			type_kind_short,
			type_kind_int,
			type_kind_long,
			type_kind_longlong,
			type_kind_int128,
			type_kind_float,
			type_kind_double,
			type_kind_longdouble,
			type_kind_nullptr,
			type_kind_pointer,
			type_kind_lvalue_ref,
			type_kind_rvalue_ref,
			type_kind_struct,
			type_kind_enum,
			type_kind_typedef,
			type_kind_constant_array,
			type_kind_incomplete_array
		};

		enum struct visibility : uint32_t {
			invalid,
			v_private,
			v_protected,
			v_public
		};

		struct cursor_type {
			string      identifier;

			bool        is_const;
			type_kind   kind;
			uint32_t    dimensions;
		};

		enum struct cursor_kind : uint32_t {
			unsupported,
			decl_struct,
			decl_class,
			decl_base_specifier,
			decl_field,
			decl_method,
			decl_function,
			decl_parameter,
			decl_namespace
		};

		struct cursor_flags {
			bool
				is_virtual 	: 1,
				is_pure 	: 1,
				is_static 	: 1,
				is_const 	: 1,
				is_ctor 	: 1;
		};

		struct cursor {
			mapper::location	location;
			string 				identifier;
			cursor_kind 		kind;
		};

		enum struct template_parameter_kind {
			unsupported,
			type,
			non_type
		};

		struct cursor_template_parameter {
			cursor_type				type;
			string      			identifier;
			template_parameter_kind	kind;
		};

		// predefine map types and ptrs
		struct type;

		using type_ptr = indexed_ptr < type >;

		struct structure;

		using structure_ptr = indexed < structure >;

		struct template_parameter {
			type_ptr				type;
			string 					identifier;
			template_parameter_kind kind;
		};

		struct template_argument {
			template_parameter	parameter;
			string 				value;
		};

		struct type {
			small_vector < template_argument, 8 >
							template_arguments;
			string			qualified_name;
			string			identifier;
			type_ptr		base;
			structure_ptr 	base_structure;
			bool 			is_const;
			type_kind		kind;
			uint32_t 		dimensions;
		};

		struct field {
			mapper::location	location;

			string				qualified_name;
			string				identifier;

			type_ptr			type;
			mapper::visibility	visibility;
		};

		struct method_parameter {
			type_ptr	type;
			string		identifier;
		};

		struct method {
			small_vector < method_parameter, 8 >
								parameters;
			mapper::location	location;
			string				identifier;
			string				qualified_name;
			type_ptr			return_type;
			mapper::visibility	visibility;
			cursor_flags		flags;
		};

		enum struct structure_kind {
			unknown,
			structure_struct,
			structure_class
		};

		struct structure {
			small_vector < field, 8 >
								fields;
			small_vector < method, 8 >
								methods;
			small_vector < structure_ptr, 4 >
								parents;
 
			string				qualified_name;
			string				identifier;

			structure_kind		kind;
		};

	}
}

#endif //_cig_mapper_model_h_
