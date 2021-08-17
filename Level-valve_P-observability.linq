<Query Kind="Statements">
  <Reference>&lt;RuntimeDirectory&gt;\System.ValueTuple.dll</Reference>
  <NuGetReference>UltraDES</NuGetReference>
  <Namespace>UltraDES</Namespace>
</Query>

var s = Enumerable.Range(0, 20).Select(ii => new State($"s{ii}", ii == 0 ? Marking.Marked : Marking.Unmarked)).ToArray();
var ms = Enumerable.Range(0, 10).Select(ii => new State($"m{ii}", Marking.Marked)).ToArray();

//exploit3 original


Event level_H1 = new Event("level_H1", Controllability.Uncontrollable);
Event level_L1 = new Event("level_L1", Controllability.Uncontrollable);
Event open_vin = new Event("open_vin", Controllability.Controllable);
Event close_vin = new Event("close_vin", Controllability.Controllable);
Event open_vout = new Event("open_vout", Controllability.Controllable);
Event close_vout = new Event("close_vout", Controllability.Controllable);
Event turn_on_mixer = new Event("turn_on_mixer", Controllability.Controllable);
Event turn_on_pump = new Event("turn_on_pump", Controllability.Controllable);
Event turn_off_pump = new Event("turn_off_pump", Controllability.Controllable);
Event turn_off_mixer = new Event("turn_off_mixer", Controllability.Controllable); 
Event turn_on_tcontrol = new Event("turn_on_tcontrol", Controllability.Controllable); 
Event turn_off_tcontrol = new Event("turn_off_tcontrol", Controllability.Controllable); 
Event full = new Event("full", Controllability.Uncontrollable);
Event heated = new Event("heated", Controllability.Uncontrollable);
Event cooled = new Event("cooled", Controllability.Uncontrollable);
Event empty = new Event("empty", Controllability.Uncontrollable);
Event start = new Event("start", Controllability.Controllable);
Event finish = new Event("finish", Controllability.Uncontrollable);




var VIN = new DeterministicFiniteAutomaton(new[]
{
	new Transition(s[0], open_vin, s[1]),
	new Transition(s[1], close_vin, s[0]),
	new Transition(s[1], level_H1, s[1]),
	
  }, s[0], "VIN");

var VOUT = new DeterministicFiniteAutomaton(new[]
{
	new Transition(s[0], open_vout, s[1]),
	new Transition(s[1], close_vout, s[0]),
	new Transition(s[1], level_L1, s[1]),

	}, s[0], "VOUT");



var PROCESS = new DeterministicFiniteAutomaton(new[]
{
	new Transition(s[0], start, s[1]),
	new Transition(s[1], finish, s[0]),
	

  }, s[0], "PROCESS");

var MIXER = new DeterministicFiniteAutomaton(new[]
{
	new Transition(s[0], turn_on_mixer, s[1]),
	new Transition(s[1], turn_off_mixer, s[0]),

	}, s[0], "MIXER");

var PUMP = new DeterministicFiniteAutomaton(new[]
{
	new Transition(s[0], turn_on_pump, s[1]),
	new Transition(s[1], turn_off_pump, s[0]),

	}, s[0], "PUMP");

var TEMP = new DeterministicFiniteAutomaton(new[]
{
	new Transition(s[0], turn_on_tcontrol, s[1]),
	new Transition(s[1], heated, s[1]),
	new Transition(s[1], cooled, s[1]),
	new Transition(s[1], turn_off_tcontrol, s[0]),

	}, s[0], "TEMP");

var E1 = new DeterministicFiniteAutomaton(new[]
{
	new Transition(s[0], start, s[1]),
	new Transition(s[1], close_vout, ms[2]),
	new Transition(ms[2], start, s[1]),
	new Transition(s[1], level_H1, s[1]),
	new Transition(s[1], level_L1, s[1]),
	new Transition(s[1], heated, s[1]),
	new Transition(s[1], cooled, s[1])
	

  }, s[0], "E1");

