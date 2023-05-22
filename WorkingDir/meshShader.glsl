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

out VS_OUT
{
	vec3 fragPos;
	vec2 texCoords;
	vec3 tangentViewPos;
	vec3 tangentFragPos;
	mat3 tbn;
} vs_out;

uniform vec3 viewPos;

out vec2 vTexCoord;
out vec3 vPosition;
out vec3 vNormal;

void main()
{
	vs_out.fragPos = vec3(uWorldMatrix * vec4(aPosition, 1.0));
	vs_out.texCoords = aTexCoord;

	vec3 T = normalize(mat3(uWorldMatrix) * aTangent);
	vec3 B = normalize(mat3(uWorldMatrix) * aBiTangent);
	vec3 N = normalize(mat3(uWorldMatrix) * aNormal);
	mat3 TBN = transpose(mat3(T, B, N));

	vs_out.tbn = TBN;
	vs_out.tangentViewPos = TBN * viewPos;
	vs_out.tangentFragPos = TBN * vs_out.fragPos;
	
	vTexCoord = aTexCoord;
	
	vPosition = vec3(uWorldMatrix * vec4(aPosition, 1.0));
	vNormal = vec3(uWorldMatrix * vec4(aNormal, 0.0));

	gl_Position = uWorldViewProjectionMatrix * vec4(aPosition, 1.0);
}

#elif defined(FRAGMENT) ///////////////////////////////////////////////

in VS_OUT{
	vec3 fragPos;
	vec2 texCoords;
	vec3 tangentViewPos;
	vec3 tangentFragPos;
	mat3 tbn;
} fs_in;

in vec2 vTexCoord;
in vec3 vNormal;
in vec3 vPosition;

uniform sampler2D uTexture;
uniform sampler2D normalMap;
uniform sampler2D depthMap;
uniform int renderMode;
uniform int useNormal;
uniform int useDepth;

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

vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir);

void main()
{
	vec3 finalLight = vec3(0.0);

	vec3 viewDir = normalize(fs_in.tangentViewPos - fs_in.tangentFragPos);
	vec2 newTexCoords = ParallaxMapping(fs_in.texCoords,  viewDir);
	vec3 diffuse = texture(uTexture, newTexCoords).rgb;
	
	if(newTexCoords.x > 1.0 || newTexCoords.y > 1.0 || newTexCoords.x < 0.0 || newTexCoords.y < 0.0)
	{
		discard;
	}

	vec3 normal = texture(normalMap, newTexCoords).rgb;
	if (useNormal == 1)
	{
		normal = normalize(normal * 2.0 - 1.0);
	}
	else if (useNormal == 0)
	{
	    normal = vNormal;
	}
	
	

	if (renderMode == 0)
	{
		for (int i = 0; i < uLightCount; ++i)
		{
			if (uLight[i].type == 0)
			{				
				finalLight += CalcDirLight(normal, uLight[i], viewDir) * diffuse;
			}
			else if (uLight[i].type == 1)
			{
				finalLight += CalcPointLight(normal, uLight[i], viewDir) * diffuse;
			}
		}
	}	
	// Store albedo color
	albedoColor = vec4(finalLight, 1.0);

	normalColor = vec4(vec3(normal), 1.0);

	positionColor = vec4(vPosition, 1.0);

	// Store albedo and specular component
	specularColor.rgb = texture(uTexture, newTexCoords).rgb;
	
	// If there's texture use the first one, if not, the second
	//specularColor.a = texture(uTexture, newTexCoords).r;
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
	vec3 reflectDir = reflect(lightDir, normal);
	float spec = pow(max(dot(viewDirection, reflectDir), 0.0), 128.0);
	vec3 specularLight = specularStrength * spec * dirLight.color;

	return diffuse + ambientLight + specularLight;
}

vec3 CalcPointLight(vec3 normal, Light pointLight, vec3 viewDirection)
{
	
	vec3 lightDir = normalize((pointLight.position * fs_in.tbn) - fs_in.tangentFragPos);
	vec3 halfwayDir = normalize(lightDir + viewDirection);

	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = diff * pointLight.color;
	
	float ambientStrength = 0.1;
	vec3 ambientLight = ambientStrength * pointLight.color;
	
	float specularStrength = 0.5;
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(normal, halfwayDir), 0.0), 128.0);
	vec3 specularLight = specularStrength * spec * pointLight.color;

	float distance = length(pointLight.position - vPosition);
	float attenuation = 1.0 / (1.0 + 0.09 * distance + 0.032 * (distance * distance));

	ambientLight *= attenuation; 
	diffuse *= attenuation;
	specularLight *= attenuation;   

	return diffuse + ambientLight + specularLight;
}

vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir)
{
	const float minLayers = 8.0;
	const float maxLayers = 32.0;

	const float numLayers = mix(maxLayers, minLayers, abs(dot(vec3(0.0, 0.0, 1.0), viewDir)));
	float layerDepth = 1.0 / numLayers;

	float currentLayerDepth = 0.0;

	vec2 P = viewDir.xy / viewDir.z * 0.1;
	vec2 deltaTexCoords = P / numLayers;

	vec2 currentTexCoords = texCoords;
	float currentDepthMapValue = texture(depthMap, currentTexCoords).r;

	while(currentLayerDepth < currentDepthMapValue)
	{
		currentTexCoords -= deltaTexCoords;
		
		currentDepthMapValue = texture(depthMap, currentTexCoords).r;  
   
		currentLayerDepth += layerDepth;  
    }

	vec2 prevTexCoords = currentTexCoords + deltaTexCoords;

	float afterDepth = currentDepthMapValue - currentLayerDepth;
	float beforeDepth = texture(depthMap, prevTexCoords).r - currentLayerDepth + layerDepth;

    float weight = afterDepth / (afterDepth - beforeDepth);
	vec2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0 - weight);
    
	return finalTexCoords;
}

#endif
#endif


// NOTE: You can write several shaders in the same file if you want as
// long as you embrace them within an #ifdef block (as you can see above).
// The third parameter of the LoadProgram function in engine.cpp allows
// chosing the shader you want to load by name.