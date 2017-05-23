#include "cig_source_model.h"

namespace cig {
	namespace source {

		void structure::apply_cursor (structure & strct, source::cursor const & cursor) {
			strct.identifier = cursor.identifier;
		}

	}
}