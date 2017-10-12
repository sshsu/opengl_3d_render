#include "Angel.h"
#include "mesh.h"
#include "FreeImage.h"
#include "Mesh_Painter.h"
#include "vary.h"
#include "time.h"
#include "windows.h"
#include "math.h"
#include "mmsystem.h"
#include "tchar.h"
#include "digitalv.h"
#include <thread>
#include <iostream>
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "FreeImage.lib")
#pragma comment(lib,"winmm.lib")

#define  GLUT_WHEEL_UP 3           //������ֲ���  
#define  GLUT_WHEEL_DOWN 4  

using namespace std;


//mesh
Mesh_Painter*	mp_;//mesh����
vector<My_Mesh*> my_meshs;//mesh����


////////////////////////////    initial �� display()    /////////////////////////////////////////////////

 //��ʼ������
void Init_board() {

	//��ֱ�ߣ�һ������
	for (int i = 0; i < xlen; i++) {
		//����ĵ�
		gridpoints[2 * i] = vec4(block_width*i, 0, 0, 1.0);
		//����ĵ�
		gridpoints[2 * i + 1] = vec4(block_width*i, 0, (zlen - 1)*block_width, 1.0);

	}

	//���̵ĺ����ߣ�һ������
	int offset = xlen * 2;
	for (int i = 0; i < zlen; i++) {
		//��ߵĵ�
		gridpoints[2 * i + offset] = vec4(0, 0, i*block_width, 1.0);
		//�ұߵĵ�
		gridpoints[2 * i + 1 + offset] = vec4((xlen - 1)*block_width, 0, i *block_width, 1.0);
	}

	//�������߻��Ƴɰ�ɫ
	for (int i = 0; i < grid_points_num; i++)
		gridcolours[i] = vec4(34/255.0,158/255.0,255/255.0,1.0);

	//�������ϵĸ���
	for (int i = 0; i < board_points_num; i++)
		boardcolours[i] = white;

	int oset = 1;
	//�����̸��Ӹ�ֵ
	for (int i = 0; i < zlen - 1; i++) {
		for (int j = 0; j < xlen - 1; j++) {

			vec4 p1 = vec4(j     * block_width, -0.005, i     * block_width, 1.0);
			vec4 p2 = vec4((j + 1) * block_width, -0.005, i     * block_width, 1.0);
			vec4 p3 = vec4(j     * block_width, -0.005, (i + 1) * block_width, 1.0);
			vec4 p4 = vec4((j + 1) * block_width, -0.005, (i + 1) * block_width, 1.0);

			boardpoints[6 * ((xlen - oset) * i + j)] = p1;
			boardpoints[6 * ((xlen - oset) * i + j) + 1] = p2;
			boardpoints[6 * ((xlen - oset) * i + j) + 2] = p3;
			boardpoints[6 * ((xlen - oset) * i + j) + 3] = p2;
			boardpoints[6 * ((xlen - oset) * i + j) + 4] = p3;
			boardpoints[6 * ((xlen - oset) * i + j) + 5] = p4;
		}
	}

	// ������ɫ��
	programID = InitShader("vshader.glsl", "fshader.glsl");
	glUseProgram(programID);


	//��ȡposition color���Ա��
	vPositionID = glGetAttribLocation(programID, "vPosition");
	vColorID = glGetAttribLocation(programID, "vColor");

	//����2��VAO vao[0]���̶˵㣬vao[1]���̸���
	glGenVertexArrays(2, &vaoIDs[0]);

	// ���̸񶥵�,����VAO[0]
	glBindVertexArray(vaoIDs[0]);
	glGenBuffers(2, vboIDs);

	// ���̸�˵��λ��
	//��VBO��ŷ���ȫ�ֱ���,GL_ARRAY_BUFFER=vboIDs[0]
	glBindBuffer(GL_ARRAY_BUFFER, vboIDs[0]);
	//������
	glBufferData(GL_ARRAY_BUFFER, grid_points_num*sizeof(vec4), gridpoints, GL_STATIC_DRAW);
	//vPosition���Խ�ʹ��vboids[0]�е�ĳ����Ϣ
	glVertexAttribPointer(vPositionID, 4, GL_FLOAT, GL_FALSE, 0, 0);
	//��vpostion�������ó�enable
	glEnableVertexAttribArray(vPositionID);

	// ���̸�˵���ɫ
	glBindBuffer(GL_ARRAY_BUFFER, vboIDs[1]);
	glBufferData(GL_ARRAY_BUFFER, grid_points_num*sizeof(vec4), gridcolours, GL_DYNAMIC_DRAW);
	//ָ��vcolor�ĸ�������(��С�����ͣ��Ƿ��һ�������ݼ������vbo�е�ƫ��)
	glVertexAttribPointer(vColorID, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vColorID);

	// ���̸�ÿ������
	glBindVertexArray(vaoIDs[1]);
	glGenBuffers(2, &vboIDs[2]);

	// ���̸�ÿ�����Ӷ���λ��
	glBindBuffer(GL_ARRAY_BUFFER, vboIDs[2]);
	glBufferData(GL_ARRAY_BUFFER, board_points_num*sizeof(vec4), boardpoints, GL_STATIC_DRAW);
	glVertexAttribPointer(vPositionID, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vPositionID);

	// ���̸�ÿ�����Ӷ�����ɫ
	glBindBuffer(GL_ARRAY_BUFFER, vboIDs[3]);
	glBufferData(GL_ARRAY_BUFFER, board_points_num*sizeof(vec4), boardcolours, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(vColorID, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vColorID);

	//���shader�Ͷ���
	glBindVertexArray(0);
	glUseProgram(0);

}

 //��ʼ��eva
void Init_eva() {
	//eva����0.04
	bodyPos.y = evaheight;
	//Init_eva
	mp_ = new Mesh_Painter;
	My_Mesh* my_mesh0 = new My_Mesh;
	my_mesh0->load_obj("texture/body.obj");
	my_mesh0->set_texture_file("texture/grey.png");
	my_meshs.push_back(my_mesh0);
	mp_->add_mesh(my_mesh0);

	My_Mesh* my_mesh1 = new My_Mesh;
	my_mesh1->load_obj("texture/rArm.obj");
	my_mesh1->set_texture_file("texture/grey.png");
	my_meshs.push_back(my_mesh1);
	mp_->add_mesh(my_mesh1);

	My_Mesh* my_mesh2 = new My_Mesh;
	my_mesh2->load_obj("texture/lArm.obj");
	my_mesh2->set_texture_file("texture/grey.png");
	my_meshs.push_back(my_mesh2);
	mp_->add_mesh(my_mesh2);

	My_Mesh* my_mesh3 = new My_Mesh;
	my_mesh3->load_obj("texture/head.obj");
	my_mesh3->set_texture_file("texture/grey.png");
	my_meshs.push_back(my_mesh3);
	mp_->add_mesh(my_mesh3);

	My_Mesh* my_mesh4 = new My_Mesh;
	my_mesh4->load_obj("texture/face.obj");
	my_mesh4->set_texture_file("texture/smile.png");
	my_meshs.push_back(my_mesh4);
	mp_->add_mesh(my_mesh4);

}

///��ʼ����պ���
void Init_sky() {
	//negz
	float skysize = 200;
	My_Mesh* my_mesh5 = new My_Mesh;
	my_mesh5->generate_square(skysize);
	my_mesh5->set_texture_file("texture/reef_negz.bmp");
	my_mesh5->set_theta(0, 0, 0);
	my_meshs.push_back(my_mesh5);
	mp_->add_mesh(my_mesh5);

	//posz
	My_Mesh* my_mesh6 = new My_Mesh;
	my_mesh6->generate_square(skysize);
	my_mesh6->set_texture_file("texture/reef_posz.bmp");
	my_mesh6->set_theta(0, 180, 0);
	my_meshs.push_back(my_mesh6);
	mp_->add_mesh(my_mesh6);

	//posx
	My_Mesh* my_mesh7 = new My_Mesh;
	my_mesh7->generate_square(skysize);
//	my_mesh7->set_texture_file("texture/skybox_posx.bmp");
	my_mesh7->set_texture_file("texture/reef_posx.bmp");
	my_mesh7->set_theta(0, 90, 0);
	my_meshs.push_back(my_mesh7);
	mp_->add_mesh(my_mesh7);


	//negx
	My_Mesh* my_mesh8 = new My_Mesh;
	my_mesh8->generate_square(skysize);
	my_mesh8->set_texture_file("texture/reef_negx.bmp");
	my_mesh8->set_theta(0, 270, 0);
	my_meshs.push_back(my_mesh8);
	mp_->add_mesh(my_mesh8);
	//posy
	My_Mesh* my_mesh9 = new My_Mesh;
	my_mesh9->generate_square(skysize);
	my_mesh9->set_texture_file("texture/reef_posy.bmp");
	my_mesh9->set_theta(90, 0, 0);
	my_meshs.push_back(my_mesh9);
	mp_->add_mesh(my_mesh9);
	//negy
	My_Mesh* my_mesh10 = new My_Mesh;
	my_mesh10->generate_square(skysize);
	my_mesh10->set_texture_file("texture/reef_negy.bmp");
	my_mesh10->set_theta(-90, 0, 0);
	my_meshs.push_back(my_mesh10);
	mp_->add_mesh(my_mesh10);



}
//��ʼ���ذ�
void Init_floor() {
	float floorsize = 4.0;
	My_Mesh* my_mesh11 = new My_Mesh;
	my_mesh11->generate_square(floorsize);
	//my_mesh11->set_texture_file("texture/wali2.png");
	my_mesh11->set_texture_file("texture/merryWife.png");
	my_mesh11->set_translate(4.0, floorsize+0.01, 4.0);
	my_mesh11->set_theta(-90, 0, 0);
	my_meshs.push_back(my_mesh11);
	mp_->add_mesh(my_mesh11);

}
//��ʼ������
void Init_home() {
	My_Mesh* my_mesh12 = new My_Mesh;
	my_mesh12->load_obj("texture/bhome.obj");
	my_mesh12->set_texture_file("texture/blue.png");
	my_meshs.push_back(my_mesh12);
	mp_->add_mesh(my_mesh12);

	My_Mesh* my_mesh13 = new My_Mesh;
	my_mesh13->load_obj("texture/whome.obj");
	my_mesh13->set_texture_file("texture/grey.png");
	my_meshs.push_back(my_mesh13);
	mp_->add_mesh(my_mesh13);
}

//��ʼ��ÿ��mesh��Ӧ��shader
void init_meshShader() {
	mp_->init_shaders("v_texture.glsl", "f_texture.glsl");
	mp_->update_vertex_buffer();
	mp_->update_texture();
}

//�ܵĳ�ʼ��
void init()
{
	//��ʼ��eva��at�������
	VectorEvaToAt = normalize(vec3(at.x - bodyPos.x, at.y - bodyPos.y, at.z - bodyPos.z));
	glEnable(GL_DEPTH_TEST);//���ֵ�����˱仯�������ػ�

	Init_board();//�ȳ�ʼ�����̵ذ�
	Init_eva();//��ʼ��������Eva
	Init_sky();//��ʼ����պ�
	Init_floor();//��ʼ���ذ�
	Init_home();//��ʼ������
	
	init_meshShader();
	glBindVertexArray(0);
	glClearColor(0, 0, 0, 0);
}


//���ذ�����
void draw_board() {

	//ʹ��������ɫ��
	glUseProgram(programID);

	//������
	viewMatrix = LookAt(eye, at, up);
	projMatrix = Perspective(70, 1.0, znear, zfar);
	//projMatrix = Ortho(-20, 20, -20, 20, -20, 20);
	modelViewProjMatrix = projMatrix * viewMatrix;

	//��ȡ�任�����ID,����ֵ
	modelViewProjMatrixID = glGetUniformLocation(programID, "modelViewProjMatrix");
	glUniformMatrix4fv(modelViewProjMatrixID, 1, GL_TRUE, &modelViewProjMatrix[0][0]);

	//�Ȼ����ӣ��ٻ���
	glBindVertexArray(vaoIDs[1]);
	glDrawArrays(GL_TRIANGLES, 0, board_points_num); // �������̸� 


	glBindVertexArray(vaoIDs[0]);
	glDrawArrays(GL_LINES, 0, grid_points_num);		 // �������̸����

													 //VA0,��ɫ����0
	glBindVertexArray(0);
	glUseProgram(0);

}

//��mesh����Ķ���
void draw_mesh() {
	//����EVA
	
	mp_->draw_meshes(0);//����eva��mesh

	for (int i = 5; i<11; i++)//������պ�
		mp_->draw_meshes(i);

	if (finishGame) 
		mp_->draw_meshes(11);//������ͼ�ذ�
	
	mp_->draw_meshes(12);//���Ʒ���
	

	//VA0,��ɫ����0
	glBindVertexArray(0);
	glUseProgram(0);
}

//�ܵ�display
void display()
{
	//�����ǰ֡����ɫ�����ݻ���
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	draw_board();//��������
	draw_mesh();//����mesh�еĶ���

	glutSwapBuffers();
}


////////////////////////////    ��Ϸϵͳ    //////////////////////////////////////////////////////////////

//��ȡeva�������ϵ�λ��
vec2 getEvaBoardPos() {
	float x, z;
	x = floor(bodyPos.x / block_width);
	z = floor(bodyPos.z / block_width);
	return vec2(z, x);
}

//��������ɫ��ֵ����֤ÿһ�ж�����ͬ
void initBoardColor() {
	int colorVis[6];
	srand((unsigned)(time(NULL)));


	//��������6����ɫ��ͬ
	memset(colorVis, 0, sizeof(colorVis));
	for (int i = 0; i < 6; i++) {
		int choseColor = rand() % 6;//���ѡ��һ����ɫ
		choseColor = (choseColor + 1) % 6;
		while (colorVis[choseColor]) {
			choseColor = (choseColor + 1) % 6;
		}
		colorVis[choseColor] = 1;
		everyRowcolor[i] = choseColor;
	}

	//ÿһ��6����ɫ��ͬ
	for (int i = 0; i < 6; i++) {		
			memset(colorVis, 0, sizeof(colorVis));
			for (int k = 0,j=0; k < 6 , j<6 ; k++,j++) {
				int choseColor = rand() % 6;//���ѡ��һ����ɫ
				while (colorVis[choseColor]) {//��ǰ��ɫ�Ѿ�ѡ����ˣ�ѡ�����һ��
					choseColor = (choseColor + 1) % 6;
				}
				colorVis[choseColor] = 1;
				boardcolorIndex[i][j] = choseColor;
			}
	}

}

//�޸�һ���������ɫ
void changeBlockcolor(vec2 pos, vec4 colour) {//��������һ���������ɫvbo
	int x = pos.x, z = pos.y;
	for (int i = 0; i < 6; i++)
		boardcolours[(int)(6*((xlen-1)*z + x))] = colour;

	vec4 newcolours[6] = { colour, colour, colour, colour, colour, colour };

	glBindBuffer(GL_ARRAY_BUFFER, vboIDs[3]);

	// ����ƫ������������ɫvbo
	int offset = 6 * sizeof(vec4) * (int)((xlen-1) * z + x);
	glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(newcolours), newcolours);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

}

