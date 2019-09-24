/**
 * @file load_save_image_test.cpp
 * @author Jeffin Sam
 *
 * Test mlpackMain() of load_save_image_main.cpp.
 *
 * mlpack is free software; you may redistribute it and/or modify it under the
 * terms of the 3-clause BSD license.  You should have received a copy of the
 * 3-clause BSD license along with mlpack.  If not, see
 * http://www.opensource.org/licenses/BSD-3-Clause for more information.
 */
#define BINDING_TYPE BINDING_TYPE_TEST

#include <mlpack/core.hpp>
static const std::string testName = "LoadSaveImage";

#include <mlpack/core/util/mlpack_main.hpp>
#include <mlpack/methods/preprocess/load_save_image_main.cpp>

#include "test_helper.hpp"
#include <boost/test/unit_test.hpp>
#include "../test_tools.hpp"

using namespace mlpack;

struct LoadSaveImageTestFixture
{
 public:
  LoadSaveImageTestFixture()
  {
    // Cache in the options for this program.
    CLI::RestoreSettings(testName);
  }

  ~LoadSaveImageTestFixture()
  {
    // Clear the settings.
    bindings::tests::CleanMemory();
    CLI::ClearSettings();
  }
};

BOOST_FIXTURE_TEST_SUITE(LoadSaveImageMainTest,
                         LoadSaveImageTestFixture);

BOOST_AUTO_TEST_CASE(LoadImageTest)
{
  SetInputParam<vector<string>>("input", {"test_image.png", "test_image.png"});
  SetInputParam("height", 50);
  SetInputParam("width", 50);
  SetInputParam("channel", 3);

  mlpackMain();
  arma::mat output = CLI::GetParam<arma::mat>("output");
  // width * height * channels.
  BOOST_REQUIRE_EQUAL(output.n_rows, 50 * 50 * 3);
  BOOST_REQUIRE_EQUAL(output.n_cols, 2);
}

BOOST_AUTO_TEST_CASE(SaveImageTest)
{
  arma::mat testimage = arma::conv_to<arma::mat>::from(
      arma::randi<arma::Mat<unsigned char>>((5 * 5 * 3), 2));
  SetInputParam<vector<string>>("input", {"test_image777.png",
      "test_image999.png"});
  SetInputParam("height", 5);
  SetInputParam("width", 5);
  SetInputParam("channel", 3);
  SetInputParam("save", true);
  SetInputParam("dataset", testimage);
  mlpackMain();

  CLI::ClearSettings();
  CLI::RestoreSettings(testName);

  SetInputParam<vector<string>>("input", {"test_image777.png",
    "test_image999.png"});
  SetInputParam("height", 5);
  SetInputParam("width", 5);
  SetInputParam("channel", 3);

  mlpackMain();
  arma::mat output = CLI::GetParam<arma::mat>("output");
  BOOST_REQUIRE_EQUAL(output.n_rows, 5 * 5 * 3);
  BOOST_REQUIRE_EQUAL(output.n_cols, 2);
  for (size_t i = 0; i < output.n_elem; ++i)
    BOOST_REQUIRE_CLOSE(testimage[i], output[i], 1e-5);
}

/**
 * Check Saved model is working.
 */
BOOST_AUTO_TEST_CASE(SavedModelTest)
{
  SetInputParam<vector<string>>("input", {"test_image.png", "test_image.png"});
  SetInputParam("height", 50);
  SetInputParam("width", 50);
  SetInputParam("channel", 3);

  mlpackMain();
  arma::mat randomOutput = CLI::GetParam<arma::mat>("output");

  SetInputParam<vector<string>>("input", {"test_image.png", "test_image.png"});
  SetInputParam("input_model",
                CLI::GetParam<ImageInfo*>("output_model"));

  mlpackMain();
  arma::mat savedOutput = CLI::GetParam<arma::mat>("output");
  CheckMatrices(randomOutput, savedOutput);
}

/**
 * Check transpose option give two different output.
 */
BOOST_AUTO_TEST_CASE(TransposeTest)
{
  SetInputParam<vector<string>>("input", {"test_image.png", "test_image.png"});
  SetInputParam("height", 50);
  SetInputParam("width", 50);
  SetInputParam("channel", 3);

  mlpackMain();
  arma::mat normalOutput = CLI::GetParam<arma::mat>("output");

  SetInputParam<vector<string>>("input", {"test_image.png", "test_image.png"});
  SetInputParam("input_model",
                CLI::GetParam<ImageInfo*>("output_model"));
  SetInputParam("transpose", true);
  mlpackMain();
  arma::mat transposeOutput = CLI::GetParam<arma::mat>("output");

  CheckMatricesNotEqual(normalOutput, transposeOutput);
}

/**
 * Check whether binding throws error if height, width or channel are not
 * specified.
 */
BOOST_AUTO_TEST_CASE(IncompleteTest)
{
  SetInputParam<vector<string>>("input", {"test_image.png", "test_image.png"});
  SetInputParam("height", 50);
  SetInputParam("width", 50);

  Log::Fatal.ignoreInput = true;
  BOOST_REQUIRE_THROW(mlpackMain(), std::runtime_error);
  Log::Fatal.ignoreInput = false;
}

/**
 * Check for invalid height values.
 */
BOOST_AUTO_TEST_CASE(InvalidInputTest)
{
  SetInputParam<vector<string>>("input", {"test_image.png", "test_image.png"});
  SetInputParam("height", -50);
  SetInputParam("width", 50);
  SetInputParam("channel", 3);

  Log::Fatal.ignoreInput = true;
  BOOST_REQUIRE_THROW(mlpackMain(), std::runtime_error);
  Log::Fatal.ignoreInput = false;
}

BOOST_AUTO_TEST_SUITE_END();