var E2 = new DeterministicFiniteAutomaton(new[]
{
	new Transition(s[0], level_H1, s[1]),
	new Transition(s[1], close_vin, s[0]),
	//new Transition(s[0], level_L1, s[0]),
	//new Transition(s[1], level_L1, s[1]),
	new Transition(s[1], level_H1, s[1])

  }, s[0], "E2");

var E3 = new DeterministicFiniteAutomaton(new[]
{
	new Transition(s[0], level_L1, s[1]),
	new Transition(s[1], close_vout, s[0]),
	new Transition(s[1], level_L1, s[1])

  }, s[0], "E3");

var E4 = new DeterministicFiniteAutomaton(new[]
{
	new Transition(s[0], start, s[1]),
	new Transition(s[1], open_vin, s[0]),
	new Transition(s[1], finish, s[1]),
	new Transition(s[0], finish, s[0]),
	
	

  }, s[0], "E4");

var E5 = new DeterministicFiniteAutomaton(new[]
{
	new Transition(s[0], cooled, s[1]),
	new Transition(s[1], open_vout, s[0]),
	new Transition(s[1], level_L1, s[1]),
	new Transition(s[0], heated, s[0]),
	new Transition(s[1], heated, s[1]),
	new Transition(s[1], cooled, s[1]),
	new Transition(s[0], level_L1, s[0]),

  }, s[0], "E5");

var E6 = new DeterministicFiniteAutomaton(new[]
{
	
	new Transition(s[0], level_H1, s[1]),
	new Transition(s[1], turn_on_mixer, s[0]),
	new Transition(s[1], level_H1, s[1]),
	
	

  }, s[0], "E6");

var E7 = new DeterministicFiniteAutomaton(new[]
{
	new Transition(s[0], heated, s[1]),
	new Transition(s[1], turn_on_pump, s[0]),
	new Transition(s[1], heated, s[1]),
	new Transition(s[1], cooled, s[1]),
	new Transition(s[0], cooled, s[0]),
	
  }, s[0], "E7");

var E8 = new DeterministicFiniteAutomaton(new[]
{
	new Transition(s[0], level_H1, s[1]),
	new Transition(s[1], turn_on_tcontrol, s[2]),
	new Transition(s[2], heated, s[3]),
	new Transition(s[3], cooled, s[4]),
	new Transition(s[4], turn_off_tcontrol, s[0]),
	new Transition(s[2], cooled, s[2]),
	new Transition(s[3], heated, s[3]),
	new Transition(s[4], heated, s[4]),
	new Transition(s[4], cooled, s[4]),
  }, s[0], "E8");

var E9 = new DeterministicFiniteAutomaton(new[]
{
	new Transition(s[0], cooled, s[1]),
	new Transition(s[1], turn_off_mixer, s[0]),
	new Transition(s[0], heated, s[0]),
	new Transition(s[1], heated, s[1]),
	new Transition(s[1], cooled, s[1]),
  }, s[0], "E9");

var E10 = new DeterministicFiniteAutomaton(new[]
{
	new Transition(s[0], cooled, s[1]),
	new Transition(s[1], turn_off_pump, s[0]),
	new Transition(s[0], heated, s[0]),
	new Transition(s[1], heated, s[1]),
	new Transition(s[1], cooled, s[1]),
  }, s[0], "E10");


//var ES = DeterministicFiniteAutomaton.ParallelComposition(new[] {E6, E9 });


//K1.ShowAutomaton();
//G1.ShowAutomaton();

//var teste = K1.IsControllable(G1);
//pequenaFabrica(out var plants, out var specs);
E9.ShowAutomaton("E");

