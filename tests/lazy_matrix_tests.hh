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
#include "rapidcheck_utils.hh"
#include <catch.hpp>
#include <linalgwrap/LazyMatrixExpression.hh>
#include <linalgwrap/MatrixTestingUtils.hh>
#include <rapidcheck.h>

// have an extra verbose output for rapidcheck function tests:
//# define HAVE_LAZYMATRIX_RC_CLASSIFY

namespace linalgwrap {
namespace tests {
using namespace rc;

/** Namespace for the components for the
 * LazyMatrix model and standard lazy matrix tests
 **/
namespace lazy_matrix_tests {

/** Default testing routines for lazy matrices to test
 *  the required functionality of a lazy matrix in the
 *  current state
 *
 *  Achieve this by comparing the outcome of the lazy
 *  matrix operation with the equivalent operation
 *  performed on a model
 *
 *  \tparam CompMatrix  Model matrix used for comparison
 *  \tparam SutMatrix   System under test matrix.
 *                      The thing we test.
 *  */
template <typename CompMatrix, typename SutMatrix>
struct FunctionalityTests
      : public matrix_tests::ComparativeTests<CompMatrix, SutMatrix> {
    typedef matrix_tests::ComparativeTests<CompMatrix, SutMatrix> base_type;
    typedef typename base_type::sutmat_type sutmat_type;
    typedef typename base_type::compmat_type compmat_type;
    typedef typename base_type::size_type size_type;
    typedef typename sutmat_type::stored_matrix_type stored_matrix_type;

    /** \brief Run all standard tests in order to compare/test model and sut. */
    static void run_all_tests(const compmat_type& model,
                              const sutmat_type& sut) {
        // Shorter aliases:
        const NumCompAccuracyLevel low = NumCompAccuracyLevel::Lower;

        // Norm value after which numerically problematic tests are left out
        // Especially tests summing a lot of matrix entries (with potentially
        // different sign) can be problematic and hence are left out
        // once the norm gets to large.
        const double problematic_norm =
              1e-2 / NumCompConstants::default_tolerance_factor;

        base_type::test_copy(model, sut);

        base_type::test_element_access(model, sut);
        base_type::test_equivalence(model, sut);
        base_type::test_extract_block(model, sut);
        base_type::test_add_block_to(model, sut);
        base_type::test_readonly_iterator(model, sut);

        base_type::template test_multiply_by<stored_matrix_type>(model, sut,
                                                                 low);

        base_type::test_norm_l1(model, sut);
        base_type::test_norm_linf(model, sut);
        base_type::test_norm_frobenius(model, sut);
        if (model.n_rows() == model.n_cols()) {
            base_type::test_trace(model, sut);
        }

        test_convert_to_stored(model, sut);

        if (model.norm_frobenius_squared() < problematic_norm) {
            // Problematic tests, which are left out once norm is too large
            base_type::test_accumulate(model, sut, low);
        }
    }

    /** Test conversion of the sut metrix to its stored_matrix_type */
    static void test_convert_to_stored(const compmat_type& model,
                                       const sutmat_type& sut,
                                       const NumCompAccuracyLevel tolerance =
                                             NumCompAccuracyLevel::Default) {
        stored_matrix_type sm = static_cast<stored_matrix_type>(sut);

        // Check that it is equivalent to the model:
        RC_ASSERT_NC(sm == numcomp(model).tolerance(tolerance));
    }
};

/** Testing library for lazy matrices.
 *
 * \tparam LazyMatrix  The matrix to test
 * \tparam LazyGenArg  The argument(s) needed to generate a lazy matrix or
 *                     the equivalent model. Use a std::tuple or
 *                     similar if more than one argument is required
 * */
template <typename LazyMatrix, typename LazyGenArg>
class TestingLibrary {
  public:
    typedef LazyMatrix matrix_type;
    typedef typename matrix_type::stored_matrix_type stored_matrix_type;
    typedef typename matrix_type::size_type size_type;
    typedef typename matrix_type::scalar_type scalar_type;
    typedef LazyGenArg genarg_type;

