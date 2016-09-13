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
#include "lazy_matrix_tests.hh"
#include "matrix_tests.hh"
#include <rapidcheck.h>
#include <rapidcheck/state.h>

// have an extra verbose output for rapidcheck function tests:
// #define HAVE_MATRIX_RC_CLASSIFY

namespace linalgwrap {
namespace tests {
using namespace rc;

namespace lazy_matrix_tests {

/** System under test for lazy matrix operations */
template <typename LazyMatrix>
struct LazyMatrixSystemUnderTest {
    typedef LazyMatrix matrix_type;
    typedef typename matrix_type::stored_matrix_type stored_matrix_type;

    //! The stored matrices, which are indirectly referred to by the
    //  matrix object below (due to subscriptions)
    //  We use a reference from global scope. This assures, that
    //  the objects are deleted after the actual matrices.
    std::list<stored_matrix_type>& stored_matrices;

    //! Interface to obtain reference to matrix:
    const matrix_type& matrix() const { return m_matrix; }

    //! Interface to set the matrix data:
    void set_matrix(const matrix_type& mat) { m_matrix = mat; }

    /** \brief construct a Test System to test a Matrix
     *
     * \param mat                     The initial matrix
     * \param stored_matrices_storage An empty list of stored matrices which
     *              will be used in order to store the stored matrices we refer
     *              to in global scope.
     * */
    LazyMatrixSystemUnderTest(
          matrix_type mat,
          std::list<stored_matrix_type>& stored_matrices_storage)
          : stored_matrices{stored_matrices_storage}, m_matrix{mat} {}

    /* Offer the class a chance to copy a matrix into internal storage
     * We will not do so unless it is a stored matrix*/
    template <typename OtherMatrix>
    OtherMatrix& copy_to_internal_storage(OtherMatrix& m) {
        return m;
    }

    stored_matrix_type& copy_to_internal_storage(stored_matrix_type& m) {
        stored_matrices.push_back(m);
        return stored_matrices.back();
    }

    const stored_matrix_type& copy_to_internal_storage(
          const stored_matrix_type& m) {
        stored_matrices.push_back(m);
        return stored_matrices.back();
    }

  private:
    matrix_type m_matrix;
};

/** Template specialisation for testing lazy matrix
 *  expressions in general */
template <typename StoredMatrix>
struct LazyMatrixSystemUnderTest<LazyMatrixExpression<StoredMatrix>> {
    typedef LazyMatrixExpression<StoredMatrix> matrix_type;
    typedef typename matrix_type::stored_matrix_type stored_matrix_type;

    //! The stored matrices, which are indirectly referred to by the
    //  matrix object below (due to subscriptions)
    //  We use a reference from global scope. This assures, that
    //  the objects are deleted after the actual matrices.
    std::list<stored_matrix_type>& stored_matrices;

    //! Interface to obtain reference to matrix:
    matrix_type& matrix() { return *m_matrix_ptr; }

    //! Interface to obtain reference to matrix:
    const matrix_type& matrix() const { return *m_matrix_ptr; }

    //! Interface to set the matrix data:
    void matrix(const matrix_type& mat) { m_matrix_ptr = mat.clone(); }

    /** \brief construct a Test System to test a Matrix
     *
     * \param mat                     The initial matrix
     * \param stored_matrices_storage An empty list of stored matrices which
     *              will be used in order to store the stored matrices we refer
     *              to in global scope.
     * */
    LazyMatrixSystemUnderTest(
          matrix_type mat,
          std::list<stored_matrix_type>& stored_matrices_storage)
          : stored_matrices{stored_matrices_storage}, m_matrix{mat} {}

    /* Offer the class a chance to copy a matrix into internal storage
     * We will not do so unless it is a stored matrix*/
    template <typename OtherMatrix>
    OtherMatrix& copy_to_internal_storage(OtherMatrix& m) {
        return m;
    }

    stored_matrix_type& copy_to_internal_storage(stored_matrix_type& m) {
        stored_matrices.push_back(m);
        return stored_matrices.back();
    }

    const stored_matrix_type& copy_to_internal_storage(
          const stored_matrix_type& m) {
        stored_matrices.push_back(m);
        return stored_matrices.back();
    }

