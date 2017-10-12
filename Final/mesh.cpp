#include "mesh.h"
#include<sstream>
#include <fstream>
#include <iosfwd>
#include <algorithm>
#include <gl/GL.h>
#include <math.h>
#include<fstream>
#include <algorithm>
#include <stdio.h>
#include "vary.h"

using namespace std;


My_Mesh::My_Mesh()
{
	
	vTranslation[0] = Theta[0] = 0;
	vTranslation[1] = Theta[1] = 0;
	vTranslation[2] = Theta[2] = 0;
	Theta[0] = 45;
}


My_Mesh::~My_Mesh()
{
	
}


void My_Mesh::load_obj(std::string obj_File)
{
	//���ڴ���Ӵ���ʵ�ֶԺ���UV�����obj�ļ��Ķ�ȡ
	std::vector<vec3> v;  //�洢����
	std::vector<vec3> vn;  //�洢����
	std::vector<vec3> vt;  //�洢����

	double szie = 0.003;    //table��wawa�Ŵ���
	if (!strcmp(obj_File.c_str(), "texture/whome.obj") || !strcmp(obj_File.c_str(), "texture/bhome.obj"))
		szie = 0.05;

	if (!strcmp(obj_File.c_str(), "texture/mewboady.obj")  || !strcmp(obj_File.c_str(), "texture/mewrightwing.obj")
		|| !strcmp(obj_File.c_str(), "texture/mewleftwing.obj"))
		szie = 0.2;

	float x, y, z;   //���ڴ洢��ȡ��.obj�ļ���������

	FILE * fin = fopen(obj_File.c_str(), "r");

	
	//std::ofstream fout;
	//fout.open("b.txt");

	char str[200];
	while (fscanf(fin, "%s", str) != EOF)//�Կո����Ϊ�ָ��ȡ
	{

		if (strcmp(str, "v") == 0)//��ȡ����
		{
			fscanf(fin, "%f %f %f\n", &x, &y, &z);
			v.push_back(vec3(x, y-min_height, z));
		}
		else if (strcmp(str, "vn") == 0)//��ȡ����
		{
			fscanf(fin, "%f %f %f\n", &x, &y, &z);
			vn.push_back(vec3(x, y, z));
		}
		else if (strcmp(str, "vt") == 0) //��ȡ����
		{
			fscanf(fin, "%f %f %f\n", &x, &y, &z);
			vt.push_back(vec3(x, y, z));
		}
		else if (strcmp(str, "f") == 0)//��ȡ��Ƭ�еĶ��㣬��������������
		{
			int vf[3], vtf[3], vnf[3];
			fscanf(fin, "%d/%d/%d %d/%d/%d %d/%d/%d\n",
				&vf[0], &vtf[0], &vnf[0],
				&vf[1], &vtf[1], &vnf[1],
				&vf[2], &vtf[2], &vnf[2]);

			//���붥������
			m_faces_.push_back(vf[0] - 1);
			m_faces_.push_back(vf[1] - 1);
			m_faces_.push_back(vf[2] - 1);

			for (int i = 0; i < 3; i++)
			{


				//fout << " " << a[i] << "/" << b[i] << "/" << c[i];
				//������˳����֯�ö��㣬����������
				m_vertices_.push_back(v[vf[i] - 1].x*szie);
				m_color_list_.push_back(160.0);
				//fout << "v " << m_vertices_.back() << '\n';
				m_vertices_.push_back(v[vf[i] - 1].y*szie);
				m_color_list_.push_back(200.0);
				//fout << "v " << m_vertices_.back() << '\n';
				m_vertices_.push_back(v[vf[i] - 1].z*szie);
				m_color_list_.push_back(50.0);
				//fout << "v " << m_vertices_.back() << '\n';

				m_vt_list_.push_back(vt[vtf[i] - 1].x);
				m_vt_list_.push_back(vt[vtf[i] - 1].y);

				m_normals_.push_back(vn[vnf[i] - 1].x);
				m_normals_.push_back(vn[vnf[i] - 1].y);
				m_normals_.push_back(vn[vnf[i] - 1].z);
			}
			
		}
	}
	fclose(fin);
};


