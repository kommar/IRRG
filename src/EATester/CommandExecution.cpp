#include "CommandExecution.h"

#include "Decomposition.h"
#include "EnumCommandParam.h"
#include "Evaluation.h"
#include "EvaluationUtils.h"
#include "FloatCommandParam.h"
#include "MathUtils.h"
#include "Problem.h"
#include "RandUtils.h"
#include "RealCoding.h"
#include "RealRandomGeneration.h"
#include "StringCommandParam.h"
#include "StringUtils.h"
#include "System.h"
#include "Transformation.h"
#include "UIntCommandParam.h"

#include <cstdint>
#include <functional>
#include <unordered_map>
#include <vector>

using namespace Decomposition;


CCommandExecution::CCommandExecution(CString sName, istream *psSettings, CLog *pcLog, ostream *psSummary)
{
	s_name = sName;
	ps_settings = psSettings;
	pc_log = pcLog;
	ps_summary = psSummary;
}//CCommandExecution::CCommandExecution(CString sName, CLog *pcLog, ostream *psSummary)

CCommandExecution::~CCommandExecution()
{

}//CCommandExecution::~CCommandExecution()


CDecompositionCommand::CDecompositionCommand(istream *psSettings, CLog *pcLog, ostream *psSummary)
	: CCommandExecution("decomposition", psSettings, pcLog, psSummary)
{

}//CDecompositionCommand::CDecompositionCommand(istream *psSettings, CLog *pcLog, ostream *psSummary)

CError CDecompositionCommand::eExecute(CCommandParamParser *pcParser, uint32_t iRandomSeed)
{
	CError c_error;

	CEvaluation<CRealCoding> *pc_evaluation = EvaluationUtils::pcGetEvaluation<CRealCoding>(ps_settings, &c_error);
	
	CProblem<CRealCoding, CRealCoding> *pc_problem = new CProblem<CRealCoding, CRealCoding>
		(
			pc_evaluation, new CUniformTransformation<CRealCoding>()
		);

	if (!c_error)
	{
		CDecomposition<CRealCoding, CRealCoding> *pc_decomposition = Decomposition::pcGetDecompositon(pc_problem, pc_log, iRandomSeed, ps_settings, &c_error);

		if (!c_error)
		{
			time_t t_start = time(nullptr);

			pc_decomposition->vRun();

			time_t t_end = time(nullptr);

			pc_log->eSave(pc_log->sGetLogFile() + "theta.txt", LOG_SYSTEM_DECOMPOSITION_THETA);

			CString s_output;
			s_output.Format("%u\t%u\t%u", pc_evaluation->iGetFFE(), t_end - t_start, iRandomSeed);

			(*ps_summary) << s_output << "\t" << pc_decomposition->sGetAdditionalSummary();

			delete pc_decomposition;
		}//if (!c_error)
	}//if (!c_error)

	delete pc_problem;

	return c_error;
}//CError CDecompositionCommand::eExecute(CCommandParamParser *pcParser, uint32_t iRandomSeed)