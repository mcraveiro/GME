
/**
 ** FFT.C ---- a complex in-place FFT routine
 **/

#ifndef	 FFT_DATA_TYPE
#include <stdlib.h>
#include <malloc.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

#include "fft.h"

#ifndef NULL
#define NULL  0
#endif
#ifndef TRUE
#define TRUE  1
#endif
#ifndef FALSE
#define FALSE 0
#endif
#ifndef PI
#ifdef	M_PI
#define PI    M_PI
#else
#define PI    3.1415926
#endif
#endif

static double *sintab = NULL;
static int *bitrevtab = NULL;
static int lastlog = (-1);
static int lastalloc = 0;

/*
 * set up a quarter period + 1 point sine table and the bitrev index table
 */
static int
setup_tables(int log2size)
{
  int offset, index1, index2;
  int buffln = 1 << log2size;
  int halfln = buffln >> 1;
  int quadln = halfln >> 1;

  if (lastalloc < buffln) {
    if (sintab)
      free(sintab);
    if (bitrevtab)
      free(bitrevtab);
    sintab = (double *)malloc(sizeof(double) * (quadln + 1));
    bitrevtab = (int *)malloc(sizeof(int) * buffln);
    if (!sintab || !bitrevtab) {
      lastalloc = 0;
      return (FALSE);
    }
    lastalloc = buffln;
  }
  for (index1 = 0; index1 <= quadln; index1++) {
    sintab[index1] = sin((double)index1 * 2.0 * PI / (double)buffln);
  }
  bitrevtab[0] = 0;
  offset = halfln;
  for (index1 = 1; index1 < buffln; index1 <<= 1) {
    for (index2 = 0; index2 < index1; index2++) {
      bitrevtab[index2 + index1] = bitrevtab[index2] + offset;
    }
    offset >>= 1;
  }
  lastlog = log2size;
  return (TRUE);
}

#define	 FFT_DATA_TYPE	float
#define	 fft		fft32
#include "fft.c"
#undef	 FFT_DATA_TYPE
#undef	 fft

#define	 FFT_DATA_TYPE	double
#endif

int
fft(FFT_DATA_TYPE re[], FFT_DATA_TYPE im[], int log2size, int inverse)
{
  int buffln, halfln, quadln;
  int blkcnt, blkidx, sinarg;
  int offset, index1, index2;

  if (!re || !im)
    return (FALSE);
  if (log2size <= 2)
    return (FALSE);
  buffln = 1 << log2size;
  halfln = buffln >> 1;
  quadln = halfln >> 1;
  if ((lastlog != log2size) && !setup_tables(log2size)) {
    return (FALSE);
  }
  /*
   * perform bit reversal
   */
  index1 = buffln;
  do {
    index1--;
    if (index1 < (index2 = bitrevtab[index1])) {
      FFT_DATA_TYPE reswap = re[index1];
      FFT_DATA_TYPE imswap = im[index1];
      re[index1] = re[index2];
      im[index1] = im[index2];
      re[index2] = reswap;
      im[index2] = imswap;
    }
  } while (index1 > 0);
  /*
   * now the actual fft (radix 2, decimated in time)
   */
  offset = 1;
  blkcnt = halfln;
  do {
    sinarg = halfln - blkcnt;
    index1 = offset + buffln;
    index2 = offset + index1;
    offset <<= 1;
    do {
      double wsin, wcos;
      if (sinarg <= quadln) {
	wsin = sintab[sinarg];
	wcos = sintab[quadln - sinarg];
      }
      else {
	wsin = sintab[halfln - sinarg];
	wcos = -sintab[sinarg - quadln];
      }
      if (!inverse) {
	wsin = -wsin;
      }
      blkidx = blkcnt;
      index1 -= (buffln + 1);
      index2 -= (buffln + 1);
      do {
	FFT_DATA_TYPE remult,immult;
	remult = (FFT_DATA_TYPE)(wcos * re[index2] - wsin * im[index2]);
        immult = (FFT_DATA_TYPE)(wcos * im[index2] + wsin * re[index2]);
	re[index2] = re[index1] - remult;
	im[index2] = im[index1] - immult;
	re[index1] += remult;
	im[index1] += immult;
	index1 += offset;
	index2 += offset;
      } while (--blkidx);
    } while ((sinarg -= blkcnt) >= 0);
    blkcnt >>= 1;
  } while (--log2size);
  return (TRUE);
}