//��ʾ����Ĵ�ɫ
void applyBoardColorchange() {//��������������ɫ��VBO
	for (int z = 0; z < zlen - 1; z++) {
		for (int x = 0; x < xlen - 1; x++) {
			changeBlockcolor(vec2(x, z), allColor[everyRowcolor[z]]);
		}
	}
}

//�����û���ÿһ�����£���������������ɫ��
void varyBoardColor() {
	for (int z = 0; z < xlen - 1; z++) {
		for (int x = 0; x < xlen - 1; x++) {
			if (boardstatus[z][x])//��ʾ��ɫ
				changeBlockcolor(vec2(x, z), allColor[ everyRowcolor[z]]);
			else//��ʾ�Ի�ɫ
				changeBlockcolor(vec2(x, z), allColor[boardcolorIndex[z][x]]);
		}
	}
}

//��ʾ��ǰ��ָ����ɫ
void showCurBoardColor() {
		for (int x = 0; x < xlen - 1; x++) 
			changeBlockcolor(vec2(x,currentBoardz), allColor[currentRowcolor]);
}

//��ʾ
void tip() {
	if (inGame && !hinnting && hinntTimes<10) {
		memcpy(boardstatus2, boardstatus, sizeof(boardstatus));
		memset(boardstatus, 1, sizeof(boardstatus));
		varyBoardColor();
		hinntstartTime = glutGet(GLUT_ELAPSED_TIME);
		hinnting = 1;
	}
}


