#ifndef STOP_CONDITION_UTILS_H
#define STOP_CONDITION_UTILS_H

#include "Error.h"
#include "Evaluation.h"
#include "StopCondition.h"

#include <istream>

using namespace std;

namespace StopConditionUtils
{
	template <class TGenotype, class TFenotype>
	CStopCondition<TGenotype, TFenotype> *pcGetStopCondition(CEvaluation<TFenotype> *pcEvaluation, istream *psSettings, CError *pcError);
}//namespace StopConditionUtils

#endif//STOP_CONDITION_UTILS_H