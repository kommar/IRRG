#include "StopConditionUtils.h"

#include "EnumCommandParam.h"
#include "RealCoding.h"
#include "RunningTimeStopCondition.h"

#include <atlstr.h>
#include <unordered_map>

template <class TGenotype, class TFenotype>
CStopCondition<TGenotype, TFenotype>* StopConditionUtils::pcGetStopCondition(CEvaluation<TFenotype> *pcEvaluation, istream *psSettings, CError *pcError)
{
	CStopCondition<TGenotype, TFenotype> *pc_stop_condition = nullptr;

	unordered_map<CString, EStopConditionType> m_stop_condition_types;
	m_stop_condition_types.insert(pair<const CString, EStopConditionType>(STOP_CONDITION_ARGUMENT_TYPE_NONE, STOP_CONDITION_NONE));
	m_stop_condition_types.insert(pair<const CString, EStopConditionType>(STOP_CONDITION_ARGUMENT_TYPE_RUNNING_TIME, STOP_CONDITION_RUNNING_TIME));
	m_stop_condition_types.insert(pair<const CString, EStopConditionType>(STOP_CONDITION_ARGUMENT_TYPE_ITERATIONS, STOP_CONDITION_ITERATIONS));
	m_stop_condition_types.insert(pair<const CString, EStopConditionType>(STOP_CONDITION_ARGUMENT_TYPE_PROGRESS, STOP_CONDITION_PROGRESS));
	m_stop_condition_types.insert(pair<const CString, EStopConditionType>(STOP_CONDITION_ARGUMENT_TYPE_FFE, STOP_CONDITION_FFE));

	CEnumCommandParam<EStopConditionType> p_type(STOP_CONDITION_ARGUMENT_TYPE, &m_stop_condition_types);
	EStopConditionType e_type = p_type.eGetValue(psSettings, pcError);

	if (!*pcError)
	{
		switch (e_type)
		{
			case STOP_CONDITION_NONE:
			{
				pc_stop_condition = new CNoneStopCondition<TGenotype, TFenotype>();
				break;
			}//case STOP_CONDITION_NONE
			case STOP_CONDITION_RUNNING_TIME:
			{
				pc_stop_condition = new CRunningTimeStopCondition<TGenotype, TFenotype>(pcEvaluation);
				break;
			}//case STOP_CONDITION_RUNNING_TIME
			case STOP_CONDITION_ITERATIONS:
			{
				pc_stop_condition = new CIterationsStopCondition<TGenotype, TFenotype>(pcEvaluation);
				break;
			}//case STOP_CONDITION_ITERATIONS
			case STOP_CONDITION_PROGRESS:
			{
				pc_stop_condition = new CProgressStopCondition<TGenotype, TFenotype>(pcEvaluation);
				break;
			}//case STOP_CONDITION_PROGRESS
			case STOP_CONDITION_FFE:
			{
				pc_stop_condition = new CFFEStopCondition<TGenotype, TFenotype>(pcEvaluation);
				break;
			}//case STOP_CONDITION_FFE
			default:
			{
				pcError->vSetError(CError::iERROR_CODE_OPERATOR_NOT_FOUND, "stop condition");
				break;
			}//default
		}//switch (e_type)

		if (!*pcError)
		{
			*pcError = pc_stop_condition->eConfigure(psSettings);
		}//if (!*pcError)
	}//if (!*pcError)

	return pc_stop_condition;
}//CStopCondition<TGenotype, TFenotype>* StopConditionUtils::pcGetStopCondition(CEvaluation<TFenotype> *pcEvaluation, istream *psSettings, CError *pcError)


template CStopCondition<CRealCoding, CRealCoding> * StopConditionUtils::pcGetStopCondition(CEvaluation<CRealCoding>*, istream*, CError*);