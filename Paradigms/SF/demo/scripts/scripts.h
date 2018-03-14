/**
 ** SCRIPTS.H ---- defined-s for the few signal processing scripts
 **/
#ifndef  __SCRIPTS_H_INCLUDED__
#define  __SCRIPTS_H_INCLUDED__

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include <mgk60.h>
#include <simgui.h>

#include "System.h"
#include "fft.h"

/* some convenient define-s */
#ifndef M_PI
#define M_PI	  3.1415926
#endif
#ifndef NULL
#define NULL	  0
#endif
#ifndef TRUE
#define TRUE	  1
#endif
#ifndef FALSE
#define FALSE	  0
#endif

#ifndef min
#define min(x,y)  (((x) < (y)) ? (x) : (y))
#endif
#ifndef max
#define max(x,y)  (((x) > (y)) ? (x) : (y))
#endif
#ifndef sqr
#define sqr(x)	  ((x) * (x))
#endif

/* data generator control names */
#define CT_SINAMP	"Chan %d sine ampl"
#define CT_SINFREQ	"Chan %d sine freq"
#define CT_NOISEAMP	"Chan %d noise ampl"

/* reset button names for envelope and filter scripts */
#define ENV_RESET	"Reset envelopes"
#define FILT_RESET	"Reset filters"

#endif

