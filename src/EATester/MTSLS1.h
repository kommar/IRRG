#ifndef MTSLS1_H
#define MTSLS1_H

#include "Error.h"
#include "Generation.h"
#include "Log.h"
#include "Optimizer.h"
#include "Problem.h"
#include "RealCoding.h"

#include <ctime>
#include <cstdint>
#include <istream>
#include <vector>

using namespace std;

class CMTSLS1 : public COptimizer<CRealCoding, CRealCoding>
{
public:
	CMTSLS1(CProblem<CRealCoding, CRealCoding> *pcProblem, CLog *pcLog, uint32_t iRandomSeed);
	CMTSLS1(CMTSLS1 *pcOther);

	virtual COptimizer<CRealCoding, CRealCoding> *pcCopy() { return new CMTSLS1(this); };

	virtual CError eConfigure(istream *psSettings);

	virtual void vInitialize(time_t tStartTime);
	virtual bool bRunIteration(uint32_t iIterationNumber, time_t tStartTime);

private:
	CIndividual<CRealCoding, CRealCoding> *pc_run_one_dimension(uint16_t iDimension);
	void v_sort_dimensions();

	CGeneration<CRealCoding> *pc_generation;

	double d_sr_initial_range;
	vector<double> v_srs;
	vector<double> v_improvements;
	vector<uint16_t> v_dimensions;
	uint16_t i_dimensions_index;
};//class CMTSLS1 : public COptimizer<CRealCoding, CRealCoding>

#endif//MTSLS1_H