#ifndef GENE_PATTERN_H
#define GENE_PATTERN_H

#include <atlstr.h>
#include <cstdint>
#include <vector>

using namespace std;

class CGenePattern
{
public:
	CGenePattern();
	CGenePattern(uint16_t iSignificantIndex);
	CGenePattern(CGenePattern *pcOtherPattern);
	CGenePattern(CGenePattern *pcOtherPattern, CGenePattern *pcParentPattern);

	~CGenePattern();

	void vAdd(uint16_t iIndex) { vAdd(iIndex, (double)0); };
	void vAdd(uint16_t iIndex, double dDifference) { vAdd(iIndex, dDifference, 0, 0, 0); };
	void vAdd(uint16_t iIndex, double dMean, double dStandardDeviation) { vAdd(iIndex, 0, dMean, dStandardDeviation, 0); };
	void vAdd(uint16_t iIndex, int32_t iBit) { vAdd(iIndex, 0, 0, 0, iBit); };
	void vAdd(uint16_t iIndex, double dDifference, double dMean, double dStandardDeviation, int32_t iBit);
	void vAdd(CGenePattern *pcGenePattern);

	bool bRemove(CGenePattern *pcOrderedGenePattern);
	bool bRemove(uint16_t iIndex);

	void vClear();

	bool bContains(CGenePattern *pcOrderedGenePattern);

	void vSort();
	void vShuffle();

	void vAddNestedPattern(CGenePattern *pcNestedPattern);
	bool bRemoveNestedPattern(CGenePattern *pcNestedPattern);
	bool bReplaceNestedPattern(CGenePattern *pcOldNestedPattern, CGenePattern *pcNewNestedPattern);
	void vClearNestedPatterns();
	void vPopNestedPattern();
	void vSortNestedPatterns();

	bool bIsEmpty() { return iGetSize() == 0; };

	uint16_t iGetSignificantIndex() { return i_significant_index; };

	uint16_t iGetSize() { return (uint16_t)pv_pattern->size(); };
	uint16_t *piGetPattern() { return pv_pattern->data(); };
	double *pdGetDifferences() { return pd_differences->data(); };
	double *pdGetMeans() { return pd_means->data(); };
	double *pdGetStandardDeviations() { return pd_standard_deviations->data(); };
	int32_t *piGetBits() { return pi_bits->data(); };

	CGenePattern *pcGetParentPattern() { return pc_parent_pattern; }

	uint32_t iGetNestedPatternsSize() { return (uint32_t)pv_nested_patterns->size(); };
	CGenePattern **ppcGetNestedPatterns() { return pv_nested_patterns->data(); };

	void vGetLeaves(vector<CGenePattern*> *pvLeaves);

	CString sToString();

private:
	uint16_t i_significant_index;

	vector<uint16_t> *pv_pattern;
	vector<double> *pd_differences;
	vector<double> *pd_means;
	vector<double> *pd_standard_deviations;
	vector<int32_t> *pi_bits;
	
	CGenePattern *pc_parent_pattern;

	vector<CGenePattern*> *pv_nested_patterns;
};//class CGenePattern

#endif//GENE_PATTERN_H