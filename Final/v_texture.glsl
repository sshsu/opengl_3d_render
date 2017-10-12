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

out vec3 N;//�任�����ķ�����
out vec3 V;
out vec3 lightPosCamSpa;
out vec2 texCoord;
out  vec3 VN;//ԭ���ķ�����

void main()
{
	//��������
    texCoord   = vTexCoord;
	VN=vNormal;

	//��ǰ���Ƶ㲻����Ӱ��ʱ��
	if(kind.x == 0.0 || kind.x==2.0){//���ǻ���Ӱ��ʱ��
	//������任���������ϵ�£���ͶӰ�淶��
		gl_Position = modelViewProjMatrix * vec4(vPosition, 1.0);	
	//��ȡ��������µĶ���λ��
		vec4 vertPos_cameraspace = modelViewMatrix * vec4(vPosition, 1.0);
	//�����V��N��������fshader
		V = vertPos_cameraspace.xyz / vertPos_cameraspace.w;
		N = (modelViewMatrix * vec4(vNormal, 0.0)).xyz;

	 //����Դ��仯���������ϵ�£�������fshader.
		vec4 temp = viewMatrix * vec4(lightPos,1.0);
		lightPosCamSpa = vec3(temp.x/temp.w,temp.y/temp.w,temp.z/temp.w);
		
	}
	else if(kind.x == 1.0){
	//�������������µ�ͶӰ������
		gl_Position = modelViewProjMatrix  * vec4(vPosition, 1.0);
	//Ͷ͸�ӳ���
		gl_Position = vec4(gl_Position.xyz / gl_Position.w,1.0);
	}



}
