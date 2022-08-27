#include "Optimizer.h"

#include "GenerationUtils.h"
#include "RealCoding.h"
#include "StopConditionUtils.h"
#include "UIntCommandParam.h"


template <class TGenotype, class TFenotype>
COptimizer<TGenotype, TFenotype>::COptimizer(CProblem<TGenotype, TFenotype> *pcProblem, CLog *pcLog, uint32_t iRandomSeed)
{
	pc_problem = pcProblem;

	pc_stop_condition = nullptr;

	pc_log = pcLog;

	pc_empty_generation = GenerationUtils::pcGetEmptyGeneration(pcProblem);
	pc_best_individual = nullptr;

	i_random_seed = iRandomSeed;

	b_own_params = true;
}//COptimizer<TGenotype, TFenotype>::COptimizer(CProblem<TGenotype, TFenotype> *pcProblem, CLog *pcLog, uint32_t iRandomSeed)

template <class TGenotype, class TFenotype>
COptimizer<TGenotype, TFenotype>::COptimizer(COptimizer<TGenotype, TFenotype> *pcOther)
{
	pc_problem = pcOther->pc_problem;

	pc_stop_condition = pcOther->pc_stop_condition;

	pc_log = pcOther->pc_log;

	pc_empty_generation = pcOther->pc_empty_generation;
	pc_best_individual = nullptr;

	i_random_seed = pcOther->i_random_seed;

	b_own_params = false;
}//COptimizer<TGenotype, TFenotype>::COptimizer(COptimizer<TGenotype, TFenotype> *pcOther)

template <class TGenotype, class TFenotype>
COptimizer<TGenotype, TFenotype>::~COptimizer()
{
	v_clear_params();

	if (b_own_params)
	{
		delete pc_empty_generation;
	}//if (b_own_params)

	vResetBestIndividual();
}//COptimizer::~COptimizer()

template <class TGenotype, class TFenotype>
CError COptimizer<TGenotype, TFenotype>::eConfigure(istream *psSettings)
{
	CError c_error;

	v_clear_params();

	pc_stop_condition = StopConditionUtils::pcGetStopCondition<TGenotype, TFenotype>(pc_problem->pcGetEvaluation(), psSettings, &c_error);

	return c_error;
}//CError COptimizer<TGenotype, TFenotype>::eConfigure(istream *psSettings)

template <class TGenotype, class TFenotype>
void COptimizer<TGenotype, TFenotype>::vInitialize(time_t tStartTime)
{
	vResetBestIndividual();
}//void COptimizer<TGenotype, TFenotype>::vInitialize(time_t tStartTime)

template <class TGenotype, class TFenotype>
void COptimizer<TGenotype, TFenotype>::vRun()
{
	CTimeCounter  c_time_counter;
	time_t t_start_time = time(nullptr);

	CString  s_buf;
	
	double  d_best_fitness;
	uint32_t i_iteration_number;
	double  d_time;
	uint64_t i_ffe;

	vInitialize(t_start_time);
	c_time_counter.vSetStartNow();

	i_iteration_number = 0;

	while (!pc_stop_condition->bStop(t_start_time, i_iteration_number, pc_problem->pcGetEvaluation()->iGetFFE(), pc_best_individual))
	{
		bRunIteration(i_iteration_number, t_start_time);
		i_iteration_number++;

		d_best_fitness = pc_best_individual->dGetFitnessValue();
		c_time_counter.bGetTimePassed(&d_time);
		i_ffe = pc_problem->pcGetEvaluation()->iGetFFE();

		CString log_message;
		log_message.AppendFormat("[PRW LOG] best fitness: \t %.8lf \t ffe: \t %u \t time: \t %.4lf", d_best_fitness, i_ffe, d_time);
		//pc_log->vPrintLine(log_message, true, PRW_LOG_SYSTEM);

		if (pc_problem->pcGetEvaluation()->bMultiObjective() == true)
		{
			pc_log->vPrintLine(pc_problem->pcGetEvaluation()->sMultiObjectiveReportIter(), true);
			//CBinaryMultiObjectiveProblem *pc_problem_multi;
			//pc_problem_multi = (CBinaryMultiObjectiveProblem *) pc_problem->pcGetEvaluation();

			//return(pc_problem_multi->dPFQualityInverseGenerationalDistance());

			//s_buf.Format("iteration: %d  PFsize:%d HyperVolume: %.8lf InvGenDist: %.8lf GenDist:%.8lf MaxSpread:%.8lf DominatedPF: %d [time:%.2lf] [ffe: %.0lf]", i_iteration_number, (int)dPFQualityPointNum(), dPFQualityHyperVolume(), dPFQualityInverseGenerationalDistance(), dPFQualityGenerationalDistance(), dPFQualityMaximumSpread(), (int)dPFQualityDominatedOptimalPoints(), d_time_passed, (double)pc_multi_problem->iGetFFE());
			//  ///pc_log->vPrintLine(s_buf, true);
		}//if (pc_problem->pcGetEvaluation()->bMultiObjective() == true)

	}//while (!pc_stop_condition->bStop(t_start_time, i_iteration_number, pc_problem->pcGetEvaluation()->iGetFFE(), pc_best_individual))


	if (pc_problem->pcGetEvaluation()->bMultiObjective() == true)
	{
		//pc_log->vPrintLine("PARETO FRONT:", true, LOG_SYSTEM_PARETO_FRONT);

		vector<CString>  v_pf_report;
		pc_problem->pcGetEvaluation()->vReportPF(&v_pf_report);

		for (int ii = 0; ii < v_pf_report.size(); ii++)
		{
			pc_log->vPrintLine(v_pf_report.at(ii), false, LOG_SYSTEM_PARETO_FRONT);
		}//for (int ii = 0; ii < v_pf_report.size(); ii++)				
	}//if (pc_problem->pcGetEvaluation()->bMultiObjective() == true)
	
}//void COptimizer<TGenotype, TFenotype>::vRun()


