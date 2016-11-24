/***************************************
*author: guohongzhi  zju
*date:2015.8.22
*func: load single object
****************************************/

#ifndef _OBJECT_H_
#define _OBJECT_H_

#include "Global.h"
#include "Vertex.h"
#include "Ray.h"
#include "AABB.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <CGAL/Simple_cartesian.h>
#include <CGAL/AABB_tree.h>
#include <CGAL/AABB_traits.h>
#include <CGAL/AABB_triangle_primitive.h>

typedef CGAL::Simple_cartesian<double> K;
typedef K::FT FT;
typedef K::Point_3 Point;
typedef K::Triangle_3 Triangle;
typedef K::Ray_3 RRay;

typedef std::vector<Triangle>::iterator Iterator;
typedef CGAL::AABB_triangle_primitive<K, Iterator> Primitive;
typedef CGAL::AABB_traits<K, Primitive> AABB_triangle_traits;
typedef CGAL::AABB_tree<AABB_triangle_traits> Tree;


class Object
{
public:
	Object(){}
	void load(const char *path);
	bool doesRayHitObject(Ray& ray) const;

	void bindBuffer();
	void render();

	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;
	AABB aabb;

	void setAABB(Tree *_aabb)
	{
		this->aabbTree = _aabb;
	}

	Tree *getAABB()
	{
		return this->aabbTree;
	}

private:
	GLuint vao, vbo, ebo;

	Tree *aabbTree;
	std::vector<Triangle> triangles;
};

#endif 