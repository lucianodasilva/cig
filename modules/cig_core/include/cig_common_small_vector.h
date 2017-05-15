#pragma once
#ifndef _cig_common_small_vector_h_
#define _cig_common_small_vector_h_

#include <algorithm>
#include <cstdio>
#include <cinttypes>
#include <stdexcept>
#include <iterator>
#include <limits>
#include <memory>
#include <type_traits>

#include "cig_common.h"

namespace cig {

	namespace common {

		namespace details {

			/*
			template < class _t, class = void >
			struct is_iterator : std::false_type {};

			template < class _t >
			struct is_iterator<_t, void_t<typename std::iterator_traits<_t>::iterator_category>> :
			std::true_type {};
			*/

			template < class _t >
			struct _not : std::integral_constant < bool, !_t::value > {};

			template < class _t >
			using is_iterator = _not < std::is_integral < _t > >;

			template<class _t, size_t _n>
			struct __typeless_array {
				uint8_t data[sizeof(_t) * _n];

				inline _t &operator[](size_t i) {
					return *reinterpret_cast <_t *> ((i * sizeof(_t)) + data);
				}

				inline const _t *location() const {
					return reinterpret_cast <const _t *> (+data);
				};

				inline _t *location() {
					return reinterpret_cast <_t *> (+data);
				};
			};

		}
	}

	template<class _t>
	struct small_vector_base {
	public:

		using value_type                = _t;
		using reference                 = _t &;
		using const_reference           = _t const &;
		using pointer                   = _t *;
		using const_pointer				= _t const *;

		using size_type					= size_t;
		using difference_type           = ptrdiff_t;

		using iterator                  = pointer;
		using const_iterator            = const_pointer;

		using reverse_iterator          = std::reverse_iterator<iterator>;
		using const_reverse_iterator	= std::reverse_iterator<const_iterator>;

		inline iterator begin() noexcept { return _begin_ptr; }

		inline const_iterator begin() const noexcept { return _begin_ptr; }

		inline iterator end() noexcept { return _end_ptr; }

		inline const_iterator end() const noexcept { return _end_ptr; }

		inline reverse_iterator rbegin() noexcept { return --end(); }

		inline const_reverse_iterator rbegin() const noexcept { return --end(); }

		inline reverse_iterator rend() noexcept { return --begin(); }

		inline const_reverse_iterator rend() const noexcept { return --begin(); }

		inline const_iterator cbegin() const noexcept { return begin(); }

		inline const_iterator cend() const noexcept { return end(); }

		inline const_reverse_iterator crbegin() const noexcept { return rbegin(); }

		inline const_reverse_iterator crend() const noexcept { return rend(); }

		inline size_type size() const noexcept {
			return _end_ptr - _begin_ptr;
		}

		inline size_type max_size() const noexcept { return std::numeric_limits<size_t>::max(); }

		inline size_type capacity() const noexcept {
			return _capacity_ptr - _begin_ptr;
		}

		inline bool empty() const noexcept {
			return _begin_ptr == _end_ptr;
		}

		inline reference front() {
			if (empty())
				throw std::runtime_error("front() called for empty vector");
			return begin()[0];
		}

		inline const_reference front() const {
			if (empty())
				throw std::runtime_error("front() called for empty vector");
			return begin()[0];
		}

		inline reference back() {
			if (empty())
				throw std::runtime_error("back() called for empty vector");
			return end()[-1];
		}

		inline const_reference back() const {
			if (empty())
				throw std::runtime_error("back() called for empty vector");
			return end()[-1];
		}

		inline reference operator[](size_type n) {
			return begin()[n];
		}

		inline const_reference operator[](size_type n) const {
			return begin()[n];
		}

		small_vector_base &operator=(const small_vector_base &v) {
			if (this == &v)
				return *this;

			assign(v.begin(), v.end());

			return *this;
		}

		small_vector_base &operator=(std::initializer_list<value_type> il) {
			assign(il);
			return *this;
		}

		small_vector_base &operator=(small_vector_base &&v) noexcept {
			swap(v);
			return *this;
		}

		inline void reserve(size_type n) {
			if (n > capacity())
				grow_near_pow_2(n);
		}

		inline void shrink_to_fit() noexcept {
			shrink(size());
		}

		template<class _input_it_t>
		inline std::enable_if_t < common::details::is_iterator < _input_it_t >::value, void > 
			assign(_input_it_t first, _input_it_t last) 
		{
			clear();

			size_type elements = last - first;

			if (capacity() < elements)
				grow_near_pow_2(elements);

			set_end(begin() + elements);
			std::uninitialized_copy(first, last, begin());
		}

