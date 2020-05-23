/**
 * @file methods/ann/activation_functions/multi_quadratic_function.hpp
 * @author Himanshu Pathak
 *
 * Definition and implementation of multi quadratic function.
 *
 * mlpack is free software; you may redistribute it and/or modify it under the
 * terms of the 3-clause BSD license.  You should have received a copy of the
 * 3-clause BSD license along with mlpack.  If not, see
 * http://www.opensource.org/licenses/BSD-3-Clause for more information.
 */
#ifndef MLPACK_METHODS_ANN_ACTIVATION_FUNCTIONS_MULTIQUAD_FUNCTION_HPP
#define MLPACK_METHODS_ANN_ACTIVATION_FUNCTIONS_MULTIQUAD_FUNCTION_HPP

#include <mlpack/prereqs.hpp>

namespace mlpack {
namespace ann /** Artificial Neural Network. */ {

/**
 * The Multi Quadratic function, defined by
 *
 * @f{eqnarray*}{
 * f(x) = x^2 \\
 * f'(x) = 2*x \\
 * @f}
 */
class MultiquadFunction
{
 public:
  /**
   * Computes the Multi Quadratic function.
   *
   * @param x Input data.
   * @return f(x).
   */
  static double Fn(const double x)
  {
    return std::pow(1 + x * x, 0.5);
  }

  /**
   * Computes the Multi Quadratic function.
   *
   * @param x Input data.
   * @param y The resulting output activation.
   */
  template<typename InputVecType, typename OutputVecType>
  static void Fn(const InputVecType& x, OutputVecType& y)
  {
    y = arma::pow(1 + arma::pow(x, 2), 2);
  }

  /**
   * Computes the first derivative of the Multi Quadratic function.
   *
   * @param y Input data.
   * @return f'(x)
   */
  static double Deriv(const double y)
  {
    return  -x / std::pow(1 + x * x, 0.5);
  }

  /**
   * Computes the first derivatives of the Multi Quadratic function.
   *
   * @param y Input data.
   * @param x The resulting derivatives.
   */
  template<typename InputVecType, typename OutputVecType>
  static void Deriv(const InputVecType& x, OutputVecType& y)
  {
    y = -x / arma::pow(1 + arma::pow(x, 2), 2);
  }
}; // class MultiFunction

} // namespace ann
} // namespace mlpack

#endif
