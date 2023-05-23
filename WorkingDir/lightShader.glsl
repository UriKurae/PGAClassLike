///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
#ifdef LIGHT_SHADER

#if defined(VERTEX) ///////////////////////////////////////////////////

layout(location=0) in vec3 aPosition;
layout(location=1) in vec3 aNormal;
layout(location=2) in vec2 aTexCoord;
layout(location=3) in vec3 aTangent;
layout(location=4) in vec3 aBiTangent;

out vec2 vTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	vTexCoord = aTexCoord;
	gl_Position = projection * view * model * vec4(aPosition, 1.0);
}

#elif defined(FRAGMENT) ///////////////////////////////////////////////

in vec2 vTexCoord;

layout(location=0) out vec4 oColor;
layout(location=1) out vec4 normal;
layout(location=2) out vec4 position;
layout(location=3) out vec4 specularColor;
layout(location=4) out vec4 brightColor;

uniform vec3 lightColor;
uniform vec3 intensity;

void main()
{
	oColor = vec4(lightColor * intensity, 1.0);
	specularColor = vec4(lightColor * intensity, 0.5);

	float brightness = dot(oColor.rgb, vec3(0.2126, 0.7152, 0.0722));
	if (brightness > 0.5)
		brightColor = vec4(oColor.rgb, 1.0);
	else
		brightColor = vec4(0.0, 0.0,0.0,1.0);
}

#endif
#endif


// NOTE: You can write several shaders in the same file if you want as
// long as you embrace them within an #ifdef block (as you can see above).
// The third parameter of the LoadProgram function in engine.cpp allows
// chosing the shader you want to load by name.