    /** Construct a testing library instance in order to check
     *  all basic functionality of a lazy matrix.
     *
     *  \param args_generator   Function that generates the args for both
     *                          the following generators
     *  \param lazy_generator   Generator that takes the args and returns a lazy
     *                          matrix.
     *  \param model_generator  Generator that takes the args and returns a
     *                          model matrix
     *  \param prefix A prefix to use in the rapidcheck description string
     */
    TestingLibrary(
          std::function<genarg_type(void)> args_generator,
          std::function<matrix_type(genarg_type)> lazy_generator,
          std::function<stored_matrix_type(genarg_type)> model_generator,
          std::string prefix = "")
          : m_prefix{prefix},
            m_gen{args_generator, lazy_generator, model_generator} {}

    void run_checks() const;

    /** Disable the matrix * stored test */
    void disable_run_matrix_times_stored() {
        m_run_matrix_times_stored = false;
    }

  protected:
    // The testing library we use
    typedef matrix_tests::ComparativeTests<stored_matrix_type, matrix_type>
          comptests;

    // The caller type we use
    typedef typename comptests::template RapidcheckTestableGenerator<LazyGenArg>
          callgen_type;

    std::string m_prefix;
    callgen_type m_gen;

  private:
    // The TransposeView * stored operation is not yet implemented for
    // views of lazy matrices. This flag allows to disable this test.
    bool m_run_matrix_times_stored = true;
};

//
// ---------------------------------------
//

template <typename LazyMatrix, typename LazyGenArg>
void TestingLibrary<LazyMatrix, LazyGenArg>::run_checks() const {
    const NumCompAccuracyLevel low = NumCompAccuracyLevel::Lower;
    const NumCompAccuracyLevel sloppy = NumCompAccuracyLevel::Sloppy;

    // Test basic equivalence:
    CHECK(rc::check(m_prefix + "Equivalence of matrix to model expression",
                    m_gen.generate(comptests::test_equivalence)));

    // Read-only element access
    CHECK(rc::check(m_prefix + "Element access via () and []",
                    m_gen.generate(comptests::test_element_access)));
    CHECK(rc::check(m_prefix + "Element access via extract_block",
                    m_gen.generate(comptests::test_extract_block)));
    CHECK(rc::check(m_prefix + "Data access via add_block_to",
                    m_gen.generate(comptests::test_add_block_to)));
    CHECK(rc::check(m_prefix + "Read-only iterator of small matrices",
                    m_gen.generate(comptests::test_readonly_iterator)));

    // Standard operations and norms
    CHECK(rc::check(m_prefix + "l1 norm",
                    m_gen.generate(comptests::test_norm_l1)));

    CHECK(rc::check(m_prefix + "linf norm",
                    m_gen.generate(comptests::test_norm_linf)));

    CHECK(rc::check(m_prefix + "Frobenius norm",
                    m_gen.generate(comptests::test_norm_frobenius)));

    CHECK(rc::check(m_prefix + "accumulate function",
                    m_gen.generate(comptests::test_accumulate, low)));

    CHECK(rc::check(m_prefix + "trace calculation",
                    m_gen.generate(comptests::test_trace, low)));

    // Operations
    typedef LazyMatrixWrapper<stored_matrix_type, stored_matrix_type>
          lazy_matrix_type;

    CHECK(rc::check(m_prefix + "Multiplication by scalar",
                    m_gen.generate(comptests::test_multiply_scalar)));
    CHECK(rc::check(m_prefix + "Divide by scalar",
                    m_gen.generate(comptests::test_divide_scalar)));
    CHECK(rc::check(
          m_prefix + "Add a stored matrix",
          m_gen.generate(comptests::template test_add<stored_matrix_type>)));
    CHECK(rc::check(
          m_prefix + "Add a lazy matrix",
          m_gen.generate(comptests::template test_add<lazy_matrix_type>)));
    CHECK(rc::check(
          m_prefix + "Subtract a stored matrix",
          m_gen.generate(
                comptests::template test_subtract<stored_matrix_type>)));
    if (m_run_matrix_times_stored) {
        CHECK(rc::check(
              m_prefix + "Multiply a stored matrix",
              m_gen.generate(
                    comptests::template test_multiply_by<stored_matrix_type>,
                    low)));
    }
    CHECK(rc::check(
          m_prefix + "Multiply a lazy matrix",
          m_gen.generate(comptests::template test_multiply_by<lazy_matrix_type>,
                         sloppy)));
}

}  // namespace lazy_matrix_tests
}  // namespace tests
}  // namescpace linalgwrap
