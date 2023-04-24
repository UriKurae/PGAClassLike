///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
#ifdef QUAD_FRAMEBUFFER

#if defined(VERTEX) ///////////////////////////////////////////////////


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

uniform sampler2D screenTexture;

uniform int renderTarget;

layout(location=0) out vec4 oColor;

void main()
{
	
	switch(renderTarget)
	{
	 case 0:
		oColor = texture(screenTexture, TexCoords);
	 break;
	 case 1:
		vec4 normalColor = texture(screenTexture, TexCoords);
		oColor = normalColor;
	 break;
	 case 2:
		float depth = texture(screenTexture, TexCoords).r;
		oColor = vec4(vec3(depth), 1.0);
	 break;
	 case 3:
		oColor = texture(screenTexture, TexCoords);
	 break;
	}
	
}

#endif
#endif


// NOTE: You can write several shaders in the same file if you want as
// long as you embrace them within an #ifdef block (as you can see above).
// The third parameter of the LoadProgram function in engine.cpp allows
// chosing the shader you want to load by name.