#include "Mesh_Painter.h"
#include "FreeImage.h"
#include "vary.h"

Mesh_Painter::Mesh_Painter()
{

}


Mesh_Painter::~Mesh_Painter()
{

}


void Mesh_Painter::draw_meshes(int index, mat4 model_view,mat4 modelTrans, mat4 rotateMatrix)
{
	vec2 tkind;
	/*
	modelTrans *= Translate(evaPos[index].x, evaPos[index].y, evaPos[index].z);
	rotateMatrix *= RotateX(evaRot[index].x) * RotateY(evaRot[index].y)  *RotateZ(evaRot[index].z);
	*/
	if (index < 5 ) {//eva的层级模型结构
		model_view *= Translate(evaPos[index].x, evaPos[index].y, evaPos[index].z)* RotateX(evaRot[index].x)
			*RotateZ(evaRot[index].z)*RotateY(evaRot[index].y);//构成平移和旋转矩阵
	}
	else if(index < 12 && index >=5){//天空盒的
		float x, y, z, rx, ry, rz;
		this->m_my_meshes_[index]->get_translate(x, y, z);//获取平移和旋转信息
		this->m_my_meshes_[index]->get_theta(rx, ry, rz);
		model_view = Translate(x, y, z)*RotateX(rx)*RotateY(ry)*RotateZ(rz);//构成平移和旋转矩阵
	}
	else if (index >= 12 && index < 14) {//房子的
			model_view *= Translate(homePos[index-12].x, homePos[index - 12].y, homePos[index - 12].z)*
				RotateX(homeRot[index-12].x)*RotateZ(homeRot[index-12].z)*RotateY(homeRot[index-12].y);//构成平移和旋转矩阵	;
	}


	viewMatrix = LookAt(eye, at, up);
	//projMatrix = Ortho(-zfar, zfar, -zfar, zfar, -zfar, zfar);
	projMatrix = Perspective(70, 1.0, znear, zfar);
	modelViewMatrix = viewMatrix * model_view;


	modelViewProjMatrix = projMatrix * modelViewMatrix;

	glUseProgram(this->program_all[index]);//指定渲染器
	glBindVertexArray(this->vao_all[index]);//指定vao

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, this->textures_all[index]);//该语句必须，否则将只使用同一个纹理进行绘制
	glUniformMatrix4fv(ViewMatrix_all[index], 1, GL_TRUE, viewMatrix);
	glUniformMatrix4fv(modelViewMatrix_all[index], 1, GL_TRUE, modelViewMatrix);
	glUniformMatrix4fv(modelViewProjMatrix_all[index], 1, GL_TRUE, modelViewProjMatrix);
	glUniform3fv(lightPos_all[index], 1, lightPos);

	if (index < 5 || index>11)
		tkind = vec2(0.0, 1.0);
	else//标志现在画的是天空
		tkind = vec2(2.0, 1.0);
	glUniform2fv(kind_all[index], 1, tkind);
	glUniform2fv(Kind_all[index], 1, tkind);

	glDrawArrays(GL_TRIANGLES, 0, this->m_my_meshes_[index]->num_faces() * 3);

	//绘制阴影
	if (index < 5 && evaPos[0].z <12.8 &&  evaPos[0].z >-83) {//画天空的时候不需要画阴影
		if (drawShadow) {
			//画当前躯干的阴影
			float lx = lightPos.x;
			float ly = lightPos.y;
			float lz = lightPos.z;
			mat4 shadowProjMatrix = mat4(-ly, 0.0, 0.0, 0.0, lx, 0.0, lz, 1.0,
				0.0, 0.0, -ly, 0.0, 0.0, 0.0, 0.0, -ly);
			//更改当前绘制点的类型为阴影点
			tkind = vec2(1.0, 1.0);
			glUniform2fv(kind_all[index], 1, tkind);
			glUniform2fv(Kind_all[index], 1, tkind);
			//更新相应的矩阵
		//	modelViewProjMatrix =projMatrix * viewMatrix * modelTrans * shadowProjMatrix ;
			modelViewProjMatrix = projMatrix * viewMatrix * shadowProjMatrix * model_view;
			//modelViewProjMatrix = shadowProjMatrix * modelViewProjMatrix;
			glUniformMatrix4fv(modelViewProjMatrix_all[index], 1, GL_TRUE, modelViewProjMatrix);

			glDrawArrays(GL_TRIANGLES, 0, this->m_my_meshes_[index]->num_faces() * 3);
		}

	}
	//递归画子节点
	for (int i = 0; i < this->m_my_meshes_[index]->son.size(); i++) {
			draw_meshes(this->m_my_meshes_[index]->son[i], model_view, modelTrans);
			draw_meshes(this->m_my_meshes_[index]->son[i], model_view, modelTrans,rotateMatrix);
	}

}