var S = DeterministicFiniteAutomaton.MonolithicReducedSupervisor(new[] { TEMP }, new[] {E8});
var Sups = DeterministicFiniteAutomaton.LocalModularSupervisor(new[] { VIN, VOUT, PROCESS, MIXER, TEMP, PUMP }, new[] {E2, E3, E4, E5, E6, E7, E8, E9, E10});
//var S = DeterministicFiniteAutomaton.MonolithicSupervisor(new[] { VIN, VOUT }, new[] {E1});
//var S = DeterministicFiniteAutomaton.MonolithicSupervisor(plants.ToArray(), specs.ToArray());
//Sups.ElementAt(0).ShowAutomaton("S1");
//Sups.ElementAt(1).ShowAutomaton("S2");
//Sups.ElementAt(2).ShowAutomaton("S3");

//ShowDisablement(S, G1, 10);


//var disS = S.DisabledEvents(new[] { G3 });

//var dis = S.DisabledEvents(new[] { G });
//Sups.ElementAt(0).ShowAutomaton("S");

//var G1 = DeterministicFiniteAutomaton.ParallelComposition(new[] { TEMP, PROCESS});
//var K1 = DeterministicFiniteAutomaton.ParallelComposition(new[] { E8 });
//var S = DeterministicFiniteAutomaton.MonolithicSupervisor(new[] { G1 }, new[] { K1 },false);
S.ShowAutomaton("S");
//List<Event> v2 = new[] { level_L1, level_H1, close_vin, open_vin, close_vout, open_vout, }.ToList();
List<Event> v2 = new[] { level_L1, level_H1, heated, cooled }.ToList();

//check_Pobservability3(G1, K1, v2);

Boolean check_Pobservability3(DeterministicFiniteAutomaton H, DeterministicFiniteAutomaton desired_language, List<Event> vulnerable_events)
{

	var attack_set = GetAllCombos(vulnerable_events);
	var sup = DeterministicFiniteAutomaton.MonolithicSupervisor(new[] { H }, new[] { desired_language }).SimplifyStatesName();
	//sup.ShowAutomaton("S");
	List<List<Event>> forbidden_events = new List<List<Event>>();
	List<List<Event>> forbidden_events2 = new List<List<Event>>();

	var ci = inconsistent_states(sup, H);
	var number_of_events = 1;

	while (number_of_events < vulnerable_events.Count())
	{
		foreach (var attacker in attack_set.Where(at => at.Count() == number_of_events).ToList())
		{
			foreach (var ev in attacker)
			{
				Console.Write(ev.ToString() + " ");
			}
			foreach (var fe in forbidden_events2)
			{
				if (attacker.Intersect(fe).Count() == fe.Count())
				{
					Console.Write("- Not P-observable*");
					forbidden_events2.Add(attacker);
					goto teste;
				}

			}


			foreach (var pair in ci)
			{
				var possible_states1 = neighbor_states2(sup, pair.Item1, 10, attacker);
				possible_states1.Add(pair.Item1);
				var possible_states2 = neighbor_states2(sup, pair.Item2, 10, attacker);
				possible_states2.Add(pair.Item2);

				if (possible_states1.Intersect(possible_states2).Count() > 0)
				{
					Console.Write("- Not P-observable");
					if (!forbidden_events.Contains(attacker))
					{
						forbidden_events.Add(attacker);
						forbidden_events2.Add(attacker);
					}
				}
				else
				{
					Console.Write("- OK");
				}
			}
		teste:;
			Console.WriteLine();

		}
		number_of_events++;
	}

	Console.WriteLine("Eventos proibidos: ");
	foreach (var set in forbidden_events)
	{
		foreach (var ev in set)
		{
			Console.Write(ev.ToString() + " ");
		}
		Console.WriteLine();
	}


	return true;

}


