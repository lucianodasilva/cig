#include <catch.hpp>
#include <cig_core.h>
#include <memory>
#include <random>
#include <utility>
#include <vector>

using namespace std;
using namespace cig;

#ifdef _MSC_VER
#define no_inline __declspec(noinline)
#else
#define no_inline __attribute__((__noinline__))
#endif

namespace cig {
    namespace tests {

		struct assign_counters {
			size_t copy_counter;
			size_t move_counter;

			inline void reset () {
				copy_counter = 0;
				move_counter = 0;
			}

			inline bool check_copy(size_t v) {
				return copy_counter == v;
			}

			inline bool check_move(size_t v) {
				return move_counter == v;
			}
		};

		// structures to be moved
		struct test_item {

			assign_counters * counters = nullptr;

            int value = 0;

            test_item(assign_counters & counters_v, int v) :
				counters (&counters_v),
                value(v)
            {}

            test_item(test_item && m)
			{
                operator = (forward<test_item>(m));
			}

            test_item(const test_item & m)
			{
				operator = (m);
			}

            test_item & operator = (const test_item & m) {
				counters 	= m.counters;
                value 		= m.value;

				++(counters->copy_counter);
				return *this;
			}

            test_item & operator = (test_item && m) {
				this->swap(m);
                ++(counters->move_counter);
                return *this;
			}

            void swap (test_item & v) {
                std::swap(counters, v.counters);
                std::swap(value, v.value);
            }

			static inline void init_items(small_vector_base < test_item > & victim, size_t item_count, assign_counters & counters) {
				for (int i = 0; i < item_count; ++i) {
					victim.emplace_back(
						counters,
                        i
					);
				}
			}

		};

        inline bool operator == (const test_item & v1, const test_item & v2) {
            return v1.value == v2.value;
        }

        SCENARIO("small_vector push_back operations", "[small_vector]"){

            assign_counters counters;

            auto make_item = [&](int v) -> test_item {
                return { counters, v };
            };

            GIVEN("a vector containing some items"){

                size_t const initial_size = 10;
                size_t const initial_capacity = initial_size * 2;

                small_vector < test_item, initial_capacity > victim;

                test_item::init_items(victim, initial_size, counters);

                counters.reset();

                WHEN("an item is added as lvalue") {
                    const auto added_value = make_item(999);

                    victim.push_back (added_value);

                    THEN("size must change and value must be present by copy"){
                        REQUIRE(victim.size() == initial_size + 1);
                        REQUIRE(victim.back() == added_value);

						REQUIRE(counters.check_move(0));
                        REQUIRE(counters.check_copy(1));
                    }
                }
                WHEN("an item is added as rvalue") {
                    int const item_value = 999;
                    auto added_value = make_item(item_value);

                    victim.push_back (std::move(added_value));

                    THEN("size must change and value must be present by move"){
                        REQUIRE(victim.size() == initial_size + 1);
                        REQUIRE(victim.back().value == item_value);

                        REQUIRE(counters.check_move(1));
						REQUIRE(counters.check_copy(0));
                    }
                }
            }
        }