//���ֲ���
void backgroundmusic(int i)
{

	MCI_OPEN_PARMS op;                                     //���ű�������
	op.dwCallback = NULL;
	op.lpstrAlias = NULL;
	op.lpstrDeviceType = "TYPE MPEGVIDEO";
	if (i == 0) {
		op.lpstrElementName = "audio/background.mp3";
		bmusicStart = glutGet(GLUT_ELAPSED_TIME);
	}
	else if (i == 1) {
		op.lpstrElementName = "audio/allgiveyou.mp3";
		bmusicStart = glutGet(GLUT_ELAPSED_TIME);
	}

	op.wDeviceID = NULL;
	UINT rs;
	rs = mciSendCommand(NULL, MCI_OPEN, MCI_OPEN_ELEMENT, (DWORD)&op);
	if (rs == 0)//����ִ�гɹ�
	{
		MCI_PLAY_PARMS pp;
		pp.dwCallback = NULL;
		pp.dwFrom = 0;
		mciSendCommand(op.wDeviceID, MCI_PLAY, MCI_DGV_PLAY_REPEAT, (DWORD)&pp);
	}
	
}



void initThreadSound() {//��ʼ�������߳�
/*	tsound[0] = std::thread(backgroundmusic, 0);
	tsound[1] = std::thread(backgroundmusic, 1);
	tsound[2] = std::thread(backgroundmusic, 2);
	tsound[3] = std::thread(backgroundmusic, 3);*/
	tsound[4] = std::thread(backgroundmusic, 4);
}



