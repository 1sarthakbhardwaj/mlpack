/**
 * @file normal_distribution.hpp
 * @author xiaohong ji
 * @author Nishant Kumar
 *
 * Definition of the Normal distribution class.
 *
 * mlpack is free software; you may redistribute it and/or modify it under the
 * terms of the 3-clause BSD license.  You should have received a copy of the
 * 3-clause BSD license along with mlpack.  If not, see
 * http://www.opensource.org/licenses/BSD-3-Clause for more information.
 */
#ifndef MLPACK_METHODS_ANN_DISTRIBUTIONS_NORMAL_DISTRIBUTION_HPP
#define MLPACK_METHODS_ANN_DISTRIBUTIONS_NORMAL_DISTRIBUTION_HPP

#include <mlpack/prereqs.hpp>
#include "../activation_functions/logistic_function.hpp"

namespace mlpack {
namespace ann /** Artificial Neural Network. */ {

/**
 * Implementation of the Normal Distribution function.
 *
 * Normal distribution is a function which accepts a mean and a standard deviation
 * term and creates a probablity distribution out of it.
 */
class NormalDistribution
{
 public:
  /**
   * Default constructor, which creates a Normal distribution with zero
   * dimension.
   */
  NormalDistribution();

  /**
   * Create a Normal distribution with the given mean and sigma.
   *
   * @param mean The mean of the normal distribution.
   * @param sigma The standard deviation of the normal distribution.
   */
  NormalDistribution(const arma::vec& mean, const arma::vec& sigma);

  /**
   * Return the probabilities of the given matrix of observations.
   *
   * @param observation The observation matrix.
   */
  arma::vec Probability(const arma::vec& observation) const
  {
    return arma::exp(LogProbability(observation));
  }

  /**
   * Return the log probabilities of the given matrix of observations.
   *
   * @param observation The observation matrix.
   */
  arma::vec LogProbability(const arma::vec& observation) const;

  /**
   * Stores the gradient of the probabilities of the observations
   * with respect to mean and standard deviation.
   *
   * @param observation The observation matrix.
   * @param dmu The gradient with respect to mean.
   * @param dsigma The gradient with respect to standard deviation.
   */
  void ProbBackward(const arma::vec& observation,
                    arma::vec& dmu,
                    arma::vec& dsigma) const;

  /**
   * Calculates the normal probability density function for each
   * data point (column) in the given matrix.
   *
   * @param x List of observations.
   * @param probabilities Output probabilities for each input observation.
   */
  void Probability(const arma::vec& x, arma::vec& probabilities) const
  {
    probabilities = Probability(x);
  }

  /**
    * Calculates the log of normal probability density function for each
    * data point (column) in the given matrix.
    *
    * @param x List of observations.
    * @param log probabilities Output probabilities for each input observation.
    */
  void LogProbability(const arma::vec& x, arma::vec& probabilities) const
  {
    probabilities = LogProbability(x);
  }

  /**
   * Return a randomly generated observation according to the probability
   * distribution defined by this object.
   *
   * @return Random observation from this Normal distribution.
   */
  arma::vec Sample() const;

  /**
   * Return the mean.
   */
  const arma::vec& Mean() const { return mean; }

  /**
   * Return a modifiable copy of the mean.
   */
  arma::vec& Mean() { return mean; }

  /**
   * Return the standard deviation.
   */
  const arma::vec& Sigma() const { return sigma; }

  /**
   * Return a modifiable copy of the standard deviation.
   */
  arma::vec& StandardDeviation() { return sigma; }

  //! Return the dimensionality of this distribution.
  size_t Dimensionality() const { return mean.n_elem; }

  /**
   * Serialize the distribution.
   */
  template<typename Archive>
  void serialize(Archive& ar, const unsigned int /* version */)
  {
    // We just need to serialize each of the members.
    ar & BOOST_SERIALIZATION_NVP(mean);
    ar & BOOST_SERIALIZATION_NVP(sigma);
  }

 private:
  //! Mean of the distribution.
  arma::vec mean;

  //! Standard deviation of the distribution.
  arma::vec sigma;
}; // class NormalDistribution

} // namespace ann
} // namespace mlpack

// Include implementation.
#include "normal_distribution_impl.hpp"

#endif
