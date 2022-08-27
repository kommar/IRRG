#ifndef RUNNING_TIME_STOP_CONDITION_H
#define RUNNING_TIME_STOP_CONDITION_H

#include "Evaluation.h"
#include "StopCondition.h"

#include <ctime>
#include <istream>

using namespace std;

template <class TGenotype, class TFenotype>
class CRunningTimeStopCondition : public CStopCondition<TGenotype, TFenotype>
{
public:
	CRunningTimeStopCondition(CEvaluation<TFenotype> *pcEvaluation);
	CRunningTimeStopCondition(CEvaluation<TFenotype> *pcEvaluation, time_t tRunningTime);

	virtual CError eConfigure(istream *psSettings);

	virtual bool bStop(time_t tStart, uint32_t iIterationNumber, uint64_t iFFE, CIndividual<TGenotype, TFenotype> *pcBestIndividual);

	virtual double dGetNow(time_t tStart, uint32_t iIterationNumber, uint64_t iFFE, CIndividual<TGenotype, TFenotype> *pcBestIndividual) { return (double)(time(nullptr) - tStart); }
	virtual double dGetMax() { return (double)t_running_time; }

private:
	time_t t_running_time;
};//class CRunningTimeStopCondition

#endif//RUNNING_TIME_STOP_CONDITION_H