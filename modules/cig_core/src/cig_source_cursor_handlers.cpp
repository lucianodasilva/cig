#include "cig_source_cursor_handlers.h"
#include "cig_source_mapper.h"

namespace cig {
	namespace source {
		namespace cursor_handlers {

			void cursor_default_handler (parser_context & cxt, const source::cursor & cursor) {}

			void struct_base_action (parser_context & cxt, const source::cursor & cursor, structure_kind kind) {
				auto structure = cxt.wip_map.get_structure (cursor.qualified_name);

				structure->semantic_path = make_semantic_path (cxt, cursor);
				structure->identifier = cursor.identifier;
				structure->kind = kind;
			}

			void struct_handler (parser_context & cxt, const source::cursor & cursor) {
				struct_base_action (cxt, cursor, structure_kind::structure_struct);
			}

			void class_handler (parser_context & cxt, const source::cursor & cursor) {
				struct_base_action (cxt, cursor, structure_kind::structure_class);
			}

			void base_spec_handler (parser_context & cxt, const source::cursor & cursor) {
				auto base_struct = cxt.wip_map.get_structure(cursor.qualified_name);
				auto parent_cursor = cxt.parser.get_semantic_parent(cursor);

				auto sem_parent_struct = cxt.wip_map.get_structure(parent_cursor.qualified_name);

				sem_parent_struct->parents.push_back(base_struct);
			}

			void field_handler (parser_context & cxt, const source::cursor & cursor) {}

			void method_handler (parser_context & cxt, const source::cursor & cursor) {}

			void function_handler (parser_context & cxt, const source::cursor & cursor) {}

			void parameter_handler (parser_context & cxt, const source::cursor & cursor) {}

			void namespace_handler (parser_context & cxt, const source::cursor & cursor) {}

		}
	}
}