#include "Generation.h"

#include "RealCoding.h"


template <class TGenotype>
uint32_t CGeneration<TGenotype>::iERROR_PARENT_CGeneration = CError::iADD_ERROR_PARENT("CGeneration");



template <class TGenotype>
CGeneration<TGenotype>::CGeneration()
{
	pc_gene_pattern = nullptr;
}//CGeneration<TGenotype>::CGeneration()



template <class TGenotype>
CGeneration<TGenotype>::~CGeneration()
{
	v_clear_best_genotypes();
}//CGeneration<TGenotype>::~CGeneration()



template <class TGenotype>
void CGeneration<TGenotype>::vUpdateBestGenotypes(uint8_t iCount, TGenotype **ppcBestGenotypes)
{
	v_clear_best_genotypes();

	TGenotype *pc_genotype_copy;

	for (uint8_t i = 0; i < iCount; i++)
	{
		pc_genotype_copy = new TGenotype(*(ppcBestGenotypes + i));
		v_best_genotypes.push_back(pc_genotype_copy);
	}//for (uint8_t i = 0; i < iCount; i++)
}//void CGeneration<TGenotype>::vUpdateBestGenotypes(uint8_t iCount, TGenotype **ppcBestGenotypes)

template <class TGenotype>
void CGeneration<TGenotype>::v_clear_best_genotypes()
{
	for (uint32_t i = 0; i < (uint32_t)v_best_genotypes.size(); i++)
	{
		delete v_best_genotypes.at(i);
	}//for (uint32_t i = 0; i < (uint32_t)v_best_genotypes.size(); i++)

	v_best_genotypes.clear();
}//void CGeneration<TGenotype>::v_clear_best_genotypes()

template class CGeneration<CRealCoding>;