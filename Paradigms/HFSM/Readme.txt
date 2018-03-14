********************************************************************************
Executable Hierarchical Finite State Machines


Institute of Software Integrated Systems, Vanderbilt University
********************************************************************************


This Paradigm has been developed to highlight the ease of use of GME in modeling 
real-life problems.

The Paradigm implements a simple Finite State Machine (FSM) with Hierarchical 
capabilities. Each State is decomposable into its substates. If decomposable, 
the state will have exactly one 'start' substate, else none. The semantic 
meaning of the hierarchy is:

First Priority to the deepest substate.
Next Priority to the Parent & so on till the root.

The input is specified in two models:
One, the State Machine that has to be run.
Two, a sequence of events that occur after any arbitrary time delay. This 
sequence must and can be linear in nature - no multiple incoming or outgoing 
arrows from/to an event. Each event may occur multiple times, and may have 
different delays.

An interpreter has been written to execute this HFSM & highlight the current 
state(s) the HFSM is in. On the occurence of an event, the HFSM will try to 
transit to the next state through an outgoing transition. If this is not 
possible, it will try it's parent state's out-going transitions, and so on, 
till it succeeds in transiting out. If no outgoing transitions are found, the 
HFSM will continue to remain in the same state.
Null Transitions are not allowed. Hence, its important to label every transition
with the event name. If Null, then the transition is treated as being absent,
and is ignored.