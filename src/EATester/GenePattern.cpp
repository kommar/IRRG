#include "GenePattern.h"

#include "VectorUtils.h"

#include <algorithm>

CGenePattern::CGenePattern()
{
	pv_pattern = new vector<uint16_t>();
	pd_differences = new vector<double>();
	pd_means = new vector<double>();
	pd_standard_deviations = new vector<double>();
	pi_bits = new vector<int32_t>();

	pc_parent_pattern = nullptr;
	pv_nested_patterns = new vector<CGenePattern*>();
}//CGenePattern::CGenePattern()

CGenePattern::CGenePattern(uint16_t iSignificantIndex)
	: CGenePattern()
{
	i_significant_index = iSignificantIndex;
}//CGenePattern::CGenePattern(uint16_t iSignificantIndex)

CGenePattern::CGenePattern(CGenePattern *pcOtherPattern)
	: CGenePattern(pcOtherPattern, nullptr)
{

}//CGenePattern::CGenePattern(CGenePattern *pcOtherPattern)

CGenePattern::CGenePattern(CGenePattern *pcOtherPattern, CGenePattern *pcParentPattern)
	: CGenePattern()
{
	i_significant_index = pcOtherPattern->i_significant_index;

	pv_pattern->reserve(pcOtherPattern->pv_pattern->size());
	pd_differences->reserve(pcOtherPattern->pd_differences->size());
	pd_means->reserve(pcOtherPattern->pd_means->size());
	pd_standard_deviations->reserve(pcOtherPattern->pd_standard_deviations->size());
	pi_bits->reserve(pcOtherPattern->pi_bits->size());

	for (uint16_t i = 0; i < pcOtherPattern->iGetSize(); i++)
	{
		pv_pattern->push_back(pcOtherPattern->pv_pattern->at(i));
		pd_differences->push_back(pcOtherPattern->pd_differences->at(i));
		pd_means->push_back(pcOtherPattern->pd_means->at(i));
		pd_standard_deviations->push_back(pcOtherPattern->pd_standard_deviations->at(i));
		pi_bits->push_back(pcOtherPattern->pi_bits->at(i));
	}//for (uint16_t i = 0; i < pcOtherPattern->iGetSize(); i++)

	pv_nested_patterns->reserve(pcOtherPattern->pv_nested_patterns->size());

	for (size_t i = 0; i < pcOtherPattern->pv_nested_patterns->size(); i++)
	{
		pv_nested_patterns->push_back(new CGenePattern(pcOtherPattern->pv_nested_patterns->at(i), this));
	}//for (size_t i = 0; i < pcOtherPattern->pv_nested_patterns->size(); i++)

	pc_parent_pattern = pcParentPattern;
}//CGenePattern::CGenePattern(CGenePattern *pcOtherPattern, CGenePattern *pcParentPattern)

CGenePattern::~CGenePattern()
{
	pv_pattern->clear();
	delete pv_pattern;

	pd_differences->clear();
	delete pd_differences;

	pd_means->clear();
	delete pd_means;

	pd_standard_deviations->clear();
	delete pd_standard_deviations;

	pi_bits->clear();
	delete pi_bits;

	vClearNestedPatterns();
	delete pv_nested_patterns;
}//CGenePattern::~CGenePattern()

void CGenePattern::vAdd(uint16_t iIndex, double dDifference, double dMean, double dStandardDeviation, int32_t iBit)
{
	pv_pattern->push_back(iIndex);
	pd_differences->push_back(dDifference);
	pd_means->push_back(dMean);
	pd_standard_deviations->push_back(dStandardDeviation);
	pi_bits->push_back(iBit);
}//void CGenePattern::vAdd(uint16_t iIndex, double dDifference, double dMean, double dStandardDeviation, int32_t iBit)

void CGenePattern::vAdd(CGenePattern *pcGenePattern)
{
	pv_pattern->reserve(pv_pattern->size() + (size_t)pcGenePattern->iGetSize());
	pd_differences->reserve(pd_differences->size() + (size_t)pcGenePattern->iGetSize());
	pd_means->reserve(pd_means->size() + (size_t)pcGenePattern->iGetSize());
	pd_standard_deviations->reserve(pd_standard_deviations->size() + (size_t)pcGenePattern->iGetSize());
	pi_bits->reserve(pi_bits->size() + (size_t)pcGenePattern->iGetSize());

	for (uint16_t i = 0; i < pcGenePattern->iGetSize(); i++)
	{
		vAdd(*(pcGenePattern->piGetPattern() + i), *(pcGenePattern->pdGetDifferences() + i), *(pcGenePattern->pdGetMeans() + i), 
			*(pcGenePattern->pdGetStandardDeviations() + i), *(pcGenePattern->piGetBits() + i));
	}//for (uint16_t i = 0; i < pcGenePattern->iGetSize(); i++)
}//void CGenePattern::vAdd(CGenePattern *pcGenePattern)

