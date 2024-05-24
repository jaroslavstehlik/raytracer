# BVH

## Data

Triangle structure
- Start index of triangle
- Indexes of each vertex
- Centroid of triangle

Node Array

## Build

Create triangles from array of indicies.\

## Node subdivision
- Iterate each triangle.
- Encapsulate each triangle in bounding box
- Store triangle index in NodeArray
### Split bounding box
- When bounding box is horizontal, slice vertically and vice versa.
- Recursively repeat node subdivision

## Intersect