        SCENARIO("small_vector construction", "[small_vector]") {

            size_t const initial_size = 10;
            size_t const initial_capacity = initial_size * 2;

            GIVEN("a small_vector construction"){
                WHEN("default") {
                    small_vector < int, initial_capacity > victim;
                    THEN("capacity must be as setted"){
                        REQUIRE(victim.size() == 0);
                        REQUIRE(victim.capacity() == initial_capacity);
                    }
                }
                WHEN("copy iterators"){
                    vector < int > const expectancy = {
                        1, 3, 5, 8, 13
                    };

                    small_vector < int, initial_capacity > victim (
                        expectancy.begin (), expectancy.end()
                    );

                    THEN("construction by iterative copy") {
                        REQUIRE(victim.size() == expectancy.size());
                        REQUIRE(victim.capacity() == initial_capacity);
                        REQUIRE(std::equal(
                                expectancy.begin(), expectancy.end(),
                                victim.begin(), victim.end()
                        ));
                    }
                }
                WHEN("copy constructor"){
                    small_vector < int, initial_capacity > const expectancy = {
                            1, 3, 5, 8, 13
                    };

                    small_vector < int, initial_capacity > victim = expectancy;

                    THEN("construction by iterative copy") {
                        REQUIRE(victim.size() == expectancy.size());
                        REQUIRE(victim.capacity() == initial_capacity);
                        REQUIRE(std::equal(
                                expectancy.begin(), expectancy.end(),
                                victim.begin(), victim.end()
                        ));
                    }
                }
                WHEN("move constructor"){

                    assign_counters counters;

                    small_vector < test_item, initial_capacity > source;

                    test_item::init_items(source, initial_size, counters);

                    small_vector < test_item, initial_capacity > victim = std::move(source);

                    THEN("construction by move") {
                        REQUIRE(victim.size() == initial_size);

                        REQUIRE(counters.check_copy(0));
                        REQUIRE(counters.check_move(initial_size));
                    }
                }
                WHEN("initializer list constructor") {
                    std::initializer_list < int > expectancy = { 1, 2, 3, 4, 5, 6, 7 ,8 ,9, 10 };

                    small_vector < int, initial_capacity > victim = expectancy;

                    THEN("construction by initializer list copy") {
                        REQUIRE(std::equal(
                                expectancy.begin(), expectancy.end(),
                                victim.begin(), victim.end()
                        ));
                    }
                }
            }
        }
		SCENARIO("small_vector destruction", "[small_vector]"){

			GIVEN("a structure item type") {

				size_t destroyed_item_count = 0;

				struct dctor_item {
					size_t * destroyed_item_count_ptr = nullptr;

					dctor_item() {}
					dctor_item (size_t * counter) : destroyed_item_count_ptr (counter) {}

					~dctor_item() {
						if (destroyed_item_count_ptr)
							++(*destroyed_item_count_ptr);
					}
				};

				WHEN("items are directly contained") {

					{
						small_vector < dctor_item, 10 > victim;
						victim.push_back({&destroyed_item_count});
						// reset counter to cancel the destruction of the copied parameter
						destroyed_item_count = 0;
					}

					THEN("items constructures should also be invoked"){
						REQUIRE(destroyed_item_count == 1);
					}
				}
				WHEN("items are heap allocated") {
					auto item = std::make_unique < dctor_item > (&destroyed_item_count);

					{

						small_vector < dctor_item *, 10 > victim;
						victim.push_back(item.get());
						// reset counter to cancel the destruction of the copied parameter
						destroyed_item_count = 0;
					}

					THEN("items constructures should also be invoked"){
						REQUIRE(destroyed_item_count == 0);
					}
				}
			}
		}
        SCENARIO("small_vector attribution", "[small_vector]") {

            size_t const initial_size = 10;
            size_t const initial_capacity = initial_size * 2;

            assign_counters counters;

            auto make_item = [&](int v) -> test_item {
                return { counters, v };
            };

            small_vector < test_item, initial_capacity > victim;

            GIVEN("lvalue assigned vector"){
                WHEN("assigned vector lesser than capacity"){
                    small_vector < test_item, initial_capacity > assigned;

                    test_item::init_items(assigned, initial_capacity, counters);
                    counters.reset ();

                    victim = assigned;

                    THEN("assignment by copy"){
                        REQUIRE(victim.capacity () == assigned.capacity());
                        REQUIRE(victim.size() == assigned.size());

                        REQUIRE(counters.check_copy(assigned.size()));
                        REQUIRE(counters.check_move(0));

                        REQUIRE(std::equal(
                                assigned.begin(), assigned.end(),
                                victim.begin(), victim.end()
                        ));
                    }
                }
				WHEN("assigned vector greater than victim capacity"){
                    size_t const initial_assigned_size = initial_capacity + initial_size;

                    size_t const expected_capacity = common::next_pow_2(
                            initial_assigned_size
                    );

                    small_vector < test_item, initial_assigned_size > assigned;

                    test_item::init_items(assigned, initial_assigned_size, counters);
                    counters.reset ();

                    victim = assigned;

                    THEN("assignment by copy and capacity growth"){
                        REQUIRE(victim.capacity () == expected_capacity);
                        REQUIRE(victim.size() == initial_assigned_size);

                        REQUIRE(counters.check_copy(initial_assigned_size));
                        REQUIRE(counters.check_move(0));

                        REQUIRE(std::equal(
                                assigned.begin(), assigned.end(),
                                victim.begin(), victim.end()
                        ));
                    }
                }
            }
            GIVEN("rvalue vector"){
                WHEN("assigned vector lesser than capacity"){
                    small_vector < test_item, initial_capacity > assigned;

                    test_item::init_items(assigned, initial_capacity, counters);
                    counters.reset ();

                    victim = std::move(assigned);

                    THEN("assignment by move"){
                        REQUIRE(victim.capacity () == initial_capacity);
                        REQUIRE(victim.size() == initial_capacity);

                        REQUIRE(counters.check_copy(0));
                        REQUIRE(counters.check_move(assigned.capacity()));
                    }
                }
                WHEN("assigned vector greater than victim capacity"){
                    size_t const initial_assigned_size = initial_capacity + initial_size;

                    size_t const expected_capacity = common::next_pow_2(
                            initial_assigned_size
                    );

                    small_vector < test_item, initial_assigned_size > assigned;

                    test_item::init_items(assigned, initial_assigned_size, counters);
                    counters.reset ();

                    victim = std::move(assigned);

                    THEN("assignment by move and capacity growth"){
                        REQUIRE(victim.capacity () == expected_capacity);
                        REQUIRE(victim.size() == initial_assigned_size);

                        REQUIRE(counters.check_copy(0));
                        REQUIRE(counters.check_move(initial_assigned_size));
                    }
                }
            }
			GIVEN("initializer an list"){

				std::initializer_list < test_item > expectancy = {
					make_item (1),
					make_item (2),
					make_item (3),
					make_item (4),
					make_item (5)
				};

				counters.reset();

				WHEN("on assignment") {

					victim = expectancy;

					THEN("copy assignement of initialized list items"){
						REQUIRE(victim.size() == expectancy.size());

						REQUIRE(counters.check_copy(expectancy.size()));
						REQUIRE(counters.check_move(0));

						REQUIRE(std::equal(
							expectancy.begin(), expectancy.end(),
							victim.begin(), victim.end()
						));
					}
				}
			}
        }
		SCENARIO("small_vector assign", "[small_vector]"){

			size_t const initial_size = 10;
			size_t const initial_capacity = initial_size * 2;

			assign_counters counters;

			auto make_item = [&](int v) -> test_item {
				return { counters, v };
			};

			small_vector < test_item, initial_capacity > victim;

			GIVEN("assign operation call") {
				WHEN("count and default value") {
					test_item item = make_item (999);

					victim.assign(initial_size, item);

					THEN("should be assigned to n copies of the default value"){
						REQUIRE(
							std::count (victim.begin(), victim.end(), item) == initial_size
						);
					}
				}
				WHEN("iterators") {

					small_vector < test_item, initial_size > expectancy;
					test_item::init_items(expectancy, initial_size, counters);

					counters.reset();

					victim.assign(expectancy.begin(), expectancy.end());

					THEN("should be assigned to copies of iterated values"){
						REQUIRE(std::equal(
							expectancy.begin(), expectancy.end(),
							victim.begin(), victim.end()
						));

						REQUIRE(counters.check_move(0));
						REQUIRE(counters.check_copy(initial_size));
					}
				}
				WHEN("initializer list") {

					std::initializer_list < test_item > expectancy = {
						make_item (1),
						make_item (2),
						make_item (3),
						make_item (4),
						make_item (5)
					};

					counters.reset();

					victim.assign(expectancy);

					THEN("should be assigned to copies of initializer list values"){
						REQUIRE(std::equal(
							expectancy.begin(), expectancy.end(),
							victim.begin(), victim.end()
						));

						REQUIRE(counters.check_move(0));
						REQUIRE(counters.check_copy(expectancy.size()));
					}
				}
			}
		}
		SCENARIO("small_vector accessor operations", "[small_vector]"){

			size_t const initial_size = 10;
			size_t const initial_capacity = initial_size * 2;

			small_vector < int, initial_capacity > victim = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };

