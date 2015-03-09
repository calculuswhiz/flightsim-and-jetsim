#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/freeglut.h>

#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctime>
#include <cstring>

#include "plane.h"

#define  PRESS_LEFT     (0x0064-0x64)
#define  PRESS_UP       (0x0065-0x64)
#define  PRESS_RIGHT    (0x0066-0x64)
#define  PRESS_DOWN     (0x0067-0x64)
#define  PRESS_CTRL_L   (0x0072-0x64)
#define  PRESS_CTRL_R   (0x0073-0x64)


int nFPS = 30;
clock_t startClock=0,curClock;
long long int prevF=0,curF=0;
int wiremode=0;
int winId;
float rot = 0;

// Global object ...
plane yourPlane;

// Control matrix:
int asciiPressed[256]={0};
int specialPressed[256]={0};  // Left up right down

void init(void) 
{
   glClearColor (0.0, 0.0, 0.0, 0.0);
   glShadeModel (GL_FLAT);
}

void
renderText(float x, float y, const char* text) {
    int viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(viewport[0], viewport[2], viewport[1], viewport[3], -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glRasterPos2f(x, viewport[3] - y);
    glutBitmapString(GLUT_BITMAP_8_BY_13, (const unsigned char*)text);
    glMatrixMode( GL_PROJECTION );
    glPopMatrix();
    glMatrixMode( GL_MODELVIEW );   
    glPopMatrix();
}

void drawText(const char *string){
    glColor3f(1.0, 1.0, 0);
    float position[3];
    yourPlane.getPosition(position);
    glRasterPos3f(position[PLANE_X], position[PLANE_Y], position[PLANE_Z]);
    glutBitmapString(GLUT_BITMAP_8_BY_13, (const unsigned char*)string);
    glutPostRedisplay();
    // glPopMatrix();
}

void timer(int v)
{
    glutTimerFunc(1000/nFPS,timer,v); // restart timer again
}

void drawOcean(void)
{
    // glLoadIdentity();
    float oceanSize = 4000;
    glColor3f (.5, .5, 1.0);
    glBegin(GL_POLYGON);
        glVertex3f(0,0,0);
        glVertex3f(0,oceanSize,0);
        glVertex3f(oceanSize,oceanSize,0);
        glVertex3f(oceanSize,0,0);
    glEnd();
    
    glBegin(GL_TRIANGLES);
        glColor3f(.5, 0, 0);
        glVertex3f(0, 0, 0);
        glVertex3f(0, oceanSize, 0);
        glVertex3f(0, oceanSize/2, oceanSize/2);
        
        glColor3f(.5, .5, 0);
        glVertex3f(0, 0, 0);
        glVertex3f(oceanSize, 0, 0);
        glVertex3f(oceanSize/2, 0, oceanSize/2);
        
        glColor3f(0, .5, 0);
        glVertex3f(oceanSize, 0, 0);
        glVertex3f(oceanSize, oceanSize, 0);
        glVertex3f(oceanSize, oceanSize/2, oceanSize/2);
        
        glColor3f(0, .5, .5);
        glVertex3f(0, oceanSize, 0);
        glVertex3f(oceanSize, oceanSize, 0);
        glVertex3f(oceanSize/2, oceanSize, oceanSize/2);
    glEnd();
    
    float dc[2];
    
    glColor3f(0, 1, 0);
    glBegin(GL_POINTS);
        for(dc[0]=0; dc[0]<oceanSize; dc[0]+=10)
        {
            for(dc[1]=0; dc[1]<oceanSize; dc[1]+=10)
            {
                glVertex3f(dc[0],dc[1],0);
            }
        }
    glEnd();
    // glColor3f(1, 1, 1);
    
    glutPostRedisplay();
    // glFlush();
}

void xdrawOcean(void)
{
    // glLoadIdentity();
    glBegin(GL_POLYGON);
        glVertex3f(0,0,0);
        glVertex3f(0,10,0);
        glVertex3f(20,10,0);
        glVertex3f(20,0,0);
    glEnd();
    
    glFlush();
}

// Use this to adjust the camera.
void display(void)
{
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    curF++;
    
    float position[3];
    yourPlane.getPosition(position);
    
    float velocity[3];
    yourPlane.getVelocity(velocity);
    float speed = sqrt(velocity[PLANE_X]*velocity[PLANE_X]+
                       velocity[PLANE_Y]*velocity[PLANE_Y]+
                       velocity[PLANE_Z]*velocity[PLANE_Z] );
    
    float accel[3];
    yourPlane.getAcceleration(accel);
    
    float yaw = yourPlane.getYaw();
    float pitch = yourPlane.getPitch();
    float roll = yourPlane.getRoll();
    
    float weight = yourPlane.getWeight();
    float thrust = yourPlane.getThrust();
    float lift = yourPlane.getLift();
    float drag = yourPlane.getDrag();
    
    // int vmag = 10;
    // float point[3], up[3];
    
    /*
        point[0] = (position[PLANE_X]+vmag)*cos(pitch)*cos(yaw) - position[PLANE_Y]*(cos(pitch)*sin(yaw)) + position[PLANE_Z]*sin(pitch);
        point[1] = (position[PLANE_X]+vmag)*(sin(roll)*sin(pitch)*cos(yaw)+cos(roll)*sin(yaw)) + position[PLANE_Y]*(cos(yaw)*cos(roll)-sin(roll)*sin(pitch)*sin(yaw)) - position[PLANE_Z]*sin(roll)*cos(pitch);
        point[2] = (position[PLANE_X]+vmag)*(sin(roll)*sin(yaw)-cos(roll)*sin(pitch)*cos(yaw)) + position[PLANE_Y]*(cos(roll)*sin(pitch)*sin(yaw)+sin(roll)*cos(yaw)) + position[PLANE_Z]*cos(roll)*cos(pitch);
        up[0] = position[PLANE_X]*cos(pitch)*cos(yaw) - position[PLANE_Y]*(cos(pitch)*sin(yaw)) + (position[PLANE_Z]+vmag)*sin(pitch);
        up[1] = position[PLANE_X]*(sin(roll)*sin(pitch)*cos(yaw)+cos(roll)*sin(yaw)) + position[PLANE_Y]*(cos(yaw)*cos(roll)-sin(roll)*sin(pitch)*sin(yaw)) - (position[PLANE_Z]+vmag)*sin(roll)*cos(pitch);
        up[2] = position[PLANE_X]*(sin(roll)*sin(yaw)-cos(roll)*sin(pitch)*cos(yaw)) + position[PLANE_Y]*(cos(roll)*sin(pitch)*sin(yaw)+sin(roll)*cos(yaw)) + (position[PLANE_Z]+vmag)*cos(roll)*cos(pitch);
    */
    
    // glPushMatrix();
    glLoadIdentity();
    gluLookAt (0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0);
    
    glPushMatrix();
    
    glRotatef(-roll*180/PI, 1, 0, 0);
    glRotatef(-pitch*180/PI, 0, 1, 0);
    glRotatef(-yaw*180/PI, 0, 0, 1);
    
    // glTranslatef(-rot, -rot, -50);
    rot += .1;
    glTranslatef(-position[PLANE_X], -position[PLANE_Y], -position[PLANE_Z]);
    // printf("%f %f %f\n", position[PLANE_X], position[PLANE_Y], position[PLANE_Z]);
    
    drawOcean();
    glPopMatrix();
    
    char posbuf[64], dirbuf[64], velobuf[64], forcebuf[64];
    char accelbuf[64];
    char miscbuf[32];
    sprintf(posbuf, "Position x:%f y:%f z:%f", position[PLANE_X], position[PLANE_Y], position[PLANE_Z]);
    sprintf(dirbuf, "Rotation Y:%f P:%f R:%f", yaw, pitch, roll);
    sprintf(velobuf, "Velocity x:%f y:%f z:%f", velocity[PLANE_X], velocity[PLANE_Y], velocity[PLANE_Z]);
    sprintf(accelbuf, "Accel x:%f y:%f z:%f", accel[PLANE_X], accel[PLANE_Y], accel[PLANE_Z]);
    sprintf(forcebuf, "w:%f t:%f l:%f d:%f", weight, thrust, lift, drag);
    renderText(0, 13, posbuf);
    renderText(0, 26, dirbuf);
    renderText(0, 39, velobuf);
    renderText(0, 52, accelbuf);
    renderText(0, 65, forcebuf);
    sprintf(miscbuf, "Gas: %f spd: %f", yourPlane.getGas(), speed);
    renderText(0, 78, miscbuf);
    
    if(yourPlane.getGas()<=0)
        renderText(0, 100, "Out of gas :(");
    
    glutPostRedisplay();
    
    glFlush();
    
    // printf("x: %f y: %f z: %f", position[PLANE_X], position[PLANE_Y], position[PLANE_Z]);
    // printf("Y: %f, P: %f, R: %f\n", yaw, pitch, roll);
    
    /*  Check for control:
        asciiPressed[256]
        specialPressed[256]*/
    // Thrusters:
    if( asciiPressed[(unsigned char)'j'] )
    {
        yourPlane.thrusterDown();
        // printf("%f\n", yourPlane.getInjectionRate());
    }
    else if (asciiPressed[(unsigned char)'k'])
    {
        yourPlane.thrusterUp();
        // printf("%f\n", yourPlane.getInjectionRate());
    }
    
    // Directional:
    
    if(specialPressed[PRESS_LEFT])
    {
        if( specialPressed[PRESS_CTRL_R] || specialPressed[PRESS_CTRL_L] )
        {
            yourPlane.ctrlYaw(PI/180/3);
        }
        else
        {
            // printf("LEFT\n");
            yourPlane.ctrlRoll(-PI/180);
        }
    }
    else if(specialPressed[PRESS_RIGHT])
    {
        if( specialPressed[PRESS_CTRL_R] || specialPressed[PRESS_CTRL_L] )
        {
            // printf("CTRL_RIGHT\n");
            yourPlane.ctrlYaw(-PI/180/3);
        }
        else
        {
            // printf("RIGHT\n");
            yourPlane.ctrlRoll(PI/180);
        }
    }
    
    if(specialPressed[PRESS_DOWN])
    {
        // printf("DOWN\n");
        yourPlane.ctrlPitch(-PI/180/2);
    }
    else if(specialPressed[PRESS_UP])
    {
        // printf("UP\n");
        yourPlane.ctrlPitch(PI/180/2);
    }
    
    // Time stuff:
    curClock=clock();
    float elapsed=(curClock-startClock)/(float)CLOCKS_PER_SEC;
    float fps=(float)(curF-prevF)/elapsed;  // Frames
    prevF=curF;
    startClock=curClock;
    
    yourPlane.updateParams(elapsed);
    
    if(yourPlane.isDead())
    {
        printf("Plane has crashed.\n");
        glutDestroyWindow(winId);
        return;
    }
    if(position[PLANE_X]>4000 || position[PLANE_Y]>4000 || position[PLANE_X]<0 || position[PLANE_Y] < 0)
    {
        printf("Out of bounds :(\n");
        glutDestroyWindow(winId);
        return;
    }
    
    glutSwapBuffers();
}

void xdisplay(void)
{
    char buf[32];
    curF++;
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glPushMatrix();
    gluLookAt (0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0);
         
    glRotatef (-45, 0.0, 0.0, 1.0);
    glRotatef (45, 1.0, -1.0, 0.0);
    rot+=1;
    
    glTranslatef(0, 0, -50);
    
    glPushMatrix();
    drawOcean();
    glPopMatrix();
    
    curClock=clock();
    float elapsed=(curClock-startClock)/(float)CLOCKS_PER_SEC;
    // if(elapsed>1.0f){
        float fps=(float)(curF-prevF)/elapsed;
        // sprintf(buf, "%f", fps);
        // drawText(buf);
        prevF=curF;
        startClock=curClock;
    // }
    

    glPopMatrix();
    glutSwapBuffers();
}

void askey(unsigned char key, int x, int y)
{
    asciiPressed[key] ^= 0x1;
    
    switch(key)
    {
        // case 'k':   // More gas
        // break;
        // case 'j':   // Less gas
        // break;
        // case 'K':   // More more gas
        // break;
        // case 'J':    // Less less gas
        // break;
        case 'q':    // Quit the simulation
            glutDestroyWindow(winId);
        break;
        default:
        break;
    }
}

void dirkey(int key, int x, int y)
{
    // switch(key)
    // {
        if(key>=0x64 && key < 0x74)
            specialPressed[key-0x64] ^= 0x1;
        /*case GLUT_KEY_LEFT:     // Roll left
        break;
        case GLUT_KEY_UP:       // Pitch down
        break;
        case GLUT_KEY_RIGHT:    // Roll right
        break;
        case GLUT_KEY_DOWN:     // Pitch up
        break;
        case GLUT_KEY_CTRL_L:   // Yaw active
        case GLUT_KEY_CTRL_R:
        default:
        break;*/
    // }
}

void reshape (int w, int h)
{
    glViewport (0, 0, (GLsizei) w, (GLsizei) h);
    glMatrixMode (GL_PROJECTION);
    glLoadIdentity ();
    glFrustum (-w/30, w/30, -h/30, h/30, 100, 2000);
    // glFrustum (-2.0, 2.0, -2.0, 2.0, 1, 10.0);
    // glOrtho(0, 2000, 0, 2000, 0, 2000);
    glMatrixMode (GL_MODELVIEW);
    glLoadIdentity();
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize (500, 500);
    glutInitWindowPosition (100, 100);
    winId = glutCreateWindow ("Fightsim and Jetsim");
    
    // Starter up!
    init();
    
    // Callbacks:
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    // glutMouseFunc(mouse);
    glutKeyboardFunc(askey);
    glutKeyboardUpFunc(askey);
    glutSpecialFunc(dirkey);
    glutSpecialUpFunc(dirkey);
    
    glutTimerFunc(100,timer,nFPS); // a periodic timer. Usually used for updating animation
    startClock=clock();
    
    glutMainLoop();
    
    return 0;
}
