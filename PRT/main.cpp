#include <iostream>

#include "Sampler.h"
#include "Scene.h"
#include "PRTRenderer.h"
#include "Global.h"

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

using namespace std;

const int sqrtNumSamples = 50;
const int nBands = BAND;


const int NMESH = 4;

void hello()
{
	std::cout << "Hello world, I'm a thread!" << std::endl;
}

int main()
{
	//PRTRenderer prtRender(600, 600);

	Sampler sampler;
	sampler.generateSamples(sqrtNumSamples, nBands);

	Light light(nBands);

	light.directLight(sampler);
	//light.rotateLightCoeffs(43.3, 225.0);
	////light.lightFromImage("./Environments/uffizi_probe.pfm", sampler, nBands);
	const char *names[6] = { "sphere.obj", "walla.obj", "wallb.obj", "wallc.obj" };


	Scene scene;
	//
	////this part: load scene and generate coefficients.  Now we store this information into a file
	/*char path[128];
	for (int i = 0; i < NMESH; ++i)
	{
		sprintf(path, "./%s/%s", "models", names[i]);
		scene.addModelFromFile(path);
	}

	bool ret = scene.generateCoeffs(sampler, nBands);

	if (!ret)
	{
		exit(-2);
	}*/
	

	//precomputation will cost lots of time, so we can save and load the coefficients to file
	//scene.saveAllData(nBands);
	scene.loadAllData(nBands);
	//////scene.printFirst(2, 20);


	PRTRenderer prtRender(600, 600);
	prtRender.compileShaderFromFile("./shaders/verShader.glsl", "./shaders/fragShader.glsl");
	prtRender.setUniform();
	prtRender.renderSceneWithLight(scene, light);
	return 0;
}


//#include <iostream>
//#include <list>
//#include <CGAL/Simple_cartesian.h>
//#include <CGAL/AABB_tree.h>
//#include <CGAL/AABB_traits.h>
//#include <CGAL/AABB_triangle_primitive.h>
//
//typedef CGAL::Simple_cartesian<double> K;
//typedef K::FT FT;
//typedef K::Ray_3 Ray;
//typedef K::Line_3 Line;
//typedef K::Point_3 Point;
//typedef K::Triangle_3 Triangle;
//typedef std::list<Triangle>::iterator Iterator;
//typedef CGAL::AABB_triangle_primitive<K, Iterator> Primitive;
//typedef CGAL::AABB_traits<K, Primitive> AABB_triangle_traits;
//typedef CGAL::AABB_tree<AABB_triangle_traits> Tree;
//int main()
//{
//	Point a(1.0, 0.0, 0.0);
//	Point b(0.0, 1.0, 0.0);
//	Point c(0.0, 0.0, 1.0);
//	Point d(0.0, 0.0, 0.0);
//	std::list<Triangle> triangles;
//	triangles.push_back(Triangle(a, b, c));
//	//triangles.push_back(Triangle(a, b, d));
//	//triangles.push_back(Triangle(a, d, c));
//	// constructs AABB tree
//	Tree tree(triangles.begin(), triangles.end());
//	// counts #intersections
//
//	Point s(0.0, 0.0, 0.0);
//	Point e(0.1, -0.1, 0.1);
//	Ray ray_query(s, e);
//	std::cout << tree.do_intersect(ray_query) << std::endl;
//	
//	return EXIT_SUCCESS;
//}