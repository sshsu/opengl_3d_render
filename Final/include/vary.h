#pragma once
#include "Angel.h"
#include "thread"

#ifndef _VARY_H
#define _VARY_H

//颜色变量
extern vec4 orange;
extern vec4 white ;
extern vec4 black ;

//颜色变量
extern vec4 orange;
extern vec4 white;
extern vec4 black;
extern vec4 blue;
extern vec4 red;
extern vec4 cyan;
extern vec4 grey;
extern vec4  allColor[];

//棋盘变量
//棋盘变量
extern float block_width;	//每个小方块的宽
extern int xlen ;//棋盘的横轴顶点数
extern int zlen ;//棋盘的纵轴顶点数
extern int grid_points_num ;//棋盘端点总数
extern int board_points_num;//棋盘顶点总数
extern int  boardcolorIndex[6][6];//棋盘颜色
extern int boardstatus[6][6];//棋盘状态
extern int boardstatus2[6][6];//棋盘状态副本
extern int blockPushDown;//是否按下当前块

extern vec4 *gridpoints; //棋盘端点数组
extern vec4 *gridcolours ;
extern vec4 *boardpoints;//棋盘方格顶点数组
extern vec4 *boardcolours ;



 //vao & vbo
extern GLuint vaoIDs[];
extern GLuint vboIDs[];

//shader变量ID
extern GLuint programID;
extern GLuint vPositionID;
extern GLuint vColorID;
extern GLuint modelViewProjMatrixID;

//windows & mouse
extern float degree;//π的一度

extern int windx;
extern int windy;

extern int oldMousex;//上一次鼠标x
extern int oldMousey;//上一次鼠标y

extern float horiAngle;//水平视角角度
extern float vertAngle;//竖直视角角度
extern float addAngle;//角度增量
extern float viewRangle;//视角范围


//view
extern vec4 eye;
extern vec4 at;
extern vec4 up;
extern float znear, zfar;
extern vec3 lightPos;
extern int drawShadow;//是否绘制阴影
extern float distEyeToEva;
extern vec3 VectorEvaToAt;

//matrix
extern mat4 modelMatrix;
extern mat4 viewMatrix ;
extern mat4 modelViewMatrix;
extern mat4 modelViewProjMatrix;
extern mat4 projMatrix;

//eva的各种信息 骨骼
extern vec3 bodyPos, headPos, facePos, rArmPos, lArmPos;//躯干，头，脸，右手，左手 位置信息
extern vec3 bodyRot, headRot, faceRot, rArmRot, lArmRot;//躯干，头，脸，右手，左手 旋转信息
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
extern int evaDownStatus;//eva的下沉状态

extern int startTime;//时间

extern int coutflag;

//键盘
const  char space=32;

//游戏参数
extern int inGame;//在游戏中
extern int startGame;//开始游戏
extern int  finishGame;//完成游戏
extern int gameStartTime;//开始游戏的时间
extern  int currentBoardz;//当前所在的行数
extern vec2 inGameBlock;//进入游戏的方块位置
extern int currentRowcolor;//当前行需要选择的颜色
extern int everyRowcolor[];//每一行的颜色
extern int hinntstartTime;//提示开始的时间
extern int hinnting;//是否已经开启提示
extern int hinntTimes;//提示次数
extern std::thread tsound[];//声音线程
extern vec3 homePos[2] , homeRot[2] ;//home的属性

//音乐参数
extern int bmusicStart;
extern int endmusic;//结束曲子


//海鸥
extern vec3 mewPos[], mewRot[];
extern float mewrotangle ;
extern float mewwingrot ;
#endif

