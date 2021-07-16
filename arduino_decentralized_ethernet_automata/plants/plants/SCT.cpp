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
  Serial.println(1<<state_from->num_state);
 

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




DES::DES():
  m_plants(NULL)
{ 
  m_num_plants = 0;
 
 
}

void DES::add_plant(Automaton* plant)
{
  m_plants = (Automaton**) realloc(m_plants, (m_num_plants + 1)
                                                       * sizeof(Automaton*));
  m_plants[m_num_plants] = plant;                          
  m_num_plants++; 
}

void DES::trigger(int event)
{
  for(int i = 0; i < m_num_plants; i++){
    m_plants[i]->trigger(event);
  }
  
}
