#include "cig_source_cursor_handlers.h"
#include "cig_source_mapper.h"

namespace cig {
	namespace source {
		namespace cursor_handlers {

			void cursor_default_handler (mapper_context & cxt, const source::cursor & cursor) {}

			void struct_base_action (mapper_context & cxt, const source::cursor & cursor, structure_kind kind) {
				auto new_structure = cxt.map.get_structure (cursor.qualified_name);

				structure::apply_cursor(*new_structure, cursor);

				new_structure->struct_path = make_struct_path (cxt, cursor);
				new_structure->kind = kind;
			}

			void struct_handler (mapper_context & cxt, const source::cursor & cursor) {
				struct_base_action (cxt, cursor, structure_kind::structure_struct);
			}

			void class_handler (mapper_context & cxt, const source::cursor & cursor) {
				struct_base_action (cxt, cursor, structure_kind::structure_class);
			}

			void base_spec_handler (mapper_context & cxt, const source::cursor & cursor) {
				// get or create base type map structure
				auto base_struct = cxt.map.get_structure(cursor.qualified_name);

				// get derived structure and add to parent list
				auto & 	cursor_stack =
					cxt.parser.get_current_cursor_stack();

				auto 	sem_parent_struct =
					cxt.map.get_structure(cursor_stack.back().qualified_name);

				sem_parent_struct->parents.push_back(base_struct);
			}

			void field_handler (mapper_context & cxt, const source::cursor & cursor) {
				auto cursor_type = cxt.parser.get_type (cursor);
				
				auto type = cxt.mapper.type_dispatcher.execute (cursor_type.kind, cxt, cursor_type);

				// get derived structure and add to field list
				auto & 	cursor_stack =
					cxt.parser.get_current_cursor_stack();

				auto 	sem_parent_struct =
					cxt.map.get_structure(cursor_stack.back().qualified_name);

				sem_parent_struct->fields.emplace_back(
					cursor.location,
					cursor.qualified_name,
					cursor.identifier,
					type,
					cxt.parser.get_visibility(cursor)
				);
			}

			void method_handler (mapper_context & cxt, const source::cursor & cursor) {}

			void function_handler (mapper_context & cxt, const source::cursor & cursor) {}

			void parameter_handler (mapper_context & cxt, const source::cursor & cursor) {}

			void namespace_handler (mapper_context & cxt, const source::cursor & cursor) {}

		}
	}
}