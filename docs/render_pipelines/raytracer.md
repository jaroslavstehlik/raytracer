Raytracer
- For each ray per pixel.
	- Traverse scene
		- Visit each model
			- Load material
				- Visit each triangle
					- Render intersecting triangle using material

Input
	world space acceleration structure
	shading program

Process
	find triangle intersecting ray
	obtain shading program
	program spawns other rays or returns a color;

Output
	Accumulated color in a texture