void Mesh_Painter::draw_sky(int start, int end) {
	


}


void Mesh_Painter::draw_meshes()
{
	//////changeStart

	float x, y, z, rx, ry, rz;
	vec2 tkind;
	//draw body;
	
	this->m_my_meshes_[0]->set_translate(bodyPos.x, bodyPos.y, bodyPos.z);
	this->m_my_meshes_[0]->get_translate(x, y, z);//获取平移和旋转信息
	this->m_my_meshes_[0]->get_theta(rx, ry, rz);
	modelMatrix = Translate(x, y, z)*RotateX(rx)*RotateY(ry)*RotateZ(rz);//构成平移和旋转矩阵
	viewMatrix = LookAt(eye, at, up);
	projMatrix = Ortho(-zfar, zfar, -zfar, zfar, -zfar, zfar);
	modelViewMatrix = viewMatrix*modelMatrix;
	modelViewProjMatrix = projMatrix * modelViewMatrix;

	glUseProgram(this->program_all[0]);//指定渲染器
	glBindVertexArray(this->vao_all[0]);//指定vao

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, this->textures_all[0]);//该语句必须，否则将只使用同一个纹理进行绘制

	glUniformMatrix4fv(modelViewMatrix_all[0], 1, GL_TRUE, modelViewMatrix);
	glUniformMatrix4fv(modelViewProjMatrix_all[0], 1, GL_TRUE, modelViewProjMatrix);
	glUniform3fv(lightPos_all[0], 1, lightPos);

	tkind = vec2(0.0, 1.0);
	glUniform2fv(kind_all[0], 1, tkind);
	glUniform2fv(Kind_all[0], 1, tkind);

	glDrawArrays(GL_TRIANGLES, 0, this->m_my_meshes_[0]->num_faces() * 3);

	//画当前躯干的阴影
	float lx = lightPos.x;
	float ly = lightPos.y;
	float lz = lightPos.z;
	mat4 shadowProjMatrix = mat4(-ly, 0.0, 0.0, 0.0,
								lx, 0.0, lz, 1.0,
								0.0, 0.0, -ly, 0.0,
								0.0, 0.0, 0.0, -ly);
	//更改当前绘制点的类型为阴影点
	tkind = vec2(1.0, 1.0);
	glUniform2fv(kind_all[0], 1, tkind);
	glUniform2fv(Kind_all[0], 1, tkind);
	//更新相应的矩阵
	modelViewProjMatrix = modelViewProjMatrix * shadowProjMatrix;
	//modelViewProjMatrix = shadowProjMatrix * modelViewProjMatrix;
	glUniformMatrix4fv(modelViewProjMatrix_all[0], 1, GL_TRUE, modelViewProjMatrix);

	glDrawArrays(GL_TRIANGLES, 0, this->m_my_meshes_[0]->num_faces() * 3);



	/////changeEnd

};


void Mesh_Painter::update_texture()
{
	this->textures_all.clear();

	for (unsigned int i = 0; i < this->m_my_meshes_.size(); i++)
	{
		GLuint textures;
		//生成纹理对象
		glGenTextures(1, &textures);
		//调用FreeImage生成纹理,往纹理对象里面放东西
		load_texture_FreeImage(this->m_my_meshes_[i]->get_texture_file(), textures);

		//指定最大最小过滤方法，此两行代码必须添加，否则将无法显示纹理
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

		this->textures_all.push_back(textures);
	}

};


