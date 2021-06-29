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


State::State(void (*on_enter)(), void (*on_exit)(), char num_state): 
	on_enter(on_enter),
	on_exit(on_exit),
	num_state(num_state)
{
}


Automaton::Automaton(State* initial_state, int num_events)
: m_current_state(initial_state),
  m_transitions(NULL),
  m_num_transitions(0)
{
  m_num_events = num_events;
}


Automaton::~Automaton()
{
  free(m_transitions);
  free(m_timed_transitions);
  m_transitions = NULL;
  m_timed_transitions = NULL;
}


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


void Automaton::add_timed_transition(State* state_from, State* state_to,
                               unsigned long interval, void (*on_transition)())
{
  if (state_from == NULL || state_to == NULL)
    return;

  Transition transition = Automaton::create_transition(state_from, state_to, 0,
                                                 on_transition);

  TimedTransition timed_transition;
  timed_transition.transition = transition;
  timed_transition.start = 0;
  timed_transition.interval = interval;

  m_timed_transitions = (TimedTransition*) realloc(
      m_timed_transitions, (m_num_timed_transitions + 1) * sizeof(TimedTransition));
  m_timed_transitions[m_num_timed_transitions] = timed_transition;
  m_num_timed_transitions++;
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


void Automaton::check_timer()
{
  for (int i = 0; i < m_num_timed_transitions; ++i)
  {
    TimedTransition* transition = &m_timed_transitions[i];
    if (transition->transition.state_from == m_current_state)
    {
      if (transition->start == 0)
      {
        transition->start = millis();
      }
      else
      {
        unsigned long now = millis();
        if (now - transition->start >= transition->interval)
        {
          m_current_state = transition->transition.make_transition();
          transition->start = 0;
        }
      }
    }
  }
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


Supervisor::Supervisor(State* initial_state, int num_events):
	Automaton(initial_state, num_events)
{
	disablements = 0;
}

//the event is disabled when its corresponding bit is 1
void Supervisor::disable(int event)
{
	this->disablements |= (1<<event);
}

void Supervisor::enable(int event)
{
	this->disablements &= ~(1<<event);
}

bool Supervisor::is_disabled(int event)
{
	return this->disablements & (1<<event);
}

DES::DES():
	m_plants(NULL),
	m_supervisors(NULL)
{	
	m_num_plants = 0;
	m_num_sups = 0;
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

void DES::trigger_if_possible(int event)
{

  for (int i = 0; i < m_num_plants; ++i)
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
  			return;
  		}
  	}
  }
  
  for (int i = 0; i < m_num_sups; ++i)
  {
	  Serial.print("Disabled: ");
    Serial.println(m_supervisors[i]->is_disabled(event));
  	if (m_supervisors[i]->is_disabled(event)){
  		Serial.println("Event disabled.");
      return;
  	}
  }
  
  for (int i = 0; i < m_num_plants; ++i)
  {
	  m_plants[i]->trigger(event);
  }

  for (int i = 0; i < m_num_sups; ++i)
  {
    m_supervisors[i]->trigger(event);
  }
  
}

void DES::trigger_supervisors(int event)
{
  for (int i = 0; i < m_num_sups; ++i)
  {
    m_supervisors[i]->trigger(event);
  }
}
 
