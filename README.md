# flightsim-and-jetsim
README (Copy-pasted from compass)

- Basic Information
    - PLATFORM - Linux
    - OS Version - 3.18.6-1 (Arch Linux)
    - Build Environment - GNU Make 4.1.1
    - Language - C++
    - External Libraries Used - gl, glu, freeglut, cmath (it still has to be linked.)

- Build Instructions
    - It should run fine after typing `make mp2`.
    - The executable shall be called `mp2`.
    - Should any changes be made to just the mp2 part, do `make mp2`, otherwise, `make`, which will do `make clean mp2`.
    - You probably don't have to change anything at all, but oversights happen, so email/pull request/open issue if there is one.
    
- How to Run
    - `./mp2` opens the flight window.
    - You will be spawned over an "ocean". The ocean is 4000x4000 'meters' big. Start position is (1000, 1000, 100) = (x,y,z).
    - Your initial velocity will be (100, 0, 0).
    - Controls:
        - To increase thrust output, press `k`. To decrease, please press `j`.
        - To roll right/left, press `→/←`.
        - To yaw right/left , press `ctrl+→/←`.
        - To pitch up/down, press `↓/↑`.
        - To quit, press `q`.
    - Rules and physics:
        - You have limited fuel, which has mass. Running out of fuel will kill the thrust output.
        - Aerodynamic forces will be at play. (Weight, lift, thrust, drag).
        - Crashing into the ocean will terminate the simulation.
        - Flying out of bounds will terminate the simulation. (Boundaries are marked by mega-sized, ridiculously-colored triangles.)
        - You have momentum. Turning right around will cause a stall and will more than likely prove fatal.
        - Weight varies with mass, always pointing down.
        - Thrust is controlled by user input. Always points forward.
        - Lift depends on speed and elevation (because of temperature and air pressure/density). Always points in the plane's 'up' direction.
        - Drag depends on speed and elevation. Always points counter velocity vector.
        - Atmosphere decreases in pressure and temperature at higher altitudes.
        - There is a rudimentary HUD, which shows some useful debugging information.
        - Fun fact: physics were modeled after an F-14 Tomcat. I know it's nowhere near perfect, but it works on some believable level, I guess. Also, for some reason (I didn't have time/forgot to implement jet engines), you only have afterburners.

- Location of Video Demo
    - http://youtu.be/elCjoBffddc

- Github repo:
    - https://github.com/calculuswhiz/flightsim-and-jetsim
