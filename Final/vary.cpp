//��������
#include "Angel.h"
#include <thread>

//��ɫ����
vec4 orange = vec4(1.0, 0.5, 0.0, 1.0);
vec4 white = vec4(1.0, 1.0, 1.0, 1.0);
vec4 black = vec4(0.0, 0.0, 0.0, 1.0);
vec4 blue(92 / 255.0, 117 / 255.0, 216 / 255.0,1.0);
vec4 red(240/255.0,68/255.0,133/255.0, 1.0);
vec4 cyan(79/255.0, 230/255.0, 218/255.0, 1.0);
vec4 grey(183 / 255.0, 183 / 255.0, 183 / 255.0, 1.0);

vec4 allColor[7] = { orange,white,black,blue,red,cyan, grey}; //��ɫ����

//���̱���
float block_width = 1.2;	//ÿ��С����Ŀ�
int xlen = 7;//���̵ĺ��ᶥ����
int zlen = 7;//���̵����ᶥ����
int grid_points_num = (xlen + zlen) * 2;//���̶˵�����
int board_points_num = (xlen - 1)*(zlen - 1) * 6;//���̸��Ӷ�������(��Ϊ��������Ƭ)

int boardcolorIndex[6][6];//���̵���ɫ
int boardcolorcover[6][6];//���̵�ǰ�����ɫ�Ƿ��û�ɫ����
int boardstatus[6][6];//���̵�״̬,�ж�eva��û�а���ȥ
int boardstatus2[6][6];//���̵�״̬����,

vec4 *gridpoints = new vec4[grid_points_num];//���̶˵�����
vec4 *gridcolours = new vec4[grid_points_num];
vec4 *boardpoints = new vec4[board_points_num];//���̷��񶥵�����
vec4 *boardcolours = new vec4[board_points_num];


int blockPushDown=0;//���°�����

//vao & vbo
GLuint vaoIDs[3];
GLuint vboIDs[6];

//shader����ID
GLuint programID;
GLuint vPositionID;
GLuint vColorID;
GLuint modelViewProjMatrixID;


//windows & mouse
float degree = M_PI / 180.0;//�е�һ��

int windx = 500;
int windy = 500;

int oldMousex;//��һ�����x
int oldMousey;//��һ�����y

 //view
vec4 eye(0.0, 0.4, 1.2, 1.0);
vec4 at(0.0, 0.0, 0.0, 0.0);
//vec4 eye(block_width*3.5, 1.5, block_width*3.5, 1.0);
//vec4 at(block_width*3.5, 0, block_width*3.5, 0);
vec4 up(0.0, 50.0, 0.0, 0.0);

float horiAngle = 81.2;//ˮƽ�ӽǽǶ�
float vertAngle = 224.2;//��ֱ�ӽǽǶ�
float addAngle = 1;//�Ƕ�����
float viewRangle = 5;//�ӽǷ�Χ

float distEyeToEva = 2.0;//�����eva���ɵľ���
vec3 VectorEvaToAt;//eva��at�������


vec3 lightPos = vec3(-100.0,100.0,100.0);
float znear = 0.1, zfar = 80.0;

int drawShadow = 1;//�Ƿ����shadow

//����matrix
mat4 modelMatrix;
mat4 viewMatrix = LookAt(eye, at, up);
mat4 modelViewMatrix;
mat4 modelViewProjMatrix;
mat4 projMatrix;

//eva�ĸ�����Ϣ
vec3 bodyPos, headPos, facePos, rArmPos, lArmPos;//���ɣ�ͷ���������֣����� λ����Ϣ
vec3 bodyRot, headRot, faceRot, rArmRot, lArmRot;//���ɣ�ͷ���������֣����� ��ת��Ϣ
//eva��Ϣ������洢
vec3 evaPos[5], evaRot[5];

//eva���������
float min_height = -225;

vec3 speed;//eva��ǰ���ٶ�
float speeddelta=0;//eva�ٶȵı仯ֵ,˥������������speed ����speed�ĸ���
float speedAngle;//eva�ߵķ����ߵķ�����泯����һ����ͬ
int rotStartTime=0x3f3f3f3f;//��ʼת����ʱ��
int rotchange = 1;//�Ƿ��Ѿ���ɽǶȸı�

float inclineAngle=70;//һ����������������ǰ��б�� y�����
float inclinedelta = 4.0;//eva�ƶ���ʱ�򣬻���ǰ�㣬����ǰ��Ƕȵı仯ֵ

float evaheight = 0.06;
float heightChange = 1.0;
int evaDownStatus;//eva�Ƿ������ƶ�


//ʱ��
 int startTime;
 //�������
 int coutflag=0;

 //��Ϸ����
 int inGame=0;//������Ϸ
 int startGame = 0;//��ʼ��Ϸ
 int finishGame = 0;//�����Ϸ
 int gameStartTime=0x3f3f3f3f;//��ʼ��Ϸ��ʱ��,��ʼ��Ϊ�����
 int currentBoardz;//��ǰ�����̵���
 int currentRowcolor;//��ǰ����Ҫѡ�����ɫ
 int everyRowcolor[6];//ÿһ�е���ɫ
 vec2 inGameBlock(0, 0); //������Ϸ�ķ���λ��
 int hinntstartTime=0x3f3f3f3f;//��ʾ��ʼ��ʱ��
 int hinnting;//�Ƿ��Ѿ�������ʾ
 int hinntTimes = 0;// ��ʾ����

 //�����߳�
 std::thread tsound[5];//�����߳�
 int bmusicStart = 0;//�������ֿ�ʼ���ŵ�ʱ��
 int endmusic = 1;//��������
 
 //home
 vec3 homePos[2] = { vec3(-43.3999,-128.899,-97.7991),vec3(0,0,0) }, homeRot[2] = {vec3(0,0,0),vec3(0,0,0)};

 //mew��Ÿ
 vec3 mewPos[3], mewRot[3];
 float mewrotangle=0;
 float mewwingrot = 0;





