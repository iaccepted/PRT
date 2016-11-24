#ifndef _GRID_H_
#define _GRID_H_
#include <climits>
#include "global.h"
#include "Ray.h"
#include "AABB.h"

class Object;

int clamp(int val, int low, int high);

class Grid
{
	struct Cell {
		// TODO remove model
		Cell(Object* model) : model(model) {}
		void insert(uint32_t idx) { triangles.push_back(idx); };
		int intersect(Ray& ray) const;

		std::vector<uint32_t> triangles;	// tris id in model->indices
		Object* model;
	};
public:
	Grid(Object* model);
	~Grid() {
		// TODO
	}
	int intersect(Ray& ray) const;
	uint32_t nCell[3];		// 
	vec3 cellSize;			// width of cells
	int ncell;				// total # of cells
	AABB bbox;
	Cell** cells;
	Object* model;

private:
	int posToCell(const vec3& p, int axis) const {
		int v = int((p[axis] - bbox.minp[axis]) / cellSize[axis]);
		return clamp(v, 0, nCell[axis] - 1);
	}
	float cellToPos(int p, int axis) const {
		return bbox.minp[axis] + p * cellSize[axis];
	}

	int offset(int x, int y, int z) const {
		//assert(x < nCell.x && y < nCell.y && z < nCell.z);
		return z*nCell[0] * nCell[1] + y*nCell[0] + x;
	}
};

#endif