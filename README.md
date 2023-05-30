# Graphics Programming Engine

This is a rendering engine for the subject advanced graphics programming at my university CITM, UPC. The engine consists in implementing different techniques such as deferred and forward rendering. For the last assignment,
we had to choose between 2 techniques related to the subject, in my case, I've choosen Bloom and Parallax mapping.

## Deferred and Forward shading

The engine includes two ways of visualizing the scene: Forward and Deferred shading, as well as options to visualize the different images of the G-Buffer.

<img src="https://github.com/UriKurae/PGAClassLike/blob/main/Pictures/Forward_001.png" >

## Bloom

The first technique we implemented is bloom, which makes excess of lighting to "Bleed" giving the illusion that it's brighter.

This is how it looks with bloom (Exaggerated)
<img src="https://github.com/UriKurae/PGAClassLike/blob/main/Pictures/WithBloom_001.png" >

And this is how it looks without bloom
<img src="https://github.com/UriKurae/PGAClassLike/blob/main/Pictures/WithoutBloom_001.png" >

* These are the options for bloom:
<img src="https://github.com/UriKurae/PGAClassLike/blob/main/Pictures/BloomOptions_001.png" >

- No iteration limits makes so that the user can surpass the 20 limit iterations. It's a safety measure since it's expensive on higher numbers, but can be surpassed at own risk.
- Bloom iterations are the iterations for the bloom, more iterations means more blur, which makes it more blurry.
- Bloom range is the number where the shader thinks the excess of light starts to count as excess. Anything above this number counts as excess of lighting. I.E.: 0.0 means anything above 0.0 would be treated as excess of lighting.

## HDR 

Bloom can work without HDR, but we got a much better effect with HDR.

This is the scene with HDR

<img src="https://github.com/UriKurae/PGAClassLike/blob/main/Pictures/WithHDR_001.png" >

This is the scene without HDR (Everything seems burned)

<img src="https://github.com/UriKurae/PGAClassLike/blob/main/Pictures/WithoutHDR_001.png" >

* These are the options for HDR:
<img src="https://github.com/UriKurae/PGAClassLike/blob/main/Pictures/HDROptions_001.png" >

- Exposure level is the amount of lighting that the HDR will give to the scene.
- Activate exposure will activate or deactivate it.

## Parallax

Parallax gives the illusion that there's extra geometry in a model when in reality is just flat. Very cool technique if you want extra geometry without a lot of extra cost.

Model without Parallax

<img src="https://github.com/UriKurae/PGAClassLike/blob/main/Pictures/Relief_002.png" >

Model with Parallax

<img src="https://github.com/UriKurae/PGAClassLike/blob/main/Pictures/Relief_001.png" >

Model without Parallax

<img src="https://github.com/UriKurae/PGAClassLike/blob/main/Pictures/WithoutRelief_001.png" >

Model with Parallax

<img src="https://github.com/UriKurae/PGAClassLike/blob/main/Pictures/WithRelief_001.png" >

* These are the options for Parallax:
<img src="https://github.com/UriKurae/PGAClassLike/blob/main/Pictures/ReliefOptions_001.png" >

- Bumpiness is the amount of "Parallax" effect that we want.
- Max and min layers effect to the quality of the parallax effect, more layers means more cost, but it looks better.

## More pictures of the techniques

All other pictures can be seen in [This link](https://github.com/UriKurae/PGAClassLike/tree/main/Pictures)

## Controls:

* Basic UI interface with ImGui straightforward. Can click all menus and numbers.
* W,A,S,D to move camera forward, back, left and right.
* E and Q to move camera up and down.
* Right click + W,A,S,D to move camera while controlling the rotation with the mouse.
* Alt + Left click to orbit around (0,0,0).
* Mouse wheel to go forward or back.
* Alt + middle mouse button to pan the camera.
* Holding shift will increase the camera movement.

## Interface buttons:

* On top there's the option "Render Targets", that can be used to show different attachments (Color, normal, position, depth...)
* Render mode toggles between Forward and Deferred rendering.
* Utils is a tab to adjust camera sensitivity, change fov
* OpenGL info just shows some info for OpenGL
* Light is a tab for lighting options like toggle debug lighting. NOTE: When in forward rendering and switching between color attachments, the debug lights disappear, it is made on purpose to have a better visualization on the color attachments, but can be activated again in the Utils tab if really needed!
* Bloom is a tab for bloom options, warning that putting more iterations on the bloom may cause trouble, it has a limit on 20, but can be surpassed.
* Bumpiness and layers for Parallax can be modified in the Entity that has them.
* All entities and lights can be moved.
* All the interface is set in a certain way, but can be moved as the user desires to.

## Shader Files
* [Mesh Shader](https://github.com/UriKurae/PGAClassLike/blob/main/WorkingDir/meshShader.glsl): How all the models are rendered (Here lies the excess of light that we need for the bloom)
* [Deferred Quad Shader](https://github.com/UriKurae/PGAClassLike/blob/main/WorkingDir/DeferredShader.glsl): The final quad for the Deferred rendering.
* [Forward Quad Shader](https://github.com/UriKurae/PGAClassLike/blob/main/WorkingDir/quadFrameBuffer.glsl): The final quad for the Forward rendering.
* [Bloom shader](https://github.com/UriKurae/PGAClassLike/blob/main/WorkingDir/bloomShader.glsl): Bloom shader where the iterations are made. This is the bloom itself, the calculations.
* [Parallax](https://github.com/UriKurae/PGAClassLike/blob/main/WorkingDir/reliefShader.glsl): Here are the calculations for the parallax mapping. Its similar to the mesh shader, but optimized for Parallax.

## Members of the team

* [Oriol Bernal Martinez](https://github.com/UriKurae)
