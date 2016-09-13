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
#include "TestConstants.hh"
#include "generators.hh"
#include "rapidcheck_utils.hh"
#include <functional>
#include <linalgwrap/MatrixTestingUtils.hh>
#include <linalgwrap/Range.hh>

// have a debug print of all generated matrices
// #define HAVE_MATRIX_DEBUG_PRINT

namespace linalgwrap {
namespace tests {
using namespace rc;
using namespace krims;

/** Namespace for the components for standard matrix tests.
 **/
namespace matrix_tests {
/** Low-level and basic function to form the matrix product out = lhs*rhs */
template <typename Matrix1, typename Matrix2, typename Matrix3>
void matrix_product(const Matrix1& lhs, const Matrix2& rhs, Matrix3& out);

/** \brief Standard test functions which test a certain
 *  functionality by executing it in the SutMatrix
 *  and in a ModelMatrix and comparing the results
 *  afterwards.
 *
 *  \tparam CompMatrix  Model matrix used for comparison
 *  \tparam SutMatrix   System under test matrix.
 *                      The thing we test.
 **/
template <typename CompMatrix, typename SutMatrix>
struct ComparativeTests {
    typedef SutMatrix sutmat_type;
    typedef CompMatrix compmat_type;
    typedef typename SutMatrix::size_type size_type;
    typedef typename SutMatrix::scalar_type scalar_type;

    /** std::function type which describes the call signature of all
     *  static test functions in this class. */
    typedef std::function<void(const compmat_type&, const sutmat_type&,
                               const NumCompAccuracyLevel)>
          testfunction_type;

    static_assert(
          std::is_same<size_type, typename CompMatrix::size_type>::value,
          "The size types of SutMatrix and CompMatrix have to agree");

    // TODO test swap function!

    /** Test wheather the two matrices are identical */
    static void test_equivalence(
          const compmat_type& model, const sutmat_type& sut,
          const NumCompAccuracyLevel tolerance = NumCompAccuracyLevel::Default);

    /** Test copying the sut */
    static void test_copy(
          const compmat_type& model, const sutmat_type& sut,
          const NumCompAccuracyLevel tolerance = NumCompAccuracyLevel::Default);

    /** Test read-only element access via () and [] at
     *  random places. Compare resulting values of the
     *  sut with the model */
    static void test_element_access(
          const compmat_type& model, const sutmat_type& sut,
          const NumCompAccuracyLevel tolerance = NumCompAccuracyLevel::Default);

    /** Test read-write element access via () at a random place.
     *  compare result against a model at all places except the
     *  changed entry.
     */
    static void test_setting_elements_indexed(
          const compmat_type& model, const sutmat_type& sut,
          const NumCompAccuracyLevel tolerance = NumCompAccuracyLevel::Default);

    /** Test read-write element access via [] at a random place.
     *  compare result against a model at all places except the
     *  changed entry.
     */
    static void test_setting_elements_vectorised(
          const compmat_type& model, const sutmat_type& sut,
          const NumCompAccuracyLevel tolerance = NumCompAccuracyLevel::Default);

    /** Test whether extracting a random block in the sut yields the data
     *  of that same block in the model i
     *
     * \note if sut and model are the same object this function compares
     *       the values yielded against values of the matrix itself.
     *  */
    static void test_extract_block(
          const compmat_type& model, const sutmat_type& sut,
          const NumCompAccuracyLevel tolerance = NumCompAccuracyLevel::Default);

    /** Test adding a block of the sut matrix to a different stored
     *  matrix. The yielded result is compared against the equivalent
     *  operation performed with the model.
     *
     * \note if sut and model are the same object this function compares
     *       the values yielded against values of the operation performed
     *       differently on the matrix itself.
     *  */
    static void test_add_block_to(
          const compmat_type& model, const sutmat_type& sut,
          const NumCompAccuracyLevel tolerance = NumCompAccuracyLevel::Default);

    /** Test whether using an iterator yields the same values as
     *  the entries in the model
     *
     * \note if sut and model are the same object this function compares
     *       the values yielded against the entries of the matrix.
     *  */
    static void test_readonly_iterator(
          const compmat_type& model, const sutmat_type& sut,
          const NumCompAccuracyLevel tolerance = NumCompAccuracyLevel::Default);

    /** Test whether modifying element entries using an iterator
     *  works.
     **/
    static void test_readwrite_iterator(
          const compmat_type& model, const sutmat_type& sut,
          const NumCompAccuracyLevel tolerance = NumCompAccuracyLevel::Default);

    /** Test the l1 norm function */
    static void test_norm_l1(
          const compmat_type& model, const sutmat_type& sut,
          const NumCompAccuracyLevel tolerance = NumCompAccuracyLevel::Default);

    /** Test the linf norm function */
    static void test_norm_linf(
          const compmat_type& model, const sutmat_type& sut,
          const NumCompAccuracyLevel tolerance = NumCompAccuracyLevel::Default);

    /** Test the frobenius norm functions (norm_frobenius and
     * norm_frobenius_squared) */
    static void test_norm_frobenius(
          const compmat_type& model, const sutmat_type& sut,
          const NumCompAccuracyLevel tolerance = NumCompAccuracyLevel::Default);

    /** Test the trace function */
    static void test_trace(
          const compmat_type& model, const sutmat_type& sut,
          const NumCompAccuracyLevel tolerance = NumCompAccuracyLevel::Default);

    /** Test the accumulate function */
    static void test_accumulate(
          const compmat_type& model, const sutmat_type& sut,
          const NumCompAccuracyLevel tolerance = NumCompAccuracyLevel::Default);

    /** Test whether multiplication by a scalar yields the same
     *  values in model and sut */
    static void test_multiply_scalar(
          const compmat_type& model, const sutmat_type& sut,
          const NumCompAccuracyLevel tolerance = NumCompAccuracyLevel::Default);

    /** Test whether division by a scalar yields the same
     *  values in model and sut */
    static void test_divide_scalar(
          const compmat_type& model, const sutmat_type& sut,
          const NumCompAccuracyLevel tolerance = NumCompAccuracyLevel::Default);

    /** Test whether addition of another arbitrary matrix gives
     *  rise to the same results in model and sut.
     */
    template <typename OtherMatrix>
    static void test_add(
          const compmat_type& model, const sutmat_type& sut,
          const NumCompAccuracyLevel tolerance = NumCompAccuracyLevel::Default);

    /** Test whether subtraction of another arbitrary matrix gives
     *  rise to the same results in model and sut.
     */
    template <typename OtherMatrix>
    static void test_subtract(
          const compmat_type& model, const sutmat_type& sut,
          const NumCompAccuracyLevel tolerance = NumCompAccuracyLevel::Default);

    /** Test whether the multiplication with another arbitrary matrix
     *  gives rise to the same result in model and sut.
     */
    template <typename OtherMatrix>
    static void test_multiply_by(
          const compmat_type& model, const sutmat_type& sut,
          const NumCompAccuracyLevel tolerance = NumCompAccuracyLevel::Default);

    /** \brief Helper class to simplify setup and calling the above functions
     *  from rapidcheck.
     *
     * The idea is that we construct an object of this type and supply it with
     * the information needed to setup the model and the sut in order to
     * call the test functions above.
     *
     * On call of the generate function a std::function<void(void)> is
     * returned which calls the actual testfunction with a model and sut,
     * generated by the sut_generator and model_generator using common
     * arguments which are in turn generated by the args_generator.
     * See stored_matrix_tests.hh for an example.
     *
     * \tparam GenArg The argument type(s) we need to generate a sut matrix
     * or a model matrix (e.g. a matrix and a factor or a matrix
     * and some sizes ...). Use a std::tuple if more than one arg is required.
     */
    template <typename GenArg>
    class RapidcheckTestableGenerator {
      public:
        /** \brief Construct a RapidcheckCallGenerator object.
         *
         * \param sutgenerator  A function that derives a System-under-test
         *                      matrix from an randomly generated core model
         *                      matrix of type compmat_type.
         * \param modelgenerator  A function that derives a compmat_type model
         *                        matrix in the same state as the test matrix
         *                        yielded by sutgenerator when applied to the
         *                        same core model matrix.
         */
        RapidcheckTestableGenerator(
              std::function<GenArg(void)> args_generator,
              std::function<sutmat_type(GenArg)> sut_generator,
              std::function<compmat_type(GenArg)> model_generator);

        /* Return a std::function object that calls the supplied
         * function func with the generated model and sut.
         *
         * Uses either the specified or the default tolerance.
         */
        std::function<void(void)> generate(
              testfunction_type func,
              const NumCompAccuracyLevel tolerance =
                    NumCompAccuracyLevel::Default) const;

      private:
        std::function<GenArg(void)> m_args_generator;
        std::function<sutmat_type(GenArg)> m_sut_generator;
        std::function<compmat_type(GenArg)> m_model_generator;
    };
};

//
// -------------------------------------------------------
//

template <typename Matrix1, typename Matrix2, typename Matrix3>
void matrix_product(const Matrix1& lhs, const Matrix2& rhs, Matrix3& out) {
    typedef typename Matrix1::size_type size_type;
    typedef typename Matrix1::scalar_type scalar_type;

    static_assert(std::is_same<typename Matrix1::scalar_type,
                               typename Matrix2::scalar_type>::value,
                  "All matrices need to have the same scalar type.");
    static_assert(std::is_same<typename Matrix2::scalar_type,
                               typename Matrix3::scalar_type>::value,
                  "All matrices need to have the same scalar type.");

    static_assert(std::is_same<typename Matrix1::size_type,
                               typename Matrix2::size_type>::value,
                  "All matrices need to have the same size type.");
    static_assert(std::is_same<typename Matrix2::size_type,
                               typename Matrix3::size_type>::value,
                  "All matrices need to have the same size type.");

    assert_size(lhs.n_rows(), out.n_rows());
    assert_size(rhs.n_cols(), out.n_cols());
    assert_size(lhs.n_cols(), rhs.n_rows());

    for (size_type i = 0; i < lhs.n_rows(); ++i) {
        for (size_type j = 0; j < rhs.n_cols(); ++j) {
            scalar_type sum = Constants<scalar_type>::zero;
            for (size_type k = 0; k < rhs.n_rows(); ++k) {
                sum += lhs(i, k) * rhs(k, j);
            }
            out(i, j) = sum;
        }
    }
}

//
// ----------
//
//
template <typename CompMatrix, typename SutMatrix>
void ComparativeTests<CompMatrix, SutMatrix>::test_equivalence(
      const compmat_type& model, const sutmat_type& sut,
      const NumCompAccuracyLevel tolerance) {
    RC_ASSERT_NC(model == numcomp(sut).tolerance(tolerance));
}

template <typename CompMatrix, typename SutMatrix>
void ComparativeTests<CompMatrix, SutMatrix>::test_copy(
      const compmat_type& model, const sutmat_type& sut,
      const NumCompAccuracyLevel tolerance) {
    sutmat_type copy{sut};

    // check that it is identical to the model
    RC_ASSERT_NC(model == numcomp(copy).tolerance(tolerance));
}

template <typename CompMatrix, typename SutMatrix>
void ComparativeTests<CompMatrix, SutMatrix>::test_element_access(
      const compmat_type& model, const sutmat_type& sut,
      const NumCompAccuracyLevel tolerance) {
    size_type row = *gen::inRange<size_type>(0, model.n_rows()).as("row index");
    size_type col = *gen::inRange<size_type>(0, model.n_cols()).as("col index");
    size_type i = *gen::inRange<size_type>(0, model.n_rows() * model.n_cols())
                         .as("vectorised index");

    RC_ASSERT_NC(model(row, col) ==
                 (numcomp(sut(row, col)).tolerance(tolerance)));
    RC_ASSERT_NC(model[i] == numcomp(sut[i]).tolerance(tolerance));
}

template <typename CompMatrix, typename SutMatrix>
void ComparativeTests<CompMatrix, SutMatrix>::test_setting_elements_indexed(
      const compmat_type& model, const sutmat_type& sut,
      const NumCompAccuracyLevel tolerance) {

    auto modify_row =
          *gen::inRange<size_type>(0, model.n_rows()).as("Row to modify");
    auto modify_col =
          *gen::inRange<size_type>(0, model.n_cols()).as("Col to modify");
    auto value = *gen::arbitrary<scalar_type>().as("New value");

    // Keep a copy of the original:
    sutmat_type sut_copy{sut};

    // Modify the value
    sut_copy(modify_row, modify_col) = value;

    // Check it:
    for (auto row : range(model.n_rows())) {
        for (auto col : range(model.n_cols())) {
            if (row == modify_row && col == modify_col) {
                RC_ASSERT_NC(sut_copy(row, col) ==
                             numcomp(value).tolerance(tolerance));
            } else {
                RC_ASSERT_NC(sut_copy(row, col) ==
                             numcomp(model(row, col)).tolerance(tolerance));
            }
        }
    }
}

template <typename CompMatrix, typename SutMatrix>
void ComparativeTests<CompMatrix, SutMatrix>::test_setting_elements_vectorised(
      const compmat_type& model, const sutmat_type& sut,
      const NumCompAccuracyLevel tolerance) {
    auto index = *gen::inRange<size_type>(0, model.n_rows() * model.n_cols())
                        .as("Index to modify");
    auto value = *gen::arbitrary<scalar_type>().as("New value");

    // Keep a copy of the original:
    sutmat_type sut_copy{sut};

    // Modify the value
    sut_copy[index] = value;

    // Check it:
    for (auto i : range(model.n_rows() * model.n_cols())) {
        if (i == index) {
            RC_ASSERT_NC(sut_copy[i] == numcomp(value).tolerance(tolerance));
        } else {
            RC_ASSERT_NC(sut_copy[i] == numcomp(model[i]).tolerance(tolerance));
        }
    }
}

template <typename CompMatrix, typename SutMatrix>
void ComparativeTests<CompMatrix, SutMatrix>::test_extract_block(
      const compmat_type& model, const sutmat_type& sut,
      const NumCompAccuracyLevel tolerance) {

    size_type min_range_size = 1;
    Range<size_type> rows =
          *gen::scale(2.0, gen::range_within<size_type>(0, model.n_rows(),
                                                        min_range_size))
                 .as("extract_block row range");
    Range<size_type> cols =
          *gen::scale(2.0, gen::range_within<size_type>(0, model.n_cols(),
                                                        min_range_size))
                 .as("extract_block col range");

    // Extract a block from the matrix:
    // The type is determined by the return value of the function.
    // It is pretty sure a matrix, so the next block of code works.
    auto block = sut.extract_block(rows, cols);

    // check that it agrees with the model:
    for (size_type row : rows) {
        for (size_type col : cols) {
            size_type i = row - rows.first();
            size_type j = col - cols.first();
            RC_ASSERT_NC(model(row, col) ==
                         numcomp(block(i, j)).tolerance(tolerance));
        }
    }
}

template <typename CompMatrix, typename SutMatrix>
void ComparativeTests<CompMatrix, SutMatrix>::test_add_block_to(
      const compmat_type& model, const sutmat_type& sut,
      const NumCompAccuracyLevel tolerance) {
    // the return type of extract_block and the type add_block_to expects
    // is identical. Hence we can use this trick in order to obtain the
    // type of matrix we need to generate.
    typedef decltype(sut.extract_block(
          Range<size_type>{0, 0}, Range<size_type>{0, 0})) block_matrix_type;

    // Then generate two sizes:
    size_type min_size = 1;
    auto n_rows = *gen::inRange<size_type>(min_size, model.n_rows() + 1)
                         .as("Number of rows of the matrix to add a block to");
    auto n_cols =
          *gen::inRange<size_type>(min_size, model.n_cols() + 1)
                 .as("Number of columns of the matrix to add a block to");

    // Generate the offsets:
    auto r_offset = *gen::inRange<size_type>(0, model.n_rows() - n_rows + 1)
                           .as("Start row for add_block_to");
    auto c_offset = *gen::inRange<size_type>(0, model.n_cols() - n_cols + 1)
                           .as("Start col for add_block_to");

    // Generate an arbitrary factor:
    auto c_this = *gen::scale(0.7, gen::arbitrary<scalar_type>()).as("c_this");

    // Generate an arbitrary matrix of this size:
    block_matrix_type inmat =
          *gen::fixed_size<block_matrix_type>(n_rows, n_cols);

    // Copy the original
    block_matrix_type inmat_copy{inmat};

    // Perform the add_block_to operation
    sut.add_block_to(inmat, r_offset, c_offset, c_this);

    // Check that the result is correct:
    for (auto row : range(n_rows)) {
        for (auto col : range(n_cols)) {
            auto res = inmat_copy(row, col) +
                       c_this * sut(row + r_offset, col + c_offset);
            RC_ASSERT_NC(inmat(row, col) == numcomp(res).tolerance(tolerance));
        }
    }
}

template <typename CompMatrix, typename SutMatrix>
void ComparativeTests<CompMatrix, SutMatrix>::test_readonly_iterator(
      const compmat_type& model, const sutmat_type& sut,
      const NumCompAccuracyLevel tolerance) {
    auto it_const = sut.cbegin();
    auto it = sut.begin();

    for (size_type i = 0; i < model.n_rows(); ++i) {
        for (size_type j = 0; j < model.n_cols(); ++j) {
            // Assert that const and non-const iterators
            // behave identically:
            RC_ASSERT(*it == *it_const);
            RC_ASSERT(it.row() == it_const.row());
            RC_ASSERT(it.col() == it_const.col());

            if (std::abs(model(i, j)) >
                10. * std::numeric_limits<scalar_type>::epsilon()) {
                // Larger than zero element -> expect to be present
                RC_ASSERT(it.row() == i);
                RC_ASSERT(it.col() == j);
                RC_ASSERT_NC(model(i, j) == numcomp(*it).tolerance(tolerance));
            }

            if (it.indices() <= std::make_pair(i, j)) {
                // increment iterators if necessary and continue
                ++it;
                ++it_const;
            }
        }
    }

    for (auto it = std::begin(sut); it != std::end(sut); ++it) {
        RC_ASSERT_NC(model(it.row(), it.col()) ==
                     numcomp(*it).tolerance(tolerance));
    }
}

template <typename CompMatrix, typename SutMatrix>
void ComparativeTests<CompMatrix, SutMatrix>::test_readwrite_iterator(
      const compmat_type& model, const sutmat_type& sut,
      const NumCompAccuracyLevel tolerance) {
    auto modify_row =
          *gen::inRange<size_type>(0, model.n_rows()).as("Row to modify");
    auto modify_col =
          *gen::inRange<size_type>(0, model.n_cols()).as("Col to modify");
    auto value = *gen::arbitrary<scalar_type>().as("New value");

    // Modify an entry
    sutmat_type sut_copy{sut};
    for (auto it = sut_copy.begin(); it != sut_copy.end(); ++it) {
        if (it.row() == modify_row && it.col() == modify_col) {
            *it = value;
            break;
        }
    }

    for (size_type i = 0; i < model.n_rows(); ++i) {
        for (size_type j = 0; j < model.n_cols(); ++j) {
            if (i == modify_row && j == modify_col) {
                RC_ASSERT_NC(sut_copy(i, j) ==
                             numcomp(value).tolerance(tolerance));
            } else {
                RC_ASSERT_NC(model(i, j) ==
                             numcomp(sut_copy(i, j)).tolerance(tolerance));
            }
        }
    }
}

template <typename CompMatrix, typename SutMatrix>
void ComparativeTests<CompMatrix, SutMatrix>::test_norm_l1(
      const compmat_type& model, const sutmat_type& sut,
      const NumCompAccuracyLevel tolerance) {

    scalar_type norm{0};
    for (size_type col = 0; col < model.n_cols(); ++col) {
        scalar_type accu{0};
        for (size_type row = 0; row < model.n_rows(); ++row) {
            accu += std::abs(model(row, col));
        }
        norm = std::max(norm, accu);
    }
    RC_ASSERT_NC(sut.norm_l1() == numcomp(norm).tolerance(tolerance));
}

template <typename CompMatrix, typename SutMatrix>
void ComparativeTests<CompMatrix, SutMatrix>::test_norm_linf(
      const compmat_type& model, const sutmat_type& sut,
      const NumCompAccuracyLevel tolerance) {

    scalar_type norm{0};
    for (size_type row = 0; row < model.n_rows(); ++row) {
        scalar_type accu{0};
        for (size_type col = 0; col < model.n_cols(); ++col) {
            accu += std::abs(model(row, col));
        }
        norm = std::max(norm, accu);
    }
    RC_ASSERT_NC(sut.norm_linf() == numcomp(norm).tolerance(tolerance));
}

template <typename CompMatrix, typename SutMatrix>
void ComparativeTests<CompMatrix, SutMatrix>::test_norm_frobenius(
      const compmat_type& model, const sutmat_type& sut,
      const NumCompAccuracyLevel tolerance) {

    scalar_type frobenius_squared{0};
    for (size_type i = 0; i < model.n_rows(); ++i) {
        for (size_type j = 0; j < model.n_cols(); ++j) {
            frobenius_squared += model(i, j) * model(i, j);
        }
    }
    RC_ASSERT_NC(sut.norm_frobenius_squared() ==
                 numcomp(frobenius_squared).tolerance(tolerance));

    scalar_type frobenius = sqrt(frobenius_squared);
    RC_ASSERT_NC(sut.norm_frobenius() ==
                 numcomp(frobenius).tolerance(tolerance));
}

template <typename CompMatrix, typename SutMatrix>
void ComparativeTests<CompMatrix, SutMatrix>::test_trace(
      const compmat_type& model, const sutmat_type& sut,
      const NumCompAccuracyLevel tolerance) {
    RC_PRE(model.n_rows() == model.n_cols());

    scalar_type res{0};
    for (size_type i = 0; i < model.n_cols(); ++i) {
        res += model(i, i);
    }

    RC_ASSERT_NC(sut.trace() == numcomp(res).tolerance(tolerance));
}

template <typename CompMatrix, typename SutMatrix>
void ComparativeTests<CompMatrix, SutMatrix>::test_accumulate(
      const compmat_type& model, const sutmat_type& sut,
      const NumCompAccuracyLevel tolerance) {
    scalar_type res{0};
    for (size_type i = 0; i < model.n_rows(); ++i) {
        for (size_type j = 0; j < model.n_cols(); ++j) {
            res += model(i, j);
        }
    }

    RC_ASSERT_NC(sut.accumulate() == numcomp(res).tolerance(tolerance));
}

template <typename CompMatrix, typename SutMatrix>
void ComparativeTests<CompMatrix, SutMatrix>::test_multiply_scalar(
      const compmat_type& model, const sutmat_type& sut,
      const NumCompAccuracyLevel tolerance) {
    // Generate an arbitrary factor, but not too large
    auto c = *gen::scale(0.6, gen::arbitrary<scalar_type>()).as("Coefficient");

    // Do the multiplication:
    auto res = sut * c;
    auto res2 = c * sut;

    compmat_type res_model{model.n_rows(), model.n_cols(), false};
    for (auto row : range(model.n_rows())) {
        for (auto col : range(model.n_cols())) {
            res_model(row, col) = model(row, col) * c;
        }
    }

    RC_ASSERT_NC(res_model == numcomp(res).tolerance(tolerance));
    RC_ASSERT_NC(res_model == numcomp(res2).tolerance(tolerance));
}

template <typename CompMatrix, typename SutMatrix>
void ComparativeTests<CompMatrix, SutMatrix>::test_divide_scalar(
      const compmat_type& model, const sutmat_type& sut,
      const NumCompAccuracyLevel tolerance) {
    // Generate an arbitrary factor, but not too large
    auto c = *gen::scale(0.6, gen::nonZero<scalar_type>()).as("Coefficient");

    // Do the multiplication:
    auto res = sut / c;

    compmat_type res_model{model.n_rows(), model.n_cols(), false};
    for (auto row : range(model.n_rows())) {
        for (auto col : range(model.n_cols())) {
            res_model(row, col) = model(row, col) / c;
        }
    }
    RC_ASSERT_NC(res_model == numcomp(res).tolerance(tolerance));
}

template <typename CompMatrix, typename SutMatrix>
template <typename OtherMatrix>
void ComparativeTests<CompMatrix, SutMatrix>::test_multiply_by(
      const compmat_type& model, const sutmat_type& sut,
      const NumCompAccuracyLevel tolerance) {
    // The size of the other matrix to multiply mlhs with:
    // Note: the RHS of inRange is exclusive
    auto othersize =
          *gen::inRange<size_type>(1, TestConstants::max_matrix_size + 1)
                 .as("Number of columns of the rhs matrix");

    // Generate another matrix:
    OtherMatrix mrhs = *gen::fixed_size<OtherMatrix>(model.n_cols(), othersize)
                              .as("Multiplication rhs");

    // Do the low-level multiplication:
    compmat_type result_model(model.n_rows(), othersize, false);
    matrix_product(model, mrhs, result_model);

    // multipy them
    auto result = sut * mrhs;

    // Check that the results are equivalent
    RC_ASSERT_NC(result_model == numcomp(result).tolerance(tolerance));
}

/** Test whether addition of another arbitrary matrix gives
 *  rise to the same results in model and sut.
 */
template <typename CompMatrix, typename SutMatrix>
template <typename OtherMatrix>
void ComparativeTests<CompMatrix, SutMatrix>::test_add(
      const compmat_type& model, const sutmat_type& sut,
      const NumCompAccuracyLevel tolerance) {
    // generate another matrix of the same size:
    auto madd = *gen::fixed_size<OtherMatrix>(model.n_rows(), model.n_cols())
                       .as("Matrix to add");

    // Perform the operation
    auto res = sut + madd;

    // and on the model:
    compmat_type res_model(model.n_rows(), model.n_cols(), false);
    for (auto row : range(model.n_rows())) {
        for (auto col : range(model.n_cols())) {
            res_model(row, col) = model(row, col) + madd(row, col);
        }
    }

    // Check that the results are equivalent
    RC_ASSERT_NC(res_model == numcomp(res).tolerance(tolerance));
}

/** Test whether subtraction of another arbitrary matrix gives
 *  rise to the same results in model and sut.
 */
template <typename CompMatrix, typename SutMatrix>
template <typename OtherMatrix>
void ComparativeTests<CompMatrix, SutMatrix>::test_subtract(
      const compmat_type& model, const sutmat_type& sut,
      const NumCompAccuracyLevel tolerance) {
    // generate another matrix of the same size:
    auto msub = *gen::fixed_size<OtherMatrix>(model.n_rows(), model.n_cols())
                       .as("Matrix to subtract");

    // Perform the operation
    auto res = sut - msub;

    // and on the model:
    compmat_type res_model(model.n_rows(), model.n_cols(), false);
    for (auto row : range(model.n_rows())) {
        for (auto col : range(model.n_cols())) {
            res_model(row, col) = model(row, col) - msub(row, col);
        }
    }

    // Check that the results are equivalent
    RC_ASSERT_NC(res_model == numcomp(res).tolerance(tolerance));
}

template <typename CompMatrix, typename SutMatrix>
template <typename GenArg>
ComparativeTests<CompMatrix, SutMatrix>::RapidcheckTestableGenerator<GenArg>::
      RapidcheckTestableGenerator(
            std::function<GenArg(void)> args_generator,
            std::function<sutmat_type(GenArg)> sut_generator,
            std::function<compmat_type(GenArg)> model_generator)
      : m_args_generator(args_generator),
        m_sut_generator(sut_generator),
        m_model_generator(model_generator) {}

template <typename CompMatrix, typename SutMatrix>
template <typename GenArg>
std::function<void(void)>
ComparativeTests<CompMatrix, SutMatrix>::RapidcheckTestableGenerator<
      GenArg>::generate(testfunction_type func,
                        const NumCompAccuracyLevel tolerance) const {
    // Return a lambda which calls the test appropriately.
    return [=] {
        // Generate the args:
        GenArg arg = m_args_generator();

        // Generate the sut:
        sutmat_type s = m_sut_generator(arg);

        // Generate the equivalent model:
        compmat_type m = m_model_generator(arg);

        // Finally call the test.
        func(m, s, tolerance);
    };
}

}  // matrix_tests
}  // namespace tests
}  // namespace linalgwrap
