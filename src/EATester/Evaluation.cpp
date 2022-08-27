#include "Evaluation.h"

#include "EvaluationUtils.h"
#include "RealCoding.h"
#include "UIntCommandParam.h"

#include <algorithm>
#include <atlstr.h>
#include <cfloat>
#include <utility>

template <class TFenotype>
uint32_t CEvaluation<TFenotype>::iERROR_PARENT_CEVALUATION = CError::iADD_ERROR_PARENT("CEvaluation");

template <class TFenotype>
CEvaluation<TFenotype>::CEvaluation()
{
	v_init(0, DBL_MAX);
}//CEvaluation<TFenotype>::CEvaluation()

template <class TFenotype>
CEvaluation<TFenotype>::CEvaluation(uint16_t iNumberOfElements, double dMaxValue)
{
	v_init(iNumberOfElements, dMaxValue);
}//CEvaluation<TFenotype>::CEvaluation(uint16_t iNumberOfElements, double dMaxValue)

template <class TFenotype>
CEvaluation<TFenotype>::~CEvaluation()
{

}//CEvaluation<TFenotype>::~CEvaluation()

template <class TFenotype>
double CEvaluation<TFenotype>::dEvaluate(TFenotype *pcFenotype)
{
	i_ffe++;
	return d_evaluate(pcFenotype, 0);
}//double CEvaluation<TFenotype>::dEvaluate(TFenotype *pcFenotype)

template <class TFenotype>
void CEvaluation<TFenotype>::v_init(uint16_t iNumberOfElements, double dMaxValue)
{
	i_ffe = 0;
	i_number_of_elements = iNumberOfElements;
	d_max_value = dMaxValue;
}//void CEvaluation<TFenotype>::v_init(uint16_t iNumberOfElements, double dMaxValue)


template class CEvaluation<CRealCoding>;