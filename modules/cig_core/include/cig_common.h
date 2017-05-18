#ifndef _cig_common_h_
#define _cig_common_h_

#include <cinttypes>
#include <utility>

using namespace std;

namespace cig {

	struct no_copy {
	protected:
		no_copy() = default;
	public:
		no_copy(const no_copy &) = delete;
		no_copy & operator = (const no_copy &) = delete;
	};

	struct move_only : public no_copy {
	public:
		move_only(move_only &&) = default;
	};

	namespace common {
		namespace details {
			// scope guard implementation
			template<class _method_t>
			class scope_guard {
			private:

				bool _active;
				_method_t _method;

			public:

				// remove copy
				scope_guard() = delete;

				scope_guard(const scope_guard<_method_t> &) = delete;

				scope_guard<_method_t> operator=(const scope_guard<_method_t> &) = delete;

				// new instance
				inline scope_guard(_method_t m) :
					_active(true),
					_method(std::move(m)) {}

				// move operator
				inline scope_guard(scope_guard<_method_t> &&o) :
					_active(o._active),
					_method(std::move(o._method)) {
					o.dismiss();
				}

				inline ~scope_guard() {
					trigger();
				}

				inline void trigger() {
					if (_active)
						_method();

					_active = false;
				}

				inline void dismiss() { _active = false; }

			};

		}

		template < class _t >
		constexpr inline _t next_pow_2(_t v) {

			_t bit_ceil = sizeof(_t) * 8;
			_t bit = 1;

			while (bit < bit_ceil) {
				v |= (v >> bit);
				bit = bit << 1;
			}

			return v + 1;
		}

	}

	template < class _method_t >
	inline common::details::scope_guard < _method_t > scope_guard(_method_t m) {
		return common::details::scope_guard < _method_t >(std::move(m));
	}

	template < class arg >
	inline void guard_dismiss(common::details::scope_guard < arg > & g) {
		g.dismiss();
	}

	template < class arg_1, class ... _arg_v >
	inline void guard_dismiss(common::details::scope_guard < arg_1 > & g1, common::details::scope_guard < _arg_v > & ... gv) {
		guard_dismiss(g1);
		guard_dismiss(gv...);
	}

#	define macro_cat_impl(x,y) x##y
#	define macro_cat(x,y) macro_cat_impl (x, y)
#	define auto_guard auto macro_cat (__scopeguard_, __LINE__) = cig::scope_guard

}

#include "cig_common_dispatcher.h"
#include "cig_common_indexed_ptr.h"
#include "cig_common_small_vector.h"

#endif //_cig_common_h_
