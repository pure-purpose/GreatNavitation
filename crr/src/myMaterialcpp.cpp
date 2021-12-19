//#include "Material.h"
//#include "stb_image.h"
//#include <sstream>
//#include <iostream>
//using namespace std;
//
//void Material::load_texture(const char* fileName)
//{
//
//	GLint width, height, nrChannels;
//	GLubyte* pixels = 0;
//	GLint last_texture_ID;
//	GLuint texture_ID = 0;
//
//	pixels = stbi_load(fileName, &width, &height, &nrChannels, 0);
//
//	glGenTextures(1, &texture_ID);
//	if (texture_ID == 0)
//	{
//		free(pixels);
//		return;
//	}
//
//	glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture_ID);
//	glBindTexture(GL_TEXTURE_2D, texture_ID);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0,
//		GL_BGR_EXT, GL_UNSIGNED_BYTE, pixels);
//	glBindTexture(GL_TEXTURE_2D, last_texture_ID);
//
//	free(pixels);
//	this->texture_list.push_back(texture_ID);
//}
//
//void Material::read(const char* fileName)
//{
//	ifstream content(fileName);
//	string temp;
//	char line[256];
//	if (content.is_open())
//	{
//		while (!content.eof())
//		{
//			content.getline(line, 256);
//			stringstream sline(line);
//			string item;
//			sline >> item;
//			if (!item.compare("newmtl"))
//			{
//				string name;
//				sline >> name;
//				material ma;
//				ma.material_name = name;
//				while (line[0] != '\n')
//				{
//					content.getline(line, 256);
//
//				}
//			}
//
//		}
//	}
//}
