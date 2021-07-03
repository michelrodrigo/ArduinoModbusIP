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

#ifndef SCT_H
#define SCT_H


#if defined(ARDUINO) && ARDUINO >= 100
  #include <Arduino.h>
#else
  #include <WProgram.h>
#endif

#include "Arduino_AVRSTL.h"
#include "map"

/* State 
 * Each state has:
 * - on_enter function: it will be executed when the state is reached
 * - on_exit function: it will be execute when the state is leaved
 * - char num_state: a number that uniquely identifies a state within an automaton
 */
struct State
{
  State(void (*on_enter)(), void (*on_exit)(), char num_state);

  void (*on_enter)();
  void (*on_exit)();
  char num_state;
};


/* Automaton
 * The main class. It implements the structure of the automaton. 
 * 
 * All the plant models are implemented by objects of this class.
 */
class Automaton
{
public:
  Automaton(State* initial_state);
  ~Automaton();

  void add_transition(State* state_from, State* state_to, int event,
                      void (*on_transition)());
  void trigger(int event);          //triggers an event           
  bool is_defined(int event);       //returns true if the given event is defined at the current state;
                                    //returns false otherwise.
  bool is_feasible(int event);      //returns true if the event can be triggered, considering the
                                    //automata in the system that may share the given event. A shared
                                    //event can only be triggered if it can be triggered in all automata
                                    //in which the event is defined; 
                                    //returns false otherwise.
  int current_state();              //returns the state number of the current state

private:
  struct Transition
  {
    State* state_from;
    State* state_to;
    int event;
    void (*on_transition)();

    State* make_transition();
  };

  static Transition create_transition(State* state_from, State* state_to,
                                      int event, void (*on_transition)());

 
  
  State* m_current_state;             //stores the current state of the automaton
  Transition* m_transitions;          //vector of transitions
  std::map<int,int>  m_feasibility;   //map events with their feasibility information:
                                      //a bit at position N is set to 1 if it is feasible
                                      //at the state whose num_state is N. The bit is set to
                                      //0 otherwise
  int m_num_transitions;              //stores the number of transitions of the automaton
};

/* Supervisor
 * In addition to an Automaton, the Supervisor can enable or disable events.
 * The disable and enable functions should be called in the function that are
 * executed when the states are entered.
 */
class Supervisor : public Automaton{

public:
	Supervisor(State* initial_state);
	void disable(int event);            //disables given event 
	void enable(int event);             //enables given event
	bool is_disabled(int event);        //returns true if given event is disabled at the currente 
                                      //state; return false othewise
	

private:
	std::map<int,bool> disablements;                   //stores the disablement information.
};

/* DES 
 * A system is composed by plant automata and supervisor automata.
 * 
 * When multiple automata are being executed at the same time, it is
 * important to verify if shared events are being exceuted at the same time
 * as well.
 */
class DES
{
	
public:
  DES(int* controllable_events, int num_c_events, int* uncontrollable_events, int num_u_events);
	void add_plant(Automaton* plant);
	void add_supervisor(Supervisor* sup);
	bool trigger_if_possible(int event); //triggers given event if it is possible
                                       //considering all other automata and supervisors
                                       //returns true if event was triggered, false otherwise
  void trigger_supervisors(int event); //triggers the events in the supervisors
  void setMode(int mode, int* list, int list_size);   //this functions sets the operation mode:
                                       //1 - random: triggers enabled events randomly, list is null
                                       //2 - priority: triggers enabled events according to priority list
                                       //3 - sequence: triggers events in sequence, according to list
  
  void updateDES();
  int* enabled_events;


private:
	void enabledEvents();
	
	Automaton** m_plants;                //vector of pointers to the plant automata
	Supervisor** m_supervisors;          //vector of pointers to the supervisor automata
  int* m_controllable_events;
  int* m_uncontrollable_events;
  int m_num_c_events;
  int m_num_u_events;
	int m_num_plants;
	int m_num_sups;
  int m_mode;
  int* m_action_list;
  int m_list_size;
  int m_next_event;
 
 

};


#endif
