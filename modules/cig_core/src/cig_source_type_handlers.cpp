#include "cig_source_type_handlers.h"

#include <functional>

using namespace std;

namespace cig {
	namespace source {


		template < class _spec_t >
		type_ptr default_type_handler (
			mapper_context & cxt,
			const source::cursor_type & type,
			_spec_t && specialization_method = nullptr
		)
		{
			source::cursor_type canon_type = type;

			// get property base canonical type for type
			// NOTE: 	is this appropriate? Must find a proper way to describe
			// 			aliases
			while (canon_type.kind != type_kind::type_kind_typedef)
				canon_type = cxt.parser.get_canonical_type(canon_type);

			auto source_type = cxt.map.get_type(canon_type.identifier);
			bool is_new = source_type->qualified_name.empty();

			if (is_new) {
				source_type->identifier = canon_type.identifier;
				source_type->qualified_name = canon_type.identifier;
				source_type->dimensions = canon_type.dimensions;
				source_type->is_const = cxt.parser.is_const_qualified (canon_type);
				source_type->kind = type.kind;

				if (specialization_method)
					specialization_method (cxt, source_type, canon_type);
			}

			return source_type;
		}

		void inplace_struct_handler (
			mapper_context & cxt,
			type_ptr & source_type,
			source::cursor_type const & cursor_type
		){
			auto decl_cursor = cxt.parser.get_type_declaration(cursor_type);
			auto decl_type = cxt.parser.get_type(decl_cursor);

			// if not structure definition then bail
			if (!(decl_cursor.kind == cursor_kind::decl_struct || decl_cursor.kind == cursor_kind::decl_class))
				return;

			source_type->kind = type_kind::type_kind_struct;
			source_type->base_structure = cxt.map.get_structure(decl_cursor.qualified_name);

			// set basic information
			structure::apply_cursor(*source_type->base_structure, decl_cursor);
		}

		type_ptr type_default_handler (mapper_context & cxt, cursor_type const & type){
			return default_type_handler (
				cxt,
				type
			);
		}

		type_ptr type_reference_handler (mapper_context & cxt, cursor_type const & type){
			return default_type_handler (
				cxt,
				type,
				[](mapper_context & cxt, type_ptr & type, cursor_type const & canon_type) {
					type->base = cxt.mapper.type_dispatcher.execute (canon_type.kind, cxt, canon_type);
				}
			);
		}

		type_ptr type_struct_handler (mapper_context & cxt, cursor_type const & type){
			return default_type_handler (
				cxt,
				type,
				[](mapper_context & cxt, type_ptr & type, cursor_type const & canon_type) {
					inplace_struct_handler(cxt, type, canon_type);
				}
			);
		}

		type_ptr type_unhandled_handler (mapper_context & cxt, cursor_type const & type){
			return default_type_handler (
				cxt,
				type,
				[](mapper_context & cxt, type_ptr & type, cursor_type const & cannon_type) {
					// find template arguments
				}
			);
		}

		type_ptr type_enum_handler (mapper_context & cxt, cursor_type const & type){
			type_ptr type = default_type_handler(cxt, type);

			type->kind = type_kind::type_kind_unhandled;
			return type;
		}

		type_ptr type_array_handler (mapper_context & cxt, cursor_type const & type){
			return default_type_handler (
				cxt,
				type,
				[](mapper_context & cxt, type_ptr & type, cursor_type const & canon_type) {
					type->base = cxt.mapper.type_dispatcher.execute (canon_type.kind, cxt, canon_type);
				}
			);
		}

	}
}
