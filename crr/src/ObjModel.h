#pragma once

#include <vector>
#include <string>
#include <gl/glut.h>

using namespace std;

struct Float3       //点的位置信息和法线信息的数据类型
{
	float Data[3];     //x,y,z
};

struct Float2      //点的纹理坐标数据类型
{
	float Data[2];   //u,v
};

struct Face          //面信息
{
	int vertex[3][3];       //构成面的三个点的索引信息
};


class objModel
{
public:
	objModel() {};
	objModel(const char* FileName);
	//void setTextureFromBmp(const char* texFileName);
	void loadTexture(const char* fileName);
	void Read(const char* FileName);
	void Draw();
private:
	vector<Float3> mLocation;   //位置信息
	vector<Float3> mNormal;     //法线信息
	vector<Float2> mTexcoord;   //纹理坐标信息
	vector<Face> mFace;         //面信息
	GLuint mTexture;
};