  private:
    matrix_type m_matrix;
    typename matrix_type::lazy_matrix_expression_ptr_type m_matrix_ptr;
};

/** Collection of traits for stateful matrix testing */
template <typename MatrixModel, typename MatrixUnderTest>
struct StatefulTestingTraits {
    typedef MatrixModel model_type;
    typedef MatrixUnderTest matrix_type;
    typedef LazyMatrixSystemUnderTest<MatrixUnderTest> sut_type;
    typedef FunctionalityTests<MatrixModel, MatrixUnderTest> statetest_type;
};

template <typename TestingTraits>
struct CommandBase : rc::state::Command<typename TestingTraits::model_type,
                                        typename TestingTraits::sut_type> {
  protected:
    typedef TestingTraits testtraits_type;
    typedef typename testtraits_type::model_type model_type;
    typedef typename testtraits_type::sut_type sut_type;
    typedef typename testtraits_type::statetest_type statetest_type;

    /** Perform the common checks for each command
     *
     * \param   model The model in the the state *before* the command has been
     *          applied i.e. the same state as run gets it.
     * \param   sut   The system under test in the state *after* the command has
     *          been applied i.e. after all the stuff of a command has been done
     *          to sut already.
     * */
    void run_common_tests(const model_type& model, const sut_type& sut) const {
#ifdef HAVE_MATRIX_DEBUG_PRINT
        std::cout << model << std::endl;
        std::cout << "------------------------------------" << std::endl;
#endif

        // Apply to a copy of the model, which has been advanced to the
        // same state
        model_type model_copy{model};
        this->apply(model_copy);

        statetest_type::run_all_tests(model_copy, sut.matrix());
    }
};

/** Add a Matrix to a MatrixUnderTest and to a Model.
 * Compare that the same thing arises.
 * Uses only low-level element-by-element operations
 * in the model, so almost all Matrix types should be
 * usable for MatrixModel or MatrixTermType */
template <typename TestingTraits, typename MatrixTermType>
struct AddMatrix : CommandBase<TestingTraits> {
    typedef CommandBase<TestingTraits> base_type;
    typedef typename base_type::model_type model_type;
    typedef typename base_type::sut_type sut_type;
    typedef typename model_type::size_type size_type;
    typedef typename model_type::scalar_type scalar_type;
    typedef typename sut_type::stored_matrix_type stored_matrix_type;

    MatrixTermType term;

    AddMatrix(const model_type& model)
          : term{*FixedSizeMatrix<MatrixTermType>::fixed_size(model.n_rows(),
                                                              model.n_cols())
                        .as("Matrix to add")} {}

    void apply(model_type& model) const override {
        // Make sure the size fits:
        RC_PRE(term.n_rows() == model.n_rows());
        RC_PRE(term.n_cols() == model.n_cols());

        // Add the stuff:
        // TODO use iterator here
        for (size_type i = 0; i < model.n_rows(); ++i) {
            for (size_type j = 0; j < model.n_cols(); ++j) {
                model(i, j) += term(i, j);
            }
        }
    }

    void run(const model_type& model, sut_type& sut) const override {
        // Copy the matrix to internal storage if neccessary and
        // apply to sut
        const MatrixTermType& term_ref = sut.copy_to_internal_storage(term);
        sut.set_matrix(sut.matrix() + term_ref);

#ifdef HAVE_MATRIX_RC_CLASSIFY
        RC_CLASSIFY(true, "AddMatrix");
#endif

        base_type::run_common_tests(model, sut);
    }

    void show(std::ostream& os) const override {
        os << "AddMatrix (" << term << ")";
    }
};  // AddMatrix

/** Subtract a Matrix from a MatrixUnderTest and from a Model.
 * Compare that the same thing arises.
 * Uses only low-level element-by-element operations
 * in the model, so almost all Matrix types should be
 * usable for MatrixModel or MatrixTermType */
template <typename TestingTraits, typename MatrixTermType>
struct SubtractMatrix : CommandBase<TestingTraits> {
    typedef CommandBase<TestingTraits> base_type;
    typedef typename base_type::model_type model_type;
    typedef typename base_type::sut_type sut_type;
    typedef typename model_type::size_type size_type;
    typedef typename model_type::scalar_type scalar_type;

    MatrixTermType term;

    SubtractMatrix(const model_type& model)
          : term{*FixedSizeMatrix<MatrixTermType>::fixed_size(model.n_rows(),
                                                              model.n_cols())
                        .as("Matrix to subtract.")} {}

    void apply(model_type& model) const override {
        // Make sure the size fits:
        RC_PRE(term.n_rows() == model.n_rows());
        RC_PRE(term.n_cols() == model.n_cols());

        // Add the stuff:
        // TODO use iterator here
        for (size_type i = 0; i < model.n_rows(); ++i) {
            for (size_type j = 0; j < model.n_cols(); ++j) {
                model(i, j) -= term(i, j);
            }
        }
    }

