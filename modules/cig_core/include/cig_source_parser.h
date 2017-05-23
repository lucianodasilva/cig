#ifndef _cig_source_parser_h_
#define _cig_source_parser_h_

#include "cig_settings.h"
#include "cig_source_map.h"

#include <string>

namespace std;

namespace cig {
	namespace source {

		class parser {
		public:

			// visitor state
			virtual cursor next() = 0;
			virtual cursor_stack const & get_current_cursor_stack () = 0;

			// get type info
			virtual cursor 		get_type_declaration 	(cursor_type const & type) const = 0;
			virtual cursor_type get_canonical_type 		(cursor_type const & type) const = 0;
			virtual bool 		is_const_qualified 		(cursor_type const & type) const = 0;

			// get cursor info
			virtual visibility 	get_visibility 			(source::cursor const & cursor) const = 0;
			virtual cursor_type get_type 				(source::cursor const & cursor) const = 0;
		};

	}
}

#endif //_cig_source_parser_h_
