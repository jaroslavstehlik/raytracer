Rasteriser
- Cull scene models based on camera frustum and model AABB
	- For each model in frustum
		- Load material
			- Visit each triangle
				- Rasterise each triangle using material

Input
	MVP, Model View Projection matrix
	Triangles
	Materials

Process
	Multiply each triangle with MVP
	obtain shading program
	Rasterise each triangle using shading program

Output
	Image on the screen

- Shading programs should be merged in to batches.
- Sort opaque programs from closest to furthest
- Render opaque programs first
- Sort transparent programs from closet to furthest
- Render transparent programs and accumulate color and alpha
- Render post effects