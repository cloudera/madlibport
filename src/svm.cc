
#include <impala_udf/udf.h>

using namespace impala_udf;

template <class T>
T* BismarckAllocate(FunctionContext* ctx, size_t len) {
  len *= sizeof(T);
  return (T*) ctx->Allocate(len);
}

#include "bismarck.h"
#include <cstdio>
#include "svm-inl.h"

using namespace hazy::bismarck;

bytea StringValToBytea(const StringVal &v) {
  bytea ba;
  ba.str = (char*) v.ptr;
  ba.len = v.len;
  return ba;
}

void SVMInit(FunctionContext* ctx, StringVal *model) {
  model->is_null = true;
}

void SVMUpdate(FunctionContext* ctx,  const StringVal &prev_model,
             const StringVal &ex, const BooleanVal &label, const DoubleVal &step_size,
             const DoubleVal &mu, StringVal *model) {

  // If first tuple, the model will be NULL
  if (model->is_null) {
    if (!prev_model.is_null) {
      // Case #2: we have a previous model to seed from
      new (model) StringVal(ctx, prev_model.len);
      memcpy(model->ptr, prev_model.ptr, prev_model.len);
    }
    model->is_null = false;
  }

  // Take the gradient step
  bytea modela = StringValToBytea(*model);
  BismarckSVM<FunctionContext>::Step(ctx,
                                     StringValToBytea(ex),
                                     label.val,
                                     &modela,
                                     step_size.val,
                                     mu.val);
  model->ptr = (uint8_t*) modela.str;
  model->len = modela.len;
}

void SVMMerge(FunctionContext* ctx, const StringVal &src,
              StringVal *dst) {
  if (src.is_null) return;
  if (dst->is_null) {
    // create a new dst
    new (dst) StringVal(ctx, src.len);
    memcpy(dst->ptr, src.ptr, src.len);
    dst->is_null = false;
  } else {
    BismarckSVM<FunctionContext>::Merge(ctx, StringValToBytea(src), StringValToBytea(*dst));
  }
}

StringVal SVMFinalize(FunctionContext* ctx, const StringVal &model) {
  return model;
}

BooleanVal SVMPredict(FunctionContext* ctx, const StringVal &model, const StringVal &ex) {
  if (model.is_null || ex.is_null) return BooleanVal::null();
  BooleanVal r;

  bytea mod = StringValToBytea(model);
  bytea e = StringValToBytea(ex);

  r.val = _SVMPredict(ctx, e, mod);
  r.is_null = false;
  return r;
}

DoubleVal SVMLoss(FunctionContext* ctx, const StringVal &model, const StringVal &ex, const BooleanVal &lbl) {
  if (model.is_null || ex.is_null || lbl.is_null) return DoubleVal::null();
  DoubleVal r;

  bytea mod = StringValToBytea(model);
  bytea e = StringValToBytea(ex);

  r.val = BismarckSVM<FunctionContext>::Loss(e, lbl.val, mod);
  r.is_null = false;
  return r;
}