		inline void assign(size_type n, const value_type &u) {
			clear();

			if (capacity() < n)
				grow_near_pow_2(n);

			set_end(begin() + n);
			std::uninitialized_fill(begin(), end(), u);
		}

		inline void assign(std::initializer_list<value_type> il) {
			assign(il.begin(), il.end());
		}

		inline reference at(size_type n) {
			if (n >= size())
				throw std::out_of_range("small_vector");
			return begin()[n];
		}

		inline const_reference at(size_type n) const {
			if (n >= size())
				throw std::out_of_range("small_vector");
			return begin()[n];
		}

		inline void clear() noexcept {
			destroy_range(begin(), end());
			set_end(begin());
		}

		pointer data() noexcept {
			return _begin_ptr;
		}

		const const_pointer data() const noexcept {
			return _begin_ptr;
		}

		inline void push_back(const_reference x) {
			if (end() >= _capacity_ptr)
				grow();

			new(_end_ptr) _t(x);
			set_end(end() + 1);
		}

		inline void push_back(value_type &&x) {
			if (end() >= _capacity_ptr)
				grow();

			new(end()) _t(std::move(x));
			set_end(end() + 1);
		}

		template<class... _args_tv>
		inline reference emplace_back(_args_tv &&... args) {
			if (end() >= _capacity_ptr)
				grow();

			new(end()) _t(std::forward < _args_tv > (args)...);
			set_end(end() + 1);

			return back ();
		}

		inline void pop_back() {
			if (empty())
				return;

			destroy(end() - 1);
			set_end(end() - 1);

			if (common::next_pow_2(size() - 1) < capacity())
				shrink_to_fit();
		}

		template<class ... _args_tv>
		inline iterator emplace(const_iterator position, _args_tv &&... args) {
			if (position == end()) {
				emplace_back(std::forward < _args_tv > (args)...);
				return end() - 1;
			}

			if (position < begin() || position > end()) {
				throw std::out_of_range("emplace () position out of range");
			}

			size_type offset = position - begin();

			if (_end_ptr == _capacity_ptr)
				grow();

			iterator place = begin() + offset;

			// move items forward
			move_range_reverse(place, end(), end());

			// emplace items
			new(place) _t(std::forward < _args_tv > (args)...);

			set_end(end() + 1);

			return place;
		}

		inline iterator insert(const_iterator position, const value_type &x) {
			return emplace(position, x);
		}

		inline iterator insert(const_iterator position, value_type &&x) {
			return emplace(position, std::move(x));
		}

		inline iterator insert(const_iterator position, size_type n, const value_type &x) {
			size_type offset = position - begin();

			if (n == 0)
				return begin () + offset;

			if (position < begin() || position > end())
				throw std::out_of_range("insert () position out of range");

			if (capacity() < (size() + n))
				grow_near_pow_2(size() + n);

			iterator place = begin() + offset;

			// move items forward
			move_range_reverse(place, end(), end() + (n - 1));
			std::uninitialized_fill_n(place, n, x);

			set_end(end() + n);

			return place;
		}

		template<class _input_it_t>
		inline std::enable_if_t < common::details::is_iterator < _input_it_t >::value, iterator >
			insert(const_iterator position, _input_it_t first, _input_it_t last) 
		{

			size_type n = last - first;
			size_type offset = position - begin();

			if (position < begin() || position > end())
				throw std::out_of_range("insert () position out of range");

			if (capacity() < (size() + n))
				grow_near_pow_2(size() + n);

			auto place = begin() + offset;

			// move items forward
			move_range_reverse(place, end(), end() + n);
			std::copy(first, last, begin() + offset);

			set_end(end() + n);

			return place;
		}

		inline iterator insert(const_iterator position, std::initializer_list<value_type> il) {
			return insert(position, il.begin(), il.end());
		}

		inline iterator erase(const_iterator position) {

			if (empty())
				return end();

			if (position < begin() || position > end())
				throw std::out_of_range("erase () position out of range");

			size_type offset = position - begin();
			iterator pos = begin () + offset;

			// is last item
			if (pos == end() - 1) {
				pop_back();
				return end();
			}

			move_range(pos + 1, end(), pos);
			pop_back();

			return begin () + offset;
		}

