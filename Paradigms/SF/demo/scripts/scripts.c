/**
 ** SCRIPTS.C ---- a few signal processing scripts
 **/

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include <mgk60.h>
#include <simgui.h>

#include "System.h"
#include "fft.h"

/* some convenient define-s */
#ifndef	M_PI
#define M_PI	  3.1415926
#endif
#ifndef NULL
#define NULL	  0
#endif
#ifndef TRUE 
#define TRUE	  1
#endif
#ifndef	FALSE
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

#if 0
/* 
 * One input one output script template working on 
 * single percision floating point buffer data.
 */
void template_script(void)
{
  mgk_data_type type;
  float *data;
  data = mgk_receive(0,&type);
  if(data && (type == (T_FLOAT | T_BUFFER))) {
    int size = mgk_buffer_size(data) / sizeof(float);
    /* Do something with the data */
    /* ... */
    /* Propagate the data. The input buffer can be reused as the Kernel */
    /* protects data from aliasing */
    mgk_propagate(0,data,(T_FLOAT | T_BUFFER));
  }
}
#endif

/* 
 * Data generator script.
 * Generates a mized sine wave and noise signal.
 * Gets generation parameters from GUI controls.
 * Gets data parameters (channel, sampling rate and buffer size) from context.
 */
void datagen_script(void)
{
  mgk_data_type type;
  datagen_script_context *cxt = mgk_node_context(mgk_current_node(),&type);
  printf("gen\n");
  fflush(stdout);
  if(cxt && (type & T_BUFFER)) {
    double samp;
    double sfreq;
    double namp;
    float *data;
    char cname[100];
    sprintf(cname,CT_SINAMP,cxt->Channel);
    samp = gui_get_control_value(cname);
    sprintf(cname,CT_SINFREQ,cxt->Channel);
    sfreq = gui_get_control_value(cname);
    sprintf(cname,CT_NOISEAMP,cxt->Channel);
    namp = gui_get_control_value(cname);
    data = mgk_allocate_buffer((sizeof(float) * cxt->Size),0);
    if(data) {
      int i;
      for(i = 0; i < cxt->Size; i++) {
        data[i] = (float)(
          samp * cos(2.0 * M_PI * sfreq * i / cxt->Frequency) +
          namp * (double)(rand() - (RAND_MAX / 2)) * (2.0 / RAND_MAX));
      }
      mgk_propagate(0,data,(T_FLOAT | T_BUFFER));
    }
  }
}

static
#if defined(__GNUC__) || defined(__cplusplus)
inline
#endif
int
ilog2(int value)
{
  int ilog;
  for(ilog = 0; (1 << ilog) != value; ilog++) {
    if((1 << ilog) == 0) {
      return(-1);
    }
  }
  return(ilog);
}

/* 
 * Time domain complex FFT script.
 * No context used.
 */
void fft_script(void)
{
  mgk_data_type rtype,itype;
  int logsize;
  float *re = mgk_receive(0,&rtype);
  float *im = mgk_receive(1,&itype);
  printf("fft\n");
  fflush(stdout);
  if((re && im) &&
     (rtype == itype) &&
     (rtype == (T_BUFFER | T_FLOAT)) &&
     (mgk_buffer_size(re) == mgk_buffer_size(im)) &&
     ((logsize = ilog2(mgk_buffer_size(re) / sizeof(float))) >= 3)) {
    fft32(re,im,logsize,0);
    mgk_propagate(0,re,rtype);
    mgk_propagate(1,im,itype);
  }
}

/* 
 * Splits a complex FFT output into separate channels.
 * Assumes that the FFT time domain inputs were two separate real data
 * channels.
 * No context used.
 */
void splitter_script(void)
{
  mgk_data_type rtype,itype;
  int size;
  float *re = mgk_receive(0,&rtype);
  float *im = mgk_receive(1,&itype);
  printf("split\n");
  fflush(stdout);
  if((re && im) &&
     (rtype == itype) &&
     (rtype == (T_BUFFER | T_FLOAT)) &&
     (mgk_buffer_size(re) == mgk_buffer_size(im)) &&
     ((size = (mgk_buffer_size(re) / sizeof(float))) >= 4) &&
     ((size & 1) == 0)) {
    int hsize = size / 2;
    float *re1 = mgk_allocate_buffer((sizeof(float) * hsize),0);
    float *im1 = mgk_allocate_buffer((sizeof(float) * hsize),0);
    float *re2 = mgk_allocate_buffer((sizeof(float) * hsize),0);
    float *im2 = mgk_allocate_buffer((sizeof(float) * hsize),0);
    if(re1 && im1 && re2 && im2) {
      int i;
      for(i = 0; i < hsize; i++) {
        int j = i ? (size - i) : 0;
        re1[i] = 0.5f * (re[i] + re[j]);
        re2[i] = 0.5f * (re[i] - re[j]);
        im1[i] = 0.5f * (im[i] - im[j]);
        im2[i] = 0.5f * (im[i] + im[j]);
      }
      mgk_propagate(0,re1,(T_FLOAT | T_BUFFER));
      mgk_propagate(1,im1,(T_FLOAT | T_BUFFER));
      mgk_propagate(2,re2,(T_FLOAT | T_BUFFER));
      mgk_propagate(3,im2,(T_FLOAT | T_BUFFER));
    }
  }
}

