#version 330 core

in  vec3 vPosition;
in  vec3 vColor;
in  vec3 vNormal;
in  vec2 vTexCoord;

uniform mat4 modelViewProjMatrix;
uniform mat4 modelViewMatrix;
uniform mat4 viewMatrix;
uniform mat4 ProjMatrix;
uniform vec2 kind;
uniform vec3 lightPos;

out vec3 N;//变换顶点后的法向量
out vec3 V;
out vec3 lightPosCamSpa;
out vec2 texCoord;
out  vec3 VN;//原来的法向量

void main()
{
	//纹理坐标
    texCoord   = vTexCoord;
	VN=vNormal;

	//当前绘制点不是阴影的时候
	if(kind.x == 0.0 || kind.x==2.0){//不是画阴影的时候
	//将顶点变换到相机坐标系下，并投影规范化
		gl_Position = modelViewProjMatrix * vec4(vPosition, 1.0);	
	//获取相机坐标下的顶点位置
		vec4 vertPos_cameraspace = modelViewMatrix * vec4(vPosition, 1.0);
	//计算出V、N向量传入fshader
		V = vertPos_cameraspace.xyz / vertPos_cameraspace.w;
		N = (modelViewMatrix * vec4(vNormal, 0.0)).xyz;

	 //将光源点变化到相机坐标系下，并传到fshader.
		vec4 temp = viewMatrix * vec4(lightPos,1.0);
		lightPosCamSpa = vec3(temp.x/temp.w,temp.y/temp.w,temp.z/temp.w);
		
	}
	else if(kind.x == 1.0){
	//想计算相机坐标下的投影点坐标
		gl_Position = modelViewProjMatrix  * vec4(vPosition, 1.0);
	//投透视除法
		gl_Position = vec4(gl_Position.xyz / gl_Position.w,1.0);
	}



}
