#pragma once
#include "Angel.h"
#include "thread"

#ifndef _VARY_H
#define _VARY_H

//��ɫ����
extern vec4 orange;
extern vec4 white ;
extern vec4 black ;

//��ɫ����
extern vec4 orange;
extern vec4 white;
extern vec4 black;
extern vec4 blue;
extern vec4 red;
extern vec4 cyan;
extern vec4 grey;
extern vec4  allColor[];

//���̱���
//���̱���
extern float block_width;	//ÿ��С����Ŀ�
extern int xlen ;//���̵ĺ��ᶥ����
extern int zlen ;//���̵����ᶥ����
extern int grid_points_num ;//���̶˵�����
extern int board_points_num;//���̶�������
extern int  boardcolorIndex[6][6];//������ɫ
extern int boardstatus[6][6];//����״̬
extern int boardstatus2[6][6];//����״̬����
extern int blockPushDown;//�Ƿ��µ�ǰ��

extern vec4 *gridpoints; //���̶˵�����
extern vec4 *gridcolours ;
extern vec4 *boardpoints;//���̷��񶥵�����
extern vec4 *boardcolours ;



 //vao & vbo
extern GLuint vaoIDs[];
extern GLuint vboIDs[];

//shader����ID
extern GLuint programID;
extern GLuint vPositionID;
extern GLuint vColorID;
extern GLuint modelViewProjMatrixID;

//windows & mouse
extern float degree;//�е�һ��

extern int windx;
extern int windy;

extern int oldMousex;//��һ�����x
extern int oldMousey;//��һ�����y

extern float horiAngle;//ˮƽ�ӽǽǶ�
extern float vertAngle;//��ֱ�ӽǽǶ�
extern float addAngle;//�Ƕ�����
extern float viewRangle;//�ӽǷ�Χ


//view
extern vec4 eye;
extern vec4 at;
extern vec4 up;
extern float znear, zfar;
extern vec3 lightPos;
extern int drawShadow;//�Ƿ������Ӱ
extern float distEyeToEva;
extern vec3 VectorEvaToAt;

//matrix
extern mat4 modelMatrix;
extern mat4 viewMatrix ;
extern mat4 modelViewMatrix;
extern mat4 modelViewProjMatrix;
extern mat4 projMatrix;

//eva�ĸ�����Ϣ ����
extern vec3 bodyPos, headPos, facePos, rArmPos, lArmPos;//���ɣ�ͷ���������֣����� λ����Ϣ
extern vec3 bodyRot, headRot, faceRot, rArmRot, lArmRot;//���ɣ�ͷ���������֣����� ��ת��Ϣ
extern vec3 evaPos[], evaRot[];
extern float min_height;

extern vec3 speed;
extern float speeddelta;
extern float speedAngle;
extern int rotStartTime;
extern int rotchange;

extern float inclineAngle;
extern float inclinedelta;
extern float rotatedelta;

extern float evaheight ;
extern float heightChange ;
extern int evaDownStatus;//eva���³�״̬

extern int startTime;//ʱ��

extern int coutflag;

//����
const  char space=32;

//��Ϸ����
extern int inGame;//����Ϸ��
extern int startGame;//��ʼ��Ϸ
extern int  finishGame;//�����Ϸ
extern int gameStartTime;//��ʼ��Ϸ��ʱ��
extern  int currentBoardz;//��ǰ���ڵ�����
extern vec2 inGameBlock;//������Ϸ�ķ���λ��
extern int currentRowcolor;//��ǰ����Ҫѡ�����ɫ
extern int everyRowcolor[];//ÿһ�е���ɫ
extern int hinntstartTime;//��ʾ��ʼ��ʱ��
extern int hinnting;//�Ƿ��Ѿ�������ʾ
extern int hinntTimes;//��ʾ����
extern std::thread tsound[];//�����߳�
extern vec3 homePos[2] , homeRot[2] ;//home������

//���ֲ���
extern int bmusicStart;
extern int endmusic;//��������


//��Ÿ
extern vec3 mewPos[], mewRot[];
extern float mewrotangle ;
extern float mewwingrot ;
#endif

