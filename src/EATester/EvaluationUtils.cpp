#include "EvaluationUtils.h"

#include "EnumCommandParam.h"
#include "RealCoding.h"
#include "RealEvaluation.h"
#include "StringUtils.h"

#include <atlstr.h>
#include <unordered_map>
#include <utility>

template <class TFenotype>
CEvaluation<TFenotype> * EvaluationUtils::pcGetEvaluation(istream *psSettings, CError *pcError)
{
	CEvaluation<TFenotype> *pc_evaluation = nullptr;

	size_t i_fenotype_type_hash_code = typeid(TFenotype).hash_code();

	CConstructorCommandParam<CEvaluation<TFenotype>> p_type(EVALUATION_ARGUMENT_TYPE);

	if (i_fenotype_type_hash_code == typeid(CRealCoding).hash_code())
	{
		p_type.vAddConstructor(EVALUATION_ARGUMENT_TYPE_REAL_CEC2013_LSGO, [&]()
		{
			return (CEvaluation<TFenotype>*)new CRealCEC2013LSGOEvaluation();
		});//p_type.vAddConstructor(EVALUATION_ARGUMENT_TYPE_REAL_CEC2013_LSGO, [&]()

		p_type.vAddConstructor(EVALUATION_ARGUMENT_TYPE_REAL_CEC2013_LSGO_SQUARE, [&]()
		{
			return (CEvaluation<TFenotype>*)new CRealCEC2013LSGOSquareEvaluation();
		});//p_type.vAddConstructor(EVALUATION_ARGUMENT_TYPE_REAL_CEC2013_LSGO_SQUARE, [&]()

		p_type.vAddConstructor(EVALUATION_ARGUMENT_TYPE_REAL_CEC2013_LSGO_SQRT, [&]()
		{
			return (CEvaluation<TFenotype>*)new CRealCEC2013LSGOSqrtEvaluation();
		});//p_type.vAddConstructor(EVALUATION_ARGUMENT_TYPE_REAL_CEC2013_LSGO_SQRT, [&]()

		p_type.vAddConstructor(EVALUATION_ARGUMENT_TYPE_REAL_COMPUTER_NETWORK, [&]()
		{
			return (CEvaluation<TFenotype>*)new CRealComputerNetworkEvaluation();
		});//p_type.vAddConstructor(EVALUATION_ARGUMENT_TYPE_REAL_COMPUTER_NETWORK, [&]()
	}//else if (i_fenotype_type_hash_code == typeid(CRealCoding).hash_code())

	pc_evaluation = p_type.pcGetValue(psSettings, pcError);

	if (!*pcError)
	{
		*pcError = pc_evaluation->eConfigure(psSettings);
	}//if (!*pcError)

	return pc_evaluation;
}//CEvaluation<TFenotype> * EvaluationUtils::pcGetEvaluation(istream *psSettings, CError *pcError)


template CEvaluation<CRealCoding> * EvaluationUtils::pcGetEvaluation(istream*, CError*);