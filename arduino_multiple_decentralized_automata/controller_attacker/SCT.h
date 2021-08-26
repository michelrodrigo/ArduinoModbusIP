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

//Modes of operation for decision making
#define RANDOM        1
#define PRIORITY      2
#define LIST          3

/* State 
 * Each state has:
 * - onEnter function: it will be executed when the state is reached
 * - onExit function: it will be execute when the state is leaved
 * - char num_state: a number that uniquely identifies a state within an automaton
 */
struct State
{
  State(void (*onEnter)(), void (*onExit)(), char num_state);

  void (*onEnter)();
  void (*onExit)();
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

  void addTransition(State* state_from, State* state_to, int event,
                      void (*onTransition)());
  void trigger(int event);          //triggers an event           
  bool isDefined(int event);       //returns true if the given event is defined at the current state;
                                    //returns false otherwise.
  bool isFeasible(int event);      //returns true if the event can be triggered, considering the
                                    //automata in the system that may share the given event. A shared
                                    //event can only be triggered if it can be triggered in all automata
                                    //in which the event is defined; 
                                    //returns false otherwise.
  int currentState();              //returns the state number of the current state

private:
  struct Transition
  {
    State* state_from;
    State* state_to;
    int event;
    void (*onTransition)();

    State* makeTransition();
  };

  static Transition createTransition(State* state_from, State* state_to,
                                      int event, void (*onTransition)());

 
  
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
	bool isDisabled(int event);        //returns true if given event is disabled at the currente 
                                      //state; return false othewise
	

private:
	std::map<int,bool> disablements;    //stores the disablement information. It is a map from event(int)
                                      //to the disablement information: true if disabled, false otherwise
};

/* DES 
 * A system is composed by plant automata and supervisor automata.
 * 
 * Plant automata must have disjoint alphabets. Use the addPlant and addSupervisor 
 * methods to add automata to the system. To trigger an event, use the triggerIfPossible method. 
 * It will take into account the information from the supervisors: an event cannot be triggered if it
 * is disabled by at least one of the supervisors. The controllable events will be triggered according to 
 * the mode set with setMode method. Controllable events should be represented by odd numbers, while 
 * uncontrollable events should be represented by even numbers.
 */
class DES
{
	
public:
  DES(int* controllable_events, int num_c_events, int* uncontrollable_events, int num_u_events);
	void addPlant(Automaton* plant);      //Adds a plant to the DES 
	void addSupervisor(Supervisor* sup);  //Adds a supervisor to the DES
	bool triggerIfPossible(int event); //triggers given event if it is possible
                                       //considering all other automata and supervisors
                                       //returns true if event was triggered, false otherwise
  void triggerSupervisors(int event); //triggers the events in the supervisors
  void setMode(int mode, int* list, int list_size);   //this functions sets the operation mode:
                                       //1 - random: triggers enabled events randomly, list is null
                                       //2 - priority: triggers enabled events according to priority list
                                       //3 - sequence: triggers events in sequence, according to list
  
  void updateDES();                    //verifies if there is a new event that can be triggered
  
  int* enabled_events;                  //a vector that stores the information about a controllable 
                                        //event being enabled


private:
	void enabledEvents();                 //updates the enabled_events vector
	
	Automaton** m_plants;                //vector of pointers to the plant automata
	Supervisor** m_supervisors;          //vector of pointers to the supervisor automata
  int* m_controllable_events;          //vector of controllable events
  int* m_uncontrollable_events;        //vector of uncontrollable events
  int m_num_c_events;                  //stores the number of controllable events
  int m_num_u_events;                  //stores the number of uncontrollable events
	int m_num_plants;                    //stores the number of plant automata
	int m_num_sups;                      //stores the number of supervisor automata
  int m_mode;                          //mode of operation. This will defined the rule of chosing controllable
                                       //events to be triggered, specially when there are more than one at
                                       //a given moment. Possible values:
                                       //     RANDOM: choose randomly among the enabled events
                                       //     PRIORITY: TODO (events should have priorities over others)
                                       //     LIST: the events will be triggered according to its position in a list
  int* m_action_list;                  //when mode is set to LIST, this vector stores the list of events
  int m_list_size;                     //when mode is set to LIST, stores the number of events
  int m_next_event;                    //stores the next event to be triggered
 
 

};


#endif
