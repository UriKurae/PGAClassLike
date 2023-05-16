///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
#ifdef QUAD_DEFERRED

#if defined(VERTEX) ///////////////////////////////////////////////////

layout(binding = 1, std140) uniform LocalParams
{
	mat4 uWorldMatrix;
	mat4 uWorldViewProjectionMatrix;
	mat4 uWorldViewMatrix;
};

layout(location=0) in vec3 aPosition;
layout(location=1) in vec2 aTexCoord;


out vec2 TexCoords;

void main()
{
	gl_Position = vec4(aPosition.x, aPosition.y, 0.0, 1.0);
	TexCoords = aTexCoord;
}

#elif defined(FRAGMENT) ///////////////////////////////////////////////

in vec2 TexCoords;

struct Light
{
	unsigned int type;
	vec3 color;
	vec3 direction;
	vec3 position;
	vec3 intensity;
};

layout(binding = 0, std140) uniform GlobalParams
{
	vec3 uCameraPosition;
	unsigned int uLightCount;
	Light uLight[16];
};

layout(location=0) out vec4 oColor;


uniform sampler2D gColor;
uniform sampler2D gNormal;
uniform sampler2D gPosition;
uniform sampler2D gAlbedoSpec;

uniform float exposureLevel;
uniform int exposureActive;


void main()
{
	vec3 FragPos = texture(gPosition, TexCoords).rgb;
	vec3 Normal = texture(gNormal, TexCoords).rgb;
	vec3 Diffuse = texture(gAlbedoSpec, TexCoords).rgb;
	float Specular = texture(gAlbedoSpec, TexCoords).a;

	if (Normal == vec3(0.0))
	{
		discard;
	}

	// TODO: Base ambient light *Hardcoded for now, must pass uniform whenever!*
	vec3 lighting = vec3(0.0);
	vec3 viewDir = normalize(uCameraPosition - FragPos);
	Normal = normalize(Normal);
	for (int i = 0; i < uLightCount; ++i)
	{
		if (uLight[i].type == 0)
		{
			vec3 lightDir = normalize(uLight[i].direction);
			
			Normal = normalize(Normal);
			// Diffuse light
			float diff = max(dot(Normal, lightDir), 0.0);
			vec3 diffuse = diff * uLight[i].color * uLight[i].intensity;
			
			float ambientStrength = 0.1;
			vec3 ambientLight = ambientStrength * uLight[i].color;


			// Specular light
			vec3 reflectDir = reflect(lightDir, Normal);
			float spec = pow(max(dot(viewDir, reflectDir), 0.0), 128.0);
			vec3 specularLight = Specular * spec * uLight[i].color * uLight[i].intensity;
			
			
			lighting += (ambientLight + diffuse + specularLight) * Diffuse;
		}
		else if (uLight[i].type == 1)
		{
			vec3 ambient = 0.1 * uLight[i].color;

			vec3 lightDir = normalize(uLight[i].position - FragPos);
			vec3 halfwayDir = normalize(lightDir + viewDir);
			Normal = normalize(Normal);
			vec3 diffuse = max(dot(Normal, lightDir), 0.0) * uLight[i].color * uLight[i].intensity;
	
			// Specular light
			vec3 reflectDir = reflect(-lightDir, Normal);
			float spec = pow(max(dot(Normal, halfwayDir), 0.0), 128.0);
			vec3 specularLight = Specular * spec * uLight[i].color * uLight[i].intensity;
			
			float distance = length(uLight[i].position - FragPos);
			float attenuation = 1.0 / (1.0 + 0.09 * distance + 0.032 * (distance * distance));

			ambient  *= attenuation; 
			diffuse  *= attenuation;
			specularLight *= attenuation;   

			lighting += (ambient + diffuse + specularLight) * Diffuse;
		}
		
	}
	
	vec3 finalColor = lighting;

	if (exposureActive == 1)
	{
		const float gamma = 2.2;
		vec3 hdrColor = finalColor.rgb;
 
		vec3 tone = vec3(1.0) - exp(-hdrColor * exposureLevel);
		
		tone = pow(tone, vec3(1.0 / gamma));
  
		oColor = vec4(tone, 1.0);
	}
	else if(exposureActive == 0)
	{
		oColor = vec4(lighting, 1.0);
	}
}

#endif
#endif


// NOTE: You can write several shaders in the same file if you want as
// long as you embrace them within an #ifdef block (as you can see above).
// The third parameter of the LoadProgram function in engine.cpp allows
// chosing the shader you want to load by name.