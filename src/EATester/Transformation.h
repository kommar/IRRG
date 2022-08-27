#ifndef TRANSFORMATION_H
#define TRANSFORMATION_H

template <class TGenotype, class TFenotype>
class CTransformation
{
public:
	CTransformation();

	virtual ~CTransformation();

	virtual TFenotype *pcTransform(TGenotype *pcGenotype);
	virtual void vTransform(TGenotype *pcGenotype, TFenotype *pcFenotype) = 0;

	virtual TGenotype *pcInverselyTransform(TFenotype *pcFenotype);
	virtual void vInverselyTransform(TFenotype *pcFenotype, TGenotype *pcGenotype) = 0;

protected:
	virtual TFenotype *pc_create_fenotype(TGenotype *pcGenotype) = 0;
	virtual TGenotype *pc_create_genotype(TFenotype *pcFenotype) = 0;
};//class CTransformation

template <class TGenotypeFenotype>
class CUniformTransformation : public CTransformation<TGenotypeFenotype, TGenotypeFenotype>
{
public:
	virtual TGenotypeFenotype *pcTransform(TGenotypeFenotype *pcGenotype) { return pcGenotype; };
	virtual void vTransform(TGenotypeFenotype *pcGenotype, TGenotypeFenotype *pcFenotype) { pcFenotype = pcGenotype; };

	virtual TGenotypeFenotype *pcInverselyTransform(TGenotypeFenotype *pcFenotype) { return pcFenotype; };
	virtual void vInverselyTransform(TGenotypeFenotype *pcFenotype, TGenotypeFenotype *pcGenotype) { pcGenotype = pcFenotype; };

protected:
	virtual TGenotypeFenotype *pc_create_fenotype(TGenotypeFenotype *pcGenotype) { return nullptr; };
	virtual TGenotypeFenotype *pc_create_genotype(TGenotypeFenotype *pcGenotype) { return nullptr; };
};//class CUniformTransformation

#endif//TRANSFORMATION_H