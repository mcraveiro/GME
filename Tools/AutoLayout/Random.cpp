#include "stdafx.h"
#include <cstdlib>
#include <ctime>
#include <cmath>
#include "Random.h"

bool   Random::haveNextNextGaussian = false;
double Random::nextNextGaussian     = 0;

void Random::setSeed( int seed )
{
    srand(seed);
}

void Random::setRandomSeed()
{
    srand( (unsigned)time( NULL ) );
}

double Random::nextDouble()
{
    return rand() / double(RAND_MAX+1);
}

int Random::nextInt( int max )
{
    return (int)floor(max * nextDouble());
}

double Random::nextGaussian()
{
    // See Knuth, ACP, Section 3.4.1 Algorithm C.
    if (haveNextNextGaussian) 
    {
    	haveNextNextGaussian = false;
    	return nextNextGaussian;
    } 
    else 
    {
        double v1, v2, s;
    	do 
        { 
            v1 = 2 * nextDouble() - 1; // between -1 and 1
            v2 = 2 * nextDouble() - 1; // between -1 and 1 
            s = v1 * v1 + v2 * v2;
    	} 
        while (s >= 1 || s == 0);
    	double multiplier = sqrt(-2 * log(s)/s);
    	nextNextGaussian = v2 * multiplier;
    	haveNextNextGaussian = true;
    	return v1 * multiplier;
    }
}