void My_Mesh::normal_to_color(float nx, float ny, float nz, float& r, float& g, float& b)
{
	r = float(std::min(std::max(0.5 * (nx + 1.0), 0.0), 1.0));
	g = float(std::min(std::max(0.5 * (ny + 1.0), 0.0), 1.0));
	b = float(std::min(std::max(0.5 * (nz + 1.0), 0.0), 1.0));
};

const VtList&  My_Mesh::get_vts()
{
	return this->m_vt_list_;
};
void My_Mesh::clear_data()
{
	m_vertices_.clear();
	m_normals_.clear();
	m_faces_.clear();
	m_color_list_.clear();
	m_vt_list_.clear();
};
void My_Mesh::get_boundingbox(point3f& min_p, point3f& max_p) const
{
	min_p = this->m_min_box_;
	max_p = this->m_max_box_;
};
const STLVectorf&  My_Mesh::get_colors()
{
	return this->m_color_list_;
};
const VertexList& My_Mesh::get_vertices()
{
	return this->m_vertices_;
};
const NormalList& My_Mesh::get_normals()
{
	return this->m_normals_;
};
const FaceList&   My_Mesh::get_faces()
{
	return this->m_faces_;
};

int My_Mesh::num_faces()
{
	return this->m_faces_.size()/3;
};
int My_Mesh::num_vertices()
{
	return this->m_vertices_.size()/3;
};

const point3f& My_Mesh::get_center()
{
	return this->m_center_;
};

void  My_Mesh::generate_square(float size) {
	this->clear_data();
	vec4 vertices[4];
	vec3 n(0.0, 0.0, 1.0);
	//������ʼ����λ��
	vertices[0] = vec4((vec3(-1.0, -1.0, -1.0))*size,1.0);
	vertices[1] = vec4(( vec3(1.0, -1.0, -1.0))*size, 1.0);
	vertices[2] = vec4((vec3(1.0, 1.0, -1.0))*size, 1.0);
	vertices[3] = vec4((vec3(-1.0, 1.0, -1.0))*size, 1.0);

	//������Ƭ��������
	//������1
	m_faces_.push_back(0);
	m_faces_.push_back(1);
	m_faces_.push_back(3);
	//������2
	m_faces_.push_back(1);
	m_faces_.push_back(2);
	m_faces_.push_back(3);

	for (int i = 0; i < m_faces_.size(); i++)
	{
		int j = m_faces_[i];
		//������õĶ���
		m_vertices_.push_back(vertices[j].x);
		m_vertices_.push_back(vertices[j].y);
		m_vertices_.push_back(vertices[j].z);
		//������õķ�������ƽ��̶�����
		m_normals_.push_back(0);
		m_normals_.push_back(1);
		m_normals_.push_back(0);
		//������õ���ɫ
		m_color_list_.push_back(0);
		m_color_list_.push_back(0);
		m_color_list_.push_back(0);
	}

	//��������u,v����
	//0
	m_vt_list_.push_back(0.0);
	m_vt_list_.push_back(0.0);
	//1
	m_vt_list_.push_back(1.0);
	m_vt_list_.push_back(0.0);
	//3
	m_vt_list_.push_back(0.0);
	m_vt_list_.push_back(1.0);

	//1
	m_vt_list_.push_back(1.0);
	m_vt_list_.push_back(0.0);
	//2
	m_vt_list_.push_back(1.0);
	m_vt_list_.push_back(1.0);
	//3
	m_vt_list_.push_back(0.0);
	m_vt_list_.push_back(1.0);
}

