/**
 * @file td_idf_encoding_policy.hpp
 * @author Jeffin Sam
 *
 * Definition of the TfIdfEncodingPolicy class.
 *
 * mlpack is free software; you may redistribute it and/or modify it under the
 * terms of the 3-clause BSD license.  You should have received a copy of the
 * 3-clause BSD license along with mlpack.  If not, see
 * http://www.opensource.org/licenses/BSD-3-Clause for more information.
 */
#ifndef MLPACK_CORE_DATA_ENCODING_POLICIES_TF_IDF_ENCODING_POLICY_HPP
#define MLPACK_CORE_DATA_ENCODING_POLICIES_TF_IDF_ENCODING_POLICY_HPP

#include <mlpack/prereqs.hpp>
#include <mlpack/core/data/string_encoding_policies/policy_traits.hpp>
#include <mlpack/core/data/string_encoding.hpp>
namespace mlpack {
namespace data {
/**
 * Definition of the TfIdfEncodingPolicy class.
 *
 * DicitonaryEnocding is used as a helper class for StringEncoding.
 * The encoder assigns a positive integer number to each unique token and treat 
 * the dataset as categorical. The numbers are assigned sequentially starting 
 * from one. The tokens are labeled in the order of their occurrence 
 * in the input dataset.
 */
class TfIdfEncodingPolicy
{
 public:
  
  enum tfTypes
  {
    rawCount,
    binary,
    sublinearTf,
    termFrequency,
  };

  TfIdfEncodingPolicy(size_t tfType = 0, bool smooth_idf = true) :
                      tfType (tfType), smooth_idf (smooth_idf) {}
  /**
  * The function initializes the output matrix.
  *
  * @param output Output matrix to store the encoded results (sp_mat or mat).
  * @param datasetSize The number of strings in the input dataset.
  * @param maxNumTokens The maximum number of tokens in the strings of the 
                        input dataset.
  * @param dictionarySize The size of the dictionary (not used).
  */
  template<typename MatType>
  static void InitMatrix(MatType& output,
                         size_t datasetSize,
                         size_t /*maxNumTokens*/,
                         size_t dictionarySize)
  {
    output.zeros(datasetSize, dictionarySize);
  }

  /**
  * The function initializes the output matrix.
  * Overloaded function to store result in vector<vector<size_t>>
  * 
  * @param output Output matrix to store the encoded results.
  * @param datasetSize The number of strings in the input dataset.
  * @param maxNumTokens The maximum number of tokens in the strings of the 
                        input dataset.
  * @param dictionarySize The size of the dictionary (not used).
  */
  template<typename OutputType>
  static void InitMatrix(std::vector<std::vector<OutputType> >& output,
                         size_t datasetSize,
                         size_t /*maxNumTokens*/,
                         size_t dictionarySize)
  {
    output.resize(datasetSize, std::vector<OutputType> (dictionarySize, 0));
  }

  /** 
  * The function performs the TfIdf encoding algorithm i.e. it writes
  * the encoded token to the ouput.
  *
  * @param output Output matrix to store the encoded results (sp_mat or mat).
  * @param value The encoded token.
  * @param row The row number at which the encoding is performed.
  * @param col The row token number at which the encoding is performed.
  */
  template<typename MatType>
  void Encode(MatType& output, size_t value, size_t row, size_t /*col*/)
  {
    // Important since Mapping starts from 1 whereas allowed column value is 0.
    double idf, tf;
    if(smooth_idf)
      idf = std::log((output.n_rows + 1) / (1 + idfdict[value-1])) + 1;
    else
      idf = std::log(output.n_rows / idfdict[value - 1]) + 1;
    if (tfType == tfTypes::termFrequency)
      tf = tokenCount[row][value - 1] / row_size[row];
    else if (tfType == tfTypes::sublinearTf)
      tf = std::log(tokenCount[row][value - 1]) + 1;
    else if (tfType == tfTypes::binary)
      tf = tokenCount[row][value - 1] > 0 ? 1 : 0;
    else
      tf = tokenCount[row][value - 1];
    output(row, value-1) =  tf * idf;
  }

  /** 
  * The function performs the TfIdf encoding algorithm i.e. it writes
  * the encoded token to the ouput.
  * Overload function to accepted vector<vector<size_t>> as output type.
  *
  * @param output Output matrix to store the encoded results (sp_mat or mat).
  * @param value The encoded token.
  * @param row The row number at which the encoding is performed.
  * @param col The row token number at which the encoding is performed.
  */
  template<typename OutputType>
  void Encode(std::vector<std::vector<OutputType> >& output, size_t value,
                     size_t row, size_t /*col*/)
  {
    // Important since Mapping starts from 1 whereas allowed column value is 0.
    double idf, tf;
    if(smooth_idf)
      idf = std::log((output.size() + 1) / (1 + idfdict[value-1])) + 1;
    else
      idf = std::log(output.size() / idfdict[value - 1]) + 1;
    if (tfType == tfTypes::termFrequency)
      tf = tokenCount[row][value - 1] / row_size[row];
    else if (tfType == tfTypes::sublinearTf)
      tf = std::log(tokenCount[row][value - 1]) + 1;
    else if (tfType == tfTypes::binary)
      tf = tokenCount[row][value - 1] > 0 ? 1 : 0;
    else
      tf = tokenCount[row][value - 1];
    output[row][value-1] =  tf * idf;
  }

  /**
   * Serialize the class to the given archive.
   */
  template<typename Archive>
  void serialize(Archive& /* ar */, const unsigned int /* version */)
  {
    // Nothing to serialize.
  }

  /*
  * The function is used to create the datastrcutre will be important to find
  * out idfvalue of words, and then wrtiting the output based on their count.
  *
  * @param row The row number at which the encoding is performed.
  * @param numToken The count of token parsed till now.
  * @param value The encoded token.
  */
   void PreprocessToken(size_t row, size_t /*numTokens*/,
                       size_t value)
  {
    if(row>=tokenCount.size())
    {
      row_size.push_back(0);
      tokenCount.push_back(std::unordered_map<size_t, double>());
    }
    tokenCount.back()[value-1]++;
    if(tokenCount.back()[value-1]==1)
      idfdict[value-1]++;
    row_size.back()++;
  }
 private:
  std::vector<std::unordered_map<size_t, double>> tokenCount;
  std::unordered_map<size_t, double> idfdict;
  std::vector<double> row_size;
  bool smooth_idf;
  size_t tfType;
};

/**
 * The specialization provides some information about the dictionary encoding
 * policy.
 */
template<>
struct StringEncodingPolicyTraits<TfIdfEncodingPolicy>
{
  /**
   * Indicates if the policy is able to encode the token at once without 
   * any information about other tokens as well as the total tokens count.
   */
  static const bool onePassEncoding = false;
};

template<typename TokenType>
using TfIdfEncoding = StringEncoding<TfIdfEncodingPolicy,
                                          StringEncodingDictionary<TokenType>>;
} // namespace data
} // namespace mlpack

#endif 