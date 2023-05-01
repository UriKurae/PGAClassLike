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
};

layout(binding = 0, std140) uniform GlobalParams
{
	vec3 uCameraPosition;
	unsigned int uLightCount;
	Light uLight[16];
};

layout(location=0) out vec4 oColor;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;


void main()
{
	vec3 FragPos = texture(gPosition, TexCoords).rgb;
	vec3 Normal = texture(gNormal, TexCoords).rgb;
	vec3 Albedo = texture(gAlbedoSpec, TexCoords).rgb;
	float Specular = texture(gAlbedoSpec, TexCoords).a;

	vec3 lighting = Albedo * 0.25;
	vec3 viewDir = normalize(uCameraPosition - FragPos);

	for (int i = 0; i < uLightCount; ++i)
	{
		if (uLight[i].type == 1)
		{
		vec3 lightDir = normalize(uLight[i].position - FragPos);
		vec3 diffuse = max(dot(Normal, lightDir), 0.0) * Albedo * uLight[i].color;

		lighting += diffuse;
		}
		
	}
	oColor = vec4(lighting,1.0);
}

#endif
#endif


// NOTE: You can write several shaders in the same file if you want as
// long as you embrace them within an #ifdef block (as you can see above).
// The third parameter of the LoadProgram function in engine.cpp allows
// chosing the shader you want to load by name.