template <class TGenotype, class TFenotype>
CString  COptimizer<TGenotype, TFenotype>::sGetLogName()
{
	if (pc_log == NULL)  return("<no log>");

	return(pc_log->sGetLogFile());
}//CString  COptimizer<TGenotype, TFenotype>::sGetLogName()



template <class TGenotype, class TFenotype>
void COptimizer<TGenotype, TFenotype>::vResetBestIndividual()
{
	delete pc_best_individual;
	pc_best_individual = nullptr;
}//void COptimizer<TGenotype, TFenotype>::vResetBestIndividual()

template <class TGenotype, class TFenotype>
void COptimizer<TGenotype, TFenotype>::vSetBestIndividual(CIndividual<TGenotype, TFenotype> *pcBestIndividual, bool bCopy)
{
	vResetBestIndividual();
	pc_best_individual = bCopy ? new CIndividual<TGenotype, TFenotype>(pcBestIndividual) : pcBestIndividual;
}//void COptimizer<TGenotype, TFenotype>::vSetBestIndividual(CIndividual<TGenotype, TFenotype> *pcBestIndividual, bool bCopy)

template <class TGenotype, class TFenotype>
bool COptimizer<TGenotype, TFenotype>::b_update_best_individual(uint32_t iIterationNumber, time_t tStartTime, CIndividual<TGenotype, TFenotype> *pcIndividual)
{
	bool b_updated = false;

	if (!pc_best_individual || pc_problem->bIsBetterIndividual(pcIndividual, pc_best_individual))
	{
		delete pc_best_individual;
		pc_best_individual = new CIndividual<TGenotype, TFenotype>(pcIndividual);

		v_update_statistics_of_best(iIterationNumber, tStartTime);

		b_updated = true;
	}//if (!pc_best_individual || pc_problem->bIsBetterIndividual(pcIndividual, pc_best_individual))

	return b_updated;
}//bool COptimizer<TGenotype, TFenotype>::b_update_best_individual(uint32_t iIterationNumber, time_t tStartTime, CIndividual<TGenotype, TFenotype> *pcIndividual)

template <class TGenotype, class TFenotype>
bool COptimizer<TGenotype, TFenotype>::b_update_best_individual(uint32_t iIterationNumber, time_t tStartTime, double dCurrentBestFitnessValue, function<void(TGenotype*)> &&fUpdateBestGenotype)
{
	bool b_updated = false;

	if (!pc_best_individual || pc_problem->bIsBetterFitnessValue(dCurrentBestFitnessValue, pc_best_individual->dGetFitnessValue()))
	{
		if (!pc_best_individual)
		{
			pc_best_individual = pc_create_individual(pc_empty_generation->pcGenerateEmpty());
		}//if (!pc_best_individual)

		fUpdateBestGenotype(pc_best_individual->pcGetGenotype());
		pc_best_individual->vSetFitnessValue(dCurrentBestFitnessValue);

		v_update_statistics_of_best(iIterationNumber, tStartTime);

		b_updated = true;
	}//if (!pc_best_individual || pc_problem->bIsBetterFitnessValue(dCurrentBestFitnessValue, pc_best_individual->dGetFitnessValue()))

	return b_updated;
}//bool COptimizer<TGenotype, TFenotype>::b_update_best_individual(uint32_t iIterationNumber, time_t tStartTime, double dCurrentBestFitnessValue, function<void(TGenotype*)> &&fUpdateBestGenotype)

template <class TGenotype, class TFenotype>
void COptimizer<TGenotype, TFenotype>::v_clear_params()
{
	if (b_own_params)
	{
		delete pc_stop_condition;
		pc_stop_condition = nullptr;
	}//if (b_own_params)
}//void COptimizer<TGenotype, TFenotype>::v_clear_params()

template <class TGenotype, class TFenotype>
void COptimizer<TGenotype, TFenotype>::v_update_statistics_of_best(uint32_t iIterationNumber, time_t tStartTime)
{
	t_best_time = time(nullptr) - tStartTime;
	i_best_ffe = pc_problem->pcGetEvaluation()->iGetFFE();
}//void COptimizer<TGenotype, TFenotype>::v_update_statistics_of_best(uint32_t iIterationNumber, time_t tStartTime)


template class COptimizer<CRealCoding, CRealCoding>;