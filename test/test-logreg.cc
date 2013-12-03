#include <cstdio>

#include "test-macros.h"
#include "bismarck-common.h"

template <class T>
T* BismarckAllocate(void* ignore, size_t len) {
  return new T[len];
}

#include "logreg-inl.h"


using namespace hazy;


/*! Check that we're linked and can call a function :)
 */
int TEST_Logrinit() {
  bismarck::bytea model;
  bismarck::BismarckLogr<void*>::Init(NULL, &model);

  EXPECT_EQ(model.str == NULL, true);
  EXPECT_EQ(model.len, 0);
  return 1;
}

/*! Allocate a simple model and take a few steps
 */
int TEST_LogrAlloc() {
  //double mod[3] = {0, 0, 0};
  double exa[3] = {1, 2, 1};
  bismarck::bytea model;
  bismarck::BismarckLogr<void*>::Init(NULL, &model);
  bismarck::bytea ex = {(char*)exa, 3*sizeof(double)};

  for (int k = 0; k < 100; k ++) {
    bismarck::BismarckLogr<void*>::Step(NULL, ex, true, &model, 0.2, 0);
  }
  EXPECT_NEAR(bismarck::_LogrPredict<void*>(NULL, ex, model), 1.0, 0.05);
  // next step shouldn't do anything since we use hinge loss
  return 1;
}


int main() {
  RUNTEST(TEST_Logrinit);
  RUNTEST(TEST_LogrAlloc);
}
