/**
 * @file one_hot_encoding.hpp
 * @author Jeffin Sam
 *
 * One hot encoding functions. The purpose of this function is to convert
 * categorical variables to binary vectors.
 *
 * mlpack is free software; you may redistribute it and/or modify it under the
 * terms of the 3-clause BSD license.  You should have received a copy of the
 * 3-clause BSD license along with mlpack.  If not, see
 * http://www.opensource.org/licenses/BSD-3-Clause for more information.
 */
#ifndef MLPACK_CORE_DATA_ONE_HOT_ENCODING_HPP
#define MLPACK_CORE_DATA_ONE_HOT_ENCODING_HPP

#include <mlpack/prereqs.hpp>

namespace mlpack {
namespace data {

/**
 * Given a set of labels of a particular datatype, convert them to binary
 * vector. The categorical values be mapped to integer values.
 * Then, each integer value is represented as a binary vector that is
 * all zero values except the index of the integer, which is marked
 * with a 1.
 *
 * @param labelsIn Input labels of arbitrary datatype.
 * @param output Binary matrix.
 */
template<class RowType, template <typename> class MatType, class eT>
void OneHotEncoding(const RowType& labelsIn,
                    MatType<eT>& output);

/**
 * Overloaded function for the above function, which takes a matrix as input
 * and also a vector of indices to encode and outputs a matrix.
 *
 * @param input Input dataset to be encoded.
 * @param indices Index of rows to be encoded.
 * @param output Encoded matrix.
 */
template<typename eT>
void OneHotEncoding(arma::Mat<eT>& input,
                    const arma::colvec indices,
                    arma::Mat<eT>& output);

} // namespace data
} // namespace mlpack

// Include implementation.
#include "one_hot_encoding_impl.hpp"

#endif
