#pragma once
#ifndef _cig_source_cursor_handlers_h_
#define _cig_source_cursor_handlers_h_

#include "cig_source_mapper.h"

namespace cig {
	namespace source {
		namespace cursor_handlers {

			void cursor_default_handler (mapper_context & cxt, const source::cursor & cursor);

			void struct_handler (mapper_context & cxt, const source::cursor & cursor);

			void class_handler (mapper_context & cxt, const source::cursor & cursor);

			void base_spec_handler (mapper_context & cxt, const source::cursor & cursor);

			void field_handler (mapper_context & cxt, const source::cursor & cursor);

			void method_handler (mapper_context & cxt, const source::cursor & cursor);

			void function_handler (mapper_context & cxt, const source::cursor & cursor);

			void parameter_handler (mapper_context & cxt, const source::cursor & cursor);

			void namespace_handler (mapper_context & cxt, const source::cursor & cursor);

		}
	}
}

#endif //_cig_source_cursor_handlers_h_