////////////////////////////    eva�ĸ����˶�����   /////////////////////////////////////////////////

//��eva��λ�ú���ת��Ϣд��eva����Ϣ����
void updateEva() {
	for (int i = 0; i < 3; i++) {
		evaPos[0][i] = bodyPos[i];
		evaPos[1][i] = rArmPos[i];
		evaPos[2][i] = lArmPos[i];
		evaPos[3][i] = headPos[i];
		evaPos[4][i] = facePos[i];

		evaRot[0][i] = bodyRot[i];
		evaRot[1][i] = rArmRot[i];
		evaRot[2][i] = lArmRot[i];
		evaRot[3][i] = headRot[i];
		evaRot[4][i] = faceRot[i];
	}
}

//����eva�˶��������ת��������λ�ã��Լ�����<eva,at>
void updateMoveStatus() {
	//���������<eva,at>���ҹ�һ��
	VectorEvaToAt = normalize(vec3(at.x - bodyPos.x, at.y - bodyPos.y, at.z - bodyPos.z));
	//�������������Z��������ģ�������<eva,at>��ȡxozƽ���ϵ����������ݸ��������������תֵ
	bodyRot.y = atan(VectorEvaToAt.x / VectorEvaToAt.z) * 180.0 / M_PI;
	if (VectorEvaToAt.z < 0)
		bodyRot.y += 180.0;
	//���������λ��
	eye = vec4(bodyPos + -VectorEvaToAt * distEyeToEva)+vec4(0,0.5,0,0);
	//����eva��λ��
	updateEva();
}

