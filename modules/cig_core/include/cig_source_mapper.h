#pragma once
#ifndef _cig_source_mapper_h_
#define _cig_source_mapper_h_

#include "cig_common.h"
#include "cig_source_map.h"
#include "cig_source_model.h"
#include "cig_source_parser.h"
#include "cig_settings.h"

namespace cig {
	namespace source {

		struct mapper_context {
			source::mapper const &	mapper;
			source::parser &		parser;
			cig::settings const &	settings;
			source::map &			map;
		};

		using cursor_dispatcher = common::dispatcher <
			cursor_kind,
			void (mapper_context & cxt, source::cursor const & cursor)
		>;

		using type_dispatcher = common::dispatcher <
			type_kind,
			type_ptr (mapper_context & cxt, source::cursor_type const & type)
		>;

		class mapper {
		public:

			source::cursor_dispatcher const	cursor_dispatcher;
			source::type_dispatcher const	type_dispatcher;

			source::map build_map (cig::settings const & settings, source::parser & parser) const;

			static mapper make_default();

		};

		struct_path_node_kind to_struct_path_node_kind (cursor_kind kind);

		struct_path_node to_semantic_node (mapper_context & context, source::cursor const & cursor);

		struct_path make_struct_path (mapper_context & context, source::cursor const & cursor);

	}
}

#endif //_cig_source_mapper_h_
