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

#include "generators.hh"
#include "lazy_matrix_tests_state.hh"
#include "rapidcheck_utils.hh"
#include <catch.hpp>
#include <linalgwrap/LazyMatrixProduct.hh>
#include <rapidcheck.h>

namespace linalgwrap {
namespace tests {
using namespace rc;

TEST_CASE("LazyMatrixProduct", "[LazyMatrixProduct]") {
    // TODO  Test swap function
    // TODO  Test constructors

    typedef double scalar_type;
    typedef SmallMatrix<scalar_type> stored_matrix_type;
    typedef stored_matrix_type model_matrix_type;

    SECTION("Default lazy matrix tests") {
        typedef double scalar_type;
        typedef SmallMatrix<scalar_type> stored_matrix_type;
        typedef typename stored_matrix_type::size_type size_type;
        typedef LazyMatrixWrapper<stored_matrix_type, stored_matrix_type>
              lazy_matrix_type;
        typedef LazyMatrixProduct<stored_matrix_type> product_type;

        // Generator for the args
        auto args_generator = []() {
            stored_matrix_type mat1 =
                  *gen::scale(0.8, gen::arbitrary<stored_matrix_type>())
                         .as("Matrix factor 1");

            // The size of the other matrix to multiply mat1 with:
            // Note: the RHS of inRange is exclusive
            auto othersize = *gen::inRange<size_type>(
                                    1, TestConstants::max_matrix_size + 1)
                                    .as("Number of columns of the 2nd matrix");

            stored_matrix_type mat2 =
                  *gen::scale(0.8, gen::fixed_size<stored_matrix_type>(
                                         mat1.n_cols(), othersize))
                         .as("Matrix factor 2");

            return std::make_pair(mat1, mat2);
        };

        // Generator for the model:
        auto model_generator = [](
              std::pair<stored_matrix_type, stored_matrix_type> p) {
            stored_matrix_type res(p.first.n_rows(), p.second.n_cols(), false);
            matrix_tests::matrix_product(p.first, p.second, res);
            return res;
        };

        // Generator for the sut:
        auto sut_generator =
              [](std::pair<stored_matrix_type, stored_matrix_type> p) {
                  lazy_matrix_type lm1 = lazy_matrix_type{std::move(p.first)};
                  lazy_matrix_type lm2 = lazy_matrix_type{std::move(p.second)};
                  product_type prod{lm1};
                  prod.push_factor(lm2);
                  return prod;
              };

        typedef lazy_matrix_tests::TestingLibrary<product_type,
                                                  decltype(args_generator())>
              testinglib;

        // Increase numeric tolerance for this scope,
        // ie results need to be less exact for passing
        // Smaller tolerance values do not work here,
        // since sometimes we have have rather in the matrices generated
        // by args_generator already.
        auto highertol = NumCompConstants::change_temporary(
              5. * krims::NumCompConstants::default_tolerance_factor);

        testinglib{args_generator, sut_generator, model_generator,
                   "LazyMatrixProduct: "}
              .run_checks();
    }

    SECTION("Random function test") {
        // Increase numeric tolerance for this scope,
        // ie results need to be less exact for passing
        auto highertol = NumCompConstants::change_temporary(
              10. * krims::NumCompConstants::default_tolerance_factor);

        auto random_test = [] {
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
                  model_matrix_type, LazyMatrixProduct<stored_matrix_type>>
                  test_library;

            // The commands we check
            auto genCommands = state::gen::execOneOfWithArgs<
                  typename test_library::op_MultiplyLazy,
                  typename test_library::op_UnaryMinus,
                  typename test_library::op_MultScalar,
                  typename test_library::op_DivideScalar>;

            // Run the check:
            test_library().run_check(in, genCommands(), 0.25);
        };

        REQUIRE(rc::check("Random function test of LazyMatrixProduct.",
                          random_test));
    }  // Random function test
}

}  // namespace tests
}  // namescpace linalgwrap
