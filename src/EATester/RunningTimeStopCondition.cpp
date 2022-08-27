#include "RunningTimeStopCondition.h"

#include "RealCoding.h"
#include "UIntCommandParam.h"

template <class TGenotype, class TFenotype>
CRunningTimeStopCondition<TGenotype, TFenotype>::CRunningTimeStopCondition(CEvaluation<TFenotype> *pcEvaluation)
	: CStopCondition<TGenotype, TFenotype>(pcEvaluation)
{

}//CRunningTimeStopCondition<TGenotype, TFenotype>::CRunningTimeStopCondition(CEvaluation<TFenotype> *pcEvaluation)

template <class TGenotype, class TFenotype>
CRunningTimeStopCondition<TGenotype, TFenotype>::CRunningTimeStopCondition(CEvaluation<TFenotype> *pcEvaluation, time_t tRunningTime)
	: CStopCondition<TGenotype, TFenotype>(pcEvaluation)
{
	t_running_time = tRunningTime;
}//CRunningTimeStopCondition<TGenotype, TFenotype>::CRunningTimeStopCondition(CEvaluation<TFenotype> *pcEvaluation, time_t tRunningTime)

template <class TGenotype, class TFenotype>
CError CRunningTimeStopCondition<TGenotype, TFenotype>::eConfigure(istream *psSettings)
{
	CError c_error = CStopCondition<TGenotype, TFenotype>::eConfigure(psSettings);

	if (!c_error)
	{
		CUIntCommandParam p_running_time(STOP_CONDITION_ARGUMENT_TIME);
		t_running_time = (time_t)p_running_time.iGetValue(psSettings, &c_error);
	}//if (!c_error)

	return c_error;
}//CError CRunningTimeStopCondition<TGenotype, TFenotype>::eConfigure(istream *psSettings)

template <class TGenotype, class TFenotype>
bool CRunningTimeStopCondition<TGenotype, TFenotype>::bStop(time_t tStart, uint32_t iIterationNumber, uint64_t iFFE, CIndividual<TGenotype, TFenotype> *pcBestIndividual)
{
	bool b_stop = CStopCondition::bStop(tStart, iIterationNumber, iFFE, pcBestIndividual);

	if (!b_stop)
	{
		time_t t_now = time(nullptr);
		b_stop = tStart + t_running_time <= t_now;
	}//if (!b_stop)

	return b_stop;
}//bool CRunningTimeStopCondition<TGenotype, TFenotype>::bStop(time_t tStart, uint32_t iIterationNumber, uint64_t iFFE, CIndividual<TGenotype, TFenotype> *pcBestIndividual)


template class CRunningTimeStopCondition<CRealCoding, CRealCoding>;