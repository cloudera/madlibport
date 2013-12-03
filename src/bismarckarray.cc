
#ifndef HAZY_BISMARCK_ARRAY_H
#define HAZY_BISMARCK_ARRAY_H

#include <cstdio>
#include <impala_udf/udf.h>
#include <string>
#include <sstream>
#include <iostream>

using namespace impala;
using namespace impala_udf;
using namespace std;


StringVal ToArray(FunctionContext* context, int n, DoubleVal *ints) {
  StringVal s(context, n * sizeof(double));
  double *darr = reinterpret_cast<double*>(s.ptr);
  for (int i = 0; i < n; i++) {
    darr[i] = ints[i].val;
  }
  return s;
}

StringVal PrintArray(FunctionContext* context, const StringVal& arr) {
  double *darr = reinterpret_cast<double*>(arr.ptr);
  int len = arr.len / sizeof(double);
  stringstream ss;
  ss << "<";
  for (int i = 0; i < len; ++i) {
    if (i != 0) ss << ", ";
    ss << darr[i];
  }
  ss << ">";
  string str = ss.str();
  StringVal result(context, str.size());
  memcpy(result.ptr, str.c_str(), str.size());
  return result;
}

DoubleVal ArrayGet(FunctionContext* context, const BigIntVal &n, const StringVal &arr) {
  size_t i = n.val;
  double *darr = reinterpret_cast<double*>(arr.ptr);
  return DoubleVal(darr[i]);
}

StringVal AllBytes(FunctionContext* context) {
  StringVal s(context, 256);

  for (int i = 0; i < 256; i++) {
    s.ptr[i] = i;
  }
  return s;
}

void inplace_dub_encode(double *arr, size_t len) {
  for (size_t i = 0; i < len; i++) {
    char *hex = (char*) &arr[i];
    // cast to float so we have 4 bytes to encode but 8 bytes of space
    float fp = arr[i];
    uint32_t *ip =  (uint32_t*) &fp;
    uint32_t f = *ip;
    for (int k = 0; k < 8; k++) {
      hex[k] = 97 + ((f >> (4*k)) & 0xF);
    }
  }
}

void inplace_dub_decode(char *arr, size_t len) {
  for (size_t i = 0; i < len; i+=8) {
    double *dub = (double*) &arr[i];
    // cast to float so we have 4 bytes to encode but 8 bytes of space
    int32_t f = 0;
    for (int k = 7; k >= 0; k--) {
      f = (f<<4) | ((arr[i+k] - 97) & 0xF);
    }
    float *fp = (float*) &f;
    *dub = (double) *fp;
  }
}

StringVal EncodeArray(FunctionContext* context, const StringVal& arr) {
  double *darr = reinterpret_cast<double*>(arr.ptr);
  int len = arr.len / sizeof(double);

  StringVal result(context, arr.len);

  memcpy(result.ptr, darr, arr.len);
  inplace_dub_encode((double*) result.ptr, len);
  return result;
}


StringVal DecodeArray(FunctionContext* context, const StringVal& arr) {
  StringVal result(context, arr.len);

  memcpy(result.ptr, arr.ptr, arr.len);
  inplace_dub_decode((char*)result.ptr, arr.len);
  return result;
}


#endif
