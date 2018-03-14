/**
 ** FFT.H ---- fft routine prototype declaration
 **/

#ifndef __FFT_H_INCLUDED__
#define __FFT_H_INCLUDED__


int fft(double re[], double im[], int log2size, int inverse);
int fft32(float re[], float im[], int log2size, int inverse);

#endif /* whole file */