//��ȡ���������ļн�
float vecIncludeAngle(vec2 a, vec2 b) {
	return ( acos((a.x*b.x + a.y*b.y) / sqrt((a.x*a.x + a.y*a.y)*(b.x * b.x + b.y * b.y))) ) * 180.0 / M_PI;
}

float vecIncludeAngle(vec3 a, vec3 b) {
	return (acos((a.x*b.x + a.y*b.y+a.z*b.z) / sqrt((a.x*a.x + a.y*a.y+a.z*a.z)*(b.x * b.x + b.y * b.y+b.z)))) * 180.0 / M_PI;
}

//eva�˶�ʱ��ᷢ������б
void evaIncline() {
	//����EVA�˶���ʱ���������ǰ��
	//������������evaΪһ���ڶ˵���ԭ��Ĺ��ӣ�evaǰ���ʱ������������ǰ���ʱ�����ڣ����Ӿ���һ������a����
	//����ǰ��ǰ������a��evaĿǰ�����ƶ��ķ��������b���������ǰ��������c
	if (inclineAngle >= 90 ) {//�Ƕȱ仯����90,�����ٶ�ͣ��
		inclineAngle = 90;
		bodyRot.x = bodyRot.z = 0;

		return;
	}

	vec3 a(0.0, 1.0, 0.0);
	vec3 b=normalize(vec3( sin(speedAngle * M_PI / 180.0), 0, cos(speedAngle * M_PI / 180.0)));
	//c��aob����ϵ�� �Ƕ� BoCΪinclindetlta����������������a�˶�ǰ��������
	vec3 c = vec3(b * cos(inclineAngle * M_PI / 180.0) + a * sin(inclineAngle * M_PI / 180));
	//��������cȥ����ǰ��ǰ������a��x�����y���ת���ĽǶ�
	//��z��
	
	bodyRot.z = vecIncludeAngle(c, vec3(1.0,0,0.0))-90;
	//��x��
	bodyRot.x = 90-vecIncludeAngle(c, vec3(0, 0,1.0));
}

