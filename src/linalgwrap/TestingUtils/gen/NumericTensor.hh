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
#include "Numeric.hh"
#include "NumericContainer.hh"
#include "NumericSize.hh"
#include "linalgwrap/BaseInterfaces.hh"
#include "linalgwrap/MultiVector.hh"
#include "linalgwrap/StoredMatrix_i.hh"

namespace linalgwrap {
namespace gen {
namespace detail {

// TODO generalise this properly once we have tensors of larger rank.

/** Default implementation: Not allowed */
template <typename Tensor, typename Enable = void>
struct NumericTensor {
    static_assert(!std::is_same<Enable, void>::value,
                  "NumericTensor is only available for stored vectors and "
                  "stored tensors.");
};

/** Implementation for vectors */
template <typename Vector>
struct NumericTensor<
      Vector, typename std::enable_if<IsStoredVector<Vector>::value>::type> {
    static rc::Gen<Vector> numeric_tensor(typename Vector::size_type count) {
        return rc::gen::cast<Vector>(
              numeric_container<std::vector<typename Vector::scalar_type>>(
                    count));
    }

    static rc::Gen<Vector> numeric_tensor() {
        return rc::gen::exec(
              [] { return *numeric_tensor(*gen::numeric_size<1>()); });
    }
};

/** Implementation for matrices */
template <typename Matrix>
struct NumericTensor<
      Matrix, typename std::enable_if<IsStoredMatrix<Matrix>::value>::type> {
    static rc::Gen<Matrix> numeric_tensor(typename Matrix::size_type rows,
                                          typename Matrix::size_type cols) {
        return rc::gen::exec([=] {
            Matrix m{rows, cols, false};
            for (size_t i = 0; i < rows * cols; ++i) {
                m[i] = *numeric<typename Matrix::scalar_type>();
            }
            return m;
        });
    }

    static rc::Gen<Matrix> numeric_tensor() {
        return rc::gen::exec([] {
            return *numeric_tensor(*gen::numeric_size<2>(),
                                   *gen::numeric_size<2>());
        });
    }
};

/** Implementation for MultiVectors */
template <typename Vector>
struct NumericTensor<
      MultiVector<Vector>,
      typename std::enable_if<IsStoredVector<Vector>::value>::type> {
    static rc::Gen<MultiVector<Vector>> numeric_tensor(
          typename Vector::size_type n_elem,
          typename Vector::size_type n_vecs) {
        return rc::gen::exec([=] {
            MultiVector<Vector> res;
            res.reserve(n_vecs);
            for (size_t i = 0; i < n_vecs; ++i) {
                res.push_back(std::move(*gen::numeric<Vector>(n_elem)));
            }
            return res;
        });
    }

    static rc::Gen<MultiVector<Vector>> numeric_tensor() {
        return rc::gen::exec([] {
            return *numeric_tensor(*gen::numeric_size<2>(),
                                   *gen::numeric_size<2>());
        });
    }
};

}  // namespace detail

/** \brief Generator for an tensor object with values generated by
 *  gen::numeric() and the size generated by gen::numeric_size() */
template <typename T>
rc::Gen<T> numeric_tensor() {
    return detail::NumericTensor<T>::numeric_tensor();
}

/** \brief Generator for a vector-like object with values generated by
 *  gen::numeric() and the provided size. */
template <typename T>
rc::Gen<T> numeric_tensor(typename T::size_type count) {
    return detail::NumericTensor<T>::numeric_tensor(count);
}

/** \brief Generator for a matrix-like object with values generated by
 *  gen::numeric() and the provided number of rows and columns. */
template <typename T>
rc::Gen<T> numeric_tensor(typename T::size_type row,
                          typename T::size_type col) {
    return detail::NumericTensor<T>::numeric_tensor(row, col);
}

}  // gen
}  // linalgwrap
