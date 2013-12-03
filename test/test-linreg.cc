
#include <cstdio>

#include <impala_udf/uda-test-harness.h>
#include "madport/port-dbconnector-inl.h"
#include "test-macros.h"
#include "src/linreg.h"

using namespace impala_udf;
using namespace std;

/* Exeutes a simple test using a 2x2 example
 */
int TEST_linreg() {
  UdaTestHarness2<StringVal, StringVal, StringVal, DoubleVal> test1(
      LinrInit, LinrUpdate,
      LinrMerge,
      NULL, LinrFinalize);
  vector<StringVal> no_nulls;
  vector<DoubleVal> labels;

  labels.resize(2);
  labels[0] = 7.0;
  labels[1] = 19.0;
  double ex1[3] = {1.0, 3.0};
  double ex2[3] = {5.0, 7.0};

  // Create the example input
  no_nulls.resize(2);
  no_nulls[0].ptr = (uint8_t*) ex1;
  no_nulls[0].len = 2 * sizeof(double);
  no_nulls[1].ptr = (uint8_t*) ex2;
  no_nulls[1].len = 2 * sizeof(double);

  // create the expected anser
  StringVal ans;
  double coef[2] = {1.0, 2.0};
  ans.ptr = (uint8_t*) coef;
  ans.len = 2 * sizeof(double);

  //test it
  bool b = test1.Execute(no_nulls, labels, ans);
  return (int)b;
}

int TEST_linpred() {
  double coef[2] = {1.0, 2.0};
  StringVal mod(reinterpret_cast<uint8_t*>(coef), sizeof(coef));

  vector<StringVal> ex;
  double ex1[3] = {1.0, 3.0};
  double ex2[3] = {5.0, 7.0};

  // Create the example input
  ex.resize(2);
  ex[0].ptr = (uint8_t*) ex1;
  ex[0].len = 2 * sizeof(double);
  ex[1].ptr = (uint8_t*) ex2;
  ex[1].len = 2 * sizeof(double);


  EXPECT_EQ(LinrPredict(NULL, mod, ex[0]) == DoubleVal(7.0), true);
  EXPECT_EQ(LinrPredict(NULL, mod, ex[1]) == DoubleVal(19.0), true);
  return 0;
}

int main(int argc, char** argv) {
  RUNTEST(TEST_linreg);
}

