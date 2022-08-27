#include "OptimizationCommand.h"

#include "EnumCommandParam.h"
#include "Evaluation.h"
#include "EvaluationUtils.h"
#include "Optimizer.h"
#include "OptimizerUtils.h"
#include "Problem.h"
#include "StringUtils.h"
#include "System.h"

#include <unordered_map>
#include <utility>


COptimizationBasedCommand::COptimizationBasedCommand(CString sName, istream *psSettings, CLog *pcLog, ostream *psSummary)
	: CCommandExecution(sName, psSettings, pcLog, psSummary)
{

}//COptimizationBasedCommand::COptimizationBasedCommand(CString sName, istream *psSettings, CLog *pcLog, ostream *psSummary)

CError COptimizationBasedCommand::eExecute(CCommandParamParser *pcParser, uint32_t iRandomSeed)
{
	CError c_error;

	unordered_map<CString, EProblemType> m_problem_types;
	m_problem_types.insert(pair<const CString, EProblemType>(PROBLEM_ARGUMENT_TYPE_REAL_REAL, PROBLEM_REAL_REAL));

	CEnumCommandParam<EProblemType> p_problem_type(PROBLEM_ARGUMENT_TYPE, &m_problem_types);
	EProblemType e_problem_type = p_problem_type.eGetValue(ps_settings, &c_error);

	if (!c_error)
	{
		switch (e_problem_type)
		{
			case PROBLEM_REAL_REAL:
			{
				c_error = e_optimize_real_real(iRandomSeed);
				break;
			}//PROBLEM_REAL_REAL
			default:
			{
				c_error.vSetError(CError::iERROR_CODE_OPERATOR_NOT_FOUND, "problem");
				break;
			}//default
		}//switch (e_problem_type)
	}//if (!c_error)

	return c_error;
}//CError COptimizationBasedCommand::eExecute(CCommandParamParser *pcParser, uint32_t iRandomSeed)


COptimizationCommand::COptimizationCommand(istream *psSettings, CLog *pcLog, ostream *psSummary)
	: COptimizationBasedCommand(SYSTEM_ARGUMENT_MODE_OPTIMIZATION, psSettings, pcLog, psSummary)
{

}//COptimizationCommand::COptimizationCommand(istream *psSettings, CLog *pcLog, ostream *psSummary)

template <class TGenotype, class TFenotype>
CError COptimizationCommand::e_optimize(uint32_t iRandomSeed)
{
	CError c_error;

	CEvaluation<TFenotype> *pc_evaluation = EvaluationUtils::pcGetEvaluation<TFenotype>(ps_settings, &c_error);

	if (!c_error)
	{
		CProblem<TGenotype, TFenotype> *pc_problem = new CProblem<TGenotype, TFenotype>(pc_evaluation, new CUniformTransformation<TGenotype>());
		COptimizer<TGenotype, TFenotype> *pc_optimizer = OptimizerUtils::pcGetOptimizer(pc_problem, pc_log, iRandomSeed, ps_settings, &c_error);

		if (!c_error)
		{
			pc_optimizer->vRun();

			CString s_summary;

			
			s_summary.Format("%.16f\t%.16f\t%llu\t%u\t%llu", pc_optimizer->pcGetBestIndividual()->dGetFitnessValue(),
				pc_problem->pcGetEvaluation()->dGetMaxValue(), pc_optimizer->iGetBestFFE(),
				(uint32_t)pc_optimizer->tGetBestTime(), pc_problem->pcGetEvaluation()->iGetFFE());


			if (pc_problem->pcGetEvaluation() != NULL)  s_summary += " " + pc_problem->pcGetEvaluation()->sAdditionalSummaryInfo();

			s_summary += " \tLogFile:\t" + pc_optimizer->sGetLogName();

			(*ps_summary) << s_summary;
			(*ps_summary) << "\t" << pc_optimizer->pcGetBestIndividual()->pcGetFenotype();
		}//if (!c_error)

		delete pc_optimizer;
		delete pc_problem;
	}//if (!c_error)
	else
	{
		delete pc_evaluation;
	}//else if (!c_error)

	return c_error;
}//CError COptimizationCommand::e_optimize(uint32_t iRandomSeed)