//eva�˶�����
void evaMove() {	
	//�˶�ǰ��б,eva��ǰ��б
	evaIncline();
	//��������λ��
	bodyPos = bodyPos + speed;
/*	if (bodyPos.x<0 || bodyPos.x>block_width*(xlen - 1) || bodyPos.z<0 || bodyPos.z>block_width*(zlen - 1))
		bodyPos -= speed;*/
	//�����ƶ������У��۽������н��иı�ģ�����ҲҪ�ı�۽���
	at = viewRangle * vec4(cos(horiAngle*degree), sin(vertAngle*degree), sin(horiAngle*degree), 0)
		+ vec4(bodyPos.x, eye.y, bodyPos.z, 0);
	//�����˶��󣬱���Ҫ���µ�ǰ�����λ��
	updateMoveStatus();

}
//eva�³�
void evaDown() {//eva�����ƶ�
	if (!evaDownStatus && heightChange>=1.0) {//���eva�Ѿ�������ԭ���ĸ߶�
		evaDownStatus = 1;//����eva���³�״̬������idle��������
		vec2 EvaChessPos = getEvaBoardPos();
		int x = EvaChessPos.y, z = EvaChessPos.x;

		if (!inGame && !finishGame) {//������Ϸ��,��Ϸ����״̬����
			if (!startGame) {//�����û�н�����Ϸ
				//ֻ���ڽ�����Ϸ����������4�β��ܽ�ȥ��Ϸ
				if (x == inGameBlock.x && z == inGameBlock.y) {//�����ǰ���µ��ǽ�����Ϸ�ķ���
					boardstatus[(int)(z)][(int)(x)] += 1;//������Ϸ�ķ���״̬+1
					//cout << boardstatus[(int)(z)][(int)(x)] << endl;
					if (boardstatus[(int)(z)][(int)(x)] >= 4) {
						startGame = 1;//������Ϸ��
						gameStartTime = glutGet(GLUT_ELAPSED_TIME);//��¼��ʼʱ
						initBoardColor();//��ʼ��������ɫ
						applyBoardColorchange();//��ʾ�����ɫ
						memset(boardstatus, 0, sizeof(boardstatus));//��ʼ������״̬
						hinntTimes = 0;
					}
				}
				else {
					//������״̬���
					memset(boardstatus, 0, sizeof(boardstatus));
				}
			}
		}
		else {
			// ����Ϸ�� ������ʾ״̬�²��ܲ��� ��������˵�ǰ�����еĴ�ɫ		
			if (!hinnting && z==currentBoardz &&  boardcolorIndex[(int)(z)][(int)(x)] == everyRowcolor[z]) {
				for (int i = 0; i < xlen - 1; i++) 
					boardstatus[(int)(z)][i] = 1;

				currentBoardz += 1;
				varyBoardColor();//�����ˣ�����ɫ
				if (currentBoardz >= 6) {
					finishGame = 1;
					inGame = startGame = 0;
					//���̿������ֺ�ģʽ����������
					varyBoardColor();
				}
			}
			else if(!hinnting) {
				currentBoardz =0;
				memset(boardstatus, 0, sizeof(boardstatus));
				varyBoardColor();
			}
		}
	}
}


