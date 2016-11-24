#include "Object.h"
#include "Grid.h"

void Object::load(const char *path)
{
	Assimp::Importer importer;

	const aiScene *scene = importer.ReadFile(path, aiProcess_GenSmoothNormals);
	const char *err = importer.GetErrorString();

	cout << err << endl;

	if (!scene)
	{
		cerr << "Failed to load model!" << endl;
		exit(-2);
	}

	unsigned nVertices = 0;
	unsigned nTriangles = 0;

	unsigned nMeshes = scene->mNumMeshes;
	for (unsigned i = 0; i < nMeshes; ++i)
	{
		aiMesh *mesh = scene->mMeshes[i];
		nVertices += mesh->mNumVertices;
		nTriangles += mesh->mNumFaces;
	}

	vertices.reserve(nVertices);
	indices.reserve(3 * nTriangles);

	//aabb needs all triangles to build kdtree, we must keep triangles and tree be valid at the same time
	triangles.reserve(nTriangles);

	for (unsigned i = 0; i < nMeshes; ++i)
	{
		aiMesh *mesh = scene->mMeshes[i];

		unsigned mnVertices = mesh->mNumVertices;
		for (unsigned j = 0; j < mnVertices; ++j)
		{
			aiVector3D v = mesh->mVertices[j];
			aiVector3D n = mesh->mNormals[j];

			vertices.push_back(Vertex(v.x, v.y, v.z, n.x, n.y, n.z));
			aabb.expand(vec3(v.x, v.y, v.z));
		}

		unsigned mnFaces = mesh->mNumFaces;
		
		for (unsigned j = 0; j < mnFaces; ++j)
		{
			aiFace face = mesh->mFaces[j];
			indices.push_back(face.mIndices[0]);
			indices.push_back(face.mIndices[1]);
			indices.push_back(face.mIndices[2]);

			Vertex &a = vertices[face.mIndices[0]];
			Vertex &b = vertices[face.mIndices[1]];
			Vertex &c = vertices[face.mIndices[2]];
			Point p(a.position.x, a.position.y, a.position.z);
			Point q(b.position.x, b.position.y, b.position.z);
			Point r(c.position.x, c.position.y, c.position.z);

			
			triangles.push_back(Triangle(p, q, r));
		}
		
	}

	cout << triangles.size() << endl;
	this->aabbTree = new Tree(triangles.begin(), triangles.end());

	cout << this->aabbTree->size() << endl;
	//cout << nVertices << "--------" << nTriangles << endl;
	//cout << vertices.size() << "--------" << indices.size() << endl;
}

bool Object::doesRayHitObject(Ray &ray) const
{
	glm::vec3 &orig = ray.orig;
	glm::vec3 &dir = ray.direction;

	Point o(orig.x, orig.y, orig.z);
	Point d(orig.x + dir.x, orig.y + dir.y, orig.z + dir.z);

	RRay query_ray(o, d);

	assert(this->aabbTree != NULL);
	return this->aabbTree->do_intersect(query_ray);
	

	//////if the ray hit the object's aabb,we should make sure if it hit any triangle
	////unsigned nTriangles = indices.size() / 3;
	////for (unsigned i = 0; i < nTriangles; ++i)
	////{
	////	unsigned idx0, idx1, idx2;
	////	idx0 = indices[i * 3];
	////	idx1 = indices[i * 3 + 1];
	////	idx2 = indices[i * 3 + 2];
	////	vec3 v0 = vertices[idx0].position;
	////	vec3 v1 = vertices[idx1].position;
	////	vec3 v2 = vertices[idx2].position;

	////	if (ray.intersectTriangle(v0, v1, v2))
	////	{
	////		return true;
	////	}
	////}
	////return false;
	//return true;
}

void Object::bindBuffer()
{
	unsigned  nIndices = indices.size();

	glGenVertexArrays(1, &this->vao);
	glGenBuffers(1, &this->vbo);
	glGenBuffers(1, &this->ebo);

	glBindVertexArray(this->vao);

	glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), &this->vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * nIndices, &indices[0], GL_STATIC_DRAW);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Object::render()
{
	GLuint cnt = this->indices.size();
	glBindVertexArray(vao);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid *)offsetof(Vertex, position));
	
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid *)offsetof(Vertex, litColor));
	
	glDrawElements(GL_TRIANGLES, cnt, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
	glDeleteVertexArrays(1, &this->vao);
	glDeleteBuffers(1, &this->vbo);
	glDeleteBuffers(1, &this->ebo);

}