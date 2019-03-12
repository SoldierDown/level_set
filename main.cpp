#include <iostream>
#include <GL/glut.h>
#include "LevelSet.h"
#define WIDTH 400
#define HEIGHT 400
#define OFFSET 200

using namespace std;

void timer(int i_);
double frame = 1000;
int time_ = 0;
float x = 0;
float y = 0;
LevelSet ls("square");
//LevelSet ls("circle");

int rows;
int cols;

void Screendump(char *tga_file, short W, short H) 
{
    FILE   *out = fopen(tga_file, "w");
    char   pixel_data[3*W*H];
    short  TGAhead[] = {0, 2, 0, 0, 0, 0, W, H, 24};
    
    glReadBuffer(GL_FRONT);
    glReadPixels(0, 0, W, H, GL_BGR, GL_UNSIGNED_BYTE, pixel_data);
    fwrite(TGAhead, sizeof(TGAhead), 1, out);
    fwrite(pixel_data, 3*W*H, 1, out);
    fclose(out); 
}
void init (void)
{
    // glClearColor (1.0, 1.0, 1.0, 0.0);
    glClear (GL_COLOR_BUFFER_BIT);
    glMatrixMode (GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-50.0, 50.0, -50.0, 50.0, -1.0, 1.0);
    glMatrixMode (GL_MODELVIEW);
    glLoadIdentity();
}


 void drawSquare()
{
    double density = 0;
    double radius = 100.0/(rows+1);
    int id = 0;
    glClear(GL_COLOR_BUFFER_BIT);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    for(int i = 0; i < rows; i++)
    {
        for(int j = 0; j < cols; j++)
        {
            id = ls.Index(i,j);
            density = abs(ls.nodes[id].phi) / ls.max;
            glColor3f(density, density, density);
            glBegin(GL_POLYGON);
                glVertex3f(-50.0 + i * radius, -50.0 + j * radius, 0.0);
                glVertex3f(-50.0 + (i+1) * radius, -50.0 + j * radius, 0.0);
                glVertex3f(-50.0 + (i+1) * radius, (j+1) * radius, 0.0);
                glVertex3f(-50.0+ i * radius, (j+1) * radius, 0.0);
            glEnd();
        }

    }
    glFlush();
  }

void display()
{   
    ls.Step();
    //char path[256];
    //sprintf(path, "Frame%05d.tga", time_);
	//Screendump(path,WIDTH,HEIGHT);
    drawSquare();   
    glFlush();
    glutSwapBuffers();
}

int main (int argc, char **argv)
{
    ls.Init();
    rows = ls.rows;
    cols = ls.cols;
    // ls.SignedDistanceFunction(0.0018);
    glutInit (&argc, argv);
    glutInitDisplayMode (GLUT_SINGLE | GLUT_RGBA);
    glutInitWindowSize (WIDTH, HEIGHT);
    glutCreateWindow ("Level Set");

    init();
    glutDisplayFunc (display);
    glutTimerFunc(1000.0/frame, timer, 0);
    glDisable (GL_TEXTURE_2D);

    glutMainLoop();
    
    return 0;
}

void timer(int i_)
{
    time_++;
    glutPostRedisplay();
    glutTimerFunc(1000.0 / frame, timer, 0);
}