////////////////////////////    ���ں����,�����¼���idle    /////////////////////////////////////////////////

// �������ŵ�ʱ�򣬴����ڲ��Ķ���������Ӧ�ı任
void reshape(GLsizei w, GLsizei h)
{
	glViewport(0, 0, w, h);
}

// �����Ӧ����
void mouse(int button, int state, int x, int y) //���������  
{
	if (state == GLUT_DOWN) //��һ����갴��ʱ,��¼����ڴ����еĳ�ʼ����  
		oldMousex = x, oldMousey = y;

	if (state == GLUT_UP && button == GLUT_WHEEL_UP) {
		distEyeToEva += 0.1;
	}
		
	if (state == GLUT_UP && button == GLUT_WHEEL_DOWN) {
		distEyeToEva -= 0.1;
	}
		
	glutPostRedisplay();

}

//����϶������¼�:ͨ�������ƶ�������ӽǷ���ĸı䲢�Ҹ���Eva���沿����
void onMouseMove(int x, int y) //��������϶�  
{

	//ͨ�������ƶ�����at��
	if (x < oldMousex)//�������������϶���ˮƽ�ӽǽ��б任
		horiAngle -= addAngle;
	else if (x > oldMousex)
		horiAngle += addAngle;

	if (y < oldMousey && vertAngle >=50)//���������������϶��������ӽǽ��б任�����趨һ������
		vertAngle -= addAngle;
	else if (y > oldMousey && vertAngle <=300)
		vertAngle += addAngle;

	oldMousex = x, oldMousey = y;
	//���¾۽���
	at = viewRangle * vec4(cos(horiAngle*degree), sin(vertAngle*degree), sin(horiAngle*degree),0)+vec4(bodyPos.x,eye.y, bodyPos.z,0);

	//�����˶���eva��ת���λ�ã��Լ������λ��
	updateMoveStatus();
	
	glutPostRedisplay();
	
}

// ������Ӧ����
void keyboard(unsigned char key, int x, int y)
{	//��ȡ��ǰeva�泯�������
	speedAngle = bodyRot.y;
	bool flag =0;
	switch (key)
	{
	case 'w':	// ǰ��
		speedAngle += 0;
		flag=true;
		break;
	case 's':	// ����
		speedAngle += 180;
		flag = true;
		break;
	case 'a':	// ������
		speedAngle += 90;
		flag = true;
		break;
	case 'd':	// ������
		speedAngle -= 90;
		flag = true;
		break;
	case space:
		evaDown();
		break;
	case 't':
		tip();
		break;
	case 'r':
		finishGame = 0;
		break;
	case 'q':
		bodyRot.y += 10;
		updateEva();
		break;


        /*�������Է���λ��
	case 'j':
		homePos.x-=0.1;
		break;
	case 'l':
		homePos.x += 0.1;
		updateEva();
		break;
	case 'k':
		homePos.y -= 0.1;
		updateEva();
		break;
	case 'i':
		homePos.y += 0.1;
		break;
	case 'z':
		homePos.z += 0.1;
		break;
	case 'c':
		homePos.z -= 0.1;
		break;
		*/

	case 'j':
		mewPos[0].x -= 2.0;
		break;
	case 'l':
		mewPos[0].x += 2.0;
		updateEva();
		break;
	case 'k':
		mewPos[0].y -= 2.0;
		updateEva();
		break;
	case 'i':
		mewPos[0].y += 2.0;
		break;
	case 'z':
		mewPos[0].z += 2.0;
		break;
	case 'c':
		mewPos[0].z -= 2.0;
		break;

	case 033:	// ESC�� �� 'q' ���˳���Ϸ
		exit(EXIT_SUCCESS);
		break;
	}
	if (flag) {
		speeddelta = 1.0;//��ǰ���ٶȱ�ֵ
		inclineAngle =70.0;
		rotStartTime = glutGet(GLUT_ELAPSED_TIME);
		speed = vec3(sin(speedAngle * M_PI / 180.0), 0.0, cos(speedAngle * M_PI / 180.0))*speeddelta*0.2;//ƽ������
		evaMove();
	}


	cout << evaPos[0] << endl;
	glutPostRedisplay();
}

