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

#define  OCEANSIZE      10000
#define  MARKER_SPACING   20

int nFPS = 30;
clock_t startClock=0,curClock;
long long int prevF=0,curF=0;
int wiremode=0;
int winId;
float rot = 0;

// Global object:
plane yourPlane;

// Control matrix:
int asciiPressed[256]={0};
int specialPressed[256]={0};  // Left up right down

void init(void) 
{
   glClearColor (0.0, 0.0, 0.0, 0.0);
   glShadeModel (GL_SMOOTH);
}

// Totally swiped this from StackOverflow:
void renderText(float x, float y, const char* text) {
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

void timer(int v)
{
    glutTimerFunc(1000/nFPS,timer,v); // restart timer again
}

// Model the world with this function:
// Green dots are used as distance markers.
void drawOcean(void)
{
    // glLoadIdentity();
    // float oceanSize = OCEANSIZE;
    glColor3f (.5, .5, 1.0);
    glBegin(GL_POLYGON);
        glColor3f(1.0, 0.0, 0.0); glVertex3f(0,0,0);
        glColor3f(0.0, 1.0, 0.0); glVertex3f(0,OCEANSIZE,0);
        glColor3f(0.0, 0.0, 1.0); glVertex3f(OCEANSIZE,OCEANSIZE,0);
        glColor3f(0.0, 1.0, 1.0); glVertex3f(OCEANSIZE,0,0);
    glEnd();
    
    glBegin(GL_TRIANGLES);
        // glColor3f(.5, 0, 0);
        glColor3f(1.0, 0.0, 0.0); glVertex3f(0, 0, 0);
        glColor3f(0.0, 1.0, 0.0); glVertex3f(0, OCEANSIZE, 0);
        glColor3f(1.0, 0.0, 0.0); glVertex3f(0, OCEANSIZE/2, OCEANSIZE/2);
        
        glColor3f(.5, .5, 0);
        glVertex3f(0, 0, 0);
        glVertex3f(OCEANSIZE, 0, 0);
        glVertex3f(OCEANSIZE/2, 0, OCEANSIZE/2);
        
        glColor3f(0, .5, 0);
        glVertex3f(OCEANSIZE, 0, 0);
        glVertex3f(OCEANSIZE, OCEANSIZE, 0);
        glVertex3f(OCEANSIZE, OCEANSIZE/2, OCEANSIZE/2);
        
        glColor3f(0, .5, .5);
        glVertex3f(0, OCEANSIZE, 0);
        glVertex3f(OCEANSIZE, OCEANSIZE, 0);
        glVertex3f(OCEANSIZE/2, OCEANSIZE, OCEANSIZE/2);
    glEnd();
    
    float dc[2];
    
    glColor3f(0, 1, 0);
    glBegin(GL_POINTS);
        for(dc[0]=0; dc[0]<OCEANSIZE; dc[0]+=MARKER_SPACING)
        {
            for(dc[1]=0; dc[1]<OCEANSIZE; dc[1]+=MARKER_SPACING)
            {
                glVertex3f(dc[0],dc[1],0);
            }
        }
    glEnd();
    
    glutPostRedisplay();
}

// Callback display function:
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
    
    glLoadIdentity();
    // Stare at +x for my conventions:
    gluLookAt (0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0);
    
    glPushMatrix();
    
    glRotatef(-roll*180/PI, 1, 0, 0);
    glRotatef(-pitch*180/PI, 0, 1, 0);
    glRotatef(-yaw*180/PI, 0, 0, 1);
    
    rot += .1;
    glTranslatef(-position[PLANE_X], -position[PLANE_Y], -position[PLANE_Z]);
    
    drawOcean();
    glPopMatrix();
    
    // HUD stuff:
    char posbuf[64], dirbuf[64], velobuf[64], forcebuf[64];
    char accelbuf[64];
    char miscbuf[32];
    sprintf(posbuf, "Pos (m) x:%.2f y:%.2f z:%.2f", position[PLANE_X], position[PLANE_Y], position[PLANE_Z]);
    sprintf(dirbuf, "Rot (deg) Y:%.2f P:%.2f R:%.2f", yaw*180/PI, pitch*180/PI, roll*180/PI);
    sprintf(velobuf, "Vel (m/s) x:%.2f y:%.2f z:%.2f", velocity[PLANE_X], velocity[PLANE_Y], velocity[PLANE_Z]);
    sprintf(accelbuf, "Acc (m/s^2) x:%.2f y:%.2f z:%.2f", accel[PLANE_X], accel[PLANE_Y], accel[PLANE_Z]);
    sprintf(forcebuf, "F (N) w:%.2f t:%.2f l:%.2f d:%.2f", weight, thrust, lift, drag);
    renderText(0, 13, posbuf);
    renderText(0, 26, dirbuf);
    renderText(0, 39, velobuf);
    renderText(0, 52, accelbuf);
    renderText(0, 65, forcebuf);
    sprintf(miscbuf, "F>%.2fL v>%.2fm/s", yourPlane.getGas(), speed);
    renderText(0, 78, miscbuf);
    sprintf(miscbuf, "%.2fkPa %.2fCelsius", yourPlane.getPressure()/1000, yourPlane.getTemp()-273.15);
    renderText(0, 91, miscbuf);
    sprintf(miscbuf, "geez:%.2f", yourPlane.getGforce());
    renderText(0, 104, miscbuf);
    
    // if(sin(yaw)<=sqrt(2)/2 && sin(yaw)>=-sqrt(2)/2)
    // {
    //     if(cos(yaw) > 0)
    //         renderText(400, 100, "E");
    //     else if (cos(yaw)<0)
    //         renderText(400, 100, "W");
    // }
    // else if(cos(yaw)<sqrt(2)/2 && cos(yaw)>-sqrt(2))
    // {
    //     if(sin(yaw)>0)
    //         renderText(400, 100, "N");
    //     else if(sin(yaw)<0)
    //         renderText(400, 100, "S");
    // }
    
    // Out of gas!!
    if(yourPlane.getGas()<=0)
        renderText(0, 150, "Out of gas. Recommend eject with 'q'");
    
    glutPostRedisplay();
    
    glFlush();
    
    /*  Check for control:
        asciiPressed[256]
        specialPressed[256]*/
    // Thrusters:
    if( asciiPressed[(unsigned char)'j'] )
    {
        yourPlane.thrusterDown();
    }
    else if (asciiPressed[(unsigned char)'k'])
    {
        yourPlane.thrusterUp();
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
            yourPlane.ctrlRoll(-PI/180);
        }
    }
    else if(specialPressed[PRESS_RIGHT])
    {
        if( specialPressed[PRESS_CTRL_R] || specialPressed[PRESS_CTRL_L] )
        {
            yourPlane.ctrlYaw(-PI/180/3);
        }
        else
        {
            yourPlane.ctrlRoll(PI/180);
        }
    }
    
    if(specialPressed[PRESS_DOWN])
    {
        yourPlane.ctrlPitch(-PI/180/4);
    }
    else if(specialPressed[PRESS_UP])
    {
        yourPlane.ctrlPitch(PI/180/4);
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
        printf("You crashed an F-14. You just wasted \033[32m$38 million\033[0m in taxpayer money. Nice one.\n");
        glutDestroyWindow(winId);
        return;
    }
    if(position[PLANE_X]>OCEANSIZE || position[PLANE_Y]>OCEANSIZE || position[PLANE_X]<0 || position[PLANE_Y] < 0)
    {
        printf("Out of bounds: self-destructed. (%f, %f)\n", position[PLANE_X], position[PLANE_Y]);
        glutDestroyWindow(winId);
        return;
    }
    
    glutSwapBuffers();
}

