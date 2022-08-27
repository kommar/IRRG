#ifndef EVALUATION_UTILS_H
#define EVALUATION_UTILS_H

#include "Error.h"
#include "Evaluation.h"

#include <istream>

using namespace std;

namespace EvaluationUtils
{
	template <class TFenotype>
	CEvaluation<TFenotype> *pcGetEvaluation(istream *psSettings, CError *pcError);
}//namespace EvaluationUtils

#endif//EVALUATION_UTILS_H