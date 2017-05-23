#pragma once
#ifndef _cig_source_type_handlers_h_
#define _cig_source_type_handlers_h_

#include "cig_source_model.h"
#include "cig_source_mapper.h"

namespace cig {
	namespace source {
		namespace type_handlers {

			void inplace_struct_handler (mapper_context & cxt, const parser::cursor_type & parser_type, parser::type & source_type, const template_arg_vector & template_arguments = template_arg_vector ());

			type_ptr type_default_handler (mapper_context & cxt, const parser::cursor_type & type);

			type_ptr type_reference_handler (mapper_context & cxt, const parser::cursor_type & type);

			type_ptr type_struct_handler (mapper_context & cxt, const parser::cursor_type & type);

			type_ptr type_unhandled_handler (mapper_context & cxt, const parser::cursor_type & type);

			type_ptr type_enum_handler (mapper_context & cxt, const parser::cursor_type & type);

			type_ptr type_array_handler (mapper_context & cxt, const parser::cursor_type & type);

		}
	}
}

#endif //_cig_source_type_handlers_h_