void askey(unsigned char key, int x, int y)
{
    asciiPressed[key] ^= 0x1;
    
    switch(key)
    {
        case 'q':    // Quit the simulation
            printf("Ejected.\n");
            glutDestroyWindow(winId);
        break;
        default:
        break;
    }
}

void dirkey(int key, int x, int y)
{
        if(key>=0x64 && key < 0x74)
            specialPressed[key-0x64] ^= 0x1;
}

void reshape (int w, int h)
{
    glViewport (0, 0, (GLsizei) w, (GLsizei) h);
    glMatrixMode (GL_PROJECTION);
    glLoadIdentity ();
    glFrustum (-w/30, w/30, -h/30, h/30, 100, 2000);
    glMatrixMode (GL_MODELVIEW);
    glLoadIdentity();
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize (500, 500);
    glutInitWindowPosition (500, 100);
    winId = glutCreateWindow ("Fightsim and Jetsim");
    
    // Starter up!
    init();
    
    // Callbacks:
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    // glutMouseFunc(mouse); // Never used.
    glutKeyboardFunc(askey);
    glutKeyboardUpFunc(askey);
    glutSpecialFunc(dirkey);
    glutSpecialUpFunc(dirkey);
    
    // Timer:
    glutTimerFunc(100,timer,nFPS);
    startClock=clock();
    
    // Hajime!
    glutMainLoop();
    
    return 0;
}
