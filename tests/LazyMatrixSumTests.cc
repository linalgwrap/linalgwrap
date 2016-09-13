//
// Copyright (C) 2016 by the linalgwrap authors
//
// This file is part of linalgwrap.
//
// linalgwrap is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published
// by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// linalgwrap is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with linalgwrap. If not, see <http://www.gnu.org/licenses/>.
//

#include "lazy_matrix_tests_state.hh"
#include <catch.hpp>
#include <linalgwrap/LazyMatrixSum.hh>
#include <rapidcheck.h>

namespace linalgwrap {
namespace tests {
using namespace rc;

TEST_CASE("LazyMatrixSum", "[LazyMatrixSum]") {
    // TODO  Test swap function
    // TODO  Test constructors

    typedef double scalar_type;
    typedef SmallMatrix<scalar_type> stored_matrix_type;
    typedef stored_matrix_type model_matrix_type;

    SECTION("Test access into empty LazyMatrixSum objects") {
        LazyMatrixSum<stored_matrix_type> sum;
        CHECK(sum.n_rows() == 0);
        CHECK(sum.n_cols() == 0);

        auto random_access = [&]() {
            auto col = *rc::gen::arbitrary<size_t>().as("Column index");
            auto row = *rc::gen::arbitrary<size_t>().as("Row index");
            RC_ASSERT(0. == sum(col, row));
        };
        REQUIRE(rc::check("Random access into empty sum", random_access));
    }

    SECTION("Test empty LazyMatrixSum objects behave as zeros") {
        typedef lazy_matrix_tests::FunctionalityTests<
              stored_matrix_type, LazyMatrixSum<stored_matrix_type>>
              testing_lib;

        auto test_add_to_empty = [](stored_matrix_type toadd) {
            stored_matrix_type tosub = -toadd;
            LazyMatrixSum<stored_matrix_type> empty;

            LazyMatrixSum<stored_matrix_type> sumres;
            sumres += toadd;
            testing_lib::run_all_tests(toadd, sumres);

            auto added = empty + toadd;
            testing_lib::run_all_tests(toadd, added);

            auto subed = empty - toadd;
            testing_lib::run_all_tests(tosub, subed);
        };

        REQUIRE(rc::check("Test adding terms to empty sum", test_add_to_empty));
    }

    SECTION("Default lazy matrix tests") {
        typedef double scalar_type;
        typedef SmallMatrix<scalar_type> stored_matrix_type;
        typedef typename stored_matrix_type::size_type size_type;
        typedef LazyMatrixWrapper<stored_matrix_type, stored_matrix_type>
              lazy_matrix_type;
        typedef LazyMatrixSum<stored_matrix_type> sum_type;

        // Generator for the args
        auto args_generator = []() {
            stored_matrix_type mat1 =
                  *gen::arbitrary<stored_matrix_type>().as("Matrix summand 1");

            stored_matrix_type mat2 = *gen::fixed_size<stored_matrix_type>(
                                             mat1.n_rows(), mat1.n_cols())
                                             .as("Matrix summand 2");

            return std::make_pair(mat1, mat2);
        };

        // Generator for the model:
        auto model_generator = [](
              std::pair<stored_matrix_type, stored_matrix_type> p) {
            stored_matrix_type res(p.first.n_rows(), p.first.n_cols(), false);
            for (size_type i = 0; i < p.first.n_rows(); ++i) {
                for (size_type j = 0; j < p.first.n_cols(); ++j) {
                    res(i, j) = p.first(i, j) + p.second(i, j);
                }
            }
            return res;
        };

        // Generator for the sut:
        auto sut_generator =
              [](std::pair<stored_matrix_type, stored_matrix_type> p) {
                  lazy_matrix_type lm1 = lazy_matrix_type{std::move(p.first)};
                  lazy_matrix_type lm2 = lazy_matrix_type{std::move(p.second)};
                  sum_type sum{lm1};
                  sum.push_term(lm2);
                  return sum;
              };

        typedef lazy_matrix_tests::TestingLibrary<sum_type,
                                                  decltype(args_generator())>
              testinglib;

        testinglib{args_generator, sut_generator, model_generator,
                   "LazyMatrixSum: "}
              .run_checks();
    }

    SECTION("Random function test") {
        // Increase numeric tolerance for this scope,
        // ie results need to be less exact for passing
        auto highertol = NumCompConstants::change_temporary(
              10. * krims::NumCompConstants::default_tolerance_factor);

        auto random_test = [] {
            // The initial value:
            model_matrix_type in(2, 3);
            in(0, 0) = 3;
            in(1, 1) = 2;
            in(0, 1) = -1;
            in(1, 0) = -4;
            in(0, 2) = 1;
            in(1, 2) = -1;

            //
            // The actual test
            //

            // The test library we use
            typedef lazy_matrix_tests::StatefulTestingLibrary<
                  model_matrix_type, LazyMatrixSum<stored_matrix_type>>
                  test_library;

            // The commands we check
            auto genCommands = state::gen::execOneOfWithArgs<
                  typename test_library::op_AddStored,
                  typename test_library::op_AddLazy,
                  typename test_library::op_SubtractStored,
                  typename test_library::op_SubtractLazy,
                  typename test_library::op_UnaryMinus,
                  typename test_library::op_MultScalar,
                  typename test_library::op_DivideScalar>;

            // Run the check:
            test_library().run_check(in, genCommands(), 0.65);
        };

        REQUIRE(
              rc::check("Random function test of LazyMatrixSum.", random_test));
    }  // Random function test
}

}  // namespace tests
}  // namescpace linalgwrap
