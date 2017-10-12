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
	if (index < 5 ) {//eva�Ĳ㼶ģ�ͽṹ
		model_view *= Translate(evaPos[index].x, evaPos[index].y, evaPos[index].z)* RotateX(evaRot[index].x)
			*RotateZ(evaRot[index].z)*RotateY(evaRot[index].y);//����ƽ�ƺ���ת����
	}
	else if(index < 12 && index >=5){//��պе�
		float x, y, z, rx, ry, rz;
		this->m_my_meshes_[index]->get_translate(x, y, z);//��ȡƽ�ƺ���ת��Ϣ
		this->m_my_meshes_[index]->get_theta(rx, ry, rz);
		model_view = Translate(x, y, z)*RotateX(rx)*RotateY(ry)*RotateZ(rz);//����ƽ�ƺ���ת����
	}
	else if (index >= 12 && index < 14) {//���ӵ�
			model_view *= Translate(homePos[index-12].x, homePos[index - 12].y, homePos[index - 12].z)*
				RotateX(homeRot[index-12].x)*RotateZ(homeRot[index-12].z)*RotateY(homeRot[index-12].y);//����ƽ�ƺ���ת����	;
	}


	viewMatrix = LookAt(eye, at, up);
	//projMatrix = Ortho(-zfar, zfar, -zfar, zfar, -zfar, zfar);
	projMatrix = Perspective(70, 1.0, znear, zfar);
	modelViewMatrix = viewMatrix * model_view;


	modelViewProjMatrix = projMatrix * modelViewMatrix;

	glUseProgram(this->program_all[index]);//ָ����Ⱦ��
	glBindVertexArray(this->vao_all[index]);//ָ��vao

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, this->textures_all[index]);//�������룬����ֻʹ��ͬһ��������л���
	glUniformMatrix4fv(ViewMatrix_all[index], 1, GL_TRUE, viewMatrix);
	glUniformMatrix4fv(modelViewMatrix_all[index], 1, GL_TRUE, modelViewMatrix);
	glUniformMatrix4fv(modelViewProjMatrix_all[index], 1, GL_TRUE, modelViewProjMatrix);
	glUniform3fv(lightPos_all[index], 1, lightPos);

	if (index < 5 || index>11)
		tkind = vec2(0.0, 1.0);
	else//��־���ڻ��������
		tkind = vec2(2.0, 1.0);
	glUniform2fv(kind_all[index], 1, tkind);
	glUniform2fv(Kind_all[index], 1, tkind);

	glDrawArrays(GL_TRIANGLES, 0, this->m_my_meshes_[index]->num_faces() * 3);

	//������Ӱ
	if (index < 5 && evaPos[0].z <12.8 &&  evaPos[0].z >-83) {//����յ�ʱ����Ҫ����Ӱ
		if (drawShadow) {
			//����ǰ���ɵ���Ӱ
			float lx = lightPos.x;
			float ly = lightPos.y;
			float lz = lightPos.z;
			mat4 shadowProjMatrix = mat4(-ly, 0.0, 0.0, 0.0, lx, 0.0, lz, 1.0,
				0.0, 0.0, -ly, 0.0, 0.0, 0.0, 0.0, -ly);
			//���ĵ�ǰ���Ƶ������Ϊ��Ӱ��
			tkind = vec2(1.0, 1.0);
			glUniform2fv(kind_all[index], 1, tkind);
			glUniform2fv(Kind_all[index], 1, tkind);
			//������Ӧ�ľ���
		//	modelViewProjMatrix =projMatrix * viewMatrix * modelTrans * shadowProjMatrix ;
			modelViewProjMatrix = projMatrix * viewMatrix * shadowProjMatrix * model_view;
			//modelViewProjMatrix = shadowProjMatrix * modelViewProjMatrix;
			glUniformMatrix4fv(modelViewProjMatrix_all[index], 1, GL_TRUE, modelViewProjMatrix);

			glDrawArrays(GL_TRIANGLES, 0, this->m_my_meshes_[index]->num_faces() * 3);
		}

	}
	//�ݹ黭�ӽڵ�
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
	this->m_my_meshes_[0]->get_translate(x, y, z);//��ȡƽ�ƺ���ת��Ϣ
	this->m_my_meshes_[0]->get_theta(rx, ry, rz);
	modelMatrix = Translate(x, y, z)*RotateX(rx)*RotateY(ry)*RotateZ(rz);//����ƽ�ƺ���ת����
	viewMatrix = LookAt(eye, at, up);
	projMatrix = Ortho(-zfar, zfar, -zfar, zfar, -zfar, zfar);
	modelViewMatrix = viewMatrix*modelMatrix;
	modelViewProjMatrix = projMatrix * modelViewMatrix;

	glUseProgram(this->program_all[0]);//ָ����Ⱦ��
	glBindVertexArray(this->vao_all[0]);//ָ��vao

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, this->textures_all[0]);//�������룬����ֻʹ��ͬһ��������л���

	glUniformMatrix4fv(modelViewMatrix_all[0], 1, GL_TRUE, modelViewMatrix);
	glUniformMatrix4fv(modelViewProjMatrix_all[0], 1, GL_TRUE, modelViewProjMatrix);
	glUniform3fv(lightPos_all[0], 1, lightPos);

	tkind = vec2(0.0, 1.0);
	glUniform2fv(kind_all[0], 1, tkind);
	glUniform2fv(Kind_all[0], 1, tkind);

	glDrawArrays(GL_TRIANGLES, 0, this->m_my_meshes_[0]->num_faces() * 3);

	//����ǰ���ɵ���Ӱ
	float lx = lightPos.x;
	float ly = lightPos.y;
	float lz = lightPos.z;
	mat4 shadowProjMatrix = mat4(-ly, 0.0, 0.0, 0.0,
								lx, 0.0, lz, 1.0,
								0.0, 0.0, -ly, 0.0,
								0.0, 0.0, 0.0, -ly);
	//���ĵ�ǰ���Ƶ������Ϊ��Ӱ��
	tkind = vec2(1.0, 1.0);
	glUniform2fv(kind_all[0], 1, tkind);
	glUniform2fv(Kind_all[0], 1, tkind);
	//������Ӧ�ľ���
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
		//�����������
		glGenTextures(1, &textures);
		//����FreeImage��������,�������������Ŷ���
		load_texture_FreeImage(this->m_my_meshes_[i]->get_texture_file(), textures);

		//ָ�������С���˷����������д��������ӣ������޷���ʾ����
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

		this->textures_all.push_back(textures);
	}

};


