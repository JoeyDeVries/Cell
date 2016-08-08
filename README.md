Cell - Graphics engine
======
Welcome to the development repository of Cell, an open-source OpenGL graphics engine aimed to serve as an educational repository 
for learning how a larger graphics engine can be structured and organized. A large focus of Cell is building an engine that is both easy 
to use and read. Cell also aims to provide both flexible and properly maintainable code.

![Logo of Cell Graphics Engine](logo.png "Cell Graphics Engine Logo")

The engine will be heavily commented and documented from within the source code itself, to motivate new graphics programmers
to take a look at any of its inner workings, without feeling too overwhelming. 

Cell is currently at the beginning stages of development, so expect an incomplete experience if you try to dive in.

Feature List (complete)
------
* Fully functioning (custom) math library (vectors, matrices, transformations, utility functions):
	1. Linear algebra: n-dimensional vectors, nxn-dimensional matrices, transformations, utility. 

Feature List (work-in-progress)
------
* Fully functioning (custom) math library (vectors, matrices, transformations, utility functions):
	2. Trigonemetry: unit circle utility functions, radian/degree conversions.
	3. Utility functions: lerp, step, smoothstep, smootherstep, clamp etc.
	4. Convert math to compiler intrinsics (SSE2); SIMD applicable (keep SIMD in CPU registers w/ [__vectorcall](http://www.codersnotes.com/notes/maths-lib-2016/))?
* Engine utility namespace:
	1. Random numbers.
	2. Logging.
	3. High-precision performance analysis and measurement.
	4. TrueType fonts to font character sheet (Use STB_TrueType).
* Flexible OpenGL initialization.
	
Feature List (todo)
------
0. Configure and enable debugging and logging utility.
1. Shader object w/ custom pre-processor (defines, includes):
	1. Include other shader files.
	2. Support multiple build paths.
	3. Re-compilation during runtime?
	4. Parse active vertex attributes and uniform variables, retrieve location and store in shader object.
	5. Easy to use state configuration per shader.
2. Material pipeline; configure render state in materials, together with shader, uniform configuration and samplers used.
	1. Allow for easy access of materials and storage; include set of default materials (default material, glass material etc.)
3. Define Mesh object and encompassing Model class.
	1. Define set of basic shape mesh generation; plane, cube, disk, sphere, cylinder, torus (donut)
	2. Load artist defined 3D models w/ Assimp (pre-process to custom binary format?).
4. Add Camera functionality; add multiple camera variations: FPS, FlightSim, ArcBall.
5. Flexible Framebuffer configuration utility:
	1. Including CubeMap use and generation (reflection probes, point shadow-maps)
	2. Post-processing.
6. Create Render Buffer, draw commands encapsulated in push to render buffer w/ relevant state:
	1. Define list of render push commands (material, mesh)
	2. Collect all render commands and seperate by pass.
	3. Sort render commands per pass; execute (batch where necessary) to minimize state changes.
7. Configure Deferred rendering pipeline:
	1. Set up light architecture; types of light, material interaction, volume calculation.
	2. Configure render buffer w/ geometry pass.
	3. Build fully functional shaders w/ Blinn-Phong lighting.
	4. Normal mapping.
	5. Fog.
	6. Shadow Mapping (default, omni, CSM).
	7. Environment lighting.
	8. Environment reflection support.
	9. Per-object motion blur.
	10. Number of lights optimization.
8. Fully functioning PBR render pipeline.
9. Optimization:
	1. Per-object Frustum culling.
	2. Spatial partitioning.
	3. Portal rendering (overkill perhaps)?
	
