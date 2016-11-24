#include "Grid.h"
#include "Object.h"

inline int clamp(int val, int low, int high) {
	if (val < low) return low;
	else if (val > high) return high;
	else return val;
}

Grid::Grid(Object* model) : model(model), cells(NULL)
{
	// compute aabb of the scene
	int totalNumTriangles = model->indices.size() / 3;
	bbox.expand(model->aabb.maxp);
	bbox.expand(model->aabb.minp);
	// create the grid
	vec3 delta = bbox.maxp - bbox.minp;
	//cout << "bbox.min = (" << bbox.min.x << ", " << bbox.min.y << ", " << bbox.min.z << ") max = (" << bbox.max.x << ", " << bbox.max.y << ", " << bbox.max.z << ")\n";
#if 1
	uint8_t maxAxis = bbox.majorAxis();
	float invMaxSize = 1.f / delta[maxAxis];
	assert(invMaxSize > 0);
	float cubeRoot = 3.f * powf(float(totalNumTriangles), 1.f / 3.f);
	float cellsPerUnitDist = cubeRoot * invMaxSize;
	for (uint8_t axis = 0; axis < 3; ++axis) {
		nCell[axis] = int(std::floor(delta[axis] * cellsPerUnitDist));
		nCell[axis] = clamp(nCell[axis], 1, 128);
	}
#else
	float cubeRoot = powf(5 * totalNumTriangles / (delta[0] * delta[1] * delta[2]), 1 / 3.f);
	cout << "cubeRoot = " << cubeRoot << endl;
	for (uint8_t i = 0; i < 3; ++i) {
		nCell[i] = std::floor(delta[i] * cubeRoot);
		nCell[i] = std::max(uint32_t(1), std::min(nCell[i], uint32_t(128)));
	}
#endif
	cellSize = vec3(delta.x / nCell[0], delta.y / nCell[1], delta.z / nCell[2]);
	//cout << "nCell    " << nCell[0] << ' ' << nCell[1] << ' ' << nCell[2] << endl; 
	//cout << "cellSize " << cellSize.x << ' ' << cellSize.x << ' ' << cellSize.x << endl; 
	// allocate memory
	ncell = nCell[0] * nCell[1] * nCell[2];
	//cout << "ncell    " << ncell << endl;
	cells = new Cell*[ncell];
	//for (int i = 0; i < ncell; i++)
	//	cells[i] = new Cell(model);
	// set all pointers to NULL
	memset(cells, 0x0, sizeof(Grid::Cell*) * ncell);

	// insert all the triangles in the cells
	for (unsigned int i = 0; i < model->indices.size(); i += 3) {
		int idx1 = model->indices[i];
		int idx2 = model->indices[i + 1];
		int idx3 = model->indices[i + 2];
		const vec3& v1 = model->vertices[idx1].position;
		const vec3& v2 = model->vertices[idx2].position;
		const vec3& v3 = model->vertices[idx3].position;
		AABB box;
		box.expand(v1);
		box.expand(v2);
		box.expand(v3);

		// convert to cell coordinates
		int vmin[3], vmax[3];
		for (int axis = 0; axis < 3; axis++) {
			vmin[axis] = posToCell(box.minp, axis);
			vmax[axis] = posToCell(box.maxp, axis);
		}

		// loop over all the cells the triangle overlaps and insert
		for (int z = vmin[2]; z <= vmax[2]; ++z) {
			for (int y = vmin[1]; y <= vmax[1]; ++y) {
				for (int x = vmin[0]; x <= vmax[0]; ++x) {
					int o = offset(x, y, z);
					if (cells[o] == NULL)
						cells[o] = new Cell(model);
					cells[o]->insert(i);
				}
			}
		}
	}

	unsigned int maxn = 0;
	for (int i = 0; i < ncell; i++)
	if (cells[i] != NULL && maxn < cells[i]->triangles.size())
		maxn = cells[i]->triangles.size();
	cout << "	max # in grid: " << maxn << endl;
}

const float EPSILON2 = 0.0001f;

bool intersectTriangle(Ray &r, vec3 &v0, vec3& v1, vec3& v2, float &t, float &u, float &v)
{
	static uint32_t ntimes = 0;
	ntimes++;
	//cout << ntimes << endl;
	vec3 edge1 = v1 - v0;
	vec3 edge2 = v2 - v0;
	vec3 pvec = glm::cross(r.direction, edge2);
	float det = glm::dot(edge1, pvec);
	if (det > -EPSILON2 && det < EPSILON2) return false;
	float invDet = 1 / det;
	vec3 tvec = r.orig - v0;
	u = glm::dot(tvec, pvec) * invDet;
	if (u < 0 || u > 1) return false;
	vec3 qvec = glm::cross(tvec, edge1);
	v = glm::dot(r.direction, qvec) * invDet;
	if (v < 0 || u + v > 1) return false;
	t = glm::dot(edge2, qvec) * invDet;
	return true;
}

