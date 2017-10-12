//变量函数
#include "Angel.h"
#include <thread>

//颜色变量
vec4 orange = vec4(1.0, 0.5, 0.0, 1.0);
vec4 white = vec4(1.0, 1.0, 1.0, 1.0);
vec4 black = vec4(0.0, 0.0, 0.0, 1.0);
vec4 blue(92 / 255.0, 117 / 255.0, 216 / 255.0,1.0);
vec4 red(240/255.0,68/255.0,133/255.0, 1.0);
vec4 cyan(79/255.0, 230/255.0, 218/255.0, 1.0);
vec4 grey(183 / 255.0, 183 / 255.0, 183 / 255.0, 1.0);

vec4 allColor[7] = { orange,white,black,blue,red,cyan, grey}; //颜色数组

//棋盘变量
float block_width = 1.2;	//每个小方块的宽
int xlen = 7;//棋盘的横轴顶点数
int zlen = 7;//棋盘的纵轴顶点数
int grid_points_num = (xlen + zlen) * 2;//棋盘端点总数
int board_points_num = (xlen - 1)*(zlen - 1) * 6;//棋盘格子顶点总数(因为是三角面片)

int boardcolorIndex[6][6];//棋盘的颜色
int boardcolorcover[6][6];//棋盘当前格的颜色是否用灰色覆盖
int boardstatus[6][6];//棋盘的状态,判断eva有没有按下去
int boardstatus2[6][6];//棋盘的状态副本,

vec4 *gridpoints = new vec4[grid_points_num];//棋盘端点数组
vec4 *gridcolours = new vec4[grid_points_num];
vec4 *boardpoints = new vec4[board_points_num];//棋盘方格顶点数组
vec4 *boardcolours = new vec4[board_points_num];


int blockPushDown=0;//向下按函数

//vao & vbo
GLuint vaoIDs[3];
GLuint vboIDs[6];

//shader变量ID
GLuint programID;
GLuint vPositionID;
GLuint vColorID;
GLuint modelViewProjMatrixID;


//windows & mouse
float degree = M_PI / 180.0;//π的一度

int windx = 500;
int windy = 500;

int oldMousex;//上一次鼠标x
int oldMousey;//上一次鼠标y

 //view
vec4 eye(0.0, 0.4, 1.2, 1.0);
vec4 at(0.0, 0.0, 0.0, 0.0);
//vec4 eye(block_width*3.5, 1.5, block_width*3.5, 1.0);
//vec4 at(block_width*3.5, 0, block_width*3.5, 0);
vec4 up(0.0, 50.0, 0.0, 0.0);

float horiAngle = 81.2;//水平视角角度
float vertAngle = 224.2;//竖直视角角度
float addAngle = 1;//角度增量
float viewRangle = 5;//视角范围

float distEyeToEva = 2.0;//相机到eva躯干的距离
vec3 VectorEvaToAt;//eva到at点的向量


vec3 lightPos = vec3(-100.0,100.0,100.0);
float znear = 0.1, zfar = 80.0;

int drawShadow = 1;//是否绘制shadow

//各种matrix
mat4 modelMatrix;
mat4 viewMatrix = LookAt(eye, at, up);
mat4 modelViewMatrix;
mat4 modelViewProjMatrix;
mat4 projMatrix;

//eva的各种信息
vec3 bodyPos, headPos, facePos, rArmPos, lArmPos;//躯干，头，脸，右手，左手 位置信息
vec3 bodyRot, headRot, faceRot, rArmRot, lArmRot;//躯干，头，脸，右手，左手 旋转信息
//eva信息的数组存储
vec3 evaPos[5], evaRot[5];

//eva的最低坐标
float min_height = -225;

vec3 speed;//eva当前的速度
float speeddelta=0;//eva速度的变化值,衰减后用来乘以speed 进行speed的更新
float speedAngle;//eva走的方向，走的方向和面朝方向不一定相同
int rotStartTime=0x3f3f3f3f;//开始转动的时间
int rotchange = 1;//是否已经完成角度改变

float inclineAngle=70;//一个用来构造物体向前倾斜的 y轴分量
float inclinedelta = 4.0;//eva移动的时候，会向前倾，这是前倾角度的变化值

float evaheight = 0.06;
float heightChange = 1.0;
int evaDownStatus;//eva是否向下移动


//时间
 int startTime;
 //调试输出
 int coutflag=0;

 //游戏参数
 int inGame=0;//进入游戏
 int startGame = 0;//开始游戏
 int finishGame = 0;//完成游戏
 int gameStartTime=0x3f3f3f3f;//开始游戏的时间,初始化为无穷大
 int currentBoardz;//当前在棋盘的行
 int currentRowcolor;//当前行需要选择的颜色
 int everyRowcolor[6];//每一行的颜色
 vec2 inGameBlock(0, 0); //进入游戏的方块位置
 int hinntstartTime=0x3f3f3f3f;//提示开始的时间
 int hinnting;//是否已经开启提示
 int hinntTimes = 0;// 提示次数

 //音乐线程
 std::thread tsound[5];//声音线程
 int bmusicStart = 0;//背景音乐开始播放的时间
 int endmusic = 1;//结束曲子
 
 //home
 vec3 homePos[2] = { vec3(-43.3999,-128.899,-97.7991),vec3(0,0,0) }, homeRot[2] = {vec3(0,0,0),vec3(0,0,0)};

 //mew海鸥
 vec3 mewPos[3], mewRot[3];
 float mewrotangle=0;
 float mewwingrot = 0;





