///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
#ifdef MESH_GEOMETRY

#if defined(VERTEX) ///////////////////////////////////////////////////

layout(binding = 1, std140) uniform LocalParams
{
	mat4 uWorldMatrix;
	mat4 uWorldViewProjectionMatrix;
	mat4 uWorldViewMatrix;
};

layout(location=0) in vec3 aPosition;
layout(location=1) in vec3 aNormal;
layout(location=2) in vec2 aTexCoord;
layout(location=3) in vec3 aTangent;
layout(location=4) in vec3 aBiTangent;

out vec2 vTexCoord;
out vec3 vPosition;
out vec3 vNormal;

void main()
{
	vTexCoord = aTexCoord;
	
	vPosition = vec3(uWorldMatrix * vec4(aPosition, 1.0));
	vNormal = vec3(uWorldMatrix * vec4(aNormal, 0.0));
	
	gl_Position = uWorldViewProjectionMatrix * vec4(aPosition, 1.0);

}

#elif defined(FRAGMENT) ///////////////////////////////////////////////

in vec2 vTexCoord;
in vec3 vNormal;
in vec3 vPosition;

uniform sampler2D uTexture;

struct Light
{
	unsigned int type;
	vec3 color;
	vec3 direction;
	vec3 position;
};

layout(binding = 0, std140) uniform GlobalParams
{
	vec3 uCameraPosition;
	unsigned int uLightCount;
	Light uLight[16];
};

layout(location=0) out vec4 oColor;
layout(location=1) out vec4 normalColor;
layout(location=2) out vec4 positionColor;
layout(location=3) out vec4 depthColor;
layout(location=4) out vec4 specularColor;

void main()
{
	vec4 textureColor = texture(uTexture, vTexCoord);

	for (int i = 0; i < uLightCount; ++i)
	{
		if (uLight[i].type == 0)
		{
			vec3 lightDir = normalize(uLight[i].direction);
			float diff = max(dot(vNormal, lightDir), 0.0);
			vec3 diffuse = diff * uLight[i].color;

			textureColor.rgb += diffuse;
		}
		else if (uLight[i].type == 1)
		{
			vec3 norm = normalize(vNormal);
			vec3 lightDir = normalize(uLight[i].position - vPosition);

			float diff = max(dot(norm, lightDir), 0.0);
			vec3 diffuse = diff * uLight[i].color;

			float ambientStrength = 0.1;
			vec3 ambientLight = ambientStrength * uLight[i].color;
			
			float specularStrength = 0.5;
			vec3 viewDir = normalize(uCameraPosition - vPosition);
			vec3 reflectDir = reflect(-lightDir, norm);
			float spec = pow(max(dot(viewDir, reflectDir), 0.0), 128.0);
			float specularLight = specularStrength * spec;

			textureColor.rgb = (diffuse + ambientLight + specularLight) * ( textureColor.rgb);
			
		}
	}
	
	
	oColor.rgb = textureColor.rgb;
	oColor.a = texture(uTexture, vTexCoord).r;
	normalColor = vec4(vec3(vNormal), 1.0);
	depthColor = vec4(vec3(texture(uTexture, vTexCoord).z), 1.0);
	positionColor = vec4(vPosition, 1.0);

	specularColor.rgb = texture(uTexture, vTexCoord).rgb;
	specularColor.a = texture(uTexture, vTexCoord).r;
}

#endif
#endif


// NOTE: You can write several shaders in the same file if you want as
// long as you embrace them within an #ifdef block (as you can see above).
// The third parameter of the LoadProgram function in engine.cpp allows
// chosing the shader you want to load by name.