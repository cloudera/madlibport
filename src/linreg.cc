
#include <cstdio>

#include <impala_udf/udf.h>

using namespace impala_udf;

template <class T>
T* BismarckAllocate(FunctionContext* ctx, size_t len) {
  len *= sizeof(T);
  return (T*) ctx->Allocate(len);
}

#include "madport/port-dbconnector-inl.h"

#include "bismarck.h"

// MADlib includes
#include "metaport/modules/linreg-inl.h"

// defines basic math operations
#include "linalg-inl.h"

// see for documentation
#include "linreg.h"

using namespace hazy;
using namespace madlib;
using namespace std;

/*! \brief Initializes the UDA state with zeros
 */
void LinrInit(FunctionContext* context, StringVal* m) {
  m->is_null = true;
}

/*! \brief Updates the input state with the given value
 */
void LinrUpdate(FunctionContext* context, const StringVal& val, const DoubleVal &y,
                StringVal* input) {
  PortAllocator pa(context);

  if (input->is_null) {
    // get a handle to our allocated state
    madlib::MemHandle<char> state = madlib::modules::regress::LinrInit(pa);
    input->is_null = false;
    input->len = state.size;
    input->ptr = reinterpret_cast<uint8_t*>(state.ptr);
  }

  // convert to types that MADlib expects
  madlib::MemHandle<char> state = {(size_t)input->len, (char*)input->ptr};
  size_t len_val = val.len/sizeof(double);
  double *v = (double*) val.ptr;

  madlib::MemHandle<char> new_state =
      madlib::modules::regress::LinrTransition(pa, state, &v[0],
                                                     len_val, y.val);

  // clean up memory if the transition function re-allocated
  if (input->ptr != (uint8_t*) new_state.ptr) {
    pa.Free(input->ptr);
  }

  // state and new_state may or maynot be backed by the same memory
  // In particular, if this is the first call to transition, these will
  // be backed by different memory
  input->is_null = false;
  input->ptr = (uint8_t*) new_state.ptr;
  input->len = new_state.size;
}

void LinrMerge(FunctionContext* context, const StringVal& src, StringVal* dst) {
  if (src.is_null) return;
  if (dst->is_null) {
    // create a new dst
      new (dst) StringVal(context, src.len);
      memcpy(dst->ptr, src.ptr, src.len);
      return;
  }
  PortAllocator pa(context);
  madlib::MemHandle<char> statea = {(size_t)dst->len, (char*)dst->ptr};
  madlib::MemHandle<char> stateb = {(size_t)src.len, (char*)src.ptr};

  madlib::MemHandle<char> combin =
      madlib::modules::regress::LinrMerge(pa, statea, stateb);
  dst->ptr = (uint8_t*) combin.ptr;
  dst->len = combin.size;
}

/*! \brief Computes the solution and returns the coefficient vector
 */
StringVal LinrFinalize(FunctionContext* context, const StringVal& input) {
  if (input.is_null) {
    // the UDA was run on an empty table
    StringVal sv;
    return sv;
  }

  PortAllocator pa(context);

  // convert to types that MADlib expects
  madlib::MemHandle<char> state = {(size_t)input.len, (char*)input.ptr};

  madlib::MemHandle<double> coef =
      madlib::modules::regress::LinrFinal(pa, state);

  StringVal sv((uint8_t*) coef.ptr, coef.size*sizeof(double));
  return sv;
}

DoubleVal LinrPredict(FunctionContext* context, const StringVal& model,
                        const StringVal& examp) {
  if (model.is_null || examp.is_null) return DoubleVal::null();
  size_t len = model.len / sizeof(double);
  double pred = simple_dot(reinterpret_cast<double*>(model.ptr),
                           reinterpret_cast<double*>(examp.ptr),
                           len);
  DoubleVal dv(pred);
  return dv;
}


