#include "Material.h"
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_resize.h"
#include <sstream>
#include <iostream>
//#include <opencv2/opencv.hpp>
//using namespace cv;
using namespace std;

GLuint Material::load_texture(const char* fileName)
{

	GLint width, height, nrChannels;
	GLubyte* ipixels = 0, *pixels=nullptr;
	GLint last_texture_ID;
	GLuint texture_ID = 0;

	pixels = stbi_load(fileName, &width, &height, &nrChannels, 0);
	/*pixels = (GLubyte*)malloc(width * height * nrChannels);
	stbir_resize(ipixels, width, height, 0, pixels, 1024, 1024, 0, STBIR_TYPE_UINT8, nrChannels, STBIR_ALPHA_CHANNEL_NONE, 0,
		STBIR_EDGE_CLAMP, STBIR_EDGE_CLAMP,
		STBIR_FILTER_BOX, STBIR_FILTER_BOX,
		STBIR_COLORSPACE_SRGB, nullptr);*/
	glGenTextures(1, &texture_ID);
	if (texture_ID == 0)
	{
		free(pixels);
		return 0;
	}

	glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture_ID);
	glBindTexture(GL_TEXTURE_2D, texture_ID);
	/*glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0,
		GL_BGR_EXT, GL_UNSIGNED_BYTE, pixels);*/
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0,
		GL_RGB, GL_UNSIGNED_BYTE, pixels);
	glBindTexture(GL_TEXTURE_2D, last_texture_ID);

	/*free(pixels);*/
	this->texture_list.push_back(texture_ID);
	return texture_ID;
}

void Material::loadFile(const char* file_name)
{

	ifstream fileObject(file_name);
	string fileObjectLine;
	
	if (fileObject.is_open()) {
		while (!fileObject.eof()) {
			getline(fileObject, fileObjectLine);
			if (fileObjectLine.c_str()[0] == 'n' && fileObjectLine.c_str()[1] == 'e')
			{
				istringstream iss(fileObjectLine);
				string temp, name;
				iss >> temp >> name;
				material now_material;
				now_material.material_name = name;
				while (fileObjectLine.c_str()[0] != '\n' && fileObjectLine.c_str()[0] != '\r')
				{
					getline(fileObject, fileObjectLine);
					if (fileObjectLine.size() < 4)
					{
						break;
					}
					else if (fileObjectLine.c_str()[0] == 'N' && fileObjectLine.c_str()[1] == 's')
					{
						istringstream iss(fileObjectLine);
						string temp;
						float Ns;
						iss >> temp >> Ns;
						now_material.Ns = Ns;
					}
					else if (fileObjectLine.c_str()[0] == 'd' && fileObjectLine.c_str()[1] == ' ')
					{
						istringstream iss(fileObjectLine);
						string temp;
						float d;
						iss >> temp >> d;
						now_material.d = d;
					}
					else if (fileObjectLine.c_str()[0] == 'N' && fileObjectLine.c_str()[1] == 'i')
					{
						istringstream iss(fileObjectLine);
						string temp;
						float Ni;
						iss >> temp >> Ni;
						now_material.Ni = Ni;
					}
					else if (fileObjectLine.c_str()[0] == 'i' && fileObjectLine.c_str()[1] == 'l')
					{
						istringstream iss(fileObjectLine);
						string temp;
						float illum;
						iss >> temp >> illum;
						now_material.illum = illum;
					}
					else if (fileObjectLine.c_str()[0] == 'K' && fileObjectLine.c_str()[1] == 'a')
					{
						istringstream iss(fileObjectLine);
						string temp;
						float a, b, c;
						iss >> temp >> a >> b >> c;
						now_material.Ka[0] = a;
						now_material.Ka[1] = b;
						now_material.Ka[2] = c;
					}
					else if (fileObjectLine.c_str()[0] == 'K' && fileObjectLine.c_str()[1] == 'd')
					{
						istringstream iss(fileObjectLine);
						string temp;
						float a, b, c;
						iss >> temp >> a >> b >> c;
						now_material.Kd[0] = a;
						now_material.Kd[1] = b;
						now_material.Kd[2] = c;
					}
					else if (fileObjectLine.c_str()[0] == 'K' && fileObjectLine.c_str()[1] == 's')
					{
						istringstream iss(fileObjectLine);
						string temp;
						float a, b, c;
						iss >> temp >> a >> b >> c;
						now_material.Ks[0] = a;
						now_material.Ks[1] = b;
						now_material.Ks[2] = c;
					}
					else if (fileObjectLine.c_str()[0] == 'm' && fileObjectLine.c_str()[1] == 'a')
					{
						istringstream iss(fileObjectLine);
						string temp, filename;
						iss >> temp >> filename;
						char file[50] = "./model/Boat";
						strcat_s(file, filename.c_str());
						now_material.texture_id = this->load_texture(file);
					}
				}
				this->material_list.push_back(now_material);
			}
		}
	}
}
