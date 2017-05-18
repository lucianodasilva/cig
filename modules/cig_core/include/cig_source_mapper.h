#pragma once
#ifndef _cig_source_mapper_h_
#define _cig_source_mapper_h_

#include "cig_common.h"
#include "cig_source_map.h"
#include "cig_source_model.h"
#include "cig_source_parser.h"

namespace cig {
	namespace source {

		using cursor_dispatcher = common::dispatcher <
			cursor_kind,
			void (parser_context & context, source::cursor const & cursor)
		>;

		using type_dispatcher = common::dispatcher <
			type_kind,
			type_ptr (parser_context & context, source::cursor_type & type)
		>;

		struct mapper {
		public:

			source::cursor_dispatcher const	cursor_dispatcher;
			source::type_dispatcher const	type_dispatcher;

			void map_cursor (source::parser & parser, source::cursor const & cursor);

			static mapper make_default();

		};

	}
}

#endif //_cig_source_mapper_h_