    void run(const model_type& model, sut_type& sut) const override {
        // Copy the matrix to internal storage if neccessary and
        // apply to sut
        const MatrixTermType& term_ref = sut.copy_to_internal_storage(term);
        sut.set_matrix(sut.matrix() - term_ref);

#ifdef HAVE_MATRIX_RC_CLASSIFY
        RC_CLASSIFY(true, "SubtractMatrix");
#endif
        base_type::run_common_tests(model, sut);
    }

    void show(std::ostream& os) const override {
        os << "SubtractMatrix (" << term << ")";
    }
};  // SubtractMatrix

/** Perform a unary minus on the matrix, ie swap the signs
 * of all matrix entries. */
template <typename TestingTraits>
struct UnaryMinusMatrix : CommandBase<TestingTraits> {
    typedef CommandBase<TestingTraits> base_type;
    typedef typename base_type::model_type model_type;
    typedef typename base_type::sut_type sut_type;
    typedef typename model_type::size_type size_type;
    typedef typename model_type::scalar_type scalar_type;

    UnaryMinusMatrix(const model_type&) {}

    void apply(model_type& model) const override {
        // swap the signs
        // TODO use iterator here
        for (size_type i = 0; i < model.n_rows(); ++i) {
            for (size_type j = 0; j < model.n_cols(); ++j) {
                model(i, j) = -model(i, j);
            }
        }
    }

    void run(const model_type& model, sut_type& sut) const override {
        // Swap the signs:
        sut.set_matrix(-sut.matrix());

#ifdef HAVE_MATRIX_RC_CLASSIFY
        RC_CLASSIFY(true, "UnaryMinusMatrix");
#endif
        base_type::run_common_tests(model, sut);
    }

    void show(std::ostream& os) const override { os << "UnaryMinusMatrix"; }
};  // UnaryMinusMatrix

/** Multiply a Matrix with a MatrixUnderTest and to a Model.
 * Compare that the same thing arises.
 * Uses only low-level element-by-element operations
 * in the model, so almost all Matrix types should be
 * usable for MatrixModel or MatrixTermType */
template <typename TestingTraits, typename MatrixTermType>
struct MultiplyMatrix : CommandBase<TestingTraits> {
    typedef CommandBase<TestingTraits> base_type;
    typedef typename base_type::model_type model_type;
    typedef typename base_type::sut_type sut_type;
    typedef typename model_type::size_type size_type;
    typedef typename model_type::scalar_type scalar_type;

    MatrixTermType term;

    MultiplyMatrix(const model_type& model)
          : term{*FixedSizeMatrix<MatrixTermType>::fixed_size(
                        model.n_cols(),
                        *gen::inRange<size_type>(
                               1, TestConstants::max_matrix_size + 1)
                               .as("No of columns of multiplied matrix"))
                        .as("Matrix to multiply the current state with.")} {}

    void apply(model_type& model) const override {
        // Make sure the size fits:
        RC_PRE(model.n_cols() == term.n_rows());

        model_type out(model.n_rows(), term.n_cols(), false);
        matrix_tests::matrix_product(model, term, out);
        model = out;
    }

    void run(const model_type& model, sut_type& sut) const override {
        // Copy the matrix to internal storage if neccessary and
        // apply to sut
        const MatrixTermType& term_ref = sut.copy_to_internal_storage(term);
        sut.set_matrix(sut.matrix() * term_ref);

#ifdef HAVE_MATRIX_RC_CLASSIFY
        RC_CLASSIFY(true, "MultiplyMatrix");
#endif
        base_type::run_common_tests(model, sut);
    }

    void show(std::ostream& os) const override {
        os << "MultiplyMatrix (" << term << ")";
    }
};  // MultiplyMatrix

/** Multiply a Matrix by a scalar.
 * Compare that the same thing arises.
 */
template <typename TestingTraits>
struct MultiplyScalar : CommandBase<TestingTraits> {
    typedef CommandBase<TestingTraits> base_type;
    typedef typename base_type::model_type model_type;
    typedef typename base_type::sut_type sut_type;
    typedef typename model_type::size_type size_type;
    typedef typename model_type::scalar_type scalar_type;

    scalar_type scalar;

    MultiplyScalar(const model_type&)
          : scalar{*rc::gen::scale(0.7, rc::gen::nonZero<scalar_type>())
                          .as("Scalar to multiply with")} {}

    void apply(model_type& model) const override {
        // Multiply all entries
        // TODO use iterator here
        for (size_type i = 0; i < model.n_rows(); ++i) {
            for (size_type j = 0; j < model.n_cols(); ++j) {
                model(i, j) = scalar * model(i, j);
            }
        }
    }

