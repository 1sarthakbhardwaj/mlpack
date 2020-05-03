/**
 * @file bayesian_ridge.cpp
 * @author Clement Mercier 
 *
 * Implementation of Bayesian Ridge regression.
 *
 * mlpack is free software; you may redistribute it and/or modify it under the
 * terms of the 3-clause BSD license.  You should have received a copy of the
 * 3-clause BSD license along with mlpack.  If not, see
 * http://www.opensource.org/licenses/BSD-3-Clause for more information.
 */
#include "bayesian_ridge.hpp"
#include <mlpack/core/util/log.hpp>
#include <mlpack/core/util/timers.hpp>

using namespace mlpack;
using namespace mlpack::regression;

BayesianRidge::BayesianRidge(const bool centerData,
                             const bool scaleData,
                             const int nIterMax,
                             const double tol) :
  centerData(centerData),
  scaleData(scaleData),
  nIterMax(nIterMax),
  tol(tol)
{/* Nothing to do */}

double BayesianRidge::Train(const arma::mat& data,
                            const arma::rowvec& responses)
{
  Timer::Start("bayesian_ridge_regression");

  arma::mat phi;
  arma::rowvec t;
  arma::colvec eigval;
  arma::mat eigvec;
  arma::colvec eigvali;

  // Preprocess the data. Center and scale.
  responsesOffset = CenterScaleData(data,
                                    responses,
                                    centerData,
                                    scaleData,
                                    phi,
                                    t,
                                    dataOffset,
                                    dataScale);

  // Compute this quantities once and for all.
  const arma::colvec vecphitT = phi * t.t();

  // Enforce symmetry of the covariance matrix before eig_sym.
  const arma::mat phiphiT =  arma::symmatu(phi * phi.t());

  if (arma::eig_sym(eigval, eigvec, phiphiT) == false)
  {
    Log::Warn << "BayesianRidge::Train(): Eigendecomposition "
              << "of covariance failed!"
              << std::endl;
    throw std::runtime_error("eig_sym() failed.");
  }
  const arma::mat eigvecInv = inv(eigvec);

  // Initialize the hyperparameters and
  // begin with an infinitely broad prior.
  alpha = 1e-6;
  beta =  1 / (var(t, 1) * 0.1);
  
  unsigned short i = 0;
  double deltaAlpha = 1.0, deltaBeta = 1.0, crit = 1.0;
  arma::mat matA = arma::eye<arma::mat>(data.n_rows, data.n_rows);

  while ((crit > tol) && (i < nIterMax))
  {
    deltaAlpha = -alpha;
    deltaBeta = -beta;

    omega = 1 / (eigval + (alpha / beta));
    omega = (eigvec * diagmat(omega)) * eigvecInv * vecphitT;

    // // with solve()
    // matA.diag().fill(alpha/beta);
    // omega = solve(matA + phiphiT, vecphitT);

    // Update alpha.
    eigvali = eigval * beta;
    gamma = sum(eigvali / (alpha + eigvali));
    alpha = gamma / dot(omega, omega);

    // Update beta.
    const arma::rowvec temp = t - omega.t() * phi;
    beta = (data.n_cols - gamma) / dot(temp, temp);

    // Compute the stopping criterion.
    deltaAlpha += alpha;
    deltaBeta += beta;
    crit = std::abs(deltaAlpha / alpha + deltaBeta / beta);
    i++;
  }
  // Compute the covariance matrice for the uncertaities later.
  matCovariance = eigvec * diagmat(1 / (beta * eigval + alpha));
  matCovariance *= eigvecInv;
  
  // with solve()
  // matA.diag().fill(alpha);
  // matCovariance = inv_sympd(matA + phiphiT * beta);

  Timer::Stop("bayesian_ridge_regression");

  return RMSE(data, responses);
}

void BayesianRidge::Predict(const arma::mat& points,
                            arma::rowvec& predictions) const
{
  // y_hat = w^T * (X - mu) / sigma + y_mean.
  predictions = omega.t() * ((points.each_col() - dataOffset).each_col()
                            / dataScale);
  predictions += responsesOffset;
}

void BayesianRidge::Predict(const arma::mat& points,
                            arma::rowvec& predictions,
                            arma::rowvec& std) const
{
  // Center and scaleData the points before applying the model.
  const arma::mat X = (points.each_col() - dataOffset).each_col() / dataScale;
  predictions = omega.t() * X;
  predictions += responsesOffset;
  std = sqrt(Variance() + sum((X % (matCovariance * X)), 0));
}

double BayesianRidge::RMSE(const arma::mat& data,
                           const arma::rowvec& responses) const
{
  arma::rowvec predictions;
  Predict(data, predictions);
  return sqrt(mean(square(responses - predictions)));
}

double BayesianRidge::CenterScaleData(const arma::mat& data,
                                      const arma::rowvec& responses,
                                      bool centerData,
                                      bool scaleData,
                                      arma::mat& dataProc,
                                      arma::rowvec& responsesProc,
                                      arma::colvec& dataOffset,
                                      arma::colvec& dataScale)
{
  // Initialize the offsets to their neutral forms.
  dataOffset = arma::zeros<arma::colvec>(data.n_rows);
  dataScale = arma::ones<arma::colvec>(data.n_rows);
  responsesOffset = 0.0;

  if (centerData)
  {
    dataOffset = mean(data, 1);
    responsesOffset = mean(responses);
  }

  if (scaleData)
    dataScale = stddev(data, 0, 1);

  // Copy data and response before the processing.
  dataProc = data;
  // Center the data.
  dataProc.each_col() -= dataOffset;
  // Scale the data.
  dataProc.each_col() /= dataScale;
  // Center the responses.
  responsesProc = responses - responsesOffset;

  return responsesOffset;
}