		inline iterator erase(const_iterator first, const_iterator last) {

			if (first < begin() || first > end())
				throw std::out_of_range("erase () out of range");

			if (last < first)
				throw std::out_of_range("erase () invalid range");

			if (last > end())
				throw std::out_of_range("erase () last past the end");

			size_type offset = first - begin();
			size_type offset_last = last - begin();

			iterator place = begin() + offset;
			iterator last_place = begin() + offset_last;

			last_place = move_range(last_place, end(), place);
			destroy_range(last_place, end());

			set_end(last_place);

			if (common::next_pow_2(size() - 1) < capacity())
				shrink(common::next_pow_2(size() - 1));

			return begin () + offset;
		}

		inline void resize(size_type sz) {
			resize(sz, _t());
		}

		inline void resize(size_type sz, const value_type &c) {
			if (sz > size())
				insert(end(), sz, c);
			else {
				shrink(sz);
			}
		}

		inline void swap(small_vector_base &v) {
			if (this == &v)
				return;

			// if both are "large"
			if (!is_small() && !v.is_small()) {
				// both "large" just swap all buffers
				swap_itrs(v);
			} else {
				if (size () > v.size())
					swap_vectors(*this, v);
				else
					swap_vectors(v, *this);
			}
		}

		~small_vector_base() {
			destroy_range(begin(), end());
			if (!is_small())
				delete[] _begin_ptr;
		}

	protected:

		inline small_vector_base(size_type n) noexcept {
			_small = {};
			_begin_ptr = _small.location();
			_end_ptr = _begin_ptr;
			_capacity_ptr = _begin_ptr + n;
		}

		inline small_vector_base(size_type n, const_reference v, size_type self_size) noexcept :
			small_vector_base(self_size) {
			assign(n, v);
		}

		small_vector_base(const small_vector_base &v, size_type self_size) :
			small_vector_base(self_size) {
			operator=(v);
		}

		small_vector_base(small_vector_base &&v, size_type self_size) noexcept :
			small_vector_base (self_size)
		{
			swap(v);
		}

		small_vector_base(std::initializer_list<value_type> il, size_type self_size) :
			small_vector_base(self_size) {
			assign(il);
		}

		template<class _input_it_t>
		small_vector_base(
			_input_it_t first, 
			_input_it_t last, 
			size_type self_size
		) :
			small_vector_base(self_size) {
			assign(first, last);
		}

		inline void grow(size_type n) {

			if (n <= capacity())
				return;

			auto new_begin = reinterpret_cast < pointer > (new uint8_t [sizeof (_t) * n]);
			auto data_size = size();

			if (std::is_trivially_copyable<_t>::value) {
				std::copy(begin(), end(), new_begin);
				destroy_range(begin(), end());
			} else {
				move_range(begin(), end(), new_begin);
			}

			if (!is_small())
				delete[] (begin());

			update_itrs(
				new_begin,
				new_begin + data_size,
				new_begin + n
			);
		}

		inline void grow() {
			grow(get_next_capacity (capacity()));
		}

		inline void grow_near_pow_2(size_type size) {
			if (size == 0)
				size = 1;

			grow(get_next_capacity(size - 1));
		}

		inline size_type get_next_capacity(size_type size) {
			size_type next_cap = common::next_pow_2(size);

			if (next_cap == 0) {
				next_cap = this->max_size();
			}

			return next_cap;
		}

		inline void shrink(size_type n) {

			if (is_small() || n >= capacity())
				return;

			auto data_size = std::min(size(), n);
			auto cut_point = begin() + data_size;

			auto new_begin = reinterpret_cast < pointer > (new uint8_t [sizeof (_t) * n]);

			if (std::is_trivially_copyable<_t>::value) {
				std::copy(begin(), cut_point, new_begin);
				destroy_range(begin(), end());
			} else {
				move_range(begin(), cut_point, new_begin);
				destroy_range(cut_point, end());
			}

			delete[] (begin());

			update_itrs(
				new_begin, // begin
				new_begin + data_size, // end
				new_begin + n
			);
		}

		inline bool is_small() const {
			return _begin_ptr == _small.location();
		}

		inline static void destroy(pointer x) {
			x->~_t();
		}

		inline static void destroy_range(pointer b, pointer e) {
			if (!std::is_pointer < _t > ()) {
				while (b < e) {
					--e;
					e->~_t();
				}
			}
		}

		template<class _input_it_t, class _output_it_t>
		inline static _output_it_t move_range(_input_it_t i, _input_it_t e, _output_it_t d) {
			for (; i < e; ++i, ++d) {
				*d = ::std::move(*i);
			}
			return d;
		};

