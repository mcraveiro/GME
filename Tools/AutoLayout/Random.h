#ifndef _RANDOM_H_
#define _RANDOM_H_

class Random  
{
public:
    static void   setSeed         ( int seed );

    static void   setRandomSeed   ();

    static double nextDouble      ();

    static int    nextInt         ( int max );

    static double nextGaussian    ();

private:
    static bool   haveNextNextGaussian;
    static double nextNextGaussian;
};

#endif // _RANDOM_H_
