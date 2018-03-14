extern void lpfilter_script(void);

typedef struct {
   void *Buffer;
   int  LastReset;
   double  Factor;
   int *Reset;
} lpfilter_script_context;

extern int FiltReset__System;

extern lpfilter_script_context LPfilter_actor_0_context;
