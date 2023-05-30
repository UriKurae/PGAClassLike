# Graphics Programming Engine

This is a rendering engine for the subject advanced graphics programming at my university CITM, UPC. The engine consists in implementing different techniques such as deferred and forward rendering. For the last assignment,
we had to choose between 2 techniques related to the subject, in my case, I've choosen Bloom and Parallax mapping.

## Deferred and Forward shading

The engine includes two ways of visualizing the scene: Forward and Deferred shading, as well as options to visualize the different images of the G-Buffer.

<img src="https://github.com/UriKurae/PGAClassLike/" >

## Bloom

The first technique we implemented is bloom, which makes excess of lighting to "Bleed" giving the illusion that it's brighter.

This is how it looks with bloom
<img src="https://github.com/UriKurae/PGAClassLike/" >

And this is how it looks without bloom

## HDR 

Bloom can work without HDR, but we got a much better effect with HDR.

This is the scene with HDR

<img src="https://github.com/UriKurae/PGAClassLike/" >

This is the scene without HDR

## Parallax

Parallax gives the illusion that there's extra geometry in a model when in reality is just flat. Very cool technique if you want extra geometry without a lot of extra cost.

Model without Parallax

<img src="https://github.com/UriKurae/PGAClassLike/" >

Model with Parallax

<img src="https://github.com/UriKurae/PGAClassLike/" >

## Controls:

Basic UI interface with ImGui straightforward. Can click all menus and numbers.
W,A,S,D to move camera forward, back, left and right.
E and Q to move camera up and down.
Right click + W,A,S,D to move camera while controlling the rotation with the mouse.
Alt + Left click to orbit around (0,0,0).
Mouse wheel to go forward or back.
Alt + middle mouse button to pan the camera.
Holding shift will increase the camera movement.

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

## Members of the team

* [Oriol Bernal Martinez](https://github.com/UriKurae)
