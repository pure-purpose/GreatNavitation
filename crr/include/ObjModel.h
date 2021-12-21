#pragma once

#include <vector>
#include <string>
#include <gl/glut.h>

using namespace std;

struct Float3       //���λ����Ϣ�ͷ�����Ϣ����������
{
	float Data[3];     //x,y,z
};

struct Float2      //�������������������
{
	float Data[2];   //u,v
};

struct Face          //����Ϣ
{
	int vertex[3][3];       //��������������������Ϣ
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
	vector<Float3> mLocation;   //λ����Ϣ
	vector<Float3> mNormal;     //������Ϣ
	vector<Float2> mTexcoord;   //����������Ϣ
	vector<Face> mFace;         //����Ϣ
	GLuint mTexture;
};