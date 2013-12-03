
#ifndef MADLIB_IMPALA_BISMARCK__H
#define MADLIB_IMPALA_BISMARCK__H

#include <impala_udf/udf.h>

namespace hazy {
namespace bismarck {

using namespace impala_udf;
using namespace std;

// Train UDA

struct bytea {
  char* str;
  size_t len;
};

template <class T>
void CoerceBytea(bytea a, T *&outp, size_t &out_len) {
  outp = (T*)(a.str);
  out_len = a.len / sizeof(T);
}


#if 0

void BismarckInit(UdfContext* ctx, BismarckModel_t* st);

template <typename INPUT1,
          void (*INIT)(UdfContext*, BismarckModel_t*),
          void (*STEP)(UdfContext*, const INPUT1&, BismarckModel_t*)>
void BismarckStep(UdfContext*, const BismarckModel_t&, const INPUT1&,
                  BismarckModel_t*);

template <typename INPUT1,
          typename INPUT2,
          void (*INIT)(UdfContext*, BismarckModel_t*),
          void (*STEP)(UdfContext*, const INPUT1&, const INPUT2&,
                       BismarckModel_t*)>
void BismarckStep2(UdfContext*, const BismarckModel_t&, const INPUT1&,
                   const INPUT2&,
                  BismarckModel_t*);

BismarckModel_t BismarckFinal(UdfContext*, const BismarckModel_t&);

template <void (*MERGE)(UdfContext*, const BismarckModel_t&, BismarckModel_t*)>
void BismarckMerge(UdfContext*, const BismarckModel_t&, BismarckModel_t*);


// Loss UDA

void BismarckLossInit(UdfContext* ctx, BismarckLoss_t* st);

template <typename INPUT1,
          double (*LOSS)(const INPUT1&, const BismarckModel_t&),
          double (*MERGE)(double, double)>
void BismarckLossStep(UdfContext* ctx, const BismarckModel_t& mod,
                      const INPUT1& in, BismarckLoss_t* loss);

template <typename INPUT1,
          typename INPUT2,
          double (*LOSS)(const INPUT1&, const INPUT2&,
                         const BismarckModel_t&),
          double (*MERGE)(double, double)>
void BismarckLossStep(UdfContext* ctx, const BismarckModel_t& mod,
                      const INPUT1& in, const INPUT2& in2,
                      BismarckLoss_t* loss);


BismarckLoss_t BismarckLossFinal(UdfContext* ctx, const BismarckLoss_t& loss);

template <double (*MERGE)(double, double)>
void BismarckLossMerge(UdfContext*, const DoubleVal &, DoubleVal*);

#endif
} // namespace bismarck
} // namespace impala

#endif
