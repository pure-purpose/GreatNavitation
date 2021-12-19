#pragma once
/* Class Model */
#include <vector>
#include <string>
#include <fstream>
#include <GL/glut.h>
#include "Material.h"

/* Class Model */

class Model
{
private:
	Material material;
public:
	void read(const char* file_name);		// load model coords from file
	void draw();

	 //vectors
	std::vector<float> vertex_list;
	std::vector<float> normal_list;
	std::vector<float> texture_list;
	std::vector<std::string> s_list;
	std::vector<std::vector<int>> face_list;
	std::vector<std::string> face_material;

};


