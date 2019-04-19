Dogfight (Post-Stall Maneuvers) by Mitchel Pederson

Engine: My custom engine written at SMU Guildhall

The focus of the project was to create a multiplayer game that works over LAN, and to refresh myself on 3D math & rendering concepts.

Features:
- Multiplayer over LAN across multiple machines in a star network pattern. Host is authoritative and clients use dead reckoning to stay as in sync with the host as possible, sending only player inputs to the host. See mitchelpederson.com for more details
- 3D plane physics based on the four fources of flight, but simplified for gameplay purposes
- Large terrain mesh generation handled asynchronously to prevent hitches when regenerating


Controls:

Keyboard:
WASD - Pitch & Roll
IJKL - Yaw & Throttle
Shift - Hold to increase effect of throttle (I + Shift = max throttle)
Q - Fire Gun
E - Acquire/Change lock
Space - Fire Missile

Xbox Controller:
Left Stick - Pitch & Roll
Right Stick - Look around
LT/RT - Throttle
LB/RB - Yaw
A - Fire Guns
B - Fire Missile
Y - Acquire/Change lock


Asset credits:
Eurofighter jet model - jasonowen https://free3d.com/3d-model/uk-eurofighter-typhoon-93005.html
Sidewinder missile model - piojoman https://www.turbosquid.com/FullPreview/Index.cfm/ID/924208

