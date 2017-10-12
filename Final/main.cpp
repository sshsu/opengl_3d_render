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

#define  GLUT_WHEEL_UP 3           //定义滚轮操作  
#define  GLUT_WHEEL_DOWN 4  

using namespace std;


//mesh
Mesh_Painter*	mp_;//mesh绘制
vector<My_Mesh*> my_meshs;//mesh回收


////////////////////////////    initial 和 display()    /////////////////////////////////////////////////

 //初始化棋盘
void Init_board() {

	//竖直线，一共七条
	for (int i = 0; i < xlen; i++) {
		//上面的点
		gridpoints[2 * i] = vec4(block_width*i, 0, 0, 1.0);
		//下面的点
		gridpoints[2 * i + 1] = vec4(block_width*i, 0, (zlen - 1)*block_width, 1.0);

	}

	//棋盘的横向线，一共七条
	int offset = xlen * 2;
	for (int i = 0; i < zlen; i++) {
		//左边的点
		gridpoints[2 * i + offset] = vec4(0, 0, i*block_width, 1.0);
		//右边的点
		gridpoints[2 * i + 1 + offset] = vec4((xlen - 1)*block_width, 0, i *block_width, 1.0);
	}

	//把所有线绘制成白色
	for (int i = 0; i < grid_points_num; i++)
		gridcolours[i] = vec4(34/255.0,158/255.0,255/255.0,1.0);

	//画棋盘上的格子
	for (int i = 0; i < board_points_num; i++)
		boardcolours[i] = white;

	int oset = 1;
	//给棋盘格子赋值
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

	// 载入着色器
	programID = InitShader("vshader.glsl", "fshader.glsl");
	glUseProgram(programID);


	//获取position color属性编号
	vPositionID = glGetAttribLocation(programID, "vPosition");
	vColorID = glGetAttribLocation(programID, "vColor");

	//生成2个VAO vao[0]棋盘端点，vao[1]棋盘格子
	glGenVertexArrays(2, &vaoIDs[0]);

	// 棋盘格顶点,激活VAO[0]
	glBindVertexArray(vaoIDs[0]);
	glGenBuffers(2, vboIDs);

	// 棋盘格端点点位置
	//将VBO编号放入全局变量,GL_ARRAY_BUFFER=vboIDs[0]
	glBindBuffer(GL_ARRAY_BUFFER, vboIDs[0]);
	//放数据
	glBufferData(GL_ARRAY_BUFFER, grid_points_num*sizeof(vec4), gridpoints, GL_STATIC_DRAW);
	//vPosition属性将使用vboids[0]中的某类信息
	glVertexAttribPointer(vPositionID, 4, GL_FLOAT, GL_FALSE, 0, 0);
	//将vpostion属性设置成enable
	glEnableVertexAttribArray(vPositionID);

	// 棋盘格端点颜色
	glBindBuffer(GL_ARRAY_BUFFER, vboIDs[1]);
	glBufferData(GL_ARRAY_BUFFER, grid_points_num*sizeof(vec4), gridcolours, GL_DYNAMIC_DRAW);
	//指定vcolor的各种属性(大小，类型，是否归一化，数据间隔，在vbo中的偏移)
	glVertexAttribPointer(vColorID, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vColorID);

	// 棋盘格每个格子
	glBindVertexArray(vaoIDs[1]);
	glGenBuffers(2, &vboIDs[2]);

	// 棋盘格每个格子顶点位置
	glBindBuffer(GL_ARRAY_BUFFER, vboIDs[2]);
	glBufferData(GL_ARRAY_BUFFER, board_points_num*sizeof(vec4), boardpoints, GL_STATIC_DRAW);
	glVertexAttribPointer(vPositionID, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vPositionID);

	// 棋盘格每个格子顶点颜色
	glBindBuffer(GL_ARRAY_BUFFER, vboIDs[3]);
	glBufferData(GL_ARRAY_BUFFER, board_points_num*sizeof(vec4), boardcolours, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(vColorID, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vColorID);

	//解绑shader和对象
	glBindVertexArray(0);
	glUseProgram(0);

}

 //初始化eva
void Init_eva() {
	//eva上移0.04
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

///初始化天空盒子
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
//初始化地板
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
//初始化房子
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

//初始化每个mesh对应的shader
void init_meshShader() {
	mp_->init_shaders("v_texture.glsl", "f_texture.glsl");
	mp_->update_vertex_buffer();
	mp_->update_texture();
}

//总的初始化
void init()
{
	//初始化eva到at点的向量
	VectorEvaToAt = normalize(vec3(at.x - bodyPos.x, at.y - bodyPos.y, at.z - bodyPos.z));
	glEnable(GL_DEPTH_TEST);//深度值发生了变化就马上重绘

	Init_board();//先初始化棋盘地板
	Init_eva();//初始化机器人Eva
	Init_sky();//初始化天空盒
	Init_floor();//初始化地板
	Init_home();//初始化房子
	
	init_meshShader();
	glBindVertexArray(0);
	glClearColor(0, 0, 0, 0);
}


//画地板棋盘
void draw_board() {

	//使用棋盘着色器
	glUseProgram(programID);

	//矩阵构造
	viewMatrix = LookAt(eye, at, up);
	projMatrix = Perspective(70, 1.0, znear, zfar);
	//projMatrix = Ortho(-20, 20, -20, 20, -20, 20);
	modelViewProjMatrix = projMatrix * viewMatrix;

	//获取变换矩阵的ID,并换值
	modelViewProjMatrixID = glGetUniformLocation(programID, "modelViewProjMatrix");
	glUniformMatrix4fv(modelViewProjMatrixID, 1, GL_TRUE, &modelViewProjMatrix[0][0]);

	//先画格子，再划线
	glBindVertexArray(vaoIDs[1]);
	glDrawArrays(GL_TRIANGLES, 0, board_points_num); // 绘制棋盘格 


	glBindVertexArray(vaoIDs[0]);
	glDrawArrays(GL_LINES, 0, grid_points_num);		 // 绘制棋盘格的线

													 //VA0,着色器置0
	glBindVertexArray(0);
	glUseProgram(0);

}

//画mesh里面的东西
void draw_mesh() {
	//绘制EVA
	
	mp_->draw_meshes(0);//绘制eva的mesh

	for (int i = 5; i<11; i++)//绘制天空盒
		mp_->draw_meshes(i);

	if (finishGame) 
		mp_->draw_meshes(11);//绘制贴图地板
	
	mp_->draw_meshes(12);//绘制房子
	

	//VA0,着色器置0
	glBindVertexArray(0);
	glUseProgram(0);
}

//总的display
void display()
{
	//清除当前帧的颜色和数据缓存
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	draw_board();//绘制棋盘
	draw_mesh();//绘制mesh中的东西

	glutSwapBuffers();
}


////////////////////////////    游戏系统    //////////////////////////////////////////////////////////////

//获取eva在棋盘上的位置
vec2 getEvaBoardPos() {
	float x, z;
	x = floor(bodyPos.x / block_width);
	z = floor(bodyPos.z / block_width);
	return vec2(z, x);
}

//给棋盘颜色赋值，保证每一行都不相同
void initBoardColor() {
	int colorVis[6];
	srand((unsigned)(time(NULL)));


	//正方格内6行颜色不同
	memset(colorVis, 0, sizeof(colorVis));
	for (int i = 0; i < 6; i++) {
		int choseColor = rand() % 6;//随机选择一个颜色
		choseColor = (choseColor + 1) % 6;
		while (colorVis[choseColor]) {
			choseColor = (choseColor + 1) % 6;
		}
		colorVis[choseColor] = 1;
		everyRowcolor[i] = choseColor;
	}

	//每一行6格颜色不同
	for (int i = 0; i < 6; i++) {		
			memset(colorVis, 0, sizeof(colorVis));
			for (int k = 0,j=0; k < 6 , j<6 ; k++,j++) {
				int choseColor = rand() % 6;//随机选择一个颜色
				while (colorVis[choseColor]) {//当前颜色已经选择过了，选择后面一个
					choseColor = (choseColor + 1) % 6;
				}
				colorVis[choseColor] = 1;
				boardcolorIndex[i][j] = choseColor;
			}
	}

}

//修改一个方块的颜色
void changeBlockcolor(vec2 pos, vec4 colour) {//更新棋盘一个方块的颜色vbo
	int x = pos.x, z = pos.y;
	for (int i = 0; i < 6; i++)
		boardcolours[(int)(6*((xlen-1)*z + x))] = colour;

	vec4 newcolours[6] = { colour, colour, colour, colour, colour, colour };

	glBindBuffer(GL_ARRAY_BUFFER, vboIDs[3]);

	// 计算偏移量，更新颜色vbo
	int offset = 6 * sizeof(vec4) * (int)((xlen-1) * z + x);
	glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(newcolours), newcolours);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

}

//显示最初的答案色
void applyBoardColorchange() {//更新整个棋盘颜色的VBO
	for (int z = 0; z < zlen - 1; z++) {
		for (int x = 0; x < xlen - 1; x++) {
			changeBlockcolor(vec2(x, z), allColor[everyRowcolor[z]]);
		}
	}
}

//根据用户的每一步跳下，更改整个棋盘颜色，
void varyBoardColor() {
	for (int z = 0; z < xlen - 1; z++) {
		for (int x = 0; x < xlen - 1; x++) {
			if (boardstatus[z][x])//显示答案色
				changeBlockcolor(vec2(x, z), allColor[ everyRowcolor[z]]);
			else//显示迷惑色
				changeBlockcolor(vec2(x, z), allColor[boardcolorIndex[z][x]]);
		}
	}
}

//显示当前行指定颜色
void showCurBoardColor() {
		for (int x = 0; x < xlen - 1; x++) 
			changeBlockcolor(vec2(x,currentBoardz), allColor[currentRowcolor]);
}

//提示
void tip() {
	if (inGame && !hinnting && hinntTimes<10) {
		memcpy(boardstatus2, boardstatus, sizeof(boardstatus));
		memset(boardstatus, 1, sizeof(boardstatus));
		varyBoardColor();
		hinntstartTime = glutGet(GLUT_ELAPSED_TIME);
		hinnting = 1;
	}
}


//音乐播放
void backgroundmusic(int i)
{

	MCI_OPEN_PARMS op;                                     //播放背景音乐
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
	if (rs == 0)//发送执行成功
	{
		MCI_PLAY_PARMS pp;
		pp.dwCallback = NULL;
		pp.dwFrom = 0;
		mciSendCommand(op.wDeviceID, MCI_PLAY, MCI_DGV_PLAY_REPEAT, (DWORD)&pp);
	}
	
}



void initThreadSound() {//初始化声音线程
/*	tsound[0] = std::thread(backgroundmusic, 0);
	tsound[1] = std::thread(backgroundmusic, 1);
	tsound[2] = std::thread(backgroundmusic, 2);
	tsound[3] = std::thread(backgroundmusic, 3);*/
	tsound[4] = std::thread(backgroundmusic, 4);
}



////////////////////////////    eva的各种运动函数   /////////////////////////////////////////////////

//将eva的位置和旋转信息写入eva的信息数组
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

//更新eva运动后身体的转向和相机的位置，以及向量<eva,at>
void updateMoveStatus() {
	//计算出向量<eva,at>并且归一化
	VectorEvaToAt = normalize(vec3(at.x - bodyPos.x, at.y - bodyPos.y, at.z - bodyPos.z));
	//最初身体是面向Z轴正方向的，用向量<eva,at>获取xoz平面上的向量，根据该向量，计算出旋转值
	bodyRot.y = atan(VectorEvaToAt.x / VectorEvaToAt.z) * 180.0 / M_PI;
	if (VectorEvaToAt.z < 0)
		bodyRot.y += 180.0;
	//更新相机的位置
	eye = vec4(bodyPos + -VectorEvaToAt * distEyeToEva)+vec4(0,0.5,0,0);
	//更新eva的位置
	updateEva();
}

//获取两个向量的夹角
float vecIncludeAngle(vec2 a, vec2 b) {
	return ( acos((a.x*b.x + a.y*b.y) / sqrt((a.x*a.x + a.y*a.y)*(b.x * b.x + b.y * b.y))) ) * 180.0 / M_PI;
}

float vecIncludeAngle(vec3 a, vec3 b) {
	return (acos((a.x*b.x + a.y*b.y+a.z*b.z) / sqrt((a.x*a.x + a.y*a.y+a.z*a.z)*(b.x * b.x + b.y * b.y+b.z)))) * 180.0 / M_PI;
}

//eva运动时候会发生的倾斜
void evaIncline() {
	//由于EVA运动的时候，身体会向前倾
	//我们首先设想eva为一根在端点在原点的棍子，eva前倾的时候就是这根棍子前倾的时候，现在，棍子就是一个向量a，则
	//先由前倾前的向量a和eva目前正在移动的方向的向量b，求出棍子前倾后的向量c
	if (inclineAngle >= 90 ) {//角度变化到了90,或者速度停了
		inclineAngle = 90;
		bodyRot.x = bodyRot.z = 0;

		return;
	}

	vec3 a(0.0, 1.0, 0.0);
	vec3 b=normalize(vec3( sin(speedAngle * M_PI / 180.0), 0, cos(speedAngle * M_PI / 180.0)));
	//c是aob坐标系中 角度 BoC为inclindetlta的向量，即是向量a运动前倾后的向量
	vec3 c = vec3(b * cos(inclineAngle * M_PI / 180.0) + a * sin(inclineAngle * M_PI / 180));
	//利用向量c去计算前倾前，向量a绕x轴和绕y轴的转动的角度
	//绕z轴
	
	bodyRot.z = vecIncludeAngle(c, vec3(1.0,0,0.0))-90;
	//绕x轴
	bodyRot.x = 90-vecIncludeAngle(c, vec3(0, 0,1.0));
}

//eva运动函数
void evaMove() {	
	//运动前倾斜,eva向前倾斜
	evaIncline();
	//更新物体位置
	bodyPos = bodyPos + speed;
/*	if (bodyPos.x<0 || bodyPos.x>block_width*(xlen - 1) || bodyPos.z<0 || bodyPos.z>block_width*(zlen - 1))
		bodyPos -= speed;*/
	//由于移动过程中，聚焦点是有进行改变的，所以也要改变聚焦点
	at = viewRangle * vec4(cos(horiAngle*degree), sin(vertAngle*degree), sin(horiAngle*degree), 0)
		+ vec4(bodyPos.x, eye.y, bodyPos.z, 0);
	//物体运动后，必须要更新当前相机的位置
	updateMoveStatus();

}
//eva下沉
void evaDown() {//eva向下移动
	if (!evaDownStatus && heightChange>=1.0) {//如果eva已经上升回原来的高度
		evaDownStatus = 1;//设置eva的下沉状态，交给idle函数处理
		vec2 EvaChessPos = getEvaBoardPos();
		int x = EvaChessPos.y, z = EvaChessPos.x;

		if (!inGame && !finishGame) {//不在游戏中,游戏结束状态结束
			if (!startGame) {//如果还没有进入游戏
				//只有在进入游戏方块连续才4次才能进去游戏
				if (x == inGameBlock.x && z == inGameBlock.y) {//如果当前踩下的是进入游戏的方块
					boardstatus[(int)(z)][(int)(x)] += 1;//进入游戏的方块状态+1
					//cout << boardstatus[(int)(z)][(int)(x)] << endl;
					if (boardstatus[(int)(z)][(int)(x)] >= 4) {
						startGame = 1;//进入游戏了
						gameStartTime = glutGet(GLUT_ELAPSED_TIME);//记录开始时
						initBoardColor();//初始化棋盘颜色
						applyBoardColorchange();//显示最初答案色
						memset(boardstatus, 0, sizeof(boardstatus));//初始化棋盘状态
						hinntTimes = 0;
					}
				}
				else {
					//把棋盘状态清空
					memset(boardstatus, 0, sizeof(boardstatus));
				}
			}
		}
		else {
			// 在游戏中 不在提示状态下才能踩下 如果踩中了当前所在行的答案色		
			if (!hinnting && z==currentBoardz &&  boardcolorIndex[(int)(z)][(int)(x)] == everyRowcolor[z]) {
				for (int i = 0; i < xlen - 1; i++) 
					boardstatus[(int)(z)][i] = 1;

				currentBoardz += 1;
				varyBoardColor();//踩中了，改颜色
				if (currentBoardz >= 6) {
					finishGame = 1;
					inGame = startGame = 0;
					//棋盘开启音乐盒模式，播放音乐
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


////////////////////////////    窗口和鼠标,键盘事件，idle    /////////////////////////////////////////////////

// 窗口缩放的时候，窗口内部的东西进行相应的变换
void reshape(GLsizei w, GLsizei h)
{
	glViewport(0, 0, w, h);
}

// 鼠标响应函数
void mouse(int button, int state, int x, int y) //处理鼠标点击  
{
	if (state == GLUT_DOWN) //第一次鼠标按下时,记录鼠标在窗口中的初始坐标  
		oldMousex = x, oldMousey = y;

	if (state == GLUT_UP && button == GLUT_WHEEL_UP) {
		distEyeToEva += 0.1;
	}
		
	if (state == GLUT_UP && button == GLUT_WHEEL_DOWN) {
		distEyeToEva -= 0.1;
	}
		
	glutPostRedisplay();

}

//鼠标拖动函数事件:通过鼠标的移动运算出视角方向的改变并且更新Eva的面部朝向
void onMouseMove(int x, int y) //处理鼠标拖动  
{

	//通过鼠标的移动更新at点
	if (x < oldMousex)//鼠标如果向左右拖动，水平视角进行变换
		horiAngle -= addAngle;
	else if (x > oldMousex)
		horiAngle += addAngle;

	if (y < oldMousey && vertAngle >=50)//鼠标如果进行上下拖动，上下视角进行变换，并设定一定限制
		vertAngle -= addAngle;
	else if (y > oldMousey && vertAngle <=300)
		vertAngle += addAngle;

	oldMousex = x, oldMousey = y;
	//更新聚焦点
	at = viewRangle * vec4(cos(horiAngle*degree), sin(vertAngle*degree), sin(horiAngle*degree),0)+vec4(bodyPos.x,eye.y, bodyPos.z,0);

	//更新运动后eva的转向和位置，以及相机的位置
	updateMoveStatus();
	
	glutPostRedisplay();
	
}

// 键盘响应函数
void keyboard(unsigned char key, int x, int y)
{	//获取当前eva面朝向的向量
	speedAngle = bodyRot.y;
	bool flag =0;
	switch (key)
	{
	case 'w':	// 前进
		speedAngle += 0;
		flag=true;
		break;
	case 's':	// 后退
		speedAngle += 180;
		flag = true;
		break;
	case 'a':	// 向左走
		speedAngle += 90;
		flag = true;
		break;
	case 'd':	// 向右走
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


        /*调整测试房子位置
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

	case 033:	// ESC键 和 'q' 键退出游戏
		exit(EXIT_SUCCESS);
		break;
	}
	if (flag) {
		speeddelta = 1.0;//当前的速度变值
		inclineAngle =70.0;
		rotStartTime = glutGet(GLUT_ELAPSED_TIME);
		speed = vec3(sin(speedAngle * M_PI / 180.0), 0.0, cos(speedAngle * M_PI / 180.0))*speeddelta*0.2;//平移向量
		evaMove();
	}


	cout << evaPos[0] << endl;
	glutPostRedisplay();
}

//循环函数
void idle(void)
{

	if (!inGame &&  glutGet(GLUT_ELAPSED_TIME) - gameStartTime >= 5000) {//进入游戏后，地板颜色开始消失
		inGame = 1;
		varyBoardColor();//根据状态显示整个棋盘的颜色
		currentBoardz = 0;//当前所在的行
		varyBoardColor();
	}

	//提示衰减处理
	if (hinnting && glutGet(GLUT_ELAPSED_TIME) - hinntstartTime > 1500) {
		memcpy(boardstatus, boardstatus2, sizeof(boardstatus));
		varyBoardColor();//先显示整个棋盘
		hinnting = 0;
	}

	//没有事件触发的时候不断的对eva的速度进行衰减
	//如果时间超过下落时间
	if (glutGet(GLUT_ELAPSED_TIME) - startTime >= 80) {//如果超过时间
		if (speeddelta >= 0) {//速度变化
			speeddelta -= 0.1;
			speed *= speeddelta*1.0;
		}

		if (glutGet(GLUT_ELAPSED_TIME) - rotStartTime > 200 && inclineAngle < 90) {//倾斜角变化
			inclineAngle += inclinedelta;
		}

		if (heightChange <= 1.0 && !evaDownStatus) {//eva不是向下移动，恢复原来的高度
			heightChange += 0.4;
			bodyPos.y = heightChange * evaheight;
		}

		if (evaDownStatus) {//如果eva正在下沉
			heightChange -= 0.5;
			bodyPos.y = heightChange * evaheight;
			if (heightChange <= 0)//下沉到极限的时候，改变下沉状态
				evaDownStatus = 0;
		}

		/*
		//海鸥旋转
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

	//播放全部都给你
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
	//鼠标拖动事件函数
	glutMotionFunc(onMouseMove);
	glutIdleFunc(idle);

	glutMainLoop();

	//删除mesh
	for (unsigned int i = 0; i < my_meshs.size(); i++)
		delete my_meshs[i];
	delete mp_;

	
	return 0;
}



