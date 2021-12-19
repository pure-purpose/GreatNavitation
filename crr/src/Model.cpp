#include "Model.h"
//#include <opencv2/opencv.hpp>
#include <iostream>
#include <sstream>
using namespace std;
//using namespace cv;

void Model::read(const char* file_name)
{
	ifstream fileObject(file_name);
	string fileObjectLine;

	if (fileObject.is_open()) {
		while (!fileObject.eof()) {
			getline(fileObject, fileObjectLine);

			if (fileObjectLine.c_str()[0] == 'v' && fileObjectLine.c_str()[1] == ' ')
			{
				float x, y, z;
				fileObjectLine[0] = ' ';
				sscanf_s(fileObjectLine.c_str(), "%f %f %f ", &x, &y, &z);
				this->vertex_list.push_back(x);
				this->vertex_list.push_back(y);
				this->vertex_list.push_back(z);
				continue;
			}
			else if (fileObjectLine.c_str()[0] == 'v' && fileObjectLine.c_str()[1] == 'n')
			{
				float x, y, z;
				fileObjectLine[0] = ' ';
				fileObjectLine[1] = ' ';
				sscanf_s(fileObjectLine.c_str(), "%f %f %f ", &x, &y, &z);
				this->normal_list.push_back(x);
				this->normal_list.push_back(y);
				this->normal_list.push_back(z);
				continue;
			}
			else if (fileObjectLine.c_str()[0] == 'v' && fileObjectLine.c_str()[1] == 't')
			{
				float x, y, z;
				fileObjectLine[0] = ' ';
				fileObjectLine[1] = ' ';
				sscanf_s(fileObjectLine.c_str(), "%f %f", &x, &y);
				this->texture_list.push_back(x);
				this->texture_list.push_back(y);
				continue;
			}
		}

		fileObject.clear();
		fileObject.seekg(0, ios::beg);

		string now_material = "";
		while (!fileObject.eof()) {
			getline(fileObject, fileObjectLine);
			if (fileObjectLine.c_str()[0] == 'u')
			{
				istringstream iss(fileObjectLine);
				string temp;
				iss >> temp >> now_material;
			}
			if (fileObjectLine.c_str()[0] == 'f')
			{

				s_list.push_back(fileObjectLine);
				fileObjectLine[0] = ' ';

				istringstream iss(fileObjectLine);

				vector<int> tri;
				while (iss)
				//for(int i=0; i<3; i++)
				{
					int value;
					char x;
					iss >> value;
					if (iss.fail()) {/* cout << "qi" << endl;*/break; }
					tri.push_back(value);
					iss >> x >> value;
					tri.push_back(value);
					//iss >> x >> value;
					tri.push_back(0);
				}
				this->face_list.push_back(tri);
				this->face_material.push_back(now_material);
			}
		}
	}
}

void Model::draw()
{
	//glBegin(GL_TRIANGLES);
	//for (auto& face : face_list)         //循环绘制每个面
	//{
	//	glNormal3fv(mNormal[face.vertex[0][2]].Data);
	//	glTexCoord2fv(mTexcoord[face.vertex[0][1]].Data);
	//	glVertex3fv(mLocation[face.vertex[0][0]].Data);

	//	glNormal3fv(mNormal[face.vertex[1][2]].Data);
	//	glTexCoord2fv(mTexcoord[face.vertex[1][1]].Data);
	//	glVertex3fv(mLocation[face.vertex[1][0]].Data);

	//	glNormal3fv(mNormal[face.vertex[2][2]].Data);
	//	glTexCoord2fv(mTexcoord[face.vertex[2][1]].Data);
	//	glVertex3fv(mLocation[face.vertex[2][0]].Data);
	//}
	//glEnd();
}
