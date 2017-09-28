Cell - Graphics engine
======
Welcome to the development repository of Cell, an open-source OpenGL graphics engine aimed to serve as an educational repository 
for learning how a larger graphics engine can be structured and organized. A large focus of Cell is building an engine that is both easy 
to use and read. Cell also aims to provide both flexible and properly maintainable code.

![Logo of Cell Graphics Engine](logo.png "Cell Graphics Engine Logo")

The engine will be heavily commented and documented from within the source code itself, to motivate new graphics programmers
to take a look at any of its inner workings, without feeling too overwhelming. 

Feature List (complete:base-functionality)
------
* Fully functioning (custom) math library (vectors, matrices, transformations, utility functions):
	1. Linear algebra: n-dimensional vectors, nxn-dimensional matrices, transformations, utility. 
	2. Trigonemetry: unit circle utility functions, radian/degree conversions.
	3. Utility functions: lerp, step, smoothstep, smootherstep, clamp etc.
    
* Engine utility namespace:
	1. High-precision performance analysis and measurement.
    2. Random numbers.

0. Configure and enable debugging and logging utility.
1. Shader object w/ custom pre-processor (defines, includes):
	1. Include other shader files.
	2. Parse active vertex attributes and uniform variables, retrieve location and store in shader object.
	3. Easy to use state configuration per shader.
    4. Support multiple build paths.
2. Material pipeline; configure render state in materials, together with shader, uniform configuration and samplers used.
	1. Allow for easy access of materials and storage; include set of default materials (default material, glass material etc.)
3. Define Mesh object and encompassing Model class.
	1. Define set of basic shape mesh generation; plane, cube, disk, sphere, cylinder, torus (donut)
    2. Load artist defined 3D models w/ Assimp.
        - Note: Assimp is built as a static library that is statically linked/merged with Cell. I also built the static zlib library for the ingration to work properly with Assimp (both 32 bit).
5. Flexible Framebuffer configuration utility:
	1. Including CubeMap use and generation (reflection probes, point shadow-maps)
	2. Post-processing.
6. Create Render Buffer, draw commands encapsulated in push to render buffer w/ relevant state:
	1. Define list of render push commands (material, mesh)
    2. Collect all render commands and seperate by pass.
    3. Sort render commands per pass; execute (batch where necessary) to minimize state changes.
7. Configure Deferred rendering pipeline:
    1. Configure render buffer w/ geometry pass.
    2. Build fully functional shaders w/ Blinn-Phong (now PBR) lighting.
    3. Normal mapping.
    4. Environment lighting.
	5. Environment reflection support.
    6. Number of lights optimization.
    7. Per-object motion blur.
8. Functioning PBR render pipeline.
	1. Calculate diffuse integral and store into cubemap.
	2. Prefilter environment map w/ integral and store into cubemap as diffuse integral.
	3. Pre-calculate active BRDF in 2D LUT (approach by Unreal for split sum approximation).
    4. Irradiance reflection probe(s): pre-calculate environment lighting approximation at any scene location.
9. Post-processing
    1. HDR/Gamma/Tone-mapping
    2. Bloom
    3. SSR
    4. Vignette
    5. SSAO 
10. Optimization:
    1. Store material-independent uniforms in UBO; together with barely changing shader parameters.   
	2. Per-object Frustum culling.    
	
Feature List (work-in-progress)
------
* Engine utility namespace:
    1. Logging.
* Flexible OpenGL initialization.

1. Add Camera functionality; add multiple camera variations: FPS, FlightSim, ArcBall.
2. Configure Deferred rendering pipeline:	
	1. Fog.
	2. Shadow Mapping (default, omni, CSM).
8. GUI
    1. IMGUI
		
Feature List (todo)
------

10. Optimization:
	1. Spatial partitioning.
	2. Portal rendering (overkill perhaps)?
    3. Cache GL state.
12. Skeletal animation:
	1. Define Bone structure on top of current scene hierarchy system.
	2. Load skeletal mesh from Assimp as skeletal hierarchy as well? Think of different design solutions that would work well in current environment.
	3. Animation blending.
