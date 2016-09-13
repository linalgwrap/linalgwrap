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

#include "stored_matrix_tests.hh"
#include <catch.hpp>
#include <linalgwrap/ArmadilloMatrix.hh>
#include <rapidcheck.h>

// Generators for necessary matrices
#include "generators.hh"

namespace linalgwrap {
namespace tests {
using namespace rc;

#ifdef LINALGWRAP_HAVE_ARMADILLO
TEST_CASE("ArmadilloMatrix class", "[ArmadilloMatrix]") {
    // The type of matrix we wish to test.
    typedef ArmadilloMatrix<double> matrix_type;

    SECTION("Norm test") {
        matrix_type m = {{1.0}, {-2.0}};
        REQUIRE(m.norm_l1() == 3.);
        REQUIRE(m.norm_linf() == 2.);

        matrix_type n = {{1.0, -2.0}};
        REQUIRE(n.norm_l1() == 2.);
        REQUIRE(n.norm_linf() == 3.);
    }

    SECTION("Default stored matrix tests") {
        typedef typename stored_matrix_tests::TestingLibrary<matrix_type>
              testinglib;

        // Decrease tolerance to require a more accurate numerical agreement
        // for passing.
        auto lowertol = NumCompConstants::change_temporary(
              0.1 * krims::NumCompConstants::default_tolerance_factor);

        // Run tests:
        testinglib("ArmadilloMatrix: ").run_checks();
    }
}
#endif

}  // tests
}  // linalgwrap