bool CGenePattern::bRemove(CGenePattern *pcOrderedGenePattern)
{
	vector<uint16_t> v_indexes_to_erase;
	v_indexes_to_erase.reserve((size_t)pcOrderedGenePattern->iGetSize());

	uint16_t i_index = 0;
	uint16_t i_other_index = 0;

	while (i_index < iGetSize() && i_other_index < pcOrderedGenePattern->iGetSize())
	{
		if (*(piGetPattern() + i_index) == *(pcOrderedGenePattern->piGetPattern() + i_other_index))
		{
			v_indexes_to_erase.push_back(i_index);

			i_index++;
			i_other_index++;
		}//if (*(piGetPattern() + i_index) == *(pcOrderedGenePattern->piGetPattern() + i_other_index))
		else if (*(piGetPattern() + i_index) < *(pcOrderedGenePattern->piGetPattern() + i_other_index))
		{
			i_index++;
		}//else if (*(piGetPattern() + i_index) < *(pcOrderedGenePattern->piGetPattern() + i_other_index))
		else
		{
			i_other_index++;
		}//else
	}//while (i_index < iGetSize() && i_other_index < pcOrderedGenePattern->iGetSize() && b_contains)

	for (size_t i = 0; i < v_indexes_to_erase.size(); i++)
	{
		pv_pattern->erase(pv_pattern->begin() + v_indexes_to_erase.at(i) - i);
		pd_differences->erase(pd_differences->begin() + v_indexes_to_erase.at(i) - i);
		pd_means->erase(pd_means->begin() + v_indexes_to_erase.at(i) - i);
		pd_standard_deviations->erase(pd_standard_deviations->begin() + v_indexes_to_erase.at(i) - i);
		pi_bits->erase(pi_bits->begin() + v_indexes_to_erase.at(i) - i);
	}//for (size_t i = 0; i < v_indexes_to_erase.size(); i++)

	return !v_indexes_to_erase.empty();
}//bool CGenePattern::bRemove(CGenePattern *pcOrderedGenePattern)

bool CGenePattern::bRemove(uint16_t iIndex)
{
	bool b_removed = false;

	vector<uint16_t>::iterator i_it = find(pv_pattern->begin(), pv_pattern->end(), iIndex);

	if (i_it != pv_pattern->end())
	{
		pv_pattern->erase(i_it);
		b_removed = true;
	}//if (i_it != pv_pattern->end())

	return b_removed;
}//bool CGenePattern::bRemove(uint16_t iIndex)

void CGenePattern::vClear()
{
	pv_pattern->clear();
}//void CGenePattern::vClear()

bool CGenePattern::bContains(CGenePattern *pcOrderedGenePattern)
{
	bool b_contains = false;

	if (pcOrderedGenePattern->iGetSize() <= iGetSize() && !pcOrderedGenePattern->bIsEmpty())
	{
		uint16_t i_index = 0;
		uint16_t i_other_index = 0;

		b_contains = true;

		while (i_index < iGetSize() && i_other_index < pcOrderedGenePattern->iGetSize() && b_contains)
		{
			if (*(piGetPattern() + i_index) == *(pcOrderedGenePattern->piGetPattern() + i_other_index))
			{
				i_index++;
				i_other_index++;
			}//if (*(piGetPattern() + i_index) == *(pcOrderedGenePattern->piGetPattern() + i_other_index))
			else if (*(piGetPattern() + i_index) < *(pcOrderedGenePattern->piGetPattern() + i_other_index))
			{
				i_index++;
			}//else if (*(piGetPattern() + i_index) < *(pcOrderedGenePattern->piGetPattern() + i_other_index))
			else
			{
				b_contains = false;
			}//else
		}//while (i_index < iGetSize() && i_other_index < pcOrderedGenePattern->iGetSize() && b_contains)

		if (b_contains)
		{
			b_contains = i_other_index == pcOrderedGenePattern->iGetSize();
		}//if (b_contains)
	}//if (pcOrderedGenePattern->iGetSize() <= iGetSize() && !pcOrderedGenePattern->bIsEmpty())

	return b_contains;
}//bool CGenePattern::bContains(CGenePattern *pcOrderedGenePattern)

void CGenePattern::vSort()
{
	sort(pv_pattern->begin(), pv_pattern->end());
}//void CGenePattern::vSort()

void CGenePattern::vShuffle()
{
	random_shuffle(pv_pattern->begin(), pv_pattern->end());
}//void CGenePattern::vShuffle()

