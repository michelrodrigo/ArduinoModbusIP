/*---------------------------------------

 Supervisory control

This library implements the automata in the Supervisory Control framework

There is two types of automata:
- generic
- supervisor

The generic one is used to implement the model of the plant. The supervisor is used to implement the supervisory control. 

Author: Michel Alves - michelrodrigo@ufmg.br
Created in 22/06/2021
*/

#ifndef FSM_H
#define FSM_H


#if defined(ARDUINO) && ARDUINO >= 100
  #include <Arduino.h>
#else
  #include <WProgram.h>
#endif


struct State
{
  State(void (*on_enter)(), void (*on_exit)(), char num_state);

  void (*on_enter)();
  void (*on_exit)();
  char num_state;
};


class Automaton
{
public:
  Automaton(State* initial_state);
  ~Automaton();

  void add_transition(State* state_from, State* state_to, int event,
                      void (*on_transition)());

  void add_timed_transition(State* state_from, State* state_to,
                            unsigned long interval, void (*on_transition)());

  void trigger(int event);
  bool is_defined(int event);
  bool is_feasible(int event);
  void check_timer();

private:
  struct Transition
  {
    State* state_from;
    State* state_to;
    int event;
    void (*on_transition)();

    State* make_transition();
  };
  struct TimedTransition
  {
    Transition transition;
    unsigned long start;
    unsigned long interval;
  };

  static Transition create_transition(State* state_from, State* state_to,
                                      int event, void (*on_transition)());

private:
  State* m_current_state;
  Transition* m_transitions;
  int* m_feasibility;
  int m_num_transitions;

  TimedTransition* m_timed_transitions;
  int m_num_timed_transitions;
};

class Supervisor : public Automaton{

public:
	Supervisor(State* initial_state);
	void disable(int event);
	void enable(int event);
	bool is_disabled(int event);
	

private:
	int disablements;


};

class DES
{
	
public:
  DES();
	void add_plant(Automaton* plant);
	void add_supervisor(Supervisor* sup);
	void trigger_if_possible(int event);
  void trigger_supervisors(int event);


private:
	Automaton** m_plants;
	Supervisor** m_supervisors;
	int m_num_plants;
	int m_num_sups;

};


#endif