int Grid::Cell::intersect(Ray&ray) const
{
	//cout << "Grid::Cell::intersect" << endl;
	//float uhit, vhit;
	for (uint32_t i = 0; i < triangles.size(); ++i) {
		//cout << "triangles.size() = " << triangles.size() << endl;
		int index = triangles[i];
		uint32_t idx0 = model->indices[index];
		uint32_t idx1 = model->indices[index + 1];
		uint32_t idx2 = model->indices[index + 2];

		//assert(idx0 >=0 && idx0 < model->vertices.size() && idx1);
		vec3& v0 = model->vertices[idx0].position;
		vec3& v1 = model->vertices[idx1].position;
		vec3& v2 = model->vertices[idx2].position;

		if (ray.intersectTriangle(v0, v1, v2)) {
			return index;
		}
	}
	return -1;
}

int Grid::intersect(Ray& ray) const
{
	//cout << "Grid::intersect";
	double rayT;
	if (bbox.isPointInside(ray(ray.tmin)))
		rayT = ray.tmin;
	else if (!bbox.isIntersect(ray, &rayT))
		return false;
	vec3 gridIntersect = ray(rayT);

	Ray r(ray);

#if 1
	// set up 3D DDA for ray
	float nextCrossingT[3], deltaT[3];
	int step[3], out[3], pos[3];
	for (int axis = 0; axis < 3; axis++) {
		// compute current cell for axis
		pos[axis] = posToCell(gridIntersect, axis);
		if (ray.direction[axis] >= 0) {
			nextCrossingT[axis] = rayT + (cellToPos(pos[axis] + 1, axis)
				- gridIntersect[axis]) / ray.direction[axis];
			deltaT[axis] = cellSize[axis] / ray.direction[axis];
			step[axis] = 1;
			out[axis] = nCell[axis];
		}
		else {
			// handle ray with negative direction for cell stepping
			nextCrossingT[axis] = rayT + (cellToPos(pos[axis], axis)
				- gridIntersect[axis]) / ray.direction[axis];
			deltaT[axis] = -cellSize[axis] / ray.direction[axis];
			step[axis] = -1;
			out[axis] = -1;
		}
	}

	// walk ray through cell grid
	bool hitsomething = false;
	while (true) {
		// check for intersection in current cell and advance to next
		Cell* cell = cells[offset(pos[0], pos[1], pos[2])];
		//if (cell != NULL)		// 2 minutes
		//	//hitsomethin |= cell->insert(ray, isect);
		//	hitsomething |= cell->intersect(ray);
		if (cell != NULL) {		// 57 s
			//hitsomethin |= cell->insert(ray, isect);
			int index = cell->intersect(ray);
			if (index > -1)
				return index;
		}

		// advance to next voxel
		// find _stepaxis_ for stepping to next voxel
		int bits = ((nextCrossingT[0] < nextCrossingT[1]) << 2) +
			((nextCrossingT[0] < nextCrossingT[2]) << 1) +
			((nextCrossingT[1] < nextCrossingT[2]));
		const int cmptoaxis[8] = { 2, 1, 2, 1, 2, 2, 0, 0 };
		int stepaxis = cmptoaxis[bits];
		if (ray.tmax < nextCrossingT[stepaxis])
			break;
		pos[stepaxis] += step[stepaxis];
		if (pos[stepaxis] == out[stepaxis])
			break;
		nextCrossingT[stepaxis] += deltaT[stepaxis];
	}
	return hitsomething;

#else

	// initialization step
	glm::ivec3 exit, step, cell;
	vec3 deltaT, nextCrossingT;
	for (uint8_t i = 0; i < 3; ++i) {
		// convert ray starting point to cell coordinates
		float rayOrigCell = ((r.source[i] + r.direction[i] * r.tmin) - bbox.min[i]);
		cell[i] = clamp(std::floor(rayOrigCell / cellSize[i]), 0, nCell[i] - 1);
		if (r.direction[i] < 0) {
			deltaT[i] = -cellSize[i] / r.direction[i];
			nextCrossingT[i] = r.tmin + (cell[i] * cellSize[i] - rayOrigCell) / r.direction[i];
			exit[i] = -1;
			step[i] = -1;
		}
		else {
			deltaT[i] = cellSize[i] / r.direction[i];
			nextCrossingT[i] = r.tmin + ((cell[i] + 1)  * cellSize[i] - rayOrigCell) / r.direction[i];
			exit[i] = nCell[i];
			step[i] = 1;
		}
	}

	// walk through each cell of the grid and test for an intersection if 
	// current cell contains geometry
	// const Object *hitObject = NULL;
	while (1) {
		//uint32_t o = cell[2] * nCell[0] * nCell[1] + cell[1] * nCell[0] + cell[0];
		int o = offset(cell[0], cell[1], cell[2]);
		assert(o >= 0 && o<ncell);
		if (cells[o] != NULL) {
			if (cells[o]->intersect(ray))
				return true;
			//if (hitObject != NULL) { ray.color = cells[o]->color; }
		}

		// to next cell
		uint8_t k =
			((nextCrossingT[0] < nextCrossingT[1]) << 2) +
			((nextCrossingT[0] < nextCrossingT[2]) << 1) +
			((nextCrossingT[1] < nextCrossingT[2]));
		static const uint8_t map[8] = { 2, 1, 2, 1, 2, 2, 0, 0 };
		assert(k<8);
		uint8_t axis = map[k];

		if (ray.tmax < nextCrossingT[axis]) break;
		cell[axis] += step[axis];
		if (cell[axis] == exit[axis]) break;
		nextCrossingT[axis] += deltaT[axis];
	}
	return false;
	//return hitObject;
#endif
}