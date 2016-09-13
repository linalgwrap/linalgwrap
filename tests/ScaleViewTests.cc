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

#include "view_tests.hh"
#include <catch.hpp>
#include <rapidcheck.h>
#include <tuple>

namespace linalgwrap {
namespace tests {
using namespace rc;

TEST_CASE("ScaleView", "[ScaleView]") {
    // Define the types we use for the test:
    struct TestTypes {
        typedef double scalar_type;
        typedef SmallMatrix<scalar_type> stored_matrix_type;
        typedef LazyMatrixWrapper<stored_matrix_type, stored_matrix_type>
              lazy_matrix_type;
        typedef view::detail::ScaleView<stored_matrix_type>
              inner_scaleview_type;

        typedef view::detail::ScaleView<const stored_matrix_type>
              view_of_stored_type;
        typedef view::detail::ScaleView<inner_scaleview_type>
              view_of_scaleview_type;
        typedef view::detail::ScaleView<lazy_matrix_type> view_of_lazy_type;
    };

    // Make types accessible
    typedef TestTypes::scalar_type scalar_type;
    typedef TestTypes::stored_matrix_type stored_matrix_type;
    typedef TestTypes::lazy_matrix_type lazy_matrix_type;
    typedef TestTypes::inner_scaleview_type inner_scaleview_type;

    // Generator for the args
    auto args_generator = [] {
        scalar_type fac = *gen::scale(0.66, gen::arbitrary<scalar_type>())
                                 .as("Scaling factor");
        RC_PRE(std::abs(fac) < 1e14);
        stored_matrix_type mat =
              *gen::arbitrary<stored_matrix_type>().as("Inner matrix");
        return std::make_pair(mat, fac);
    };

    // Generator for the model (taking the args)
    auto model_generator = [](std::pair<stored_matrix_type, scalar_type> t) {
        return t.second * t.first;
    };

    // Decrease numeric tolerance for this scope,
    // ie results need to be more exact for passing
    // Note: Unlike BlockView and TransposeView we cannot go as low as
    // 0.01 * default here, since this gives numerical trouble if the
    // scaling factor in the args_generator above is chosen too large
    // and  the inner matrix contains small entries.
    auto lowertol = NumCompConstants::change_temporary(
          0.1 * krims::NumCompConstants::default_tolerance_factor);

    // Generators for the test case views:
    typedef view_tests::StandardViewGenerators<TestTypes, scalar_type>
          standard_generators;

    SECTION("Default view tests on the stored view") {
        typedef view_tests::TestingLibrary<TestTypes::view_of_stored_type,
                                           decltype(args_generator())>
              testlib;

        auto make_view = [](const stored_matrix_type& sm, scalar_type s) {
            return view::scale(sm, s);
        };

        // Generator for the stored view
        standard_generators::stored_view_generator svg(make_view);

        // Test library for the stored view
        testlib{args_generator, svg, model_generator,
                "ScaleView(stored matrix): "}
              .run_checks();
    }

    SECTION("Default view tests on the lazy view") {
        typedef view_tests::TestingLibrary<TestTypes::view_of_lazy_type,
                                           decltype(args_generator())>
              testlib;

        auto make_view = [](lazy_matrix_type& sm, scalar_type s) {
            return view::scale(sm, s);
        };

        // Generator for the lazy view
        standard_generators::lazy_view_generator lvg(make_view);

        // Test library for the stored view
        testlib lib{args_generator, lvg, model_generator,
                    "ScaleView(lazy matrix): "};

        // Run the tests:
        lib.run_checks();
    }

    SECTION("Default view tests on the view view") {
        typedef view_tests::TestingLibrary<TestTypes::view_of_scaleview_type,
                                           decltype(args_generator())>
              testlib;

        auto make_view = [](inner_scaleview_type& sm, scalar_type s) {
            return view::scale(sm, s);
        };

        // Generator for the scale-view view
        standard_generators::view_view_generator vvg(make_view);

        // Test library for the scale-view view
        testlib lib{args_generator, vvg, model_generator,
                    "ScaleView(inner stored view): "};

        // Run the tests:
        lib.run_checks();
    }

}  // TEST_CASE
}  // tests
}  // linalgwrap
