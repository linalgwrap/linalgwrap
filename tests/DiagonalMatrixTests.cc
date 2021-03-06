//
// Copyright (C) 2016-17 by the lazyten authors
//
// This file is part of lazyten.
//
// lazyten is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published
// by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// lazyten is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with lazyten. If not, see <http://www.gnu.org/licenses/>.
//

#include "lazy_matrix_tests.hh"
#include <catch.hpp>
#include <lazyten/DiagonalMatrix.hh>
#include <lazyten/SmallVector.hh>
#include <rapidcheck.h>

namespace lazyten {
namespace tests {

TEST_CASE("DiagonalMatrix class", "[DiagonalMatrix]") {
  // Test constructor
  // Test swapping

  typedef double scalar_type;
  typedef SmallMatrix<scalar_type> stored_matrix_type;
  typedef typename stored_matrix_type::size_type size_type;
  typedef SmallVector<scalar_type> vector_type;
  typedef DiagonalMatrix<stored_matrix_type> diagonal_type;

  // Generator for the args
  auto args_generator = [] {
    auto count = *gen::scale(0.6, gen::numeric_size<1>()).as("Diagonal size");
    // TODO allow zero-sized matrices
    RC_PRE(count > 0u);
    auto vals = *rc::gen::container<std::vector<scalar_type>>(
                       count, gen::numeric_around<scalar_type>(1.0))
                       .as("Diagonal values");
    return vector_type{vals};
  };

  // Generator for the model.
  auto model_generator = [](vector_type diag) {
    stored_matrix_type m(diag.size(), diag.size(), true);
    for (size_type i = 0; i < diag.size(); ++i) {
      m(i, i) = diag(i);
    }
    return m;
  };

  // Generator for the sut
  class DiagonalGenerator {
   public:
    DiagonalMatrix<stored_matrix_type> operator()(vector_type diag) {
      m_diag_ptr.reset(new vector_type{std::move(diag)});
      return make_diagmat(*m_diag_ptr);
    }

   private:
    std::shared_ptr<vector_type> m_diag_ptr;
  };

  SECTION("Default lazy matrix tests") {
    typedef lazy_matrix_tests::TestingLibrary<diagonal_type, decltype(args_generator())>
          testlib;

    // Decrease numeric tolerance for this scope,
    // ie results need to be more exact for passing
    auto lowertol = NumCompConstants::change_temporary(
          0.01 * krims::NumCompConstants::default_tolerance_factor);

    testlib tl{args_generator, model_generator, DiagonalGenerator{}, "DiagonalMatrix: "};

    // Only enable the inverse tests if this predicate is true:
    auto enable_inverse_predicate = [](const vector_type& d) {
      for (const auto& elem : d) {
        if (std::abs(elem) < 1e-5) return false;
      }
      return true;
    };
    tl.enable_inverse_apply_if(enable_inverse_predicate);
    tl.run_checks();
  }
}

}  // namespace tests
}  // namespace lazyten
