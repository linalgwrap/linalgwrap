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

    // TODO run the lazy_matrix_tests TestingLibrary run_checks on
    // one kind of lazy_matrix_product (maybe with only one or two internal
    // matrices under full or even tighter accuracy bounds (see
    // View tests for example)

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
