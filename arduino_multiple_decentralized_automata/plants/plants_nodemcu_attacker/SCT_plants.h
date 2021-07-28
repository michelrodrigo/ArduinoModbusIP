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

//#include "Arduino_AVRSTL.h"
//#include "map"

//Modes of operation for decision making
#define RANDOM        1
#define PRIORITY      2
#define LIST          3
#define PLANT         4

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
  
  int m_num_transitions;              //stores the number of transitions of the automaton
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
  DES();
	void add_plant(Automaton* plant);
  void trigger(int event);

private:
	
	
	Automaton** m_plants;                //vector of pointers to the plant automata
	
	int m_num_plants;
	
 
 

};


#endif
