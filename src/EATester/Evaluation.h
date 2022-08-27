#ifndef EVALUATION_H
#define EVALUATION_H

#define EVALUATION_ARGUMENT_TYPE "evaluation_type"
#define EVALUATION_ARGUMENT_TYPE_REAL_CEC2013_LSGO "real_cec2013_lsgo"
#define EVALUATION_ARGUMENT_TYPE_REAL_CEC2013_LSGO_SQRT "real_cec2013_lsgo_sqrt"
#define EVALUATION_ARGUMENT_TYPE_REAL_CEC2013_LSGO_SQUARE "real_cec2013_lsgo_square"
#define EVALUATION_ARGUMENT_TYPE_REAL_COMPUTER_NETWORK "real_computer_network"

#define EVALUATION_ARGUMENT_RANDOM_SEED "evaluation_random_seed"

#define EVALUATION_ARGUMENT_NUMBER_OF_ELEMENTS "evaluation_number_of_elements"


#include "Error.h"

#include <cstdint>
#include <istream>
#include <vector>

using namespace std;


template <class TFenotype>
class CEvaluation
{
public:
	CEvaluation();
	CEvaluation(uint16_t iNumberOfElements, double dMaxValue);

	virtual ~CEvaluation();

	virtual CError eConfigure(istream *psSettings) { return CError(iERROR_PARENT_CEVALUATION); };

	double dEvaluate(TFenotype *pcFenotype);

	virtual TFenotype *pcCreateSampleFenotype() = 0;

	virtual bool bIsMaxValue(double dValue) { return dValue == d_max_value; };
	virtual bool bOptimalFound() { return(false); }

	uint16_t iGetNumberOfElements() { return i_number_of_elements; };

	double dGetMaxValue() { return d_max_value; };

	uint64_t iGetFFE() { return i_ffe; };

	void vSetFFE(uint64_t iFFE) { i_ffe = iFFE; };
	void vSetZeroFFE() { vSetFFE(0); };

	void vIncreaseFFE() { i_ffe++; };
	void vDecreaseFFE() { i_ffe--; };


	virtual CString  sAdditionalSummaryInfo() { return(""); }
	virtual bool  bMultiObjective() { return(false); }
	virtual void  vReportPF(vector<CString> *pvPFReport) {};
	virtual CString  sMultiObjectiveReportIter() { return(""); };

protected:
	static uint32_t iERROR_PARENT_CEVALUATION;

	virtual double d_evaluate(TFenotype *pcFenotype, uint16_t iShift) = 0;

	void v_init(uint16_t iNumberOfElements, double dMaxValue);

	uint16_t i_number_of_elements;

	double d_max_value;

	uint64_t i_ffe;
};//class CEvaluation

#endif//EVALUATION_H