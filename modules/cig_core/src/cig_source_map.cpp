#include "cig_source_map.h"

namespace cig {
	namespace source {

		structure_ptr map::find_structure(string const & qualified_name) {
			auto it = _struct_index.find (qualified_name);

			if (it == _struct_index.end())
				return {};

			return make_indexed(_structures, it->second);
		}

		structure_const_ptr map::find_structure(string const & qualified_name) const {
			auto it = _struct_index.find (qualified_name);

			if (it == _struct_index.end())
				return {};

			return make_indexed(_structures, it->second);
		}

		structure_ptr map::get_structure(string const & qualified_name) {
			auto ptr = find_structure(qualified_name);

			if (!ptr) {
				auto index = _structures.size();
				_structures.emplace_back();
				_structures.back().qualified_name = qualified_name;

				_struct_index[qualified_name] = index;

				ptr = make_indexed (_structures, index);
			}

			return ptr;
		}

		vector < structure > const map::get_structures() const {
			return _structures;
		}

		type_ptr map::find_type(string const & qualified_name) {
			auto it = _struct_index.find (qualified_name);

			if (it == _struct_index.end())
				return {};

			return make_indexed(_types, it->second);
		}

		type_const_ptr map::find_type(string const & qualified_name) const {
			auto it = _struct_index.find (qualified_name);

			if (it == _struct_index.end())
				return {};

			return make_indexed(_types, it->second);
		}

		type_ptr map::get_type(string const & qualified_name) {
			auto ptr = find_type(qualified_name);

			if (!ptr) {
				auto index = _types.size();
				_types.emplace_back();
				_types.back().qualified_name = qualified_name;

				_struct_index[qualified_name] = index;

				ptr = make_indexed (_types, index);
			}

			return ptr;
		}

		vector < type > const map::get_types() const {
			return _types;
		}

	}
}