void My_Mesh::generate_cylinder(int num_division, float height)
{
	this->clear_data();
	this->m_center_ = point3f(0, 0, 0);
	this->m_min_box_ = point3f(-1, -1, -height);
	this->m_max_box_ = point3f(1, 1, height);

	int num_samples = num_division;
	float z = -height;
	float pi = 3.14159265;
	float step = 1.0 * 360 / num_samples;
	float rr = pi / 180;
	//Բ����Z�����ϣ���cos��sin����x��y����
	for (int i = 0; i < num_samples; i++)
	{
		float r_r_r = i * step * rr;
		float x = cos(r_r_r);
		float y = sin(r_r_r);
		m_vertices_.push_back(x);
		m_vertices_.push_back(y);
		m_vertices_.push_back(z);

		m_normals_.push_back(x);
		m_normals_.push_back(y);
		m_normals_.push_back(0);
		//������������
		float r;
		float g;
		float b;
		My_Mesh::normal_to_color(x, y, z, r, g, b);
		//������÷�����������ɫ��ѧ�������Զ����Լ�����ɫ���ɷ�ʽ
		m_color_list_.push_back(r);
		m_color_list_.push_back(g);
		m_color_list_.push_back(b);
	}

	z = height;
	//Բ����Z�����ϣ���cos��sin����x��y���꣬
	for (int i = 0; i < num_samples; i++)
	{
		float r_r_r = i * step * rr;
		float x = cos(r_r_r);
		float y = sin(r_r_r);
		m_vertices_.push_back(x);
		m_vertices_.push_back(y);
		m_vertices_.push_back(z);

		m_normals_.push_back(x);
		m_normals_.push_back(y);
		m_normals_.push_back(0);
		//������������
		float r;
		float g;
		float b;
		My_Mesh::normal_to_color(x, y, z, r, g, b);
		m_color_list_.push_back(r);
		m_color_list_.push_back(g);
		m_color_list_.push_back(b);
		//������÷�����������ɫ��ѧ�������Զ����Լ�����ɫ���ɷ�ʽ
	}

	for (int i = 0; i < num_samples; i++)
	{
		m_faces_.push_back(i);
		m_faces_.push_back((i + 1) % num_samples);
		m_faces_.push_back((i + num_samples) % (num_samples)+num_samples);

		m_faces_.push_back((i + num_samples) % (num_samples)+num_samples);
		m_faces_.push_back((i + 1) % num_samples);
		m_faces_.push_back((i + num_samples + 1) % (num_samples)+num_samples);
		//����������Ƭ

		m_vt_list_.push_back(1.0 * i / num_samples);
		m_vt_list_.push_back(0.0);
		//��������
		m_vt_list_.push_back(1.0 * ((i + 1)) / num_samples);
		m_vt_list_.push_back(0.0);
		//��������
		m_vt_list_.push_back(1.0 * i / num_samples);
		m_vt_list_.push_back(1.0);
		//��������

		m_vt_list_.push_back(1.0 * i / num_samples);
		m_vt_list_.push_back(1.0);
		//��������
		m_vt_list_.push_back(1.0 * ((i + 1)) / num_samples);
		m_vt_list_.push_back(0.0);
		//��������
		m_vt_list_.push_back(1.0 * ((i + 1)) / num_samples);
		m_vt_list_.push_back(1.0);
		//��������
	}

};


void My_Mesh::set_texture_file(std::string s)
{
	this->texture_file_name = s;
};
std::string My_Mesh::get_texture_file()
{
	return this->texture_file_name;
};

void My_Mesh::set_translate(float x, float y, float z)
{
	vTranslation[0] = x;
	vTranslation[1] = y;
	vTranslation[2] = z;
};
void My_Mesh::get_translate(float& x, float& y, float& z)
{
	x = vTranslation[0];
	y = vTranslation[1];
	z = vTranslation[2];
};

void My_Mesh::set_theta(float x, float y, float z)
{
	Theta[0] = x;
	Theta[1] = y;
	Theta[2] = z;
};
void My_Mesh::get_theta(float& x, float& y, float& z)
{
	x = Theta[0];
	y = Theta[1];
	z = Theta[2];
};

void My_Mesh::set_theta_step(float x, float y, float z)
{
	Theta_step[0] = x;
	Theta_step[1] = y;
	Theta_step[2] = z;
};

void My_Mesh::add_theta_step()
{
	Theta[0] = Theta[0] + Theta_step[0];
	Theta[1] = Theta[1] + Theta_step[1];
	Theta[2] = Theta[2] + Theta_step[2];
};