			GIVEN("operation at"){
				WHEN("within range") {
					THEN("should return proper value"){
						REQUIRE(victim.at(5) == victim.data()[5]);
					}
				}
				WHEN("out of range") {
					THEN("should throw out of range exception"){
						REQUIRE_THROWS_AS(victim.at(initial_size + initial_size), std::out_of_range);
					}
				}
			}
			GIVEN("operator indexer"){
				WHEN("within range"){
					THEN("should return proper value"){
						REQUIRE(victim[5] == victim.data()[5]);
					}
				}
			}
			GIVEN("operation back"){
				WHEN("with elements"){
					THEN("should return last item"){
						REQUIRE(victim.back() == victim.data()[9]);
					}
				}
				WHEN("const"){
					auto const & const_victim = victim;
					THEN("should return last item"){
						REQUIRE(const_victim.back() == const_victim.data()[9]);
					}
				}
			}
			GIVEN("operation front"){
				WHEN("with elements"){
					THEN("should return first item"){
						REQUIRE(victim.front() == victim.data()[0]);
					}
				}
				WHEN("const"){
					auto const & const_victim = victim;
					THEN("should return first item"){
						REQUIRE(const_victim.front() == const_victim.data()[0]);
					}
				}
			}
			GIVEN("operation data"){
				WHEN("with elements"){
					THEN("should return pointer to data"){
						REQUIRE(victim.data()[0] == victim.front());
					}
				}
				WHEN("const"){
					THEN("should return pointer to data"){
						auto const & const_victim = victim;
						REQUIRE(const_victim.data()[0] == const_victim.front());
					}
				}
			}
		}
		SCENARIO("small_vector accessor reference editing", "[small_vector]"){

			size_t const initial_size = 10;
			size_t const initial_capacity = initial_size * 2;

			small_vector < int, initial_capacity > victim = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };

