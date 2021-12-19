#include "Ship.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

GLdouble position[3] = { 10, 0, 1 };//直角坐标
GLdouble direction[3] = { 1, 0, 0 };//极坐标
GLdouble speed[3] = { 0, 0, 0 };//极坐标
u_char perspective = 0;//视角
const auto ob_dis = 10;
GLdouble god[3] = { position[0] - ob_dis * cos(direction[1] * DEGREE), ob_dis * position[1] - sin(direction[1] * DEGREE), ob_dis };//直角坐标
GLdouble other[3] = { position[0] - ob_dis * cos(direction[1] * DEGREE), ob_dis * position[1] - sin(direction[1] * DEGREE), 0 };
GLdouble player[3] = { 0, 0, 0 };
GLdouble eye[3] = { 10, 0, 180 };//极坐标


GLdouble MousePosition[2];
bool MouseDown = FALSE;

objModel Ship;

GLint lake;

GLuint loadTexture(const char* file_name)
{
	GLint width, height, nrChannels;
	GLuint texture_ID = 0;

	// 分配一个新的纹理编号

	unsigned char* data = stbi_load(file_name, &width, &height, &nrChannels, 0);
	glGenTextures(1, &texture_ID);

	// 绑定新的纹理，载入纹理并设置纹理参数
	// 在绑定前，先获得原来绑定的纹理编号，以便在最后进行恢复
	//GLint lastTextureID = last_texture_ID;
	//glGetIntegerv(GL_TEXTURE_BINDING_2D, &lastTextureID);
	glBindTexture(GL_TEXTURE_2D, texture_ID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
	/*glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);*/

	//	glTexImage2D(GL_TEXTURE_2D, 0, 3, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0,
		GL_RGB, GL_UNSIGNED_BYTE, data);
	glBindTexture(GL_TEXTURE_2D, texture_ID);  //恢复之前的纹理绑定
	return texture_ID;
}

void init(void)
{
	Ship.Read("./model/ship.obj");
	Ship.loadTexture("./model/.png");
	lake = loadTexture("./images/lake.jpeg");
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glEnable(GL_DEPTH_TEST | GL_AUTO_NORMAL);
}

void displayShip(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(75, 1, 1, 2000);
	//glViewport(0, 0, 1000, 1000);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//gluLookAt(eye[0] * cos(eye[1] * DEGREE) * cos(eye[2] * DEGREE), eye[0] * cos(eye[1] * DEGREE) * sin(eye[2] * DEGREE), eye[0] * sin(eye[1] * DEGREE), 0, 0, 0, 0, 0, upz);
	switch (perspective)
	{
	case 0:
		gluLookAt(god[0], god[1], god[2], position[0], position[1], position[2], 0, 0, 1);
		break;
	case 1:
		gluLookAt(other[0], other[1], other[2], position[0], position[1], position[2], 0, 0, 1);
		break;
	case 2:
		//gluLookAt(player[0], player[1], player[2], player[0] + );
		gluLookAt(god[0], god[1], god[2], position[0], position[1], position[2], 0, 0, 1);
		break;
	}
	

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, lake);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-100.0f, -100.0f, 0.0f);
	glTexCoord2f(0.0f, 5.0f); glVertex3f(-100.0f, 100.0f, 0.0f);
	glTexCoord2f(5.0f, 5.0f); glVertex3f(100.0f, 100.0f, 0.0f);
	glTexCoord2f(5.0f, 0.0f); glVertex3f(100.0f, -100.0f, 0.0f);
	glEnd();
	glDisable(GL_TEXTURE_2D);

	

	glPushMatrix();
	glTranslatef(position[0], position[1], position[2]);
	/*glRotatef(90, 0, 1, 0);
	glRotatef(90, 0, 0, 1);
	glRotatef(direction[1], 0, 1, 0);*/
	glRotatef(90, 1, 0, 0);
	glRotatef(-90, 0, 1, 0);
	glRotatef(direction[1], 0, 1, 0);
	Ship.Draw();
	glPopMatrix();
	
	glutSwapBuffers();
	glFlush();
}

void Reshape(GLint newWidth, GLint newHeight)
{
	glViewport(0, 0, newWidth, newHeight);

	glMatrixMode(GL_PROJECTION);
	glFrustum(-1, 1, -1, 1, 2, 20);

	glMatrixMode(GL_MODELVIEW);
	glClear(GL_COLOR_BUFFER_BIT);
}

void MouseMotion(GLint x, GLint y)
{
	if (eye[1] > 180)				//限制变化范围
		eye[1] -= 360;
	if (eye[1] < -180)
		eye[1] += 360;
	GLint width = glutGet(GLUT_WINDOW_WIDTH);
	GLint height = glutGet(GLUT_WINDOW_HEIGHT);
	if (MouseDown)
	{
		//glutPostRedisplay();
		GLdouble tmp = eye[1] + (MousePosition[1] - y) / height * 100;
		if ((tmp - 90) * (eye[1] - 90) < 0 || (tmp + 90) * (eye[1] + 90) < 0)	//处理转过极点时的情况
		{
			eye[2] = -eye[2];
			//upz = -upz;
			//std::cout << "reverse" << std::endl;
		}
		eye[1] += (MousePosition[1] - y) / height * 100;
		eye[2] += (MousePosition[0] - x) / width * 100;
		MousePosition[0] = x;
		MousePosition[1] = y;
		//std:: cout << eye[1] << std::endl;
	}
}

void Mouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		MouseDown = true;
		MousePosition[0] = x;
		MousePosition[1] = y;
	}
	else
		MouseDown = false;
}

void Keyboard(GLubyte key, GLint x, GLint y)
{
	/*if (eye[1] > 180)
		eye[1] -= 360;
	if (eye[1] < -180)
		eye[1] += 360;*/
	switch (key)
	{
	case 'w':
		position[0] += 0.5 * direction[0] * cos(direction[1] * DEGREE);
		position[1] += 0.5 * direction[0] * sin(direction[1] * DEGREE);
		break;
	case 's':
		position[0] -= 0.5 * direction[0] * cos(direction[1] * DEGREE);
		position[1] -= 0.5 * direction[0] * sin(direction[1] * DEGREE);
		break;
	case 'a':
		direction[1] += 0.5;
		break;
	case 'd':
		direction[1] -= 0.5;
		break;
	case 'q':
		if ((eye[1] < 90 && eye[1] + 0.97 > 90) || (eye[1] < -90 && eye[1] + 0.97 > -90))
		{
			eye[2] = -eye[2];
			//upz = -upz;
		}
		eye[1] += 0.97;
		break;
	case 'e':
		if ((eye[1] > -90 && eye[1] - 0.97 < -90) || (eye[1] > 90 && eye[1] - 0.97 < 90))
		{
			eye[2] = -eye[2];
			//upz = -upz;
		}

		eye[1] -= 0.97;
		break;
	case 'c':
		perspective < 3 ? perspective++: perspective = 0;
		break;//
	default:
		break;
	}
	other[0] = god[0] = position[0] - ob_dis * cos(direction[1] * DEGREE);
	other[1] = god[1] = position[1] - ob_dis * sin(direction[1] * DEGREE);
}

