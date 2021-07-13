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
State::State(void (*on_enter)(), void (*on_exit)(), char num_state): 
  on_enter(on_enter),
  on_exit(on_exit),
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
void Automaton::add_transition(State* state_from, State* state_to, int event,
                         void (*on_transition)())
{
  if (state_from == NULL || state_to == NULL)
    return;

  Transition transition = Automaton::create_transition(state_from, state_to, event,
                                               on_transition);
  m_transitions = (Transition*) realloc(m_transitions, (m_num_transitions + 1)
                                                       * sizeof(Transition));
  m_transitions[m_num_transitions] = transition;
  m_num_transitions++;
  
  Serial.print("Fs: ");
  Serial.print(event);
  Serial.print(1<<state_from->num_state);
  m_feasibility[event] |= (1<<state_from->num_state);
  Serial.println(String(" ")+ m_feasibility[event]);

}



Automaton::Transition Automaton::create_transition(State* state_from, State* state_to,
                                       int event, void (*on_transition)())
{
  Transition t;
  t.state_from = state_from;
  t.state_to = state_to;
  t.event = event;
  t.on_transition = on_transition;

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
      m_current_state = m_transitions[i].make_transition();
      return;
    }
  }
}

bool Automaton::is_defined(int event){
  if (m_feasibility[event] > 0){
    return true;
  }
  else{
    return false;
  } 
}

bool Automaton::is_feasible(int event){
  return m_feasibility[event] & 1<<m_current_state->num_state;
}

int Automaton::current_state(){
  return m_current_state->num_state;
}



State* Automaton::Transition::make_transition()
{
  // Execute the handlers in the correct order.
  if (state_from->on_exit != NULL)
    state_from->on_exit();

  if (on_transition != NULL)
    on_transition();

  if (state_to->on_enter != NULL)
    state_to->on_enter();

  return state_to;
}


Supervisor::Supervisor(State* initial_state):
  Automaton(initial_state)
{
}

//the event is disabled when its corresponding bit is 1
void Supervisor::disable(int event)
{
  disablements[event] = true;
}

void Supervisor::enable(int event)
{
  disablements[event] = false;
}

bool Supervisor::is_disabled(int event)
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

void DES::add_plant(Automaton* plant)
{
  m_plants = (Automaton**) realloc(m_plants, (m_num_plants + 1)
                                                       * sizeof(Automaton*));
  m_plants[m_num_plants] = plant;                          
  m_num_plants++;                         
}

void DES::add_supervisor(Supervisor* sup)
{
  m_supervisors = (Supervisor**) realloc(m_supervisors, (m_num_sups + 1)
                                                       * sizeof(Supervisor*));
  m_supervisors[m_num_sups] = sup;                           
  m_num_sups++;                         
}

/*An event can be triggered if it is feasible in all plants that share the event and 
 * if it is not disabled by any supervisor. 
 */
bool DES::trigger_if_possible(int event)
{

  for (int i = 0; i < m_num_plants; i++)
  {
    Serial.print("Is defined: ");
    Serial.print(m_plants[i]->is_defined(event)+ String(" "));
    Serial.println();
    if (m_plants[i]->is_defined(event)){
      Serial.print(" Is feasible: ");
      Serial.print(m_plants[i]->is_feasible(event)+ String(" "));
      Serial.println();
      if(!m_plants[i]->is_feasible(event)){
        Serial.println("  Event not possible.");
        return false;
      }
    }
  }
  
  for (int i = 0; i < m_num_sups; i++)
  {
    Serial.print("Disabled: ");
    Serial.println(m_supervisors[i]->is_disabled(event));
    if (m_supervisors[i]->is_disabled(event)){
      Serial.println("Event disabled.");
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
  
  this->enabledEvents();
  this->updateDES();
  Serial.print("Enabled events: ");
   for(int i = 0; i < m_num_c_events; i++){
      Serial.print(enabled_events[i] + String(" "));
   }
   Serial.println();
  return true;
}

void DES::trigger_supervisors(int event)
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
        if(m_plants[j]->is_defined(m_controllable_events[i])){            
            not_defined = false;
            if(m_plants[j]->is_feasible(m_controllable_events[i])){  
              for(int k = 0; k < m_num_sups; k++){
                if(m_supervisors[k]->is_disabled(m_controllable_events[i])){
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
  m_action_list = (int*) malloc (list_size * sizeof(int));
  m_action_list = list;
  m_list_size = list_size;

   Serial.print("Action list: ");
       for(int i = 0; i < m_list_size; i++){
          Serial.print(m_action_list[i] + String(" "));
       }
       Serial.println();

  switch(m_mode){

    case(RANDOM):
      break;

    case(PRIORITY):
      break;
    
    case(LIST):
      m_next_event = 0;
      break;
  }
}

void DES::updateDES(){

  int aux = 0;
  int events_to_choose_from[m_list_size];

  this->enabledEvents();

  Serial.print("Enabled events from update: ");
 for(int i = 0; i < m_num_c_events; i++){
    Serial.print(enabled_events[i] + String(" "));
 }
 Serial.println();
  switch(m_mode){

    case(RANDOM):
      for(int i = 0; i < m_num_c_events; i++){
        if(enabled_events[i] == 1){
          events_to_choose_from[aux] = m_controllable_events[i];
          Serial.println(String("Enabled event: ") + i);
          aux++;
        }
      }

      Serial.print("Events to choose from: ");
       for(int i = 0; i < aux; i++){
          Serial.print(events_to_choose_from[i] + String(" "));
       }
       Serial.println();
      if(aux > 0){
        m_next_event = random(0, aux);
        Serial.println(String("Random event: ") + m_next_event);
        this->trigger_if_possible(events_to_choose_from[m_next_event]);       
      }     
      
      break;

    case(PRIORITY):
      //TODO
      break;
    
    case(LIST)://sequence of events
      

       Serial.print("Action list: ");
       for(int i = 0; i < m_list_size; i++){
          Serial.print(m_action_list[i] + String(" "));
       }
       Serial.println();
       Serial.println(String("next event") + m_next_event);
       Serial.println(String("Event: ")+ m_action_list[m_next_event] + String(" is ") +enabled_events[m_next_event]);
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

  this->enabledEvents();
  
}
