OSCToolset is a runtime plugin that simplifies the [OSC](https://dev.epicgames.com/documentation/en-us/unreal-engine/osc-plugin-overview-for-unreal-engine) workflow in Unreal Engine. It streamlines the lifecycle of the Client/Server and is designed with simplicity in mind.

The main design principles are simplicity and modularity. The user creates and sends a properly formatted address and the rest is handled by the Plugin itself. That includes:

- Data driven modules (receivers and senders)
- Value parsing.
- Tick interpolation.
- Automatic routing.
- Debuging
- Filtering

If you are interested you can read more in the [[docs]].