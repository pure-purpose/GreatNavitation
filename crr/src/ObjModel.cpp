#include "ObjModel.h"
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include "stb_image.h"
#define _USE_MATH_DEFINES
#include <math.h>

constexpr auto DEGREE = M_PI / 180;

objModel::objModel(const char* FileName)
{
	Read(FileName);
}

void objModel::loadTexture(const char* fileName)
{
	GLint width, height, nrChannels;
	GLuint texture_ID = 0;

	// 分配一个新的纹理编号

	unsigned char* data = stbi_load(fileName, &width, &height, &nrChannels, 0);
	glGenTextures(1, &mTexture);

	// 绑定新的纹理，载入纹理并设置纹理参数
	// 在绑定前，先获得原来绑定的纹理编号，以便在最后进行恢复
	//GLint lastTextureID = last_texture_ID;
	//glGetIntegerv(GL_TEXTURE_BINDING_2D, &lastTextureID);
	glBindTexture(GL_TEXTURE_2D, mTexture);
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
	glBindTexture(GL_TEXTURE_2D, mTexture);  //恢复之前的纹理绑定
}

void objModel::Read(const char* FileName)
{
	ifstream Content(FileName);
	string temp;       //接受无关信息
	char szoneLine[256];        //读取一行的数据
	while (!Content.eof())
	{
		memset(szoneLine, 0, 256);        //  每次循环初始化数组szoneLine
		Content.getline(szoneLine, 256);
		if (strlen(szoneLine) > 0)
		{
			if (szoneLine[0] == 'v')     //顶点相关信息
			{
				stringstream ssOneLine(szoneLine);
				if (szoneLine[1] == 't')       //纹理
				{
					ssOneLine >> temp;
					Float2 tempTexcoord;
					ssOneLine >> tempTexcoord.Data[0] >> tempTexcoord.Data[1];
					mTexcoord.push_back(move(tempTexcoord));
				}
				else if (szoneLine[1] == 'n')            //顶点法线
				{
					ssOneLine >> temp;
					Float3 tempNormal;
					ssOneLine >> tempNormal.Data[0] >> tempNormal.Data[1] >> tempNormal.Data[2];
					mNormal.push_back(move(tempNormal));
				}
				else                          //顶点位置
				{
					ssOneLine >> temp;
					Float3 tempLocation;
					ssOneLine >> tempLocation.Data[0] >> tempLocation.Data[1] >> tempLocation.Data[2];
					tempLocation.Data[0] = tempLocation.Data[0] * 5;
					tempLocation.Data[1] = tempLocation.Data[1] * 5;
					tempLocation.Data[2] = tempLocation.Data[2] * 5;
					mLocation.push_back(move(tempLocation));
				}
			}
			else if (szoneLine[0] == 'f')          //面信息
			{
				stringstream ssOneLine(szoneLine);
				ssOneLine >> temp;
				string vertexStr;
				Face tempFace;
				for (int i = 0; i < 3; ++i)         //每个面三个点
				{
					ssOneLine >> vertexStr;
					size_t pos = vertexStr.find_first_of('/');       //根据/的位置分割点的位置、纹理和法线信息
					string locIndexStr = vertexStr.substr(0, pos);
					size_t pos2 = vertexStr.find_first_of('/', pos + 1); 
					string texIndexSrt = vertexStr.substr(pos + 1, pos2 - 1 - pos);
					string norIndexSrt = vertexStr.substr(pos2 + 1, vertexStr.length() - 1 - pos2);
					tempFace.vertex[i][0] = atoi(locIndexStr.c_str()) - 1;
					tempFace.vertex[i][1] = atoi(texIndexSrt.c_str()) - 1;
					tempFace.vertex[i][2] = atoi(norIndexSrt.c_str()) - 1;
				}
				mFace.push_back(move(tempFace));
			}
		}
	}
	Content.close();
}

void objModel::Draw()
{
	/*glBindTexture(GL_TEXTURE_2D, mTexture);
	glEnable(GL_TEXTURE_2D);*/
	//glPushMatrix();
	glBegin(GL_TRIANGLES);
	for (auto &face : mFace)         //循环绘制每个面
	{
		glNormal3fv(mNormal[face.vertex[0][2]].Data);
		glTexCoord2fv(mTexcoord[face.vertex[0][1]].Data);
		glVertex3fv(mLocation[face.vertex[0][0]].Data);

		glNormal3fv(mNormal[face.vertex[1][2]].Data);
		glTexCoord2fv(mTexcoord[face.vertex[1][1]].Data);
		glVertex3fv(mLocation[face.vertex[1][0]].Data);

		glNormal3fv(mNormal[face.vertex[2][2]].Data);
		glTexCoord2fv(mTexcoord[face.vertex[2][1]].Data);
		glVertex3fv(mLocation[face.vertex[2][0]].Data);
	}
	glEnd();
	//glPopMatrix();
	/*glDisable(GL_TEXTURE_2D);*/
}