List<(AbstractState, AbstractState)> inconsistent_states(DeterministicFiniteAutomaton Sup, DeterministicFiniteAutomaton H)
{

	var disablements = Sup.DisabledEvents(new[] { H });
	List<(AbstractState, AbstractState)> CI = new List<(AbstractState, AbstractState)>();

	Console.WriteLine("Inconsistent states:");

	foreach (var st in Sup.States)
	{
		foreach (var st2 in Sup.States)
		{
			if (st != st2)
			{
				if (feasible_event(Sup, st).Intersect(disablements[st2]).Count() > 0 || feasible_event(Sup, st2).Intersect(disablements[st]).Count() > 0)
				{
					//if (!CI.Contains((st2, st)))
					//{
					CI.Add((st, st2));
					//}
					//Console.WriteLine("\n States " + st.ToString() + " and " + st2.ToString() + " are not control consistent");

				}
			}

		}

	}


	foreach (var pair in CI)
	{
		Console.WriteLine(pair.Item1.ToString() + ", " + pair.Item2.ToString());
	}

	return CI;
}

List<List<T>> GetAllCombos<T>(List<T> list)
{
	List<List<T>> result = new List<List<T>>();
	// head
	result.Add(new List<T>());
	result.Last().Add(list[0]);
	if (list.Count == 1)
		return result;
	// tail
	List<List<T>> tailCombos = GetAllCombos(list.Skip(1).ToList());
	tailCombos.ForEach(combo =>
	{
		result.Add(new List<T>(combo));
		combo.Add(list[0]);
		result.Add(new List<T>(combo));
	});
	return result;
}

List<AbstractState> neighbor_states2(DeterministicFiniteAutomaton H, AbstractState state, int n, List<Event> vulnerable_events)
{

	var radius = 1;
	List<Event> manipulated_events = new List<Event>();

	List<AbstractState> visited_states = new List<AbstractState>();
	List<AbstractState> frontier_after = new List<AbstractState>();
	List<AbstractState> frontier_after_aux = new List<AbstractState>();
	List<AbstractState> frontier_before = new List<AbstractState>();
	List<AbstractState> frontier_before_aux = new List<AbstractState>();


	frontier_after.Add(state);
	frontier_before.Add(state);

	while (radius <= n)
	{
		foreach (var s2 in frontier_after)
		{

			foreach (var t in H.Transitions.Where(jj => (jj.Origin == s2)))
			{
				var ev = t.Trigger;
				if (vulnerable_events.Contains(ev))
				{
					visited_states.Add(t.Destination);

					var ns = t.Destination;
					var nt = H.Transitions.Where(jj => (jj.Origin == ns) && (jj.Trigger == ev));
					while (nt.Count() == 1 && !(visited_states.Contains(nt.ElementAt(0).Destination)))
					{
						ns = nt.ElementAt(0).Destination;
						nt = H.Transitions.Where(jj => (jj.Origin == ns) && (jj.Trigger == ev));
						visited_states.Add(ns);
					}
					frontier_after_aux.Add(ns);
				}

			}
		}

		foreach (var s2 in frontier_before)
		{
			foreach (var t in H.Transitions.Where(jj => (jj.Destination == s2)))
			{
				var ev = t.Trigger;
				if (vulnerable_events.Contains(t.Trigger) && !(visited_states.Contains(t.Origin)))
				{
					visited_states.Add(t.Origin);

					var ns = t.Origin;
					var nt = H.Transitions.Where(jj => (jj.Destination == ns) && (jj.Trigger == ev));
					while (nt.Count() == 1 && !(visited_states.Contains(nt.ElementAt(0).Origin)))
					{
						ns = nt.ElementAt(0).Origin;
						nt = H.Transitions.Where(jj => (jj.Destination == ns) && (jj.Trigger == ev));
						visited_states.Add(ns);
					}
					frontier_before_aux.Add(ns);
				}
			}
		}
		frontier_after.Clear();
		frontier_before.Clear();
		frontier_after.AddRange(frontier_after_aux);
		frontier_before.AddRange(frontier_before_aux);
		frontier_after_aux.Clear();
		frontier_before_aux.Clear();


		radius++;
	}
	return visited_states;
}

List<AbstractEvent> feasible_event(DeterministicFiniteAutomaton H, AbstractState state)
{

	List<AbstractEvent> events = new List<AbstractEvent>();
	foreach (var t in H.Transitions.Where(jj => (jj.Origin == state)))
	{
		events.Add(t.Trigger);
	}
	return events;
}