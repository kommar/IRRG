#ifndef GENERATION_UTILS_H
#define GENERATION_UTILS_H

#include "Error.h"
#include "Generation.h"
#include "Problem.h"
#include "RealCoding.h"

#include <istream>

using namespace std;

namespace GenerationUtils
{
	template <class TGenotype, class TFenotype>
	CGeneration<TFenotype> *pcGetSampleFenotypeGeneration(CProblem<TGenotype, TFenotype> *pcProblem);

	template <class TGenotype, class TFenotype>
	CGeneration<TGenotype> *pcGetEmptyGeneration(CProblem<TGenotype, TFenotype> *pcProblem);

	template <class TGenotype, class TFenotype>
	CGeneration<TGenotype> *pcGetGeneration(CProblem<TGenotype, TFenotype> *pcProblem, istream *psSettings, 
		CError *pcError, bool bIsObligatory = true);
};//namespace GenerationUtils

#endif//GENERATION_UTILS_H