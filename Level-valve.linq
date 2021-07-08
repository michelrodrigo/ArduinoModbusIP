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
Event full = new Event("full", Controllability.Uncontrollable);
Event heated = new Event("heated", Controllability.Uncontrollable);
Event cooled = new Event("cooled", Controllability.Uncontrollable);
Event empty = new Event("empty", Controllability.Uncontrollable);
Event start = new Event("start", Controllability.Controllable);






var VIN = new DeterministicFiniteAutomaton(new[]
{
	new Transition(s[0], open_vin, s[1]),
	new Transition(s[1], close_vin, s[0]),
	
  }, s[0], "VIN");

var VOUT = new DeterministicFiniteAutomaton(new[]
{
	new Transition(s[0], open_vout, s[1]),
	new Transition(s[1], close_vout, s[0]),

	}, s[0], "VOUT");

var TANK = new DeterministicFiniteAutomaton(new[]
{
	new Transition(s[0], open_vin, s[1]),
	new Transition(s[1], level_H1, s[2]),
	new Transition(s[2], open_vout, s[3]),
	new Transition(s[3], level_L1, s[0])

  }, s[0], "TANK");

var PROCESS = new DeterministicFiniteAutomaton(new[]
{
	new Transition(s[0], start, s[1]),
	new Transition(s[1], level_H1, s[2]),
	new Transition(s[2], heated, s[3]),
	new Transition(s[3], cooled, s[4]),
	new Transition(s[4], level_L1, s[0]),
	

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
	//new Transition(s[1], level_H1, s[1])

  }, s[0], "E2");

var E3 = new DeterministicFiniteAutomaton(new[]
{
	new Transition(s[0], level_L1, s[1]),
	new Transition(s[1], close_vout, s[0]),
	

  }, s[0], "E3");

var E4 = new DeterministicFiniteAutomaton(new[]
{
	new Transition(s[0], start, s[1]),
	new Transition(s[1], open_vin, s[0]),
	//new Transition(s[1], open_vin, s[1]),
	

  }, s[0], "E4");

var E5 = new DeterministicFiniteAutomaton(new[]
{
	new Transition(s[0], cooled, s[1]),
	new Transition(s[1], open_vout, s[0])

  }, s[0], "E5");

var E6 = new DeterministicFiniteAutomaton(new[]
{
	
	new Transition(s[0], level_H1, s[1]),
	new Transition(s[1], turn_on_mixer, s[1]),
	new Transition(s[1], cooled, s[2]),
	new Transition(s[2], turn_off_mixer, s[2]),
	new Transition(s[2], level_L1, s[0]),
	

  }, s[0], "E6");

var E7 = new DeterministicFiniteAutomaton(new[]
{
	new Transition(s[0], heated, s[1]),
	new Transition(s[1], turn_on_pump, s[1]),
	new Transition(s[1], cooled, s[2]),
	new Transition(s[2], turn_off_pump, s[2]),
	new Transition(s[2], level_L1, s[0]),
  }, s[0], "E7");


var G1 = DeterministicFiniteAutomaton.ParallelComposition(new[] {E6, E7 });


//K1.ShowAutomaton();
//G1.ShowAutomaton();

//var teste = K1.IsControllable(G1);
//pequenaFabrica(out var plants, out var specs);
var Monolitico = DeterministicFiniteAutomaton.MonolithicSupervisor(new[] { VIN, VOUT, TANK, PROCESS, MIXER, PUMP }, new[] { E1, E2, E3, E4, E5, E6, E7},false);
var S = DeterministicFiniteAutomaton.MonolithicReducedSupervisor(new[] {VOUT, TANK, PROCESS }, new[] {E1});
//var S = DeterministicFiniteAutomaton.MonolithicReducedSupervisor(new[] { G1 }, new[] {E3});
//var S = DeterministicFiniteAutomaton.MonolithicSupervisor(new[] { VIN, VOUT }, new[] {E1});
//var S = DeterministicFiniteAutomaton.MonolithicSupervisor(plants.ToArray(), specs.ToArray());
//Sups.ElementAt(0).ShowAutomaton("S1");
//Sups.ElementAt(1).ShowAutomaton("S2");
//Sups.ElementAt(2).ShowAutomaton("S3");

//ShowDisablement(S, G1, 10);

var K1 = E1;
//var disS = S.DisabledEvents(new[] { G3 });
S.ShowAutomaton("S");
//var dis = S.DisabledEvents(new[] { G });
//Sups.ElementAt(0).ShowAutomaton("S");