//ѭ������
void idle(void)
{

	if (!inGame &&  glutGet(GLUT_ELAPSED_TIME) - gameStartTime >= 5000) {//������Ϸ�󣬵ذ���ɫ��ʼ��ʧ
		inGame = 1;
		varyBoardColor();//����״̬��ʾ�������̵���ɫ
		currentBoardz = 0;//��ǰ���ڵ���
		varyBoardColor();
	}

	//��ʾ˥������
	if (hinnting && glutGet(GLUT_ELAPSED_TIME) - hinntstartTime > 1500) {
		memcpy(boardstatus, boardstatus2, sizeof(boardstatus));
		varyBoardColor();//����ʾ��������
		hinnting = 0;
	}

	//û���¼�������ʱ�򲻶ϵĶ�eva���ٶȽ���˥��
	//���ʱ�䳬������ʱ��
	if (glutGet(GLUT_ELAPSED_TIME) - startTime >= 80) {//�������ʱ��
		if (speeddelta >= 0) {//�ٶȱ仯
			speeddelta -= 0.1;
			speed *= speeddelta*1.0;
		}

		if (glutGet(GLUT_ELAPSED_TIME) - rotStartTime > 200 && inclineAngle < 90) {//��б�Ǳ仯
			inclineAngle += inclinedelta;
		}

		if (heightChange <= 1.0 && !evaDownStatus) {//eva���������ƶ����ָ�ԭ���ĸ߶�
			heightChange += 0.4;
			bodyPos.y = heightChange * evaheight;
		}

		if (evaDownStatus) {//���eva�����³�
			heightChange -= 0.5;
			bodyPos.y = heightChange * evaheight;
			if (heightChange <= 0)//�³������޵�ʱ�򣬸ı��³�״̬
				evaDownStatus = 0;
		}

		/*
		//��Ÿ��ת
		mewrotangle += 3;
		mewPos[0] = vec3(15 * cos(mewrotangle * M_PI / 180), -44, -5 + 15 * sin(mewrotangle * M_PI / 180));
		mewRot[0] = vec3(0,vecIncludeAngle(vec3(0, -44, -5)-mewPos[0],vec3(0,-44,-5)) , 0);


		if (mewPos[0].x < 0)
			mewRot[0].y = 360 - mewRot[0].y;
		*/

		mewwingrot = (mewwingrot + 3.0) >= 60 ? 0 : mewwingrot + 3.0;
		mewRot[1].z = mewwingrot;
		mewRot[2].z = -mewwingrot;
		mewPos[0] = vec3(0, -44, 0);


		evaMove();
		startTime = glutGet(GLUT_ELAPSED_TIME);
	}

	//����ȫ��������
	if (finishGame && endmusic ==1) {
		endmusic = 0;
		backgroundmusic(1);

	}
		
	glutPostRedisplay();
}

////////////////////////////    Main()    /////////////////////////////////////////////////
int main(int argc, char **argv)
{
	
	srand((unsigned)(time(NULL)));
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(737, 770);
	glutCreateWindow("OpenGL-Tutorial");

	glewInit();
	

	init();
//	backgroundmusic(0);

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutMouseFunc(mouse);
	glutKeyboardFunc(keyboard);
	//����϶��¼�����
	glutMotionFunc(onMouseMove);
	glutIdleFunc(idle);

	glutMainLoop();

	//ɾ��mesh
	for (unsigned int i = 0; i < my_meshs.size(); i++)
		delete my_meshs[i];
	delete mp_;

	
	return 0;
}



