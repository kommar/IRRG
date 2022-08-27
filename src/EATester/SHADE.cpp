#include "SHADE.h"

#include "PointerUtils.h"

#include "FloatCommandParam.h"
#include "UIntCommandParam.h"

CSHADE::CSHADE(CProblem<CRealCoding, CRealCoding> *pcProblem, CLog *pcLog, uint32_t iRandomSeed)
	: COptimizer<CRealCoding, CRealCoding>(pcProblem, pcLog, iRandomSeed), c_shade(pcProblem->pcGetEvaluation())
{

}//CSHADE::CSHADE(CProblem<CRealCoding, CRealCoding> *pcProblem, CLog *pcLog, uint32_t iRandomSeed)

CSHADE::CSHADE(CSHADE *pcOther)
	: COptimizer<CRealCoding, CRealCoding>(pcOther), c_shade(pcOther->pcGetProblem()->pcGetEvaluation())
{
	i_population_size = pcOther->i_population_size;
	d_arc_rate = pcOther->d_arc_rate;
	d_pbest_rate = pcOther->d_pbest_rate;
	i_memory_size = pcOther->i_memory_size;
}//CSHADE::CSHADE(CSHADE *pcOther)

CError CSHADE::eConfigure(istream *psSettings)
{
	CError c_error = COptimizer<CRealCoding, CRealCoding>::eConfigure(psSettings);

	if (!c_error)
	{
		CUIntCommandParam p_population_size(SHADE_ARGUMENT_POPULATION_SIZE, 1, UINT32_MAX);
		i_population_size = p_population_size.iGetValue(psSettings, &c_error);
	}//if (!c_error)

	if (!c_error)
	{
		CDoubleCommandParam p_arc_rate(SHADE_ARGUMENT_ARC_RATE);
		d_arc_rate = p_arc_rate.dGetValue(psSettings, &c_error);
	}//if (!c_error)

	if (!c_error)
	{
		CDoubleCommandParam p_pbest_rate(SHADE_ARGUMENT_PBEST_RATE);
		d_pbest_rate = p_pbest_rate.dGetValue(psSettings, &c_error);
	}//if (!c_error)

	if (!c_error)
	{
		CUIntCommandParam p_population_size(SHADE_ARGUMENT_MEMORY_SIZE, 1, UINT32_MAX);
		i_memory_size = p_population_size.iGetValue(psSettings, &c_error);
	}//if (!c_error)

	if (!c_error)
	{
		c_shade.setPopSize((int)i_population_size);
		c_shade.setArcRate(d_arc_rate);
		c_shade.setPBestRate(d_pbest_rate);
		c_shade.setMemorySize(i_memory_size);
	}//if (!c_error)

	return c_error;
}//CError CSHADE::eConfigure(istream *psSettings)

void CSHADE::vInitialize(time_t tStartTime)
{
	COptimizer<CRealCoding, CRealCoding>::vInitialize(tStartTime);

	c_shade.initialize();
	b_update_best_individual(0, tStartTime);
}//void CSHADE::vInitialize(time_t tStartTime)

bool CSHADE::bRunIteration(uint32_t iIterationNumber, time_t tStartTime)
{
	c_shade.runIteration(nullptr);

	bool b_updated = b_update_best_individual(iIterationNumber, tStartTime);

	CString s_log_message;

	s_log_message.Format
	(
		"iteration: %u; best fitness: %f; ffe: %u; time: %u",
		iIterationNumber,
		pc_best_individual->dGetFitnessValue(), 
		pc_problem->pcGetEvaluation()->iGetFFE(),
		(uint32_t)(time(nullptr) - tStartTime)
	);//s_log_message.Format

	pc_log->vPrintLine(s_log_message, true);

	return b_updated;
}//bool CSHADE::bRunIteration(uint32_t iIterationNumber, time_t tStartTime)

bool CSHADE::b_update_best_individual(uint32_t iIterationNumber, time_t tStartTime)
{
	bool b_updated = b_update_best_individual(iIterationNumber, tStartTime, -c_shade.getBSFFitness(), [&](CRealCoding *pcBestGenotype)
	{
		for (uint16_t i = 0; i < pc_problem->pcGetEvaluation()->iGetNumberOfElements(); i++)
		{
			*(pcBestGenotype->pdGetValues() + i) = *(c_shade.getBSFSolution() + i);
		}//for (uint16_t i = 0; i < pc_problem->pcGetEvaluation()->iGetNumberOfElements(); i++)
	});//bool b_updated = b_update_best_individual(iIterationNumber, tStartTime, -c_shade.getBSFFitness(), [&](CRealCoding *pcBestGenotype)

	return b_updated;
}//bool CSHADE::b_update_best_individual(uint32_t iIterationNumber, time_t tStartTime)