			random_device rdev;
			default_random_engine re (rdev());

			uniform_int_distribution < int > value_dist (1000, 10000);

			GIVEN("operation at"){
				WHEN("within range") {
					auto change_value = value_dist(re);

					victim.at(5) = change_value;

					THEN("should have edited value"){
						REQUIRE(victim [5] == change_value);
					}
				}
			}
			GIVEN("operator indexer"){
				WHEN("within range"){
					auto change_value = value_dist(re);

					victim[5] = change_value;

					THEN("should have edited value"){
						REQUIRE(victim [5] == change_value);
					}
				}
			}
			GIVEN("operation back"){
				WHEN("with elements"){
					auto change_value = value_dist(re);

					victim.back() = change_value;

					THEN("should have edited value"){
						REQUIRE(victim.back() == change_value);
					}
				}
			}
			GIVEN("operation front"){
				WHEN("with elements"){
					auto change_value = value_dist(re);

					victim.front() = change_value;

					THEN("should have edited value"){
						REQUIRE(victim.front() == change_value);
					}
				}
			}
		}
		SCENARIO("small_vector state operations", "[small_vector]") {

			size_t const initial_size = 10;
			size_t const initial_capacity = initial_size * 2;

			small_vector<int, initial_capacity> victim = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

			GIVEN("empty"){
				WHEN("is empty"){
					small_vector<int, initial_capacity> empty_victim;

					THEN("empty should be true"){
						REQUIRE(empty_victim.empty());
					}
				}
				WHEN("is not empty"){
					THEN("empty should be false"){
						REQUIRE(!victim.empty());
					}
				}
			}
			GIVEN("size"){
				WHEN("called"){
					THEN("should return the appropriate number of contained items"){
						REQUIRE(victim.size() == initial_size);
					}
				}
			}
			GIVEN("max_size"){
				WHEN("called"){
					THEN("should return the expected maximum number of contained items"){
						auto expected_max_size = std::numeric_limits < typename small_vector < int, initial_capacity >::size_type >::max();
						REQUIRE(victim.max_size() == expected_max_size);
					}
				}
			}
		}
		SCENARIO("small_vector manage capacity", "[small_vector]"){

			size_t const initial_size = 10;
			size_t const initial_capacity = initial_size * 2;

			small_vector<int, initial_capacity> victim = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

			GIVEN("reserve") {
				WHEN("called with larger capacity"){
					auto const new_capacity = initial_capacity * 2;

					victim.reserve(new_capacity);

					THEN("small_vector capacity should change to the nearest pow of 2"){
						auto const expected_capacity = common::next_pow_2(new_capacity);
						REQUIRE(victim.capacity() == expected_capacity);
					}
				}
				WHEN("called with smaller capacity"){
					auto const new_capacity = initial_capacity / 2;

					victim.reserve(new_capacity);

					THEN("small_vector capacity should have remained the same"){
						REQUIRE(victim.capacity() == initial_capacity);
					}
				}
			}
		}
		SCENARIO("small_vector shrink_to_fit", "[small_vector]"){

			size_t const initial_size = 10;
			size_t const initial_capacity = initial_size * 2;

			small_vector<int, initial_capacity> victim = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

			GIVEN("shrink_to_fit") {
				WHEN("called with 'large' small_vector"){
					auto const new_capacity = initial_capacity * 2;
					victim.reserve(new_capacity);

					victim.shrink_to_fit();
					THEN("small_vector capacity should shrink to data"){
						REQUIRE(victim.capacity() == victim.size());
					}
				}
				WHEN("called with 'small' small_vector"){
					victim.shrink_to_fit();

					THEN("small_vector capacity should remain the same"){
						REQUIRE(victim.capacity() == initial_capacity);
					}
				}
			}
		}
		SCENARIO("small_vector clear", "[small_vector]"){
			size_t const initial_size = 10;
			size_t const initial_capacity = initial_size * 2;

			GIVEN("a small_vector containing items"){
				small_vector<int, initial_capacity> victim = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

				WHEN("clear called") {
					victim.clear();

					THEN("items should be removed"){
						REQUIRE(victim.size() == 0);
						REQUIRE(victim.capacity() == initial_capacity);
					}
				}
			}
			GIVEN("an empty small_vector"){
				small_vector<int, initial_capacity> victim;

				WHEN("clear called") {
					victim.clear();

					THEN("nothing should happen"){
						REQUIRE(victim.size() == 0);
						REQUIRE(victim.capacity() == initial_capacity);
					}
				}
			}
		}
		SCENARIO("small_vector insert", "[small_vector]"){
			size_t const initial_size = 10;
			size_t const initial_capacity = initial_size * 2;

			assign_counters counters;

			auto make_item = [&](int v) -> test_item {
				return { counters, v };
			};

			random_device rdev;
			default_random_engine re (rdev());

			uniform_int_distribution < int > value_dist (1000, 10000);

			GIVEN("empty small_vector") {

				small_vector < test_item, initial_capacity > victim;

				WHEN("insert lvalue at begin"){
					auto item = make_item(1234);

					counters.reset();

					victim.insert(victim.begin(), item);

					THEN("value should have been inserted in the first position"){
						REQUIRE(victim.size() == 1);
						REQUIRE(victim.back() == item);

						REQUIRE(counters.check_copy(1));
						REQUIRE(counters.check_move(0));
					}
				}
                WHEN("insert rvalue at begin"){
                    auto item = make_item(1234);

                    counters.reset();

                    victim.insert(victim.begin(), std::move(item));

                    THEN("value should have been inserted in the first position"){
                        REQUIRE(victim.size() == 1);
                        REQUIRE(victim.back().value == 1234);

                        REQUIRE(counters.check_copy(0));
                        REQUIRE(counters.check_move(1));
                    }
                }
				WHEN("insert lvalue at invalid position"){
					auto item = make_item(1234);

					counters.reset();

					THEN("throw out_of_range"){
						REQUIRE_THROWS_AS(victim.insert(victim.end() + 1, item), std::out_of_range);
					}
				}
                WHEN("insert rvalue at invalid position"){
                    auto item = make_item(1234);

                    counters.reset();

                    THEN("throw out_of_range"){
                        REQUIRE_THROWS_AS(victim.insert(victim.end() + 1, std::move(item)), std::out_of_range);
                    }
                }
			}
			GIVEN("filled small_vector") {

				small_vector < test_item, initial_capacity > victim;

				test_item::init_items(victim, initial_size, counters);
				counters.reset ();

				WHEN("insert lvalue at begin"){
					auto item = make_item(1234);

					counters.reset();

					victim.insert(victim.begin(), item);

					THEN("value should have been inserted in the first position"){
						REQUIRE(victim.size() == initial_size + 1);
						REQUIRE(victim.front() == item);

						REQUIRE(counters.check_copy(1));
						REQUIRE(counters.check_move(10));
					}
				}
                WHEN("insert lvalue at middle"){
                    auto item = make_item(1234);

                    counters.reset();

                    size_t offset = victim.size () / 2;
                    victim.insert(victim.begin() + offset, item);

                    THEN("value should have been inserted in the middle position"){
                        REQUIRE(victim.size() == initial_size + 1);
                        REQUIRE(victim[offset] == item);

                        REQUIRE(counters.check_copy(1));
                        REQUIRE(counters.check_move(offset));
                    }
                }
                WHEN("insert lvalue at end"){
                    auto item = make_item(1234);

                    counters.reset();

                    victim.insert(victim.end(), item);

                    THEN("value should have been inserted in the last position"){
                        REQUIRE(victim.size() == initial_size + 1);
                        REQUIRE(victim.back() == item);

                        REQUIRE(counters.check_copy(1));
                        REQUIRE(counters.check_move(0));
                    }
                }
				WHEN("insert lvalue at invalid position"){
					auto item = make_item(1234);

					counters.reset();

					THEN("throw out_of_range"){
						REQUIRE_THROWS_AS(victim.insert(victim.end() + 1, item), std::out_of_range);
					}
				}
                WHEN("insert rvalue at begin"){
                    auto item = make_item(1234);

                    counters.reset();

                    victim.insert(victim.begin(), std::move(item));

                    THEN("value should have been inserted in the first position"){
                        REQUIRE(victim.size() == initial_size + 1);
                        REQUIRE(victim.front().value == 1234);

                        REQUIRE(counters.check_copy(0));
                        REQUIRE(counters.check_move(11));
                    }
                }
                WHEN("insert rvalue at middle"){
                    auto item = make_item(1234);

                    counters.reset();

                    size_t offset = victim.size () / 2;
                    victim.insert(victim.begin() + offset, std::move(item));

                    THEN("value should have been inserted in the middle position"){
                        REQUIRE(victim.size() == initial_size + 1);
                        REQUIRE(victim[offset].value == 1234);

                        REQUIRE(counters.check_copy(0));
                        REQUIRE(counters.check_move(1 + offset));
                    }
                }
                WHEN("insert rvalue at end"){
                    auto item = make_item(1234);

                    counters.reset();

                    victim.insert(victim.end(), std::move(item));

                    THEN("value should have been inserted in the last position"){
                        REQUIRE(victim.size() == initial_size + 1);
                        REQUIRE(victim.back().value == 1234);

                        REQUIRE(counters.check_copy(0));
                        REQUIRE(counters.check_move(1));
                    }
                }
                WHEN("insert rvalue at invalid position"){
                    auto item = make_item(1234);

                    counters.reset();

                    THEN("throw out_of_range"){
                        REQUIRE_THROWS_AS(victim.insert(victim.end() + 1, std::move(item)), std::out_of_range);
                    }
                }
			}
		}
	}
}
/*

		TEST(unit_small_vector_test, insert_n_copies_at_position) {

			size_t const insert_value = 1;
			size_t const insert_count = 5;
			size_t const border_count = 4;
			size_t const half_border = border_count / 2;
			size_t const expected_size = insert_count + border_count;
			size_t const border_value = 8;

			small_vector < int, unit_small_vector_test::test_size >
				victim(border_count, border_value);

			auto position = victim.insert(victim.begin() + half_border, insert_count, insert_value);

			EXPECT_EQ(expected_size, victim.size());

			const size_t left_border_count = std::count(
				victim.begin(), victim.begin() + half_border,
				border_value
			);

			EXPECT_EQ(left_border_count, half_border);

			const size_t right_border_count = std::count(
				victim.end() - half_border, victim.end(),
				border_value
			);

			EXPECT_EQ(right_border_count, half_border);

			const size_t inserted_items_count = std::count(
				victim.begin() + half_border, victim.end() - half_border,
				insert_value
			);

			EXPECT_EQ(inserted_items_count, insert_count);

			EXPECT_EQ(position, victim.begin() + half_border);
		}

		TEST(unit_small_vector_test, insert_zero_copies_at_position) {

			size_t const expected_count = 8;
			size_t const expected_value = 8;

			size_t const expected_pos = expected_count / 2;

			small_vector < int, unit_small_vector_test::test_size >
				victim(expected_count, expected_value);

			auto position = victim.insert(victim.begin() + expected_pos, 0, expected_value);

			EXPECT_EQ(victim.size(), expected_count);
			EXPECT_EQ(position, victim.begin() + expected_pos);
		}

		TEST(unit_small_vector_test, insert_position_end) {

			size_t const initial_count = 8;
			size_t const initial_value = 8;
			size_t const insert_count = 3;
			size_t const insert_value = 3;

			size_t const expected_count = initial_count + insert_count;

			small_vector < int, unit_small_vector_test::test_size >
				victim(initial_count, initial_value);

			victim.insert(victim.end(), insert_count, insert_value);

			EXPECT_EQ(expected_count, victim.size());

			EXPECT_EQ(
				std::count(victim.end() - insert_count, victim.end(), insert_value),
				insert_count
			);
		}

		TEST(unit_small_vector_test, insert_position_out_of_range) {

			std::unique_ptr < int > dummy_pos = make_unique < int >(123);

			small_vector < int, unit_small_vector_test::test_size >
				victim(8, 8);

			EXPECT_THROW(victim.insert(dummy_pos.get(), 8, 8), std::out_of_range);
		}

		TEST(unit_small_vector_test, insert_position_span) {

			small_vector < int, unit_small_vector_test::test_size >
				victim = { 0, 1, 2, 3, 4, 5 };

			size_t
				victim_original_size = victim.size(),
				victim_original_half_size = victim_original_size / 2;

			auto insert_pos = victim.begin () + victim_original_half_size;

			small_vector < int, unit_small_vector_test::test_size >
				source = { 10, 11, 12, 13 };

			auto pos = victim.insert(insert_pos, source.begin(), source.end());

			EXPECT_EQ(victim_original_size + source.size(), victim.size());

			EXPECT_TRUE(std::equal(
				pos, pos + source.size(),
				source.begin(), source.end()
			));
		}

		TEST(unit_small_vector_test, insert_position_span_out_of_range) {

			std::unique_ptr < int > dummy_pos = make_unique < int >(123);

			small_vector < int, unit_small_vector_test::test_size >
				victim(8, 8);

			small_vector < int, unit_small_vector_test::test_size >
				source = { 10, 11, 12, 13 };

			EXPECT_THROW(victim.insert(dummy_pos.get(), source.begin(), source.end()), std::out_of_range);
		}

		TEST(unit_small_vector_test, insert_position_init_list) {

			small_vector < int, unit_small_vector_test::test_size >
				victim = { 0, 1, 2, 3, 4, 5 };

			size_t
				victim_original_size = victim.size(),
				victim_original_half_size = victim_original_size / 2;

			auto insert_pos = victim.begin() + victim_original_half_size;

			std::initializer_list < int >
				source = { 10, 11, 12, 13 };

			auto pos = victim.insert(insert_pos, source);

			EXPECT_EQ(victim_original_size + source.size(), victim.size());

			EXPECT_TRUE(std::equal(
				pos, pos + source.size(),
				source.begin(), source.end()
			));
		}

		TEST(unit_small_vector_test, emplace_at_position) {

			int const value_a = 999;
			int const value_b = 666;

			int const insert_position = 5;
			int const expected_size = 11;

			struct test_struct {
				int a;
				int b;

				test_struct() = default;
				test_struct(const test_struct &) = default;
				test_struct (int va, int vb) : a (va), b (vb) {}
			};

			small_vector < test_struct, unit_small_vector_test::test_size >
				victim(10, { 1, 1 });

			auto position = victim.emplace(victim.begin() + insert_position, value_a, value_b);

			EXPECT_EQ(expected_size, victim.size());

			EXPECT_EQ(value_a, victim[insert_position].a);
			EXPECT_EQ(value_b, victim[insert_position].b);

			EXPECT_EQ(victim.begin() + insert_position, position);
		}

		TEST(unit_small_vector_test, erase_at_position) {

			int const expected_size = 9;
			int const erase_index = 5;
			int const erase_value = 5;

			small_vector < int, unit_small_vector_test::test_size >
				victim = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };

			auto position = victim.erase(victim.begin() + erase_index);

			EXPECT_EQ(victim.size(), expected_size);
			EXPECT_EQ(find(victim.begin(), victim.end(), erase_value), victim.end());
			EXPECT_EQ(victim.begin() + erase_index, position);
		}

		TEST(unit_small_vector_test, erase_iterator_range) {

			const small_vector < int, unit_small_vector_test::test_size >
				origin = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };

			small_vector < int, unit_small_vector_test::test_size >
				victim = origin;

			size_t const erase_offset = 3;
			size_t const expected_size = erase_offset * 2;

			auto position = victim.erase(victim.begin() + erase_offset, victim.end() - erase_offset);

			EXPECT_EQ(victim.size(), expected_size);

			EXPECT_TRUE(std::equal(
				victim.begin(),
				victim.begin() + erase_offset,
				origin.begin(),
				origin.begin() + erase_offset
			));

			EXPECT_TRUE(std::equal(
				victim.end() - erase_offset,
				victim.end(),
				origin.end() - erase_offset,
				origin.end()
			));

		}

		TEST(unit_small_vector_test, emplace_back) {

			int const value_a = 999;
			int const value_b = 666;

			int const insert_position = 5;
			int const expected_size = 11;

			struct test_struct {
				int a;
				int b;

				test_struct() = default;
				test_struct(const test_struct &) = default;
				test_struct(int va, int vb) : a(va), b(vb) {}
			};

			small_vector < test_struct, unit_small_vector_test::test_size >
				victim(10, { 1, 1 });

			victim.emplace_back(value_a, value_b);

			EXPECT_EQ(expected_size, victim.size());

			EXPECT_EQ(value_a, victim.back().a);
			EXPECT_EQ(value_b, victim.back().b);

		}

		TEST(unit_small_vector_test, pop_back) {

			const small_vector < int, unit_small_vector_test::test_size >
				origin = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };

			size_t const expected_size = origin.size() - 1;

			small_vector < int, unit_small_vector_test::test_size >
				victim = origin;

			victim.pop_back();

			EXPECT_EQ(victim.size(), expected_size);
			
			EXPECT_TRUE(std::equal(
				victim.begin(),
				victim.end(),
				origin.begin(),
				origin.begin() + expected_size
			));
		}

		TEST(unit_small_vector_test, resize_size_large) {

			small_vector < int, unit_small_vector_test::test_size >
				victim;

			size_t const expected_size = unit_small_vector_test::test_size * 2;

			victim.resize(expected_size);

			EXPECT_EQ(victim.size(), expected_size);
			EXPECT_EQ(
				std::count(victim.begin(), victim.end(), int()),
				expected_size
			);
		}

		TEST(unit_small_vector_test, resize_size_large_with_default_value) {

			small_vector < int, unit_small_vector_test::test_size >
				victim;

			size_t const expected_size = unit_small_vector_test::test_size * 2;
			int const default_value = 999;

			victim.resize(expected_size, default_value);

			EXPECT_EQ(victim.size(), expected_size);
			EXPECT_EQ(
				std::count(victim.begin(), victim.end(), default_value),
				expected_size
			);
		}

		TEST(unit_small_vector_test, resize_size_small) {

			small_vector < int, unit_small_vector_test::test_size >
				victim;

			size_t const expected_size = unit_small_vector_test::test_size;

			victim.resize(expected_size * 2);
			victim.resize(unit_small_vector_test::test_size);

			EXPECT_EQ(victim.size(), expected_size);
			EXPECT_EQ(
				std::count(victim.begin(), victim.end(), int()),
				expected_size
			);
		}

		TEST(unit_small_vector_test, resize_size_small_with_default_value) {

			small_vector < int, unit_small_vector_test::test_size >
				victim;

			size_t const expected_size = unit_small_vector_test::test_size;
			int const default_value = 999;

			victim.resize(expected_size * 2, default_value);
			victim.resize(expected_size, default_value);

			EXPECT_EQ(victim.size(), expected_size);
			EXPECT_EQ(
				std::count(victim.begin(), victim.end(), default_value),
				expected_size
			);
		}

		TEST(unit_small_vector_test, swap_small_small) {

			const small_vector < int, unit_small_vector_test::test_size >
				origin_a = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };

			const small_vector < int, unit_small_vector_test::test_size >
				origin_b = { 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };

			auto victim_a = origin_a;
			auto victim_b = origin_b;

			std::swap(victim_a, victim_b);

			EXPECT_TRUE(std::equal(victim_a.begin(), victim_a.end(), origin_b.begin(), origin_b.end()));
			EXPECT_TRUE(std::equal(victim_b.begin(), victim_b.end(), origin_a.begin(), origin_a.end()));
		}

		TEST(unit_small_vector_test, swap_small_large) {

			const small_vector < int, unit_small_vector_test::test_size >
				origin_a = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };

			small_vector < int, unit_small_vector_test::test_size >
				origin_b = { 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };

			origin_b.resize(unit_small_vector_test::test_size * 2);

			auto victim_a = origin_a;
			auto victim_b = origin_b;

			std::swap(victim_a, victim_b);

			EXPECT_TRUE(std::equal(victim_a.begin(), victim_a.end(), origin_b.begin(), origin_b.end()));
			EXPECT_TRUE(std::equal(victim_b.begin(), victim_b.end(), origin_a.begin(), origin_a.end()));
		}

		TEST(unit_small_vector_test, swap_large_large) {

			small_vector < int, unit_small_vector_test::test_size >
				origin_a = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };

			small_vector < int, unit_small_vector_test::test_size >
				origin_b = { 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };

			origin_a.resize(unit_small_vector_test::test_size * 2);
			origin_b.resize(unit_small_vector_test::test_size * 2);

			auto victim_a = origin_a;
			auto victim_b = origin_b;

			std::swap(victim_a, victim_b);

			EXPECT_TRUE(std::equal(victim_a.begin(), victim_a.end(), origin_b.begin(), origin_b.end()));
			EXPECT_TRUE(std::equal(victim_b.begin(), victim_b.end(), origin_a.begin(), origin_a.end()));
		}
*/
