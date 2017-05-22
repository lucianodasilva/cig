#include "cig_source_mapper.h"

namespace cig {
	namespace source {

		semantic_node_kind to_semantic_node_kind (cursor_kind kind) {
			switch(kind) {
				case cursor_kind::decl_namespace:
					return semantic_node_kind::namespace_node;
				case cursor_kind::decl_class:
					return semantic_node_kind::structure_node;
				case cursor_kind::decl_struct:
					return semantic_node_kind::structure_node;
				default:
					return semantic_node_kind::unsupported;
			}
		}

		semantic_node to_semantic_node (parser_context & context, source::cursor const & cursor) {
			structure_ptr ptr;

			auto node_kind = to_semantic_node_kind (cursor.kind);

			if (node_kind == semantic_node_kind::structure_node)
				ptr = context.wip_map.find_structure(cursor.qualified_name);

			return {
				cursor.identifier,
				ptr,
				node_kind
			};
		}

		semantic_path make_semantic_path (parser_context & context, source::cursor cursor) {

			semantic_path semantic_stack;

			while(!cursor.is_empty()) {
				cursor = context.parser.get_semantic_parent(cursor);
				semantic_stack.push_back(to_semantic_node(context, cursor));
			}

			reverse(semantic_stack.begin(), semantic_stack.end());

			return semantic_stack;

		}

	}
}