void Mesh_Painter::load_texture_FreeImage(std::string file_name, GLuint& m_texName)
{
	//1 ��ȡͼƬ��ʽ
	FREE_IMAGE_FORMAT fifmt = FreeImage_GetFileType(file_name.c_str(), 0);

	//2 ����ͼƬ
	FIBITMAP *dib = FreeImage_Load(fifmt, file_name.c_str(), 0);

	//3 ת��Ϊrgb 24ɫ;
	dib = FreeImage_ConvertTo24Bits(dib);

	//4 ��ȡ����ָ��
	BYTE *pixels = (BYTE*)FreeImage_GetBits(dib);

	int width = FreeImage_GetWidth(dib);
	int height = FreeImage_GetHeight(dib);
	glBindTexture(GL_TEXTURE_2D, m_texName);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,width, height,0, GL_BGR_EXT, GL_UNSIGNED_BYTE, pixels);
	FreeImage_Unload(dib);

	
	/*
	glTexImage2D(
		GL_TEXTURE_2D, //! ָ���Ƕ�άͼƬ
		0, //! ָ��Ϊ��һ�������������mipmap,��lod,����ľͲ��ü����ģ�Զ��ʹ�ý�С������
		GL_RGB, //! �����ʹ�õĴ洢��ʽ
		width, //! ��ȣ���һ����Կ�����֧�ֲ��������������Ⱥ͸߶Ȳ���2^n��
		height, //! ��ȣ���һ����Կ�����֧�ֲ��������������Ⱥ͸߶Ȳ���2^n��
		0, //! �Ƿ�ı�
		GL_BGR_EXT, //! ���ݵĸ�ʽ��bmp�У�windows,����ϵͳ�д洢��������bgr��ʽ
		GL_UNSIGNED_BYTE, //! ������8bit����
		pixels
		);

	FreeImage_Unload(dib);*/
};


