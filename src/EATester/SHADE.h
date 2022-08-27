#ifndef SHADE_H
#define SHADE_H

#define SHADE_ARGUMENT_POPULATION_SIZE "population_size"
#define SHADE_ARGUMENT_ARC_RATE "arc_rate"
#define SHADE_ARGUMENT_PBEST_RATE "pbest_rate"
#define SHADE_ARGUMENT_MEMORY_SIZE "memory_size"

#include "Error.h"
#include "Log.h"
#include "Optimizer.h"
#include "Problem.h"
#include "RealCoding.h"

#include "../SHADE/de.h"

#include <ctime>
#include <cstdint>
#include <istream>

class CSHADE : COptimizer<CRealCoding, CRealCoding>
{
public:
	CSHADE(CProblem<CRealCoding, CRealCoding> *pcProblem, CLog *pcLog, uint32_t iRandomSeed);
	CSHADE(CSHADE *pcOther);

	virtual CError eConfigure(istream *psSettings);

	virtual COptimizer<CRealCoding, CRealCoding> *pcCopy() { return new CSHADE(this); };

	virtual void vInitialize(time_t tStartTime);
	virtual bool bRunIteration(uint32_t iIterationNumber, time_t tStartTime);

private:
	using COptimizer<CRealCoding, CRealCoding>::b_update_best_individual;
	bool b_update_best_individual(uint32_t iIterationNumber, time_t tStartTime);

	uint32_t i_population_size;
	double d_arc_rate;
	double d_pbest_rate;
	uint32_t i_memory_size;

	SHADE c_shade;
};//class CSHADE : COptimizer<CRealCoding, CRealCoding>

#endif//SHADE_H