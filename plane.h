#ifndef PLANE_H
#define PLANE_H

#include <cmath>
#include <cstring>
#include <cstdio>

// Magic numbers:
#define PLANE_X 0
#define PLANE_Y 1
#define PLANE_Z 2
// Loss of consciousness parameters. Didn't get to them.
#define PILOT_D_GLOC 6
#define PILOT_D_GDIE 25
#define PILOT_U_GLOC -3
#define PILOT_LOC_RECOV 12
// Lift/drag coefficients:
#define COMBAT_CL 1.75
#define COMBAT_CD .021
// Duh:
#define PI 3.141592654

class plane
{
public:
    // Default:
    plane();
    // plane(float m, float A, float gas, float maxThrust, float xyz[3], float g);
    ~plane();
    
    void updateParams(float timestep);
    
    // Flight controls: the magic happens here.
    // Relative orientation settings:
    void ctrlPitch(float angle);    // Up/down arrows
    void ctrlRoll(float angle);     // Left/right arrows
    void ctrlYaw(float angle);      // Ctrl+left/right arrows
    
    void thrusterUp(void);          // Increase/decrease thrust
    void thrusterDown(void);
    // void injectControl(float rate);  // Not needed
    
    // Getters for the HUD/main program:
    float getGforce(void);
    
    float getGas(void);
    
    void getPosition(float* buf);
    void setPosition(float x, float y, float z);
    
    void getVelocity(float * buf);
    void setVelocity(float x, float y, float z);
    
    void getAcceleration(float * buf);
    
    float getInjectionRate();
    
    float getYaw();
    // void setYaw(float newYaw);
    
    float getPitch();
    // void setPitch(float newPitch);
    
    float getRoll();
    // void setRoll(float newRoll);
    
    float getWeight(void);
    float getThrust(void);
    float getLift(void);
    float getDrag(void);
    
    float getTemp(void);
    float getPressure(void);
    
    int isDead();
    
private:
    /* data */
    // Intrinsic:
    float mass;             // kg
    float wingArea;         // How large is the wing: m^2
    float maxThrust;        // N. Thrust caps out here.
    
    // Properties:
    float fuel;             // L
    float fuelDensity;      // kg/L - mass per volume
    // float eDensity;         // J/L - energy per volume: deprecated.
    
    // Control:
    float injectionRate;    // L/s - fuel consumption rate
    float pitch, roll, yaw;     // Angles with respect to the ocean plane.
    
    // Mechanics:
    float position[3];      // xyz axes
    float velocity[3];      // vector (has direction)
    float acceleration[3];  // sampled
    
    // Forces:
    float weight;   // Acts down
    float thrust;   // Acts forward
    float lift;     // Acts 'up'
    float drag;     // Acts counter velocity
    
    float gforce;   // Not implemented yet, sorry.
    
    int dead;       // semaphore to terminate simulation.
    
    // void kill();    // set semaphore. Not needed
    
    // Helper to orient plane.
    void pitchPlane(float angle);   
    void yawPlane(float angle);
    void rollPlane(float angle);
};

#endif