    void run(const model_type& model, sut_type& sut) const override {
        // Apply to sut:
        sut.set_matrix(scalar * sut.matrix());

#ifdef HAVE_MATRIX_RC_CLASSIFY
        RC_CLASSIFY(true, "MultiplyScalar");
#endif
        base_type::run_common_tests(model, sut);
    }

    void show(std::ostream& os) const override {
        os << "MultiplyScalar (" << scalar << ")";
    }
};  // MultiplyScalar

/** Divide a Matrix by a scalar.
 * Compare that the same thing arises.
 */
template <typename TestingTraits>
struct DivideScalar : CommandBase<TestingTraits> {
    typedef CommandBase<TestingTraits> base_type;
    typedef typename base_type::model_type model_type;
    typedef typename base_type::sut_type sut_type;
    typedef typename model_type::size_type size_type;
    typedef typename model_type::scalar_type scalar_type;

    scalar_type scalar;

    DivideScalar(const model_type&)
          : scalar{*rc::gen::scale(0.7, rc::gen::nonZero<scalar_type>())
                          .as("Scalar to divide by")} {}

    void apply(model_type& model) const override {
        // Multiply all entries
        // TODO use iterator here
        for (size_type i = 0; i < model.n_rows(); ++i) {
            for (size_type j = 0; j < model.n_cols(); ++j) {
                model(i, j) = model(i, j) / scalar;
            }
        }
    }

    void run(const model_type& model, sut_type& sut) const override {
        // Apply to sut:
        sut.set_matrix(sut.matrix() / scalar);

#ifdef HAVE_MATRIX_RC_CLASSIFY
        RC_CLASSIFY(true, "MultiplyScalar");
#endif
        base_type::run_common_tests(model, sut);
    }

    void show(std::ostream& os) const override {
        os << "MultiplyScalar (" << scalar << ")";
    }
};  // DivideScalar

/** Test library for lazy matrix stateful testing
 *
 * \tparam MatrixModel   The matrix type with which we model the matrix under
 *                       test
 * \tparam MatrixUnderTest    The lazy matrix we wish to test.
 */
template <typename MatrixModel, typename MatrixUnderTest>
struct StatefulTestingLibrary {
    //! The collection of traits we use for testing:
    typedef StatefulTestingTraits<MatrixModel, MatrixUnderTest> traits_type;
    typedef typename traits_type::model_type model_type;
    typedef typename traits_type::sut_type sut_type;

    //! The matrix type we test.
    typedef typename traits_type::matrix_type matrix_type;

    //! The stored matrix of the matrix type we test:
    typedef typename matrix_type::stored_matrix_type stored_matrix_type;

    //! The default lazy matrix type to use:
    typedef LazyMatrixWrapper<stored_matrix_type, stored_matrix_type>
          lazy_matrix_type;

    //@{
    /** The default commands defined */
    typedef MultiplyMatrix<traits_type, lazy_matrix_type> op_MultiplyLazy;
    typedef UnaryMinusMatrix<traits_type> op_UnaryMinus;
    typedef MultiplyScalar<traits_type> op_MultScalar;
    typedef DivideScalar<traits_type> op_DivideScalar;
    typedef AddMatrix<traits_type, lazy_matrix_type> op_AddLazy;
    typedef AddMatrix<traits_type, stored_matrix_type> op_AddStored;
    typedef SubtractMatrix<traits_type, lazy_matrix_type> op_SubtractLazy;
    typedef SubtractMatrix<traits_type, stored_matrix_type> op_SubtractStored;
    //@}

    /** Setup the test from an initial state and a command generator
     *
     * \param initial_state The initial state to use
     * \param generation_func  The generation functor for the commands
     * \param scale    The scaling to apply to the command length.
     *                 Smaller scaling means smaller command chains and
     *                 hence tests which are easier to pass.
     */
    template <typename GenFunc>
    void run_check(const model_type& initial_state, GenFunc&& generation_func,
                   double scale = 1.0) const {

        //! The global space used for strong matrices the lazy matrix
        //  expressions point to indirectly
        std::list<stored_matrix_type> stored_matrices;

        // Setup the initial system and the initial_sut:
        LazyMatrixWrapper<stored_matrix_type, model_type> wrap{initial_state};
        matrix_type model{wrap};
        sut_type initial_sut{model, stored_matrices};

        // TODO: Until something better exists use this function
        // Run it through rapidcheck
        rapidcheck_utils::state_check_scaled(initial_state, initial_sut, scale,
                                             generation_func);
    }
};

}  // lazy_matrix_tests
}  // tests
}  // linalgwrap
