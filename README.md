OSCToolset is a runtime plugin that simplifies the [OSC](https://dev.epicgames.com/documentation/en-us/unreal-engine/osc-plugin-overview-for-unreal-engine) workflow in Unreal Engine. It streamlines the lifecycle of the Client/Server and is designed with simplicity in mind.

The main design principles are simplicity and modularity. The user creates and sends a properly formatted address and the rest is handled by the Plugin itself. That includes:

- Managed Server and Client lifecycle
- Modules
    - Receivers
        - Packs
        - Tick interpolation
    - Senders
    - Debug
- Router
- Filtering
- Runtime settings menu

If you are interested you can read more in the [docs](https://www.eusebijucgla.com/tools/osctoolset/docs/).

---

# REM Edition — Additions (v1.0.0)

This is a fork (REMvisual) that adds Blueprint-facing tooling on top of OSCToolset. All additions are editor-only nodes or pure Blueprint functions — the runtime OSC pipeline is unchanged. The original plugin by Eusebi Jucgla is the upstream (links below).

## Nodes

- **OSC Switch Values** — the headline node. Drive it from any `GET_*` OSCT Router event (especially the new `GET_All`) by wiring that event's exec + `Receiver`. Declare a set of addresses (inline list and/or an `FOSCT_ReceiverRow` DataTable + filter, set in the **Details** panel). For each address you get **both** an exec output (routed when that address changes, like a switch) **and** a typed value pin with its live value — float, vector, transform, color, bool, string, etc. Plus a `Default` exec.
- **Add Receivers From Data Table** — registers receivers for the owning actor from a DataTable with an optional address filter. Table + Filter are available as input pins (wire variables for a procedural setup) or in Details. Drive from BeginPlay.

## Functions

- **Get OSC Float / Integer / Vector2 / Vector3 / Rotation / Color / Transform / Bool / String** — read the current live value for a receiver address (the address is a runtime pin). The receiver must already be registered. Use these for fully data-driven reads.

## Router events

- **GET_All / GET_All_Tick** — fire for *any* registered receiver type (float, vector, transform, string, note, event, packs…), carrying just the `Receiver` (no value). Implement `GET_All` to handle many types through one event.

## Recommended workflow

```
Event BeginPlay ─► Add Receivers From Data Table (Table, Filter "KnobA1 Main")   ← register once
Event GET_All   ─► OSC Switch Values (Receiver wired; same Table/Filter in Details)
                   └─ KnobA1 / KnobA1 Value / Main / Main Value / Default          ← route + read
```

Register your addresses on BeginPlay; route and read them with one event via OSC Switch Values. For a fully procedural alternative, use `GET_All` → break the `Receiver` → `Get OSC Float(Address)` with the runtime address (no fixed pins).

**Filter syntax** (Add Receivers + OSC Switch Values): space-separated terms are OR-ed; `*` is a wildcard — e.g. `KnobA1 Main` or `Knob*`.

> Note: `OSC Switch Values`' Table/Filter are set in Details (they define the case pins at compile time, like the engine's Switch-on-Enum), so they can't be driven by runtime variables. Use the procedural `Get OSC <Type>(Address)` path for that.

---

# Example Files
The example files can be purchased in my website:
- [OSCT-Examples-Basic](https://www.eusebijucgla.com/tools/osctoolset/unrealengine/)

 
# Bindings
## [OSCToolset-TouchDesigner](https://www.eusebijucgla.com/tools/osctoolset/touchdesigner/)
If you want to use TouchDesigner together with OSCToolset, I recommend you to see my OSCToolset-TouchDesigner component, which will make your life much easier in TouchDesigner:

## [OSCToolset-Ableton](https://www.eusebijucgla.com/updates/tools/osctoolset/ableton)
I am also working on an Ableton bindig which you can get updates for when it is finshed here.

# Downloads
I provide the latest 3 Unreal Engine versions build for Windows. But you can package this Plugin for MacOS or Linux if you hve the machine to do so.

From this repository releases:
- [Releases](https://github.com/eusebijucgla/OSCToolset/releases)

Or download them from my website:
- [OSCToolset](https://www.eusebijucgla.com/tools/osctoolset/unrealengine/)

- Gives access to [discord](https://www.eusebijucgla.com/tools/osctoolset/unrealengine/discord)