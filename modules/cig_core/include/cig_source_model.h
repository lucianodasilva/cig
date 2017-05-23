#pragma once
#ifndef _cig_source_model_h_
#define _cig_source_model_h_

#include <cig_common.h>

#include <cinttypes>
#include <string>
#include <vector>

using namespace std;

namespace cig {
	namespace source {

		size_t const low_freq_cap = 4;

		size_t const med_freq_cap = 8;

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
			source::location	location;
			string				qualified_name;
			string 				identifier;
			cursor_kind 		kind;

			inline bool is_empty() const {
				return location.is_empty();
			}
		};

		using cursor_stack = small_vector < cursor, med_freq_cap >;

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

		using type_ptr = indexed_ptr < vector < type > >;
		using type_const_ptr = indexed_ptr < vector < type > const >;

		struct structure;

		using structure_ptr = indexed_ptr < vector < structure > >;
		using structure_const_ptr = indexed_ptr < vector < structure > const >;

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
			small_vector < template_argument, med_freq_cap >
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
			source::location	location;

			string				qualified_name;
			string				identifier;

			type_ptr			type;
			source::visibility	visibility;
		};

		struct method_parameter {
			type_ptr	type;
			string		identifier;
		};

		struct method {
			small_vector < method_parameter, med_freq_cap >
								parameters;
			source::location	location;
			string				identifier;
			string				qualified_name;
			type_ptr			return_type;
			source::visibility	visibility;
			cursor_flags		flags;
		};

		enum struct structure_kind {
			unsupported,
			structure_struct,
			structure_class
		};

		enum struct struct_path_node_kind {
			unsupported,
			namespace_node,
			structure_node
		};

		struct struct_path_node {
			string					identifier;
			structure_ptr 			structure;
			struct_path_node_kind	kind;
		};

		using struct_path = small_vector < struct_path_node, med_freq_cap >;

		struct structure {
			small_vector < template_parameter, low_freq_cap >
								template_parameters;
			small_vector < field, med_freq_cap >
								fields;
			small_vector < method, med_freq_cap >
								methods;
			small_vector < structure_ptr, low_freq_cap >
								parents;

			source::struct_path struct_path;

			string				qualified_name;
			string				identifier;

			structure_kind		kind;
			source::visibility	visibility;

			static void apply_cursor (structure & strct, source::cursor const & cursor);
		};

	}
}

#endif //_cig_source_model_h_
