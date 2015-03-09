#include "plane.h"
#include "earthPhys.h"


plane::plane()
{ 
    mass = 19838;       // kg
    wingArea = 54.5;    // m**2
    maxThrust = 249e3;  // kg*m/s**2 (afterburners)
    
    fuel = 10040;       // L
    fuelDensity = .81;  // kg/L
    eDensity = 35e6;    // J/L
    
    // Control:
    thrust = 40e3;      // N (kg*m/s**2)
    roll = 0; pitch = 0; yaw = 0;   // radians
    // PI mode for testing:
    // roll = 0; pitch = PI; yaw = 0;   // radians
    
    // Mechanics:
    position[PLANE_X]=3000;     // m
    position[PLANE_Y]=1000;     // m
    position[PLANE_Z]=500;      // m
    velocity[PLANE_X]=-100;        // m/s
    velocity[PLANE_Y]=0;        // m/s
    velocity[PLANE_Z]=0;        // m/s
    
    // Effect of thrust control
    injectionRate = 55.83/fuelDensity;    // L/(N*s)
        
    // Forces: all in Newtons
    weight = mass*EARTH_G;
    // thrust = sqrt(injectionRate*eDensity*mass/(2*1));
    float p = EARTH_P0*pow(1-EARTH_L*position[PLANE_Z]/EARTH_T0, EARTH_G*EARTH_T0/EARTH_R);
    float T = EARTH_T0 - EARTH_L*position[PLANE_Z];
    float rho = p*EARTH_M/EARTH_R/T;
    float vmagsq = velocity[PLANE_X]*velocity[PLANE_X] + 
                    velocity[PLANE_Y]*velocity[PLANE_Y] + 
                    velocity[PLANE_Z]*velocity[PLANE_Z];
    // float TAS = vmagsq
    lift = .5*rho*(vmagsq)*wingArea*COMBAT_CL;
    drag = .5*rho*(vmagsq)*wingArea*COMBAT_CD;
    
    gforce = 0;
            
    dead = 0;
}

plane::~plane()
{  }

int plane::isDead()
{
    return dead;
}

/* Actual functions: */
/*  Equations:
    | 1   0   0   0 |  | c2  0   s2  0 |  | c3  -s3 0   0 |
    | 0   c1  -s1 0 |  | 0   1   0   0 |  | s3  c3  0   0 |
    | 0   s1  c1  0 |  | -s2 0   c2  0 |  | 0   0   1   0 |
    | 0   0   0   1 |  | 0   0   0   1 |  | 0   0   0   1 |
            R                   P                   Y      
    | 0 |  | T |
    | 0 |  | 0 |
    | L |  | 0 |
    | 1 |  | 1 |
    Lift   Thrust
            
    RPY:
    | c2c3         -c2s3        s2    0 |
    | s1s2c3+c1s3  -s1s2s3+c1c3 -s1c2 0 |
    | -c1s2c3+s1s3 c1s2s3+s1c3  c1c2  0 |
    | 0            0            0     1 |

    RPYL:           RPYT:
    | Ls2    |      | Tc2c3          |  => |x|
    | -Ls1c2 |      | T(s1s2c3+c1s3) |  => |y|
    | Lc1c2  |      | T(s1s3-c1s2c3) |  => |z|
    | 1      |      | 1              |  => |w|

    Notes:
        s -> sin
        c -> cos
        1 -> (roll)
        2 -> (pitch)
        3 -> (yaw)
*/
void plane::updateParams(float timestep)
{
    // printf("elapsed: %f\n", timestep);
    position[PLANE_X] += timestep * velocity[PLANE_X];
    position[PLANE_Y] += timestep * velocity[PLANE_Y];
    position[PLANE_Z] += timestep * velocity[PLANE_Z];
    
    // printf("%f\n", position[PLANE_X]);
    
    float vmagsq = velocity[PLANE_X]*velocity[PLANE_X] + 
                    velocity[PLANE_Y]*velocity[PLANE_Y] + 
                    velocity[PLANE_Z]*velocity[PLANE_Z];
    float dvx = (thrust*cos(pitch)*cos(yaw) + 
                        lift*sin(pitch) -
                        drag*velocity[PLANE_X]/sqrt(vmagsq))/mass;
    float dvy = (thrust*(sin(roll)*sin(yaw)-
                    cos(roll)*sin(pitch)*cos(yaw)) -
                lift*(sin(roll)*cos(pitch)) -
                drag*velocity[PLANE_Y]/sqrt(vmagsq))/mass;
    float dvz = (thrust*(sin(roll)*sin(yaw)-
                    cos(roll)*sin(pitch)*cos(yaw)) +
                lift*(cos(roll)*cos(pitch)) -
                drag*velocity[PLANE_Z]/sqrt(vmagsq) - 
                weight)/mass;
    velocity[PLANE_X] += dvx*timestep;
    velocity[PLANE_Y] += dvy*timestep;
    velocity[PLANE_Z] += dvz*timestep;
    
    acceleration[PLANE_X] = dvx;
    acceleration[PLANE_Y] = dvy;
    acceleration[PLANE_Z] = dvz;
    
    // Gas update:
    if(fuel>0)
    {
        float fuelstep = injectionRate*timestep;
        fuel -= fuelstep;
        mass -= fuelstep*fuelDensity;
    }
    else
    {
        fuel = 0;
        thrust = 0;
    }
    
    // Forces update:
    float p = EARTH_P0*pow(1-EARTH_L*position[PLANE_Z]/EARTH_T0, EARTH_G*EARTH_T0/EARTH_R);
    float T = EARTH_T0 - EARTH_L*position[PLANE_Z];
    float rho = p*EARTH_M/EARTH_R/T;
    vmagsq = velocity[PLANE_X]*velocity[PLANE_X] + 
                    velocity[PLANE_Y]*velocity[PLANE_Y] + 
                    velocity[PLANE_Z]*velocity[PLANE_Z];
    // printf("%f\n", rho);
    lift = .5*rho*(vmagsq)*wingArea*COMBAT_CL;
    drag = .5*rho*(vmagsq)*wingArea*COMBAT_CD;
    
    weight = mass*EARTH_G;
    
    if(position[PLANE_Z] <= 0 || gforce > 25)
    {
        dead = true;
    }
}

