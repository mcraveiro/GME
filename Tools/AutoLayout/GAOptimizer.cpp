#include "stdafx.h"
#include <cfloat>
#include <cmath>
#include <algorithm>

#include "GAOptimizer.h"
#include "Random.h"

namespace GAOptimizer
{

bool genotypeCompare( IGenotype* elem1, IGenotype* elem2 )
{
   return (elem1->m_fitness > elem2->m_fitness);
}

TestGenoType::TestGenoType()
{
    random();
}

void TestGenoType::derive( IGenotype * parent1, IGenotype * parent2 )
{
    m_x = ((TestGenoType*)parent1)->m_x + 0.05 * Random::nextGaussian();
}

void TestGenoType::random()
{
    m_x = Random::nextDouble();
}

IGenotype * TestProblem::createRandomSolution()
{
    return new TestGenoType();
}

double TestProblem::evaluteSolution( IGenotype * solution )
{
    double x = ((TestGenoType*)solution)->m_x;
    return sin(10*x)+0.4*sin(5*(x+4));
}

Optimizer::Optimizer()
{
    m_problem      = NULL;
    m_bestSolution = NULL;
}

Optimizer::~Optimizer()
{
    clear();
}

void Optimizer::clear()
{
    for( unsigned int i=0; i<m_population.size(); ++i )
        delete m_population[i];
    m_population.clear();
    m_subPopulation.clear();
    m_problem      = NULL;
    m_bestSolution = NULL;
}


void Optimizer::init( IProblem * problem, int populationSize, int subPopulationSize )
{
    clear();

    m_problem           = problem;
    m_populationSize    = populationSize;
    m_subPopulationSize = subPopulationSize;

    // initialize population with random solutions, find the best individal
    m_maxFitness = -DBL_MAX;
    m_population.resize( m_populationSize );
    for( int i=0; i<m_populationSize; ++i )
    {
        m_population[i] = m_problem->createRandomSolution(i);
        m_population[i]->m_fitness = m_problem->evaluteSolution(m_population[i]);
        if( m_population[i]->m_fitness > m_maxFitness )
        {
            m_maxFitness   = m_population[i]->m_fitness;
            m_bestSolution = m_population[i]; 
        }
    }

    // create subpopulation
    m_subPopulation.resize( m_subPopulationSize );

    m_bestIndNum  = (int)floor(0.2 * m_subPopulationSize + 0.5);
    m_firstBadInd = m_subPopulationSize - m_bestIndNum;
}

void Optimizer::step()
{
    int i;
        
    // select random subpopulation
    for( i=0; i<m_subPopulationSize; ++i )
        m_subPopulation[i] = m_population[Random::nextInt(m_populationSize)];

    // sort subpopulation            
    std::sort( m_subPopulation.begin(), m_subPopulation.end(), genotypeCompare );

    // generate new individuals
    for( i=m_firstBadInd; i<m_subPopulationSize; ++i )
    {
        IGenotype * parent1 = m_subPopulation[Random::nextInt(m_bestIndNum)];
        IGenotype * parent2 = m_subPopulation[Random::nextInt(m_bestIndNum)];
        IGenotype * child   = m_subPopulation[i];
        
        if( child != m_bestSolution && child != parent1 && child != parent2 && parent1 != parent2 )
        {           
            child->derive( parent1, parent2 );
            m_subPopulation[i]->m_fitness = m_problem->evaluteSolution( m_subPopulation[i] );

            if( m_subPopulation[i]->m_fitness > m_maxFitness )
            {
                m_maxFitness   = m_subPopulation[i]->m_fitness;
                m_bestSolution = m_subPopulation[i]; 
            } 
        }
    }
}

void Optimizer::step( int n )
{
    for( int i=0; i<n; ++i )
        step();
}

IGenotype * Optimizer::getBest()
{
    return m_bestSolution;
}

GenotypeVec& Optimizer::getPopulation()
{
    return m_population;
}

double Optimizer::getMaxFitness()
{
    return m_maxFitness;
}

}