void Mesh_Painter::load_texture_FreeImage(std::string file_name, GLuint& m_texName)
{
	//1 获取图片格式
	FREE_IMAGE_FORMAT fifmt = FreeImage_GetFileType(file_name.c_str(), 0);

	//2 加载图片
	FIBITMAP *dib = FreeImage_Load(fifmt, file_name.c_str(), 0);

	//3 转化为rgb 24色;
	dib = FreeImage_ConvertTo24Bits(dib);

	//4 获取数据指针
	BYTE *pixels = (BYTE*)FreeImage_GetBits(dib);

	int width = FreeImage_GetWidth(dib);
	int height = FreeImage_GetHeight(dib);
	glBindTexture(GL_TEXTURE_2D, m_texName);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,width, height,0, GL_BGR_EXT, GL_UNSIGNED_BYTE, pixels);
	FreeImage_Unload(dib);

	
	/*
	glTexImage2D(
		GL_TEXTURE_2D, //! 指定是二维图片
		0, //! 指定为第一级别，纹理可以做mipmap,即lod,离近的就采用级别大的，远则使用较小的纹理
		GL_RGB, //! 纹理的使用的存储格式
		width, //! 宽度，老一点的显卡，不支持不规则的纹理，即宽度和高度不是2^n。
		height, //! 宽度，老一点的显卡，不支持不规则的纹理，即宽度和高度不是2^n。
		0, //! 是否的边
		GL_BGR_EXT, //! 数据的格式，bmp中，windows,操作系统中存储的数据是bgr格式
		GL_UNSIGNED_BYTE, //! 数据是8bit数据
		pixels
		);

	FreeImage_Unload(dib);*/
};


////////////////////////////////////////////////
//根据每个mesh中的内容，将其对应的数据写入vao、vbo中
void Mesh_Painter::update_vertex_buffer()
{
	this->vao_all.clear();
	this->buffer_all.clear();
	this->vPosition_all.clear();
	this->vColor_all.clear();
	this->vTexCoord_all.clear();
	this->vNormal_all.clear();

	for (unsigned int m_i = 0; m_i < this->m_my_meshes_.size(); m_i++)
	{
		int num_face = this->m_my_meshes_[m_i]->num_faces();
		//int num_vertex = this->m_my_meshes_[m_i]->num_vertices();

		const VertexList& vertex_list = this->m_my_meshes_[m_i]->get_vertices();
		const NormalList& normal_list = this->m_my_meshes_[m_i]->get_normals();
		const FaceList&  face_list = this->m_my_meshes_[m_i]->get_faces();
		const STLVectorf& color_list = this->m_my_meshes_[m_i]->get_colors();
		const VtList& vt_list = this->m_my_meshes_[m_i]->get_vts();

		//获得v,vt,vn,color的大小，以方便下文的地址偏移offset的计算
		int num_vertex = vertex_list.size();
		int num_normal = normal_list.size();
		int num_color = color_list.size();
		int num_vt = vt_list.size();

		// Create a vertex array object
		GLuint vao;
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		GLuint buffer;
		glGenBuffers(1, &buffer);
		glBindBuffer(GL_ARRAY_BUFFER, buffer);
		glBufferData(GL_ARRAY_BUFFER,sizeof(float)*num_vertex+ sizeof(float)*num_normal+ sizeof(float)*num_color
		+ sizeof(float)*num_vt,NULL, GL_STATIC_DRAW);
		//获得足够的空间存储坐标，颜色，法线以及纹理坐标等等,只是开辟空间，并没有放入数据


		//往VBO中放入数据
		GLintptr offset = 0;

		glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(float)*num_vertex, &vertex_list[0]);
		offset += sizeof(float)*num_vertex;

		glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(float)*num_normal, &normal_list[0]);
		offset += sizeof(float)*num_normal;

		glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(float)*num_color, &color_list[0]);
		offset += sizeof(float)*num_color;

		glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(float)*num_vt, &vt_list[0]);


		//重新绑定vao中各属性对应的缓存位置
		offset = 0;

		GLuint vPosition;
		vPosition = glGetAttribLocation(this->program_all[m_i], "vPosition");
		glEnableVertexAttribArray(vPosition);
		glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0,BUFFER_OFFSET(offset));
		offset += sizeof(float)*num_vertex;
		//指定vPosition在shader中使用时的位置

		GLuint vNormal;
		vNormal = glGetAttribLocation(this->program_all[m_i], "vNormal");
		glEnableVertexAttribArray(vNormal);
		glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0,BUFFER_OFFSET(offset));
		offset += sizeof(float)*num_normal;
		//指定vNormal在shader中使用时的位置

		GLuint vColor;
		vColor = glGetAttribLocation(this->program_all[m_i], "vColor");
		glEnableVertexAttribArray(vColor);
		glVertexAttribPointer(vColor, 3, GL_FLOAT, GL_FALSE, 0,BUFFER_OFFSET(offset));
		offset += sizeof(float)*num_color;
		//指定vColor在shader中使用时的位置

		GLuint vTexCoord;
		vTexCoord = glGetAttribLocation(this->program_all[m_i], "vTexCoord");
		glEnableVertexAttribArray(vTexCoord);
		glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, 0,
			BUFFER_OFFSET(offset));
		//指定vTexCoord在shader中使用时的位置

		this->vao_all.push_back(vao);
		this->buffer_all.push_back(buffer);
		this->vPosition_all.push_back(vPosition);
		this->vColor_all.push_back(vColor);
		this->vTexCoord_all.push_back(vTexCoord);
		this->vNormal_all.push_back(vNormal);

	}

	//VA0,着色器置0
	glBindVertexArray(0);
	glUseProgram(0);
};



