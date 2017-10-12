#pragma once
#include "mesh.h"
class Mesh_Painter
{
public:
	Mesh_Painter();
	~Mesh_Painter();

	void draw_meshes();
	void update_vertex_buffer();
	void update_texture();

	void init_shaders(std::string vs, std::string fs);
	void add_mesh(My_Mesh*);
	void clear_mehs();
	void draw_meshes(int index,  mat4 model_view = mat4(1.0), mat4 modelTrans = mat4(1.0),mat4 rotateMatrix=mat4(1.0));
	void draw_sky(int start, int end);//绘制天空盒子(天空和的起末mesh坐标)
	

private:

	void load_texture_FreeImage(std::string file_name, GLuint& m_texName);
	std::vector<GLuint> textures_all;
	std::vector<GLuint> program_all;
	std::vector<GLuint> vao_all;
	std::vector<GLuint> buffer_all;
	std::vector<GLuint> vPosition_all;
	std::vector<GLuint> vColor_all;
	std::vector<GLuint> vTexCoord_all;
	std::vector<GLuint> vNormal_all;
	std::vector<GLuint>  theta_all;	//旋转
	std::vector<GLuint>  trans_all;	//平移   
	std::vector<GLuint>  ViewMatrix_all;
	std::vector<GLuint>  modelViewProjMatrix_all;
	std::vector<GLuint>  modelViewMatrix_all;
	std::vector<GLuint> kind_all;	//当前顶点类型不是阴影
	std::vector<GLuint> Kind_all;	//当前顶点类型是阴影
	std::vector<GLuint> lightPos_all;//光源位置设置
	std::vector<My_Mesh*> m_my_meshes_;
	int vis[15];
};

