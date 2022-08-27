#include "Transformation.h"

#include "RealCoding.h"

template <class TGenotype, class TFenotype>
CTransformation<TGenotype, TFenotype>::CTransformation()
{

}//CTransformation<TGenotype, TFenotype>::CTransformation()

template <class TGenotype, class TFenotype>
CTransformation<TGenotype, TFenotype>::~CTransformation()
{

}//CTransformation<TGenotype, TFenotype>::~CTransformation()

template <class TGenotype, class TFenotype>
TFenotype * CTransformation<TGenotype, TFenotype>::pcTransform(TGenotype *pcGenotype)
{
	TFenotype *pc_fenotype = pc_create_fenotype(pcGenotype);
	vTransform(pcGenotype, pc_fenotype);

	return pc_fenotype;
}//TFenotype * CTransformation<TGenotype, TFenotype>::pcTransform(TGenotype *pcGenotype)

template <class TGenotype, class TFenotype>
TGenotype * CTransformation<TGenotype, TFenotype>::pcInverselyTransform(TFenotype *pcFenotype)
{
	TGenotype *pc_genotype = pc_create_genotype(pcFenotype);
	vInverselyTransform(pcFenotype, pc_genotype);

	return pc_genotype;
}//TGenotype * CTransformation<TGenotype, TFenotype>::pcInverselyTransform(TFenotype *pcFenotype)


template class CTransformation<CRealCoding, CRealCoding>;