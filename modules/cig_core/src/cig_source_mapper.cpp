#include "cig_source_mapper.h"

namespace cig {
	namespace source {

		source::map mapper::build_map (cig::settings const & settings, source::parser & parser) const {

			source::map 	map;
			source::cursor 	cursor;

			mapper_context cxt {
				*this,
				parser,
				settings,
				map
			};

			while (!(cursor = parser.next()).is_empty())
				cursor_dispatcher.execute (cursor.kind, cxt, cursor);

			return map;
		}

		struct_path_node_kind to_struct_path_node_kind (cursor_kind kind) {
			switch(kind) {
				case cursor_kind::decl_namespace:
					return struct_path_node_kind::namespace_node;
				case cursor_kind::decl_class:
					return struct_path_node_kind::structure_node;
				case cursor_kind::decl_struct:
					return struct_path_node_kind::structure_node;
				default:
					return struct_path_node_kind::unsupported;
			}
		}

		struct_path_node to_struct_path_node (mapper_context & context, source::cursor const & cursor) {
			structure_ptr ptr;

			auto node_kind = to_struct_path_node_kind (cursor.kind);

			if (node_kind == struct_path_node_kind::structure_node)
				ptr = context.map.find_structure(cursor.qualified_name);

			return {
				cursor.identifier,
				ptr,
				node_kind
			};
		}

		source::struct_path make_struct_path (mapper_context & context, source::cursor cursor) {

			auto & 		stack = context.parser.get_current_cursor_stack();
			struct_path path;

			for (auto & c : stack)
				path.push_back (to_semantic_node (context, c));

			return path;

		}

	}
}