# AlienIsolation.DevTools

### :warning: Jank code alert! This project is still in the proof of concept stages and contains unfinished code.

A project to allow you to control the _CATHODE_ scripting system in _Alien: Isolation_'s engine.

In its current state, the project can intercept requests from the game's code to _CATHODE_ for script parameter values on entities.

It also features an ImGui-based in-game UI allowing you to control some visual demonstrations of what this project could allow you to do.

### How does this work (right now)?
The interaction between the game's code and _CATHODE_ (as I understand it) is as follows:
1. The game's code requests the value of a node's parameter on an entity via _CATHODE_'s function "find_parameter".
2. _CATHODE_ searches for the parameter on that entity, if it finds it, "find_parameter" returns true and stores the value of that parameter in the "output_ptr" parameter.
3. The game's code updates the entity's state in-game with the new value of that parameter.

By hijacking this interaction between the game and _CATHODE_, we can effectively control (in theory) the state of almost any parameter on any entity in the game, 
as long as we have two key parameters:
1. The pointer to the entity's object in memory (I currently have no automated solution for acquiring this).
2. The parameter's **ShortGuid** (essentially a SHA1 hash of the SHA1 hash of the parameter's name). 

It will eventually provide a facility for you to **globally** override the return value for any of these requests with your own desired value, 
as long as you know the ShortGuid of the parameter.

Generally, _CATHODE_ will tolerate you doing things wrong in the scripting system, in fact, if you completely disable a node in the scripting system,
the game will not crash, and instead that node's call seems to just be skipped in a flowgraph.

You can break things in _interesting_ ways by doing that, (like making it okay for you to attack story critical NPCs).

Ripley, in the 3rd person, when I blocked all requests from the game to _CATHODE_ for the value of almost any Enum parameters.

![image](https://cdn.discordapp.com/attachments/595332211826229257/873632514781089882/unknown.png)

(This forces the game to use the default values which are hard-coded into the game's binary for all of the blocked Enums, which makes all NPCs your ally, and causes the camera to do
the above, amongst others).

## Todo
- [ ] Rewrite (using templates) the repetitive janky mess that I wrote to hijack _CATHODE_'s templated "find_parameter" function. 
- [ ] Intercept requests from the game's code to _CATHODE_'s templated function "create_entity", this _might_ let us track which entities exist and display them in the UI.
