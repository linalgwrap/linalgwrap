//
// Copyright (C) 2017 by the linalgwrap authors
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
#include "LinearSolverBaseKeys.hh"
#include "LinearSolverStateBase.hh"
#include "SolverBase.hh"

namespace linalgwrap {

/** Each linear solver should support the following:
 *
 * - Default construction
 * - Construction from parameter map
 * - A function update_control_params to update the control parameters.
 *
 */
template <typename State>
class LinearSolverBase : public SolverBase<State> {
  static_assert(IsLinearSolverState<State>::value,
                "State needs to be a type derived from LinearSolverStateBase");

 public:
  typedef SolverBase<State> base_type;
  typedef typename base_type::state_type state_type;

  /** \name Types forwarded from the state */
  ///@{
  /** The type of the linear problem */
  typedef typename state_type::linproblem_type linproblem_type;

  /** The matrix type used within the linear problem */
  typedef typename state_type::stored_matrix_type stored_matrix_type;

  /** The size type used in the matrices*/
  typedef typename state_type::size_type size_type;

  /** The scalar type used in the matrices*/
  typedef typename state_type::scalar_type scalar_type;

  /** The real type used in the matrices */
  typedef typename state_type::real_type real_type;

  /** The vector type used for the vectors x in
   *  $A x = b$ */
  typedef typename state_type::multivector_type multivector_type;

  /** The vector type used for the vectors b in
   *  $A x = b$ */
  typedef typename state_type::const_multivector_type const_multivector_type;

  /** The type of the matrix A in $A x = \lambda x$ */
  typedef typename state_type::matrix_type matrix_type;
  ///@}

  /** \name Iteration control */
  ///@{

  /** The convergence tolerance to use within the linear solver */
  real_type tolerance = Constants<real_type>::default_tolerance;

  /** Bulk-update control parameters from a parameter map.
   *
   * For the list of available keys, see LinearSolverBaseKeys.hh
   */
  void update_control_params(const krims::ParameterMap& map) {
    base_type::update_control_params(map);
    tolerance = map.at(LinearSolverBaseKeys::tolerance, tolerance);
  }
  ///@}

  /** \name Run a linear solver
   */
  ///@{

  /** \brief Run the linear solver with the provided lineare problem and return
   * the final state.
   *
   * If the solver does not manage to achieve convergence a
   * SolverException
   * is thrown and the state's fail_bit will be set accompanied with
   * an appropriate fail message.
   *
   * \param problem   The problem to solve
   * \param solution  The place to store the solution
   */
  virtual state_type solve(const linproblem_type problem,
                           multivector_type& solution) const {
    // Note: eproblem_type only contains pointers or references,
    // so copying is ok.
    state_type state{std::move(problem), solution};

    // TODO: It would be a good idea to do this
    // Zero the solution vector:
    // state.solution().set_zero();

    this->solve_state(state);
    return state;
  }

  /** \brief Run the solver starting from an old state.
   *
   * It is assumed, that the input state is not failed.
   * Note that the fail bit can be unset using the clear_failed() function
   * in order to continue off a failed state using different solver
   * control parameters.
   */
  virtual state_type solve(const state_type& old_state) const {
    assert_dbg(!old_state.is_failed(),
               krims::ExcInvalidState("Cannot make use of a failed state"));
    state_type state{old_state};
    this->solve_state(state);
    return state;
  }
  ///@}
};

}  // namespace linalgwrap