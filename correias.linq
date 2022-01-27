<Query Kind="Statements">
  <Reference>&lt;RuntimeDirectory&gt;\System.ValueTuple.dll</Reference>
  <NuGetReference>UltraDES</NuGetReference>
  <Namespace>UltraDES</Namespace>
</Query>

var s = Enumerable.Range(0, 20).Select(ii => new State($"s{ii}", ii == 0 ? Marking.Marked : Marking.Unmarked)).ToArray();
var ms = Enumerable.Range(0, 10).Select(ii => new State($"m{ii}", Marking.Marked)).ToArray();

//exploit3 original


Event a1 = new Event("a1", Controllability.Controllable);
Event a2 = new Event("a2", Controllability.Controllable);
Event a3 = new Event("a3", Controllability.Controllable);
//Event a4 = new Event("a4", Controllability.Controllable);
//Event a5 = new Event("a5", Controllability.Controllable);
//Event a6 = new Event("a6", Controllability.Controllable);
Event b1 = new Event("b1", Controllability.Uncontrollable);
Event b2 = new Event("b2", Controllability.Uncontrollable);
Event b3 = new Event("b3", Controllability.Uncontrollable);
//Event b4 = new Event("b4", Controllability.Uncontrollable);
//Event b5 = new Event("b5", Controllability.Uncontrollable);
//Event b6 = new Event("b6", Controllability.Uncontrollable);
Event blue = new Event("blue", Controllability.Uncontrollable);
Event red = new Event("red", Controllability.Uncontrollable);
//Event big = new Event("big", Controllability.Uncontrollable);
//Event small = new Event("small", Controllability.Uncontrollable);




var C1 = new DeterministicFiniteAutomaton(new[]
{
	new Transition(s[0], a1, s[1]),
	//new Transition(s[0], red, s[0]),
	//new Transition(s[0], blue, s[0]),
	new Transition(s[1], b1, s[0]),
	new Transition(s[1], red, s[1]),
	new Transition(s[1], blue, s[1]),

  }, s[0], "C1");

var C2 = new DeterministicFiniteAutomaton(new[]
{
	new Transition(s[0], a2, s[1]),
	new Transition(s[1], b2, s[0]),
	new Transition(s[0], a3, s[2]),
	new Transition(s[2], b3, s[0])

	}, s[0], "C2");



var E1 = new DeterministicFiniteAutomaton(new[]
{
	new Transition(s[0], b1, s[1]),
	new Transition(s[1], a2, s[0]),
	new Transition(s[1], a3, s[0])

  }, s[0], "E1");

var E2 = new DeterministicFiniteAutomaton(new[]
{
	new Transition(s[0], red, s[1]),
	new Transition(s[1], a2, s[0]),
	new Transition(s[1], red, s[1]),
	new Transition(s[0], blue, s[2]),
	new Transition(s[2], a3, s[0]),
	new Transition(s[2], blue, s[2]),


	}, s[0], "E2");

var E3 = new DeterministicFiniteAutomaton(new[]
{
	new Transition(s[0], blue, s[1]),
	new Transition(s[1], a3, s[0]),
	new Transition(s[1], blue, s[1]),
	
	
	}, s[0], "E3");

var E4 = new DeterministicFiniteAutomaton(new[]
{
	new Transition(s[0], red, s[1]),
	new Transition(s[1], a2, s[0]),
	new Transition(s[1], red, s[1]),


	}, s[0], "E4");









//var G1 = DeterministicFiniteAutomaton.ParallelComposition(new[] {E6, E7 });


//K1.ShowAutomaton();
//G1.ShowAutomaton();

//var teste = K1.IsControllable(G1);
//pequenaFabrica(out var plants, out var specs);
//E3.ShowAutomaton("E3");

var Monolitico = DeterministicFiniteAutomaton.MonolithicSupervisor(new[] { C1, C2 }, new[] { E1, E2});
//Monolitico.ShowAutomaton("Mono");
var S = DeterministicFiniteAutomaton.LocalModularReducedSupervisor(new[] {C1, C2 }, new[] {E1, E3});
//var S = DeterministicFiniteAutomaton.MonolithicReducedSupervisor(new[] { G1 }, new[] {E3});
//var S = DeterministicFiniteAutomaton.MonolithicSupervisor(new[] { VIN, VOUT }, new[] {E1});
//var S = DeterministicFiniteAutomaton.MonolithicSupervisor(plants.ToArray(), specs.ToArray());
//Sups.ElementAt(0).ShowAutomaton("S1");
//Sups.ElementAt(1).ShowAutomaton("S2");
//Sups.ElementAt(2).ShowAutomaton("S3");

//ShowDisablement(S, G1, 10);

S.ElementAt(0).ShowAutomaton("S1");
S.ElementAt(1).ShowAutomaton("S2");
S.ElementAt(2).ShowAutomaton("S3");

//var K1 = E1;
//var disS = S.DisabledEvents(new[] { G3 });
//S.ShowAutomaton("S");
//var comp = S.ElementAt(1).ParallelCompositionWith(S.ElementAt(2));
//comp.ShowAutomaton();
//var dis = S.DisabledEvents(new[] { G });
//Sups.ElementAt(0).ShowAutomaton("S");