void plane::kill()
{
    dead = true;
}

// Flight controls:
void plane::ctrlPitch(float angle)
{
    pitchPlane(angle*cos(roll));
    if(cos(pitch)>0)
        yawPlane(angle*sin(roll));
    else
        yawPlane(-angle*sin(roll));
}
void plane::ctrlRoll(float angle)
{
    rollPlane(angle);
}
void plane::ctrlYaw(float angle)
{
    pitchPlane(-angle*sin(roll));
    if(cos(pitch)>0)    // Normal operation
    {
        // pitchPlane(angle*sin(roll));
        yawPlane(angle*cos(roll));
    }
    else
    {
        yawPlane(-angle*cos(roll));
    }
}

void plane::injectControl(float rate)
{
    injectionRate += rate;
}

void plane::thrusterUp(void)
{
    if(thrust<maxThrust && fuel >0)
        thrust+=1e3;
    else if(fuel>0)
        thrust=maxThrust;
    else
        thrust = 0;
}
void plane::thrusterDown(void)
{
    if(thrust>0)
        thrust-=1e3;
    else
        thrust = 0;
}

void plane::pitchPlane(float angle)
{
    pitch += angle;
}
void plane::yawPlane(float angle)
{
    yaw += angle;
}
void plane::rollPlane(float angle)
{
    roll += angle;
}

// Getters and setters:
float plane::getGas(void)
{
    return fuel;
}

void plane::getPosition(float* buf)
{
    std::memcpy(buf, position, 3*sizeof(float));
}
void plane::setPosition(float x, float y, float z)
{
    position[0] = x;
    position[1] = y;
    position[2] = z;
}

void plane::getVelocity(float * buf)
{
    std::memcpy(buf, velocity, 3*sizeof(float));
}
void plane::setVelocity(float x, float y, float z)
{
    velocity[0] = x;
    velocity[1] = y;
    velocity[2] = z;
}

void plane::getAcceleration(float* buf)
{
    std::memcpy(buf, acceleration, 3*sizeof(float));
}

float plane::getInjectionRate()
{
    return injectionRate;
}

float plane::getYaw()
{
    return yaw;
}
void plane::setYaw(float newYaw)
{
    yaw = newYaw;
}

float plane::getPitch()
{
    return pitch;
}
void plane::setPitch(float newPitch)
{
    pitch = newPitch;
}

float plane::getRoll()
{
    return roll;
}
void plane::setRoll(float newRoll)
{
    roll = newRoll;
}


float plane::getWeight(void)
{
    return weight;
}
float plane::getThrust(void)
{
    return thrust;
}
float plane::getLift(void)
{
    return lift;
}
float plane::getDrag(void)
{
    return drag;
}

float plane::getTemp(void)
{
    return EARTH_T0 - EARTH_L * position[PLANE_Z];
}
float plane::getPressure(void)
{
    return EARTH_P0*pow(1-EARTH_L * position[PLANE_Z]/EARTH_T0, EARTH_G*EARTH_T0/EARTH_R);
}
