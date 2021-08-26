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

#include "SCT.h"

// Struct constructor
State::State(void (*onEnter)(), void (*onExit)(), char num_state): 
  onEnter(onEnter),
  onExit(onExit),
  num_state(num_state)
{
}

//Automaton constructor
Automaton::Automaton(State* initial_state)
: m_current_state(initial_state),
  m_transitions(NULL),
  m_num_transitions(0)
{
}

//Automaton destructor
Automaton::~Automaton()
{
  free(m_transitions);
  m_transitions = NULL;
 
}

//Create a new transition in the automaton
void Automaton::addTransition(State* state_from, State* state_to, int event,
                         void (*onTransition)())
{
  if (state_from == NULL || state_to == NULL)
    return;

  Transition transition = Automaton::createTransition(state_from, state_to, event,
                                               onTransition);
  m_transitions = (Transition*) realloc(m_transitions, (m_num_transitions + 1)
                                                       * sizeof(Transition));
  m_transitions[m_num_transitions] = transition;
  m_num_transitions++;
  
  m_feasibility[event] |= (1<<state_from->num_state);


}



Automaton::Transition Automaton::createTransition(State* state_from, State* state_to,
                                       int event, void (*onTransition)())
{
  Transition t;
  t.state_from = state_from;
  t.state_to = state_to;
  t.event = event;
  t.onTransition = onTransition;

  return t;
}

void Automaton::trigger(int event)
{
  // Find the transition with the current state and given event.
  for (int i = 0; i < m_num_transitions; ++i)
  {
    if (m_transitions[i].state_from == m_current_state &&
        m_transitions[i].event == event)
    {
      m_current_state = m_transitions[i].makeTransition();
      return;
    }
  }
}

//returns true if the given event is defined at any state of the automaton, 
//        false, otherwise
bool Automaton::isDefined(int event){
  if (m_feasibility[event] > 0){
    return true;
  }
  else{
    return false;
  } 
}

//returns true if the given event is feasible at the current state of the automaton
//        false, otherwise
bool Automaton::isFeasible(int event){
  return m_feasibility[event] & 1<<m_current_state->num_state;
}

//returns the number corresponding to the automaton's current state
int Automaton::currentState(){
  return m_current_state->num_state;
}



State* Automaton::Transition::makeTransition()
{
  // Execute the handlers in the correct order.
  if (state_from->onExit != NULL)
    state_from->onExit();

  if (onTransition != NULL)
    onTransition();

  if (state_to->onEnter != NULL)
    state_to->onEnter();

  return state_to;
}


Supervisor::Supervisor(State* initial_state):
  Automaton(initial_state)
{
}

//disables the given event
void Supervisor::disable(int event)
{
  disablements[event] = true;
}

//enables the given event
void Supervisor::enable(int event)
{
  disablements[event] = false;
}

//returns true if the given event is disabled
bool Supervisor::isDisabled(int event)
{
 return disablements[event];
}


DES::DES(int* controllable_events, int num_c_events, int* uncontrollable_events, int num_u_events):
  m_plants(NULL),
  m_supervisors(NULL)
{ 
  m_num_plants = 0;
  m_num_sups = 0;
  m_controllable_events = controllable_events;
  m_uncontrollable_events = uncontrollable_events;
  m_num_c_events = num_c_events;
  m_num_u_events = num_u_events;

  enabled_events = (int*) malloc (m_num_c_events * sizeof(int));
}

void DES::addPlant(Automaton* plant)
{
  m_plants = (Automaton**) realloc(m_plants, (m_num_plants + 1)
                                                       * sizeof(Automaton*));
  m_plants[m_num_plants] = plant;                          
  m_num_plants++;                         
}

void DES::addSupervisor(Supervisor* sup)
{
  m_supervisors = (Supervisor**) realloc(m_supervisors, (m_num_sups + 1)
                                                       * sizeof(Supervisor*));
  m_supervisors[m_num_sups] = sup;                           
  m_num_sups++;                         
}

/*An event can be triggered if it is not disabled by any supervisor. 
 */
bool DES::triggerIfPossible(int event)
{

  for (int i = 0; i < m_num_sups; i++)
  {
    if (m_supervisors[i]->isDisabled(event)){
      return false;
    }
  }
  
  for (int i = 0; i < m_num_plants; i++)
  {
    m_plants[i]->trigger(event);
  }

  for (int i = 0; i < m_num_sups; i++)
  {
    m_supervisors[i]->trigger(event);
  }
  
  this->updateDES();
  
  return true;
}

void DES::triggerSupervisors(int event)
{
  for (int i = 0; i < m_num_sups; i++)
  {
    m_supervisors[i]->trigger(event);
  }
}

void DES::enabledEvents(){

  bool not_defined = true;
  int i;
  for(i = 0; i < m_num_c_events; i++){
    enabled_events[i] = 1;
  }

  for(i = 0; i < m_num_c_events; i++){
    not_defined = true;
    for(int j = 0; j < m_num_plants; j++){
        if(m_plants[j]->isDefined(m_controllable_events[i])){            
            not_defined = false;
            if(m_plants[j]->isFeasible(m_controllable_events[i])){  
              for(int k = 0; k < m_num_sups; k++){
                if(m_supervisors[k]->isDisabled(m_controllable_events[i])){
                  enabled_events[i] = 0;
                  break;
                }
              }        
            }
            else{   
              enabled_events[i] = 0;
              break;
            }
          }      
    }
    if(not_defined){
      enabled_events[i] = 0;
    }
  }
}

void DES::setMode(int mode, int* list, int list_size){

  m_mode = mode;

  
  switch(m_mode){

    case(RANDOM):
      break;

    case(PRIORITY):
      break;
    
    case(LIST):
      m_action_list = (int*) malloc (list_size * sizeof(int));
      m_action_list = list;
      m_list_size = list_size;
      m_next_event = 0;
      break;
  }
}

void DES::updateDES(){

  int aux = 0;
  int events_to_choose_from[m_list_size];

  this->enabledEvents();

  switch(m_mode){

    case(RANDOM):
      for(int i = 0; i < m_num_c_events; i++){
        if(enabled_events[i] == 1){
          events_to_choose_from[aux] = m_controllable_events[i];
          aux++;
        }
      }
      if(aux > 0){
        m_next_event = random(0, aux);
        this->triggerIfPossible(events_to_choose_from[m_next_event]);       
      }     
     
      break;

    case(PRIORITY):
      //TODO
      break;
    
    case(LIST)://sequence of events
      if(enabled_events[m_next_event] == 1){
        for (int i = 0; i < m_num_plants; i++) {
          m_plants[i]->trigger(m_action_list[m_next_event]);
        }
      
        for (int i = 0; i < m_num_sups; i++){
          m_supervisors[i]->trigger(m_action_list[m_next_event]);
        }
       
        m_next_event++;
        if(m_next_event >= m_list_size){
          m_next_event = 0;
        }
      }

      break;
  }

  
}