//////////////////////////////////////////////////////
////初始化shader

void Mesh_Painter::init_shaders(std::string vs, std::string fs)
{
	//清除shader program列表,旋转列表，平移列表
	this->program_all.clear();
	this->theta_all.clear();
	this->trans_all.clear();
	this->modelViewProjMatrix_all.clear();
	this->modelViewProjMatrix_all.clear();
	this->modelViewMatrix_all.clear();
	this->kind_all.clear();
	this->Kind_all.clear();
	this->lightPos_all.clear();
	this->ViewMatrix_all.clear();
	memset(this->vis, 0, sizeof(this));


	for (unsigned int i = 0; i < this->m_my_meshes_.size(); i++)
	{
		this->m_my_meshes_[0]->son.clear();
		//对于每个mesh，为其创建一个shader
		GLuint program = InitShader(vs.c_str(), fs.c_str());
		glUseProgram(program);
		this->program_all.push_back(program);

		//获取每组shader下的变量对应的索引
		GLuint 	theta = glGetUniformLocation(program, "theta");
		GLuint  trans = glGetUniformLocation(program, "translation");

		//将该索引保存在theta_all中
		theta_all.push_back(theta);
		trans_all.push_back(trans);

		//////changeStart 获取shader中的变量索引
		GLuint  viewMatrixID_2= glGetUniformLocation(program, "viewMatrix");
		GLuint  modelViewProjMatrixID_2 = glGetUniformLocation(program, "modelViewProjMatrix");
		GLuint  modelViewMatrixID_2 = glGetUniformLocation(program, "modelViewMatrix");
		GLuint  kindID_2 = glGetUniformLocation(program, "kind");
		GLuint  KindID_2 = glGetUniformLocation(program, "Kind");
		GLuint  lightPosID_2 = glGetUniformLocation(program, "lightPos");
		
		ViewMatrix_all.push_back(viewMatrixID_2);
		modelViewProjMatrix_all.push_back(modelViewProjMatrixID_2);
		modelViewMatrix_all.push_back(modelViewMatrixID_2);
		kind_all.push_back(kindID_2);
		Kind_all.push_back(KindID_2);
		lightPos_all.push_back(lightPosID_2);
		//////changeEnd
	}

	

	//EVA创建父子关系
	this->m_my_meshes_[0]->son.push_back(1); //body rarm
	this->m_my_meshes_[0]->son.push_back(2); //body larm
	this->m_my_meshes_[0]->son.push_back(3); //body head
	this->m_my_meshes_[3]->son.push_back(4); //head face

	this->m_my_meshes_[12]->son.push_back(13); //房子之间的层级

	//VA0,着色器置0
	glBindVertexArray(0);
	glUseProgram(0);
};

///////////////////////////////////////////////
///增加mesh
void Mesh_Painter::add_mesh(My_Mesh* m)
{
	this->m_my_meshes_.push_back(m);
};



/////////////////////////////////////////////
///清除所有的mesh缓存数据
void Mesh_Painter::clear_mehs()
{
	this->m_my_meshes_.clear();

	this->textures_all.clear();
	this->program_all.clear();
	this->vao_all.clear();
	this->buffer_all.clear();
	this->vPosition_all.clear();
	this->vColor_all.clear();
	this->vTexCoord_all.clear();
	this->vNormal_all.clear();
	this->modelViewMatrix_all.clear();
	this->modelViewProjMatrix_all.clear();
	this->Kind_all.clear();
	this->kind_all.clear();
	this->lightPos_all.clear();
};
