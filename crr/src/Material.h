#pragma once
/* Class Model */
#include <vector>
#include <string>
#include <fstream>
#include <GL/glut.h>

/* Class Materail */

struct material {
	std::string material_name = "";
	int texture_id = -1;
	float Ns = 1024;
	//Ns exponent 指定材质的反射指数，定义了反射高光度.exponent是反射指数值，该值越高则高光越密集，一般取值范围在0~1000。
	float d = 1;
	//取值为1.0表示完全不透明，取值为0.0时表示完全透明
	float Ni = 0.001;
	//Ni ptical density 指定材质表面的光密度，即折射值。可在0.001到10之间进行取值。若取值为1.0，光在通过物体的时候不发生弯曲。玻璃的折射率为1.5。
	int illum = 2;
	// Highlight on
	GLfloat Ka[3] = { 1.0, 0.0, 0.0 };
	//环境反射
	GLfloat Kd[3] = { 1.0, 0.0, 0.0 };
	//漫反射
	GLfloat Ks[3] = { 1.0, 0.0, 0.0 };
	//镜面反射
};

class Material
{
public:
	void loadFile(const char* file_name);
	void load_texture(const char* file_name);

	//vectors
	std::vector<material> material_list;
	std::vector<GLuint> texture_list;

};