		template<class _input_it_t, class _output_it_t>
		inline static _output_it_t move_range_reverse(_input_it_t i, _input_it_t e, _output_it_t d_e) {
			for (; i < e; --e, --d_e) {
				*d_e = ::std::move(*(e - 1));
			}
			return d_e;
		};

		template < class _input_it_t, class _output_it_t >
		inline static _output_it_t move_uninit_range (_input_it_t i, _input_it_t e, _output_it_t d) {
			for (; i < e; ++i, ++d) {
				new (d) _t (std::move(*i));
			}

			return d;
		};

		inline static void swap_vectors ( small_vector_base & large, small_vector_base & small ) {
			size_type
				large_size = large.size(),
				small_size = small.size();

			if (small.capacity() < large_size)
				small.grow_near_pow_2(large_size);

			// move stuff around
			small.set_end (small.begin() + large_size);
			large.set_end (large.begin() + small_size);

			// move common range
			move_range(
				small.begin(),
				small.begin () + small_size,
				large.begin ()
			);

			// move uninitialized range
			move_uninit_range(
				large.begin() + small_size,
				large.begin() + large_size,
				small.begin () + small_size
			);

			// shrink new small
			large.shrink(small_size);
		}

		inline void swap_itrs(iterator &begin_ptr, iterator &end_ptr, iterator &capacity_ptr) {
			std::swap(_begin_ptr, begin_ptr);
			std::swap(_end_ptr, end_ptr);
			std::swap(_capacity_ptr, capacity_ptr);
		}

		inline void swap_itrs(small_vector_base &v) {
			std::swap(_begin_ptr, v._begin_ptr);
			std::swap(_end_ptr, v._end_ptr);
			std::swap(_capacity_ptr, v._capacity_ptr);
		}

		inline void update_itrs(iterator begin_ptr, iterator end_ptr, iterator capacity_ptr) {
			_begin_ptr = begin_ptr;
			_end_ptr = end_ptr;
			_capacity_ptr = capacity_ptr;
		}

		inline void set_end(iterator end) {
			_end_ptr = end;
		}

		inline iterator capacity_it() {
			return _capacity_ptr;
		}

		pointer _begin_ptr;
		pointer _end_ptr;
		pointer _capacity_ptr;

		common::details::__typeless_array<_t, 1> _small;
		// reserved: do not define any variables after _first
	};

	template<class _t, size_t _n>
	struct small_vector : public small_vector_base<_t> {
	public:

		using value_type		= typename small_vector_base<_t>::value_type;
		using const_reference 	= typename small_vector_base<_t>::const_reference;
		using size_type			= typename small_vector_base<_t>::size_type;

		inline small_vector() noexcept :
			small_vector_base<_t>::small_vector_base(_n) {}

		inline small_vector(size_type n, const_reference v) noexcept :
			small_vector_base<_t>::small_vector_base(n, v, _n) {
		}

		template < size_t _v_n >
		inline small_vector(const small_vector & v ) :
			small_vector_base<_t>::small_vector_base(v, _n) {}

		inline small_vector (const small_vector & v ) :
			small_vector_base<_t>::small_vector_base(v, _n) {}

		inline small_vector(small_vector && v) noexcept :
			small_vector_base<_t>::small_vector_base(std::move(v), _n) {}

		inline small_vector(small_vector_base<_t> && v) noexcept :
			small_vector_base<_t>::small_vector_base(std::move(v), _n) {}

		inline small_vector(std::initializer_list<value_type> il) :
			small_vector_base<_t>::small_vector_base(il, _n) {}

		template<class _input_it_t>
		inline small_vector(
			_input_it_t first, 
			_input_it_t last,
			typename std::enable_if_t < common::details::is_iterator < _input_it_t >::value> * = nullptr
		) :
			small_vector_base<_t>::small_vector_base(first, last, _n) {}

		inline small_vector & operator = (const small_vector_base < _t > & v) {
			small_vector_base < _t >::operator = (v);
			return *this;
		}

		inline small_vector & operator = (const small_vector & v) {
			small_vector_base < _t >::operator = (v);
			return *this;
		}

		inline small_vector & operator = (small_vector_base < _t > && v) {
			small_vector_base < _t >::operator = (std::move(v));
			return *this;
		}

		inline small_vector & operator = (small_vector && v) {
			small_vector_base < _t >::operator = (std::move(v));
			return *this;
		}

		inline small_vector & operator = (std::initializer_list<_t> il) {
			small_vector_base < _t >::operator = (il);
			return *this;
		}

	private:
		common::details::__typeless_array<_t, _n - 1> _small_data;
	};

}

#endif