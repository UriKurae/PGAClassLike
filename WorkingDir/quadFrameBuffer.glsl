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
uniform sampler2D bloomBlur;

uniform int renderTarget;
uniform float exposureLevel;
uniform int exposureActive;

layout(location=0) out vec4 oColor;

float near = 0.1; 
float far  = 100.0; 
  
float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));	
}

void main()
{
	
	switch(renderTarget)
	{
	 case 0:
	 
	 if (exposureActive == 1)
	 {
		const float gamma = 2.2;
		vec3 hdrColor = texture(screenTexture, TexCoords).rgb;
 
		vec3 tone = vec3(1.0) - exp(-hdrColor * exposureLevel);
	
		tone = pow(tone, vec3(1.0 / gamma));
  
		oColor = vec4(tone, 1.0);
		oColor += texture(bloomBlur, TexCoords);
	 }
	 else if (exposureActive == 0)
	 {
		oColor = texture(screenTexture, TexCoords);
		oColor += texture(bloomBlur, TexCoords);
	 }

	 break;
	 case 1:
		vec4 normalColor = texture(screenTexture, TexCoords);
		oColor = normalColor;
	 break;
	 case 2:
	    oColor = texture(screenTexture, TexCoords);
	 break;
	 case 3:
		oColor = vec4(vec3(texture(screenTexture, TexCoords).a), 1.0);
	 break;
	 case 4:
		// First is regular depth, second is linear one
		//float depth = texture(screenTexture, TexCoords).r;
		float depth = LinearizeDepth(texture(screenTexture, TexCoords).r) / far; // divide by far for demonstration
		oColor = vec4(vec3(depth), 1.0);
	 break;
	}
	
}

#endif
#endif


// NOTE: You can write several shaders in the same file if you want as
// long as you embrace them within an #ifdef block (as you can see above).
// The third parameter of the LoadProgram function in engine.cpp allows
// chosing the shader you want to load by name.