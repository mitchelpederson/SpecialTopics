Asteroids Gold by Mitchel Pederson

Known Issues:

Occasional issue with performance. I didn't encounter it in release mode, so it might just be my laptop. I used the performance analyzer in Visual Studio a couple times and it seems that the constant creation of new Disc2 objects for collision detection was a large percentage of the time, but other tests just showed OpenGL function calls.


How To Use:

Controller:
Left stick - Aim and thrust
A	   - Shoot

Keyboard: 
P - toggle pause
T - hold to slow down time
I - add an asteroid
O - remove an asteroid
N - respawn if the ship is destroyed

Space - shoot
E/Up Arrow - Thrust forward
S/Left Arrow - rotate counter-clockwise
F/Right Arrow - rotate clockwise

F1 - toggle developer mode


Deep Learning:

Throughout all of the Asteroids project, I found myself spending a lot of time thinking about prioritization. This became especially obvious to me in this last phase of it, as I ended up nearly running out of time to implement all of the required features. Due to being sick and missing class, I was behind. Fortunately the assignment document contains a very detailed list of tasks. I used the same process I talked about in my previous deep learning but really emphasized priority of features this time around, thinking in terms of what MUST be completed, what SHOULD and what I was willing to sacrifice on in points if I ran out of time. 

For this specific assignment, I maximized the number of points for the amount of time I would have to spend on it. This led me to tackle the engine features before even thinking about juice. I knew I would start putting all my time into unnecessary features like UFOs and seeking missiles if I did not explicitly tell myself not to focus on that. For example, I completed all of the refactoring first, as I knew that would affect all of my future projects if I did not do it properly from the start and would make things like the Renderer and InputSystem more difficult to implement. 

In the future I will continue to try and apply strict prioritization first. It is easy to do when a list of tasks and requirements are cleanly laid out for me but when that is not available, I will make a strong effort to quantize things before touching any code. I realize this is also a strong motivation for using Scrum, so I am excited to finally put it into practice for TGP.