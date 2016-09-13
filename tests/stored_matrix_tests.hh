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

#pragma once
#include "matrix_tests.hh"
#include <catch.hpp>
#include <linalgwrap/Exceptions.hh>

namespace linalgwrap {
namespace tests {
using namespace rc;

/** Namespace for default tests for stored matrices */
namespace stored_matrix_tests {

template <typename Matrix>
class TestingLibrary {
  public:
    typedef Matrix matrix_type;
    typedef typename matrix_type::size_type size_type;
    typedef typename matrix_type::scalar_type scalar_type;

    /** Construct a testing library instance in order to check
     *  all basic functionality of a stored matrix
     *
     *  \param prefix A prefix to use in the rapidcheck description string
     *  \param tolerance Numeric tolerance used for comparison in (some) checks.
     *                   The checks for which machine epsilon is used as
     *                   tolerance are:
     *  - Copying a stored matrix
     *  - Setting and getting elements via [], () or iterator
     */
    TestingLibrary(std::string prefix = "")
          : m_prefix{prefix}, m_gen{argsgen, identity, identity} {}

    void run_checks() const;

  private:
    // The testing library we use
    typedef matrix_tests::ComparativeTests<matrix_type, matrix_type> comptests;

    // The caller type we use
    typedef
          typename comptests::template RapidcheckTestableGenerator<matrix_type>
                callgen_type;

    /** Identity operation on the matrix */
    static constexpr matrix_type identity(matrix_type m) { return m; };

    /** Argument generation */
    static constexpr matrix_type argsgen() {
        return *gen::arbitrary<matrix_type>().as("Matrix");
    };

    void once_test_initialiser_list_constructor() const;

    std::string m_prefix;
    callgen_type m_gen;
};

//
// ---------------------------------------
//

template <typename Matrix>
void TestingLibrary<Matrix>::once_test_initialiser_list_constructor() const {
    matrix_type m{{11.0, 12.0}, {21.0, 22.0}, {31.0, 32.0}};

    CHECK((m.n_rows() == 3));
    CHECK((m.n_cols() == 2));

    for (size_type i = 0; i < m.n_rows(); ++i) {
        for (size_type j = 0; j < m.n_cols(); ++j) {
            CHECK((m(i, j) == 10. * (i + 1) + j + 1));
        }
    }

#ifdef DEBUG
    CHECK_THROWS_AS((matrix_type{{1.0, 2.0}, {1.0}}), krims::ExcSizeMismatch);
#endif
}

template <typename Matrix>
void TestingLibrary<Matrix>::run_checks() const {
    // Shorter aliases:
    const NumCompAccuracyLevel high = NumCompAccuracyLevel::Higher;
    const NumCompAccuracyLevel low = NumCompAccuracyLevel::Lower;
    const NumCompAccuracyLevel sloppy = NumCompAccuracyLevel::Sloppy;
    const NumCompAccuracyLevel eps = NumCompAccuracyLevel::MachinePrecision;

    // Test construction from initialiser list
    once_test_initialiser_list_constructor();

    // Test copying stored matrices
    CHECK(rc::check(m_prefix + "Test copying stored matrices",
                    m_gen.generate(comptests::test_copy, eps)));

    // Read-only element access
    CHECK(rc::check(m_prefix + "Element access via () and []",
                    m_gen.generate(comptests::test_element_access, eps)));
    CHECK(rc::check(m_prefix + "Element access via extract_block",
                    m_gen.generate(comptests::test_extract_block, eps)));
    CHECK(rc::check(m_prefix + "Data access via add_block_to",
                    m_gen.generate(comptests::test_add_block_to, eps)));
    CHECK(rc::check(m_prefix + "Read-only iterator of small matrices",
                    m_gen.generate(comptests::test_readonly_iterator, eps)));

    // Read-write element access
    CHECK(rc::check(
          m_prefix + "Altering elements via ()",
          m_gen.generate(comptests::test_setting_elements_indexed, eps)));
    CHECK(rc::check(
          m_prefix + "Altering elements via []",
          m_gen.generate(comptests::test_setting_elements_vectorised, eps)));
    CHECK(rc::check(m_prefix + "Altering elements via iterator",
                    m_gen.generate(comptests::test_readwrite_iterator, eps)));

    // Standard operations and norms
    CHECK(rc::check(m_prefix + "l1 norm",
                    m_gen.generate(comptests::test_norm_l1, eps)));

    CHECK(rc::check(m_prefix + "linf norm",
                    m_gen.generate(comptests::test_norm_linf)));

    CHECK(rc::check(m_prefix + "Frobenius norm",
                    m_gen.generate(comptests::test_norm_frobenius)));

    CHECK(rc::check(m_prefix + "accumulate function",
                    m_gen.generate(comptests::test_accumulate, low)));

    CHECK(rc::check(m_prefix + "trace calculation",
                    m_gen.generate(comptests::test_trace, low)));

    // Operations
    CHECK(rc::check(m_prefix + "Multiplication by scalar",
                    m_gen.generate(comptests::test_multiply_scalar)));
    CHECK(rc::check(m_prefix + "Divide by scalar",
                    m_gen.generate(comptests::test_divide_scalar)));
    CHECK(rc::check(m_prefix + "Add a matrix",
                    m_gen.generate(comptests::template test_add<Matrix>)));
    CHECK(rc::check(m_prefix + "Subtract a matrix",
                    m_gen.generate(comptests::template test_subtract<Matrix>)));
    CHECK(rc::check(m_prefix + "Matrix multiplication",
                    m_gen.generate(comptests::template test_multiply_by<Matrix>,
                                   sloppy)));
}

}  // namespace stored_matrix_tests
}  // namespace tests
}  // namespace linalgwrap
