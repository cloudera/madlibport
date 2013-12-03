#ifndef MADLIB_MODULES_IMPALA_LINREG_INL_H
#define MADLIB_MODULES_IMPALA_LINREG_INL_H

#include <cstdio>

#include <impala_udf/udf.h>

using namespace impala_udf;
using namespace std;

/*! \brief Initializes the UDA state with zeros
 */
void LinrInit(FunctionContext* context, StringVal* m);

/*! \brief Updates the input state with the given value
 */
void LinrUpdate(FunctionContext* context, const StringVal& val,
                  const DoubleVal &y, StringVal* input);

/*! Combines two linear regression states
 */
void LinrMerge(FunctionContext* context, const StringVal& src, StringVal* dst);

/*! \brief Computes the solution and returns the coefficient vector
 */
StringVal LinrFinalize(FunctionContext* context, const StringVal& input);


/*! \brief Uses the trained model (output from LinRegFinalize) to predict an ex
 *
 * Does not allocate memory.  This is a simple dot product.
 * \param context Not used to allocate memroy
 * \param model the tarined model, result of LinRegFinalize
 * \param examp a double array of the example vector to predict
 * \return the predicted label of the example
 */
DoubleVal LinrPredict(FunctionContext* context, const StringVal& model,
                        const StringVal& examp);


#endif