void CGenePattern::vAddNestedPattern(CGenePattern *pcNestedPattern)
{
	if (pcNestedPattern->pc_parent_pattern != this)
	{
		if (pcNestedPattern->pc_parent_pattern)
		{
			vector<CGenePattern*>::iterator it_begin = pcNestedPattern->pc_parent_pattern->pv_nested_patterns->begin();
			vector<CGenePattern*>::iterator it_end = pcNestedPattern->pc_parent_pattern->pv_nested_patterns->end();

			vector<CGenePattern*>::iterator it_value = find(it_begin, it_end, pcNestedPattern);

			if (it_value != it_end)
			{
				pcNestedPattern->pc_parent_pattern->pv_nested_patterns->erase(it_value);
			}//if (it_value != it_end)
		}//if (pcNestedPattern->pc_parent_pattern)

		pcNestedPattern->pc_parent_pattern = this;
		pv_nested_patterns->push_back(pcNestedPattern);
	}//if (pcNestedPattern->pc_parent_pattern != this)
}//void CGenePattern::vAddNestedPattern(CGenePattern *pcNestedPattern)

bool CGenePattern::bRemoveNestedPattern(CGenePattern *pcNestedPattern)
{
	return VectorUtils::bRemove(pv_nested_patterns, pcNestedPattern);
}//bool CGenePattern::bRemoveNestedPattern(CGenePattern *pcNestedPattern)

bool CGenePattern::bReplaceNestedPattern(CGenePattern *pcOldNestedPattern, CGenePattern *pcNewNestedPattern)
{
	bool b_replaced = false;

	for (size_t i = 0; i < pv_nested_patterns->size() && !b_replaced; i++)
	{
		if (pv_nested_patterns->at(i) == pcOldNestedPattern)
		{
			pv_nested_patterns->at(i) = pcNewNestedPattern;
			pcNewNestedPattern->pc_parent_pattern = this;
			b_replaced = true;
		}//if (pv_nested_patterns->at(i) == pcOldNestedPattern)
	}//for (size_t i = 0; i < pv_nested_patterns->size() && !b_replaced; i++)

	return b_replaced;
}//bool CGenePattern::bReplaceNestedPattern(CGenePattern *pcOldNestedPattern, CGenePattern *pcNewNestedPattern)

void CGenePattern::vClearNestedPatterns()
{
	for (uint32_t i = 0; i < (uint32_t)pv_nested_patterns->size(); i++)
	{
		delete pv_nested_patterns->at(i);
	}//for (uint32_t i = 0; i < (uint32_t)pv_nested_patterns->size(); i++)

	pv_nested_patterns->clear();
}//void CGenePattern::vClearNestedPatterns()

void CGenePattern::vPopNestedPattern()
{
	delete pv_nested_patterns->at(pv_nested_patterns->size() - 1);
	pv_nested_patterns->pop_back();
}//void CGenePattern::vPopNestedPattern()

#include <iostream>

void CGenePattern::vSortNestedPatterns()
{
	sort(pv_nested_patterns->begin(), pv_nested_patterns->end(), [](CGenePattern *pcGenePattern0, CGenePattern *pcGenePattern1)
	{
		return pcGenePattern0->iGetSize() < pcGenePattern1->iGetSize();
	});//sort(pv_nested_patterns->begin(), pv_nested_patterns->end(), [](CGenePattern *pcGenePattern0, CGenePattern *pcGenePattern1)
}//void CGenePattern::vSortNestedPatterns()

void CGenePattern::vGetLeaves(vector<CGenePattern*> *pvLeaves)
{
	CGenePattern *pc_nested_pattern;

	for (uint32_t i = 0; i < iGetNestedPatternsSize(); i++)
	{
		pc_nested_pattern = *(ppcGetNestedPatterns() + i);

		if (pc_nested_pattern->iGetSize() == 1)
		{
			pvLeaves->push_back(pc_nested_pattern);
		}//if (pc_nested_pattern->iGetSize() == 1)
		else
		{
			pc_nested_pattern->vGetLeaves(pvLeaves);
		}//else if (pc_nested_pattern->iGetSize() == 1)
	}//for (uint32_t i = 0; i < iGetNestedPatternsSize(); i++)
}//void CGenePattern::vGetLeaves(vector<CGenePattern*> *pvLeaves)

CString CGenePattern::sToString()
{
	CString s_pattern;

	for (uint16_t i = 0; i < (uint16_t)pv_pattern->size(); i++)
	{
		s_pattern.AppendFormat("%d ", pv_pattern->at(i));
	}//for (uint16_t i = 0; i < (uint16_t)pv_pattern->size(); i++)

	return s_pattern;
}//CString CGenePattern::sToString()