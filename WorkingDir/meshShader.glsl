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
uniform int renderMode;

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

layout(location=0) out vec4 albedoColor;
layout(location=1) out vec4 normalColor;
layout(location=2) out vec4 positionColor;
layout(location=3) out vec4 specularColor;

vec3 CalcDirLight(vec3 normal, Light dirLight, vec3 viewDirection);
vec3 CalcPointLight(vec3 normal, Light pointLight, vec3 viewDirection);

void main()
{
	vec4 textureColor = texture(uTexture, vTexCoord);

	if (renderMode == 0)
	{
		for (int i = 0; i < uLightCount; ++i)
		{
			if (uLight[i].type == 0)
			{
				vec3 norm = normalize(vNormal);
				vec3 viewDir = normalize(uCameraPosition - vPosition);
				
				vec3 finalLight = CalcDirLight(norm, uLight[i], viewDir);

				textureColor.rgb += finalLight * textureColor.rgb;
			}
			else if (uLight[i].type == 1)
			{
				vec3 norm = normalize(vNormal);
				vec3 viewDir = normalize(uCameraPosition - vPosition);

				vec3 finalLight = CalcPointLight(norm, uLight[i], viewDir);

				textureColor.rgb += finalLight * textureColor.rgb;
				
			}
		}
	}	
	// Store albedo color
	albedoColor.rgb = textureColor.rgb;

	normalColor = vec4(vec3(vNormal), 1.0);

	positionColor = vec4(vPosition, 1.0);

	// Store albedo and specular component
	specularColor.rgb = texture(uTexture, vTexCoord).rgb;
	
	// If there's texture use the first one, if not, the second
	//specularColor.a = texture(uTexture, vTexCoord).r;
	specularColor.a = 0.5;
}

vec3 CalcDirLight(vec3 normal, Light dirLight, vec3 viewDirection)
{
	vec3 lightDir = normalize(dirLight.direction);
	
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = diff * dirLight.color;
	
	float ambientStrength = 0.1;
	vec3 ambientLight = ambientStrength * dirLight.color;
	
	float specularStrength = 0.5;
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDirection, reflectDir), 0.0), 128.0);
	vec3 specularLight = specularStrength * spec * dirLight.color;

	return diffuse + ambientLight + specularLight;
}

vec3 CalcPointLight(vec3 normal, Light pointLight, vec3 viewDirection)
{
	vec3 lightDir = normalize(pointLight.position - vPosition);
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = diff * pointLight.color;
	
	float ambientStrength = 0.1;
	vec3 ambientLight = ambientStrength * pointLight.color;
	
	float specularStrength = 0.5;
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDirection, reflectDir), 0.0), 128.0);
	vec3 specularLight = specularStrength * spec * pointLight.color;

	return diffuse + ambientLight + specularLight;
}

#endif
#endif


// NOTE: You can write several shaders in the same file if you want as
// long as you embrace them within an #ifdef block (as you can see above).
// The third parameter of the LoadProgram function in engine.cpp allows
// chosing the shader you want to load by name.