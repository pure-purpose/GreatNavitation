//#include "Ship.h"
//
///**
// * .多键输入，根据位置绘制
// * 
// * \param argc
// * \param argv
// * \return 
// */
//int main(int argc, char** argv)
//{
//	
//	GLsizei winWidth = 800, winHeight = 800;
//	glutInit(&argc, argv);
//	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
//	//glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);
//	//glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
//	glutInitWindowPosition(100, 100);
//	glutInitWindowSize(winWidth, winHeight);
//	glutCreateWindow("GreatNavitation");
//
//	init();
//	glutDisplayFunc(&displayShip);
//	glutReshapeFunc(&Reshape);
//	glutIdleFunc(&displayShip);
//	glutMouseFunc(&Mouse);
//	glutMotionFunc(&MouseMotion);
//	glutKeyboardFunc(&Keyboard);
//
//	glEnable(GL_DEPTH_TEST);
//	glutMainLoop();
//
//	return 0;
//}
#include "Model.h"
#include "Material.h"
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <cassert>
#include <GL/glut.h>
//#include <opencv2/opencv.hpp>
using namespace std;
//using namespace cv;

Model model_object;		// an instance of Model
Material material_object;
GLdouble perspective = 100.0;
double rot_x = 0.0, rot_y = 0;
double dx = 0.0, dy = 0.0;
double mouse_x = 0.0, mouse_y = 0.0;
int moving = 0;
void set_light()
{
    float light[] = { 16.0,16.0,0.0,0.0 };
    glGetLightfv(GL_LIGHT0, GL_POSITION, light);
}

void set_camera()
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(75, 1, 1, 21);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(10, 10, 10, 0, 0, 0, 0, 1, 0);
}

void reshape(int w, int h)
{
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(perspective, (GLfloat)w / (GLfloat)h, 10.0, 200.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0.0, 0.0, -30.0);
}


void initialize()
{
    glClearColor(0.0, 0.0, 0.0, 0.0);
    set_light();
    glShadeModel(GL_SMOOTH);
    //glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
}

void display(void)
{
    // 清除屏幕
    //glColor3f(0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // 设置光和视角
    set_light();
    set_camera();
    //处理旋转
    //glColor3f(1, 0, 0);
    glRotatef(rot_x, 1.0f, 0.0f, 0.0f);
    glRotatef(rot_y, 0.0f, 1.0f, 0.0f);
    GLint last_texture_ID;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture_ID);
    for (int i = 0; i < model_object.face_list.size(); i++)
    {
        vector<int>tri = model_object.face_list.at(i);
        string material_name = model_object.face_material.at(i);
        material now_material;
        for (int k = 0; k < material_object.material_list.size(); k++)
        {
            now_material = material_object.material_list.at(k);
            if (now_material.material_name == material_name)
            {
                break;
            }
        }
        if (now_material.texture_id != -1)
        {
            //glEnable(GL_TEXTURE_2D);
            //glBindTexture(GL_TEXTURE_2D, material_object.texture_list.at(now_material.texture_id));
            glBindTexture(GL_TEXTURE_2D, now_material.texture_id);
            glBegin(GL_QUADS);
            for (int j = 0; j < tri.size(); j += 3)
            {
                int value = tri.at(j + 1);
                glTexCoord2f(model_object.texture_list.at(2 * (value - 1)), model_object.texture_list.at(2 * (value - 1) + 1));
                value = tri.at(j + 2);
                //glNormal3f(model_object.normal_list.at(3 * (value - 1)) + dx, model_object.normal_list.at(3 * (value - 1) + 1) + dy, model_object.normal_list.at(3 * (value - 1) + 2));
                value = tri.at(j);
                glVertex3f(model_object.vertex_list.at(3 * (value - 1)) + dx, model_object.vertex_list.at(3 * (value - 1) + 1) + dy, model_object.vertex_list.at(3 * (value - 1) + 2));
            }
            glEnd();
            //glDisable(GL_TEXTURE_2D);
        }
        else {
            glMaterialfv(GL_FRONT, GL_SPECULAR, now_material.Ks);
            glMaterialfv(GL_FRONT, GL_DIFFUSE, now_material.Kd);
            glLightModelfv(GL_LIGHT_MODEL_AMBIENT, now_material.Ka);
            //glMaterialfv(GL_FRONT, GL_SHININESS, shininess);
            glBegin(GL_QUADS);
            for (int j = 0; j < tri.size(); j += 3)
            {
                int value = tri.at(j + 2);
                //glNormal3f(model_object.normal_list.at(3 * (value - 1)) + dx, model_object.normal_list.at(3 * (value - 1) + 1) + dy, model_object.normal_list.at(3 * (value - 1) + 2));
                value = tri.at(j);
                glVertex3f(model_object.vertex_list.at(3 * (value - 1)) + dx, model_object.vertex_list.at(3 * (value - 1) + 1) + dy, model_object.vertex_list.at(3 * (value - 1) + 2));
            }
            glEnd();
        }
    }
    glBindTexture(GL_TEXTURE_2D, last_texture_ID);
    glutSwapBuffers();
}

void keyboard_recall(unsigned char key, int x, int y)
{
    if (key == 'a')
    {
        dx -= 0.1;
    }
    else if (key == 'd')
    {
        dx += 0.1;
    }
    else if (key == 'w')
    {
        dy += 0.1;
    }
    else if (key == 's')
    {
        dy -= 0.1;
    }
    glutPostRedisplay();
}

void mouse_recall(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON)
    {
        if (state == GLUT_DOWN)
        {
            if (moving == 0)
            {
                moving = 1;
                mouse_x = x - rot_y;
                mouse_y = -y + rot_x;
            }
        }
        else if (state == GLUT_UP)
        {
            mouse_x = -1;
            mouse_y = -1;
            moving = 0;
        }
    }
}

void mouse_move_recall(int x, int y)
{
    if (moving == 1)
    {
        rot_y = x - mouse_x;
        rot_x = y + mouse_y;
        glutPostRedisplay();
    }
}

int main(int argc, char* argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(500, 500);
    glutCreateWindow("test");
    initialize();
    glutReshapeFunc(reshape);
    //glutIdleFunc(timeadd);
    glutKeyboardFunc(keyboard_recall);
    glutMouseFunc(mouse_recall);
    glutMotionFunc(mouse_move_recall);
    glutDisplayFunc(&display);
    glutIdleFunc(&display);

    //load the material
    material_object.loadFile("./model/Boat/boat.mtl");

    // load the model
    model_object.read("./model/Boat/boat.obj");
    
    glutMainLoop();

    return 0;
}