/*
 * Converts a complex input pair into absolute value and phase outputs.
 * No context used.
 */
void abs_phase_script(void)
{
  mgk_data_type rtype,itype;
  int size;
  float *re = mgk_receive(0,&rtype);
  float *im = mgk_receive(1,&itype);
  printf("abs+phase\n");
  fflush(stdout);
  if((re && im) &&
     (rtype == itype) &&
     (rtype == (T_BUFFER | T_FLOAT)) &&
     (mgk_buffer_size(re) == mgk_buffer_size(im)) &&
     ((size = (mgk_buffer_size(re) / sizeof(float))) > 0)) {
    int i;
    for(i = 0; i < size; i++) {
      float magn = (float)(sqrt((re[i] * re[i]) + (im[i] * im[i])));
      float phase = (float)(atan2(im[i],re[i]) * 180.0 / M_PI);
      re[i] = magn;
      im[i] = phase;
    }
    mgk_propagate(0,re,rtype);
    mgk_propagate(1,im,itype);
  }
}

/* 
 * Computes envelope (= peak hold) for an input signal. 
 * The context stores the previous maximums in a buffer allocated
 * by the script first time it is executed.
 * The other elements of the context are used to reset the envelopes:
 * There is a global reset count parameter and a local copy of it.
 * Anytime the two are different, the script resets the envelope.
 * The script also monitors the GUI envlope reset button and 
 * whenever depressed it increments the global reset count parameter.
 * This is to ensure that all envelope scripts notice the "reset" event.
 */
void envelope_script(void)
{
  mgk_data_type type;
  envelope_script_context *cxt = mgk_node_context(mgk_current_node(),&type);
  printf("env\n");
  fflush(stdout);
  if(cxt && (type & T_BUFFER)) {
    int i,size,do_reset = FALSE;
    float *data = mgk_receive(0,&type);
    if((data) &&
       (type == (T_FLOAT | T_BUFFER)) &&
       ((size = mgk_buffer_size(data) / sizeof(float)) > 0)) {
      if(!cxt->Buffer) {
	cxt->Buffer = mgk_allocate_buffer((sizeof(float) * size),0);
	if(!cxt->Buffer) {
	  return;
        }
	mgk_protect_buffer(cxt->Buffer);
	do_reset = TRUE;
      }
      if(gui_get_button_value(ENV_RESET)) {
	(*(cxt->Reset))++;
      }
      if(*(cxt->Reset) != cxt->LastReset) {
	do_reset = TRUE;
	cxt->LastReset = *(cxt->Reset);
      }
      if(do_reset) {
        for(i = 0; i < size; i++) {
          ((float *)(cxt->Buffer))[i] = data[i];
        }
      }
      else {
        for(i = 0; i < size; i++) {
          data[i] = ((float *)(cxt->Buffer))[i] = 
	    max(data[i],((float *)(cxt->Buffer))[i]);
        }
      }
      mgk_propagate(0,data,(T_BUFFER | T_FLOAT));
    }
  }
}
  
/*
 * Low-pass filter script.
 * Uses exponential averaging: 
 * out = history = in * Factor + history * (1 - Factor)
 * The filter factor comes from the context.
 * Uses a storage buffer and reset mechanism similar to the 
 * envelope script.
 */
void lpfilter_script(void)
{
  mgk_data_type type;
  lpfilter_script_context *cxt = mgk_node_context(mgk_current_node(),&type);
  printf("lpfilter\n");
  fflush(stdout);
  if(cxt && (type & T_BUFFER)) {
    int i,size,do_reset = FALSE;
    float *data = mgk_receive(0,&type);
    if((data) &&
       (type == (T_FLOAT | T_BUFFER)) &&
       ((size = mgk_buffer_size(data) / sizeof(float)) > 0)) {
      if(!cxt->Buffer) {
	cxt->Buffer = mgk_allocate_buffer((sizeof(float) * size),0);
	if(!cxt->Buffer) {
	  return;
        }
	mgk_protect_buffer(cxt->Buffer);
	do_reset = TRUE;
      }
      if(gui_get_button_value(FILT_RESET)) {
	(*cxt->Reset)++;
      }
      if(*cxt->Reset != cxt->LastReset) {
	do_reset = TRUE;
	cxt->LastReset = *cxt->Reset;
      }
      if(do_reset) {
        for(i = 0; i < size; i++) {
          ((float *)(cxt->Buffer))[i] = data[i];
        }
      }
      else {
        for(i = 0; i < size; i++) {
          data[i] = ((float *)(cxt->Buffer))[i] = (float)(
	    data[i] * cxt->Factor +
	    ((float *)(cxt->Buffer))[i] * (1.0 - cxt->Factor));
        }
      }
      mgk_propagate(0,data,(T_BUFFER | T_FLOAT));
    }
  }
}