////////////////////////////////////////////////
//����ÿ��mesh�е����ݣ������Ӧ������д��vao��vbo��
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

		//���v,vt,vn,color�Ĵ�С���Է������ĵĵ�ַƫ��offset�ļ���
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
		//����㹻�Ŀռ�洢���꣬��ɫ�������Լ���������ȵ�,ֻ�ǿ��ٿռ䣬��û�з�������


		//��VBO�з�������
		GLintptr offset = 0;

		glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(float)*num_vertex, &vertex_list[0]);
		offset += sizeof(float)*num_vertex;

		glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(float)*num_normal, &normal_list[0]);
		offset += sizeof(float)*num_normal;

		glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(float)*num_color, &color_list[0]);
		offset += sizeof(float)*num_color;

		glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(float)*num_vt, &vt_list[0]);


		//���°�vao�и����Զ�Ӧ�Ļ���λ��
		offset = 0;

		GLuint vPosition;
		vPosition = glGetAttribLocation(this->program_all[m_i], "vPosition");
		glEnableVertexAttribArray(vPosition);
		glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0,BUFFER_OFFSET(offset));
		offset += sizeof(float)*num_vertex;
		//ָ��vPosition��shader��ʹ��ʱ��λ��

		GLuint vNormal;
		vNormal = glGetAttribLocation(this->program_all[m_i], "vNormal");
		glEnableVertexAttribArray(vNormal);
		glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0,BUFFER_OFFSET(offset));
		offset += sizeof(float)*num_normal;
		//ָ��vNormal��shader��ʹ��ʱ��λ��

		GLuint vColor;
		vColor = glGetAttribLocation(this->program_all[m_i], "vColor");
		glEnableVertexAttribArray(vColor);
		glVertexAttribPointer(vColor, 3, GL_FLOAT, GL_FALSE, 0,BUFFER_OFFSET(offset));
		offset += sizeof(float)*num_color;
		//ָ��vColor��shader��ʹ��ʱ��λ��

		GLuint vTexCoord;
		vTexCoord = glGetAttribLocation(this->program_all[m_i], "vTexCoord");
		glEnableVertexAttribArray(vTexCoord);
		glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, 0,
			BUFFER_OFFSET(offset));
		//ָ��vTexCoord��shader��ʹ��ʱ��λ��

		this->vao_all.push_back(vao);
		this->buffer_all.push_back(buffer);
		this->vPosition_all.push_back(vPosition);
		this->vColor_all.push_back(vColor);
		this->vTexCoord_all.push_back(vTexCoord);
		this->vNormal_all.push_back(vNormal);

	}

	//VA0,��ɫ����0
	glBindVertexArray(0);
	glUseProgram(0);
};



//////////////////////////////////////////////////////
////��ʼ��shader

void Mesh_Painter::init_shaders(std::string vs, std::string fs)
{
	//���shader program�б�,��ת�б�ƽ���б�
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
		//����ÿ��mesh��Ϊ�䴴��һ��shader
		GLuint program = InitShader(vs.c_str(), fs.c_str());
		glUseProgram(program);
		this->program_all.push_back(program);

		//��ȡÿ��shader�µı�����Ӧ������
		GLuint 	theta = glGetUniformLocation(program, "theta");
		GLuint  trans = glGetUniformLocation(program, "translation");

		//��������������theta_all��
		theta_all.push_back(theta);
		trans_all.push_back(trans);

		//////changeStart ��ȡshader�еı�������
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

	

	//EVA�������ӹ�ϵ
	this->m_my_meshes_[0]->son.push_back(1); //body rarm
	this->m_my_meshes_[0]->son.push_back(2); //body larm
	this->m_my_meshes_[0]->son.push_back(3); //body head
	this->m_my_meshes_[3]->son.push_back(4); //head face

	this->m_my_meshes_[12]->son.push_back(13); //����֮��Ĳ㼶

	//VA0,��ɫ����0
	glBindVertexArray(0);
	glUseProgram(0);
};

///////////////////////////////////////////////
///����mesh
void Mesh_Painter::add_mesh(My_Mesh* m)
{
	this->m_my_meshes_.push_back(m);
};



/////////////////////////////////////////////
///������е�mesh��������
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
