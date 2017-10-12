#version 330 core

in vec3 N;
in vec3 V;
in vec3 lightPosCamSpa;
in vec2 texCoord;
in vec3 VN;

uniform vec2 Kind;

out vec4 fColor;
out vec4 fNormal;

uniform sampler2D texture;

void main()
{

if( Kind.x == 0.0){//画机器模型，加上光照
	// TODO 设置三维物体的材质属性
	//获取纹理颜色,并且将其设置为物体的环境光
    fColor = texture2D( texture, texCoord );

	vec3 ambiColor = vec3(0.6, 0.6, 0.6);
	vec3 diffColor = vec3(0.7, 0.7, 0.7);
	vec3 specColor = vec3(0.7, 0.7, 0.7);

	// TODO 计算N，L，V，R四个向量并归一化
	vec3 N_norm = normalize(N);
	vec3 L_norm = normalize(lightPosCamSpa - V);
	vec3 V_norm = normalize(-V);
	vec3 R_norm = reflect(L_norm, N_norm);

	// TODO 计算漫反射系数和镜面反射系数
	float lambertian = clamp(dot(L_norm, N_norm), 0.0,1.0);
	float specular = clamp(dot(R_norm, V_norm), 0.0, 1.0);

	float shininess = 5.0;
	vec4 ambientFinal = vec4(ambiColor,1.0);
	vec4 diffuseFinal = vec4(diffColor * lambertian,1.0);
	vec4 specularFinal = vec4(specColor *pow(specular, shininess),1.0);
	
	// TODO 计算最终每个片元的输出颜色

	
	//fColor = vec4(ambiColor+ diffColor * lambertian + specColor * pow(specular, shininess), 1.0);
	//fColor = fColor * ambientFinal + fColor * diffuseFinal + fColor * specularFinal;
	fColor = fColor * (ambientFinal + diffuseFinal + specularFinal);
	fNormal = vec4(VN,0);

	}
	else if(Kind.x == 2.0){//画天空盒，不要加光照
		 fColor = texture2D( texture, texCoord );
		 fNormal = vec4(VN,0);
	}
	else if(Kind.x==1.0){//阴影
		fColor=vec4(0.7,0.7,0.7,1.0);
		fNormal = vec4(VN,0);
	}
}

