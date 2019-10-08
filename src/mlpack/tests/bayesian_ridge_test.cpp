/**
 * @file bayesian_ridge_test.cpp
 * @author Clement Mercier
 *
 * Test for BayesianRidge.
 *
 * mlpack is free software; you may redistribute it and/or modify it under the
 * terms of the 3-clause BSD license.  You should have received a copy of the
 * 3-clause BSD license along with mlpack.  If not, see
 * http://www.opensource.org/licenses/BSD-3-Clause for more information.
 */


#include <mlpack/core/data/load.hpp>
#include <mlpack/methods/bayesian_ridge/bayesian_ridge.hpp>

#include <boost/test/unit_test.hpp>

using namespace mlpack::regression;
using namespace mlpack::data;

BOOST_AUTO_TEST_SUITE(BayesianRidgeTest);

void GenerateProblem(arma::mat& X,
		     arma::rowvec& y,
		     size_t nPoints,
		     size_t nDims,
		     float sigma=0.0)
{
  arma::arma_rng::set_seed(4);
  
  X = arma::randn(nDims, nPoints);
  arma::colvec omega = arma::randn(nDims);
  arma::colvec noise = arma::randn(nPoints) * sigma;
  y = (omega.t() * X);
  y += noise;
}

// Ensure that predictions are close enough to the target
// for a free noise dataset.
BOOST_AUTO_TEST_CASE(BayesianRidgeRegressionTest)
{
  arma::mat X;
  arma::rowvec y, predictions;

  GenerateProblem(X, y, 200, 10);

  // Instanciate and train the estimator.
  BayesianRidge estimator(true);
  estimator.Train(X, y);
  estimator.Predict(X, predictions);
  
  for (size_t i = 0; i < y.size(); i++)
    {
      BOOST_REQUIRE_CLOSE(predictions[i], y[i], 1e-6);
    }
  // Check that the estimated variance is zero.
  BOOST_REQUIRE_SMALL(estimator.Variance(), 1e-6);
}


// Verify fitIntercept and normalize equal false do not affect the solution.
BOOST_AUTO_TEST_CASE(TestCenter0Normalize0)
{
  arma::mat X;
  arma::rowvec y;
  size_t nDims = 30, nPoints = 100;
  GenerateProblem(X, y, nPoints, nDims, 0.5);

  BayesianRidge estimator(false, false);
  estimator.Train(X, y);

  // To be neutral data_offset must be all 0.
  BOOST_REQUIRE(sum(estimator.Data_offset()) == 0);

  // To be neutral responses_offset must be 0.
  BOOST_REQUIRE(estimator.Responses_offset() == 0);

  // To be neutral data_scale must be all 1.
  BOOST_REQUIRE(sum(estimator.Data_scale()) == nDims);
}

// Verify that centering and normalization are correct.
BOOST_AUTO_TEST_CASE(TestCenter1Normalize1)
{
  arma::mat X;
  arma::rowvec y;
  size_t nDims = 30, nPoints = 100;
  GenerateProblem(X, y, nPoints, nDims, 0.5);

  BayesianRidge estimator(true, true);
  estimator.Train(X, y);

  arma::colvec x_mean = arma::mean(X, 1);
  arma::colvec x_std = arma::stddev(X, 0, 1);
  double y_mean = arma::mean(y);

  BOOST_REQUIRE_SMALL((double) abs(sum(estimator.Data_offset() - x_mean)), 1e-6);

  BOOST_REQUIRE_SMALL((double) abs(estimator.Responses_offset() - y_mean), 1e-6);

  BOOST_REQUIRE_SMALL((double) abs(sum(estimator.Data_scale() - x_std)), 1e-6);
}



BOOST_AUTO_TEST_CASE(ColinearTest)
{
  arma::mat X;
  arma::rowvec y;

  Load("lars_dependent_x.csv", X, false, true);
  Load("lars_dependent_y.csv", y, false, true);

  BayesianRidge estimator(false, false);
  estimator.Train(X, y);
}

BOOST_AUTO_TEST_CASE(OnePointTest)
{
  arma::mat X;
  arma::rowvec y;
  arma::rowvec predictions, std;
  double y_i, std_i;
  
  GenerateProblem(X, y, 100, 10, 2.0);
  BayesianRidge estimator(false, false);
  estimator.Train(X, y);

  // Predict on all the points.
  estimator.Predict(X, predictions);

  // Ensure that the single prediction from column vector are possible and
  // equal to the matrix version.
  for (size_t i = 0; i < y.size(); i++)
    {
      estimator.Predict(X.col(i), y_i);
      BOOST_REQUIRE_CLOSE(predictions(i), y_i, 1e-5);
    }

  // Ensure that the single prediction from column vector are possible and
  // equal to the matrix version. Idem for the std.
  estimator.Predict(X, predictions, std);
  for (size_t i = 0; i < y.size(); i++)
    {
      estimator.Predict(X.col(i), y_i, std_i);
      BOOST_REQUIRE_CLOSE(predictions(i), y_i, 1e-5);
      BOOST_REQUIRE_CLOSE(std(i), std_i, 1e-5);
    }
}

BOOST_AUTO_TEST_SUITE_END();