/* 
 * Data plotter script.
 * It just enters its input data into the data base of the GUI toolkit.
 * The actual plot updades will be done under the control of the GUI event
 * loop.
 * The script requires a preset character string context which contains the
 * (space separated) names for the GUI variable and data set.
 */
void plotter_script(void)
{
  char varname[100];
  mgk_data_type type;
  plotter_script_context *cxt = mgk_node_context(mgk_current_node(),&type);
  printf("plot\n");
  fflush(stdout);
  if(cxt && (type & T_BUFFER)) {
    int size;
    float *data = mgk_receive(0,&type);
    if((data) &&
       (type == (T_FLOAT | T_BUFFER)) &&
       ((size = mgk_buffer_size(data) / sizeof(float)) > 0)) {
      sprintf(varname,"Chan%02d",cxt->Channel);
      gui_clear_data(varname,cxt->DBase);
      gui_append_data(varname,cxt->DBase,data,size);
    }
  }
}

/* 
 * Complex data generator script.
 * Propagates its input data unchanged on its first output.
 * Generates an identically sized zero-filled data buffer for the second
 * output.
 * Can be used to generate the complex input for the FFT script from
 * single channel real data.
 * Uses no context.
 */
void real2complex_script(void)
{
  mgk_data_type type;
  float *data;
  printf("real2complex\n");
  fflush(stdout);
  data = mgk_receive(0,&type);
  if(data && (type == (T_FLOAT | T_BUFFER))) {
    int i,size = mgk_buffer_size(data) / sizeof(float);
    float *zero = mgk_allocate_buffer((sizeof(float) * size),0);
    if(zero) {
      for(i = 0; i < size; i++) {
        zero[i] = 0.0f;
      }
      mgk_propagate(0,data,(T_FLOAT | T_BUFFER));
      mgk_propagate(1,zero,(T_FLOAT | T_BUFFER));
    }
  }
}

/*
 * GUI setup script.
 * Should run before the GUI toolkit is started.
 * It creates the necessary controls for the data generators and the
 * envelope/filter reset mechanism.
 * The script takes the number of channels and sampling frequency 
 * parameters (these are necessary to create the controls) from its
 * context. 
 * The script has no input or output data.
 * After running once, the script deactivates the node.
 * The node using this script should be created with the highest
 * priority so it runs first.
 */
void guisetup_script(void)
{
  mgk_data_type type;
  guisetup_script_context *cxt = mgk_node_context(mgk_current_node(),&type);
  if(cxt && (type & T_BUFFER)) {
    int i;
    char name[100];
    gui_define_button(ENV_RESET);
    gui_define_button(FILT_RESET);
    for(i = 1; i <= cxt->NumChannels; i++) {
      sprintf(name, CT_SINAMP, i);
      gui_define_control(name, 0.1, 10.0, 1.0);
      sprintf(name, CT_SINFREQ, i);
      gui_define_control(name, 0.0, cxt->SampRate * 0.49, cxt->SampRate * 0.05 * i);
      sprintf(name, CT_NOISEAMP, i);
      gui_define_control(name, 0.1, 10.0, 1.0);
    }
  }
  mgk_set_node_priority(mgk_current_node(),0);
}

/*
 * GUI data base setup script.
 * Should be run once for every data base (data set) used in 
 * the processing.
 * Initializes the data base, defines the default X axis variable for
 * X-Y plots and initializes these variables.
 * Takes the number of samples, sampling frequency and time/frequency 
 * domain flag parameters form the context.
 * No input/output data propagation.
 * Deactivates itself after first run 
 */
void dbsetup_script(void)
{
  mgk_data_type type;
  dbsetup_script_context *cxt = mgk_node_context(mgk_current_node(),&type);
  if(cxt && (type & T_BUFFER)) {
    int i;
    int size = cxt->IsFrequency ? 
      cxt->Size / 2 : 
      cxt->Size;
    char *xname = cxt->IsFrequency ? 
      "Frequency" : 
      "Time";
    double step = cxt->IsFrequency ? 
      cxt->SampRate / cxt->Size : 
      1.0 / cxt->SampRate;
    float *data = mgk_allocate_buffer((sizeof(float) * size),FALSE);
    for(i = 0; i < size; i++) {
      data[i] = (float)i * (float)step;
    }
    gui_setup_database(cxt->Name,xname);
    gui_clear_database(cxt->Name);
    gui_append_data(xname, cxt->Name, data, size);
  }
  mgk_set_node_priority(mgk_current_node(),0);
}
