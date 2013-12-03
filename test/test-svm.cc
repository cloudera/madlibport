#include <cstdio>

#include "test-macros.h"
#include "bismarck-common.h"

template <class T>
T* BismarckAllocate(void* ignore, size_t len) {
  return new T[len];
}

#include "svm-inl.h"


using namespace hazy;

/*! Check that we're linked and can call a function :)
 */
int TEST_SVMinit() {
  bismarck::bytea model;
  bismarck::BismarckSVM<void*>::Init(NULL, &model);

  EXPECT_EQ(model.str == NULL, true);
  EXPECT_EQ(model.len, 0);
  return 1;
}

/*! Allocate a simple model and take a few steps
 */
int TEST_SVMAlloc() {
  //double mod[3] = {0, 0, 0};
  double exa[3] = {1, 2, 1};
  bismarck::bytea model;
  bismarck::BismarckSVM<void*>::Init(NULL, &model);
  bismarck::bytea ex = {(char*)exa, 3*sizeof(double)};

  bismarck::BismarckSVM<void*>::Step(NULL, ex, true, &model, 0.2, 0.0);
  EXPECT_EQ(DP(model.str)[0], 0.2);
  EXPECT_EQ(DP(model.str)[1], 0.4);
  EXPECT_EQ(DP(model.str)[2], 0.2);

  // next step shouldn't do anything since we use hinge loss
  bismarck::BismarckSVM<void*>::Step(NULL, ex, true, &model, 0.2, 0.0);
  EXPECT_EQ(DP(model.str)[0], 0.2);
  EXPECT_EQ(DP(model.str)[1], 0.4);
  EXPECT_EQ(DP(model.str)[2], 0.2);
  return 1;
}

/*! Check that the loss function behaves in a sane way
 */
int TEST_SVMloss() {
  double mod[2] = {1, 1};
  double exa[2] = {2, 2};
  bismarck::bytea model = {(char*)mod, 2*sizeof(double)};
  bismarck::bytea ex = {(char*)exa, 2*sizeof(double)};

  double ans = bismarck::BismarckSVM<void*>::Loss(ex, true, model);
  EXPECT_EQ(ans, 0);
  ans = bismarck::BismarckSVM<void*>::Loss(ex, false, model);
  EXPECT_EQ(ans, 5);
  return 1;
}

/*! Test that the SVM prediction function works
 */
int TEST_SVMpred() {
  double mod[2] = {1, 1};
  double exa[2] = {2, 2};
  double exa2[2] = {-2, -2};
  bismarck::bytea model = {(char*)mod, 2*sizeof(double)};
  bismarck::bytea ex = {(char*)exa, 2*sizeof(double)};
  bismarck::bytea ex2 = {(char*)exa2, 2*sizeof(double)};
  EXPECT_EQ(bismarck::_SVMPredict<void*>(NULL, ex, model), true);
  EXPECT_EQ(bismarck::_SVMPredict<void*>(NULL, ex2, model), false);
  return 1;
}

int main() {
  RUNTEST(TEST_SVMinit);
  RUNTEST(TEST_SVMAlloc);
  RUNTEST(TEST_SVMloss);
  RUNTEST(TEST_SVMpred);
}
