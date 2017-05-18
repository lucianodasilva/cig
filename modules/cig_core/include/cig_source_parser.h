#ifndef _cig_source_parser_h_
#define _cig_source_parser_h_

#include "cig_settings.h"
#include "cig_source_map.h"

namespace cig {
	namespace source {

		class parser;

		struct parser_context {
			source::parser &	parser;
			cig::settings 		settings;
			map 				wip_map;
		};

		class parser {
		public:

			virtual source::cursor get_semantic_parent (source::cursor const & cursor) = 0;

		};

	}
}

#endif //_cig_source_parser_h_
