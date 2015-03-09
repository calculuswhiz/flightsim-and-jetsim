#ifndef PLANE_H
#define PLANE_H

#include <cmath>
#include <cstring>
#include <cstdio>

#define PLANE_X 0
#define PLANE_Y 1
#define PLANE_Z 2
#define PILOT_D_GLOC 6
#define PILOT_D_GDIE 25
#define PILOT_U_GLOC -3
#define PILOT_LOC_RECOV 12
#define COMBAT_CL 1.75
#define COMBAT_CD .021
#define PI 3.141592654

// #define POINT_RADIUS 0
// #define POINT_ZENITH 1
// #define POINT_AZIMUTH 2

class plane
{
public:
    // Default:
    plane();
    // plane(float m, float A, float gas, float maxThrust, float xyz[3], float g);
    ~plane();
    
    void updateParams(float timestep);
    
    // Flight controls:
    void ctrlPitch(float angle);
    void ctrlRoll(float angle);
    void ctrlYaw(float angle);
    
    void injectControl(float rate);
    
    void thrusterUp(void);
    void thrusterDown(void);
    
    // Getters/setters:
    float getGas(void);
    
    void getPosition(float* buf);
    void setPosition(float x, float y, float z);
    
    void getVelocity(float * buf);
    void setVelocity(float x, float y, float z);
    
    void getAcceleration(float * buf);
    
    float getInjectionRate();
    
    float getYaw();
    void setYaw(float newYaw);
    
    float getPitch();
    void setPitch(float newPitch);
    
    float getRoll();
    void setRoll(float newRoll);
    
    float getWeight(void);
    float getThrust(void);
    float getLift(void);
    float getDrag(void);
    
    int isDead();
    
private:
    /* data */
    // Intrinsic:
    float mass;
    float wingArea;
    float maxThrust;
    
    // Properties:
    float fuel;             // L
    float fuelDensity;      // kg/L
    float eDensity;         // J/L
    
    // Control:
    float injectionRate;    // L/s
    float pitch, roll, yaw;     // These are all angles
    
    // Mechanics:
    float position[3];
    float velocity[3];
    float acceleration[3];
    
    // Forces:
    float weight;   // Acts down
    float thrust;   // Acts on plane's y axis. This is integrated.
    float lift;     // Acts on plane's z axis
    float drag;     // Acts on plane's -y axis
    
    float gforce;   // Not implemented yet.
    
    int dead;
    
    void kill();
    
    void pitchPlane(float angle);
    void yawPlane(float angle);
    void rollPlane(float angle);
};

#endif
