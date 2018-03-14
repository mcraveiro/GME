#ifndef _GAOPTIMIZER_H_
#define _GAOPTIMIZER_H_

#include <vector>
#include <functional>

namespace GAOptimizer
{

class IGenotype
{
public:
    virtual void derive ( IGenotype * parent1, IGenotype * parent2 ) = 0;

    virtual void random () = 0;

    double m_fitness;
};

typedef std::vector<IGenotype*> GenotypeVec;

class IProblem
{
public:
    virtual IGenotype * createRandomSolution( int i ) = 0;
    
    virtual double      evaluteSolution( IGenotype * solution ) = 0;
};

class TestProblem: public IProblem
{
public:
    virtual IGenotype * createRandomSolution();
    
    virtual double      evaluteSolution( IGenotype * solution );
};

class TestGenoType: public IGenotype
{
public:
                 TestGenoType();

    virtual void derive ( IGenotype * parent1, IGenotype * parent2 );

    virtual void random ();

    double m_x;
};


class Optimizer  
{
public:
	            Optimizer     ();

	virtual    ~Optimizer     ();

    void        init          ( IProblem * problem, int populationSize, int subPopulationSize );

    void        step          ();

    void        step          ( int n );

    IGenotype * getBest       ();

    GenotypeVec& getPopulation ();

    double      getMaxFitness ();

private:
    void     clear();

private:
    IProblem *     m_problem;
    int            m_populationSize;
    int            m_subPopulationSize;
    GenotypeVec    m_population;
    GenotypeVec    m_subPopulation;
    IGenotype *    m_bestSolution;
    double         m_maxFitness;
    int            m_bestIndNum;
    int            m_firstBadInd;
};

}

#endif // _GAOPTIMIZER_H_
