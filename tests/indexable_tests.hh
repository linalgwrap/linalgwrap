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

#pragma once
#include "RCTestableGenerator.hh"
#include "generators.hh"
#include "rapidcheck_utils.hh"
#include <catch.hpp>
#include <functional>
#include <krims/Functionals.hh>
#include <lazyten/Base/Interfaces.hh>
#include <lazyten/TestingUtils.hh>

namespace lazyten {
namespace tests {

// Macro to declare a test
#define lazyten_declare_comptest(__testname)          \
  static void __testname(                             \
        const model_type& model, const sut_type& sut, \
        const NumCompAccuracyLevel tolerance = NumCompAccuracyLevel::Default)

// Macro to define a test, which was declared inside a ComparativeTests
// class
#define lazyten_define_comptest(__testname)                              \
  template <typename Model, typename Sut>                                \
  void ComparativeTests<Model, Sut>::__testname(const model_type& model, \
                                                const sut_type& sut,     \
                                                const NumCompAccuracyLevel tolerance)

// Macro to define a test, which was declared inside a ComparativeTests
// class with an extra template argument
#define lazyten_define_comptest_tmpl(__testname, __extratemplate)        \
  template <typename Model, typename Sut>                                \
  template <typename __extratemplate>                                    \
  void ComparativeTests<Model, Sut>::__testname(const model_type& model, \
                                                const sut_type& sut,     \
                                                const NumCompAccuracyLevel tolerance)

/** Namespace for the components for standard tests for all
 * indexable objects **/
namespace indexable_tests {
using namespace rc;
using namespace krims;

/** Model implementation of a MutableMemoryVector for use in the comparative
 * tests */
template <typename Scalar>
struct VectorModel : public MutableMemoryVector_i<Scalar>, public Stored_i {
  typedef Vector_i<Scalar> base_type;
  typedef typename base_type::size_type size_type;
  typedef typename base_type::scalar_type scalar_type;

  // Implement what is needed to be a MutableMemoryVector_i
  size_type size() const override { return m_cont.size(); }
  size_type n_elem() const override { return size(); }
  Scalar operator()(size_type i) const override { return (*this)[i]; }
  Scalar operator[](size_type i) const override { return m_cont[i]; }
  Scalar& operator()(size_type i) override { return (*this)[i]; }
  Scalar& operator[](size_type i) override { return m_cont[i]; }
  void set_zero() override {
    for (auto& elem : m_cont) elem = 0;
  }

  Scalar* memptr() override { return m_cont.data(); }
  const Scalar* memptr() const override { return m_cont.data(); }

  VectorModel(std::vector<Scalar> v) : m_cont(v) {}

  VectorModel(size_type c, bool initialise) : m_cont(c) {
    if (initialise) {
      for (auto& elem : *this) elem = 0;
    }
  }

  VectorModel(std::initializer_list<Scalar> il) : m_cont(il) {}

  template <typename Iterator>
  VectorModel(Iterator begin, Iterator end) : m_cont(begin, end) {}

  template <typename Indexable,
            typename = typename std::enable_if<IsIndexable<Indexable>::value>::type>
  VectorModel(Indexable i) : m_cont(i.begin(), i.end()) {}

 private:
  typedef typename std::vector<Scalar> container_type;
  container_type m_cont;
};

/** \brief Standard test functions which test a certain
 *  functionality by executing it in the Sut indexable
 *  and in a Model indexable and comparing the results
 *  afterwards.
 *
 *  tests for indexables
 *
 *  \tparam Model  Model indexable used for comparison
 *  \tparam Sut   System under test indexable.
 *                      The thing we test.
 **/
template <typename Model, typename Sut>
struct ComparativeTests {
  typedef Model model_type;
  typedef Sut sut_type;
  typedef typename sut_type::size_type size_type;
  typedef typename sut_type::scalar_type scalar_type;
  typedef typename krims::RealTypeOf<scalar_type>::type real_type;

  static constexpr bool cplx = krims::IsComplexNumber<scalar_type>::value;

  static_assert(std::is_same<size_type, typename Model::size_type>::value,
                "The size types of Sut and Model have to agree");

  /** Test whether the two matrices are identical */
  lazyten_declare_comptest(test_equivalence);

  /** Test copying the sut */
  lazyten_declare_comptest(test_copy);

  /** Test read-only element access via [] at
   *  random places. Compare resulting values of the
   *  sut with the model */
  lazyten_declare_comptest(test_element_access_vectorised);

  /** Test whether using an iterator yields the same values as
   *  the entries in the model
   *
   * \note if sut and model are the same object this function compares
   *       the values yielded against the entries of the matrix.
   *  */
  lazyten_declare_comptest(test_readonly_iterator);

  /** Test the accumulate function */
  lazyten_declare_comptest(test_accumulate);

  /** Test the functions dot and cdot*/
  template <typename OtherIndexable>
  lazyten_declare_comptest(test_dot);

  /** Test the functions min and max*/
  lazyten_declare_comptest(test_minmax);

  /** Test the elementwise functions abs, conj, sqrt and square */
  lazyten_declare_comptest(test_elementwise);

  /** Run all comparative tests */
  template <typename Args>
  static void run_all(const RCTestableGenerator<Model, Sut, Args>& gen,
                      const std::string& prefix);

 private:
  /** Inner struct for conditional minmax test.
   *  Since complex number do not support operator< or operator>,
   *  this test should only ever be called if the scalar_type is real
   *  Otherwise it is a noop */
  template <typename Scalar>
  struct MinMaxTest {
    static void run(const model_type& model, const sut_type& sut,
                    const NumCompAccuracyLevel tolerance);
  };

  template <typename Scalar>
  struct MinMaxTest<std::complex<Scalar>> {
    static void run(const model_type&, const sut_type&, const NumCompAccuracyLevel) {}
  };
};

//
// ---------------------------------------------------------------
//

lazyten_define_comptest(test_equivalence) {
  (void)tolerance;  // fake-use tolerance
  RC_ASSERT_NC(model == sut);

  size_type i = *gen::inRange<size_type>(0, model.n_elem())
                       .as("index where change is introduced");
  model_type modelcopy(model);
  modelcopy[i] = modelcopy[i] + lazyten::Constants<scalar_type>::one;
  RC_ASSERT_NC(modelcopy != sut);
}

lazyten_define_comptest(test_copy) {
  sut_type copy{sut};

  sut_type copy2{sut};
  copy2 = copy;

  // check that it is identical to the model
  RC_ASSERT_NC(model == numcomp(copy).tolerance(tolerance));
  RC_ASSERT_NC(model == numcomp(copy2).tolerance(tolerance));
}

lazyten_define_comptest(test_element_access_vectorised) {
  size_type i = *gen::inRange<size_type>(0, model.n_elem()).as("vectorised index");
  RC_ASSERT_NC(model[i] == numcomp(sut[i]).tolerance(tolerance));
}

lazyten_define_comptest(test_readonly_iterator) {
  auto it_const = sut.cbegin();
  auto it = sut.begin();

  for (size_type i = 0; i < model.n_elem(); ++i, ++it, ++it_const) {
    // Assert that const and non-const iterators
    // behave identically:
    RC_ASSERT(*it == *it_const);
    RC_ASSERT_NC(model[i] == numcomp(*it).tolerance(tolerance));
  }

  for (auto it = std::begin(sut); it != std::end(sut); ++it) {
    auto i = std::distance(std::begin(sut), it);
    RC_ASSERT_NC(model[i] == numcomp(*it).tolerance(tolerance));
  }
}

lazyten_define_comptest(test_accumulate) {
  scalar_type res{0};
  for (size_type i = 0; i < model.n_elem(); ++i) {
    res += model[i];
  }

  RC_ASSERT_NC(accumulate(sut) == numcomp(res).tolerance(tolerance));
}

lazyten_define_comptest_tmpl(test_dot, OtherIndexable) {
  OtherIndexable ind =
        *gen::numeric_tensor<OtherIndexable>(model.n_elem()).as("Indexable to dot with");

  auto res = dot(sut, ind);
  auto cres = cdot(sut, ind);

  // Test identities:
  krims::ConjFctr conj;
  RC_ASSERT_NC(dot(ind, sut) == numcomp(res).tolerance(tolerance));
  RC_ASSERT_NC(cdot(ind, sut) == numcomp(conj(cres)).tolerance(tolerance));

  // Compare with reference:
  decltype(model[0] * ind[0]) accu{0};
  decltype(conj(model[0]) * ind[0]) caccu{0};
  for (size_t i = 0; i < model.n_elem(); ++i) {
    accu += model[i] * ind[i];
    caccu += conj(model[i]) * ind[i];
  }
  RC_ASSERT_NC(accu == numcomp(res).tolerance(tolerance));
  RC_ASSERT_NC(caccu == numcomp(cres).tolerance(tolerance));
}

lazyten_define_comptest(test_minmax) {
  MinMaxTest<scalar_type>::run(model, sut, tolerance);
}

template <typename Model, typename Sut>
template <typename Scalar>
void ComparativeTests<Model, Sut>::MinMaxTest<Scalar>::run(
      const model_type& model, const sut_type& sut,
      const NumCompAccuracyLevel tolerance) {
  // Min and max make no sense for complex numbers,
  // so this test is only run for real Scalar types

  // if empty, return:
  if (model.n_elem() == 0) return;

  auto mini = min(sut);
  auto maxi = max(sut);
  auto min_max = std::minmax_element(model.begin(), model.end());

  RC_ASSERT_NC(*min_max.first == numcomp(mini).tolerance(tolerance));
  RC_ASSERT_NC(*min_max.second == numcomp(maxi).tolerance(tolerance));
}

lazyten_define_comptest(test_elementwise) {
  auto rabs = abs(sut);
  auto rabssqrt = sqrt(rabs);
  auto rsquare = square(sut);
  auto rconj = conj(sut);

  // Test some properties
  RC_ASSERT_NC(abs(square(sut)) == numcomp(square(abs(sut))).tolerance(tolerance));
  RC_ASSERT_NC(abs(conj(sut)) == numcomp(abs(sut)).tolerance(tolerance));

  krims::ConjFctr conj;

  // Test results:
  for (size_type i = 0; i < model.n_elem(); ++i) {
    RC_ASSERT_NC(numcomp(rabs[i]).tolerance(tolerance) == std::abs(model[i]));
    RC_ASSERT_NC(numcomp(rabssqrt[i]).tolerance(tolerance) ==
                 std::sqrt(std::abs(model[i])));
    RC_ASSERT_NC(numcomp(rsquare[i]).tolerance(tolerance) == model[i] * model[i]);
    RC_ASSERT_NC(numcomp(rconj[i]).tolerance(tolerance) == conj(model[i]));
  }
}

template <typename Model, typename Sut>
template <typename Args>
void ComparativeTests<Model, Sut>::run_all(
      const RCTestableGenerator<Model, Sut, Args>& gen, const std::string& prefix) {
  const NumCompAccuracyLevel low = NumCompAccuracyLevel::Lower;
  const NumCompAccuracyLevel sloppy = NumCompAccuracyLevel::Sloppy;
  const NumCompAccuracyLevel supersloppy = NumCompAccuracyLevel::SuperSloppy;
  const NumCompAccuracyLevel eps = NumCompAccuracyLevel::MachinePrecision;

#ifdef DEBUG_GENERATORS
  std::cout << sut << std::endl;
  std::cout << "-------------------------" << std::endl;
#endif

  CHECK(gen.run_test(prefix + "==", test_equivalence, eps));

  CHECK(gen.run_test(prefix + "Copying", test_copy, eps));

  CHECK(gen.run_test(prefix + "Element access via []", test_element_access_vectorised,
                     eps));
  CHECK(gen.run_test(prefix + "Read-only iterator.", test_readonly_iterator, eps));

  CHECK(gen.run_test(prefix + "Accumulate function", test_accumulate,
                     cplx ? supersloppy : sloppy));
  CHECK(gen.run_test(prefix + "Dot and cdot function with indexable", test_dot<Sut>,
                     cplx ? sloppy : low));
  CHECK(gen.run_test(prefix + "Dot and cdot function with real model vector",
                     test_dot<VectorModel<real_type>>, cplx ? sloppy : low));
  CHECK(gen.run_test(prefix + "Elementwise functions", test_elementwise));

  if (!cplx) {
    CHECK(gen.run_test(prefix + "Min and max function", test_minmax, eps));
  }
}

}  // namespace indexable_tests
}  // namespace tests
}  // namespace lazyten
