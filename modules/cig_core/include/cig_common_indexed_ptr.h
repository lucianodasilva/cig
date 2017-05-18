#ifndef _cig_common_indexed_ptr_h_
#define _cig_common_indexed_ptr_h_

#include <type_traits>

using namespace std;

namespace cig {

	template < class _src_t >
	class indexed_ptr {
	public:

		using source_type	= _src_t;

		using element_type	= typename source_type::value_type;
		using pointer		= typename source_type::pointer;
		using reference		= typename source_type::reference;
		using size_type		= typename source_type::size_type;

		constexpr indexed_ptr() noexcept { reset(); }

		explicit indexed_ptr(_src_t & source, size_type index) { reset(source, index); }

		indexed_ptr(const indexed_ptr & v) { reset(*v._source, v._index); }

		indexed_ptr(indexed_ptr && v) : indexed_ptr () { this->swap(v); }

		indexed_ptr & operator = (const indexed_ptr & v) noexcept {
			reset(*v._source, v._index);
			return *this;
		}

		indexed_ptr & operator = (indexed_ptr && v) noexcept {
			this->swap(v);
			return *this;
		}

		void reset() noexcept {
			_source = nullptr;
			_index = size_type();
		}
		
		void reset(source_type & source, size_type index) {
			_source = &source;
			_index = index;
		}

		void swap(indexed_ptr & r) noexcept {
			std::swap(_source, r._source);
			std::swap(_index, r._index);
		}

		reference operator * () const noexcept { return _source[_index]; }
		pointer operator -> () const noexcept { return &_source[_index]; }

		operator bool() const noexcept { return _source != nullptr; }

	private:

		source_type * 	_source;
		size_type 		_index;

	};

	template < class _lh, class _rh >
	inline bool operator == (const indexed_ptr < _lh > & lhs, const indexed_ptr < _rh > & rhs) noexcept {
		return
			lhs._source == rhs._source &&
			lhs._index == rhs._index;
	}

	template < class _lh, class _rh >
	inline bool operator != (const indexed_ptr < _lh > & lhs, const indexed_ptr < _rh > & rhs) noexcept {
		return
			lhs._source != rhs._source ||
			lhs._index != rhs._index;
	}

	template < class _src_t >
	inline indexed_ptr < _src_t > make_indexed( _src_t & source, typename _src_t::size_type index ) {
		return indexed_ptr < _src_t > { source, index };
	}
	
}

#endif //_cig_common_indexed_ptr_h_
