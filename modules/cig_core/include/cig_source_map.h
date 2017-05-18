#pragma once
#ifndef _cig_source_map_h_
#define _cig_source_map_h_

#include "cig_source_model.h"

#include <string>
#include <vector>
#include <unordered_map>

using namespace std;

namespace cig {
	namespace source {

		class map {
		public:

			structure_ptr find_structure (string const & qualified_name);
			structure_const_ptr find_structure (string const & qualified_name) const;

			structure_ptr get_structure (string const & qualified_name);

			vector < structure > const get_structures() const;

			type_ptr find_type (string const & qualified_name);
			type_const_ptr find_type (string const & qualified_name) const;

			type_ptr get_type (string const & qualified_name);

			vector < type > const get_types () const;

		private:

			vector < structure >				_structures;
			unordered_map < string, size_t > 	_struct_index;

			vector < type > 					_types;
			unordered_map < string, size_t >	_type_index;

		};

	}
}

#endif //_cig_source_map_h_
