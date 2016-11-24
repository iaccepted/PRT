#ifndef _GLOBAL_H_
#define _GLOBAL_H_


#include <iostream>
#include <stdlib.h>
#include <glm/glm.hpp>
#include <vector>
#include <time.h>
#include <float.h>
#include <GL/glew.h>


using std::cout;
using std::endl;
using std::cerr;
using std::string;
using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::mat4;
using glm::mat3;

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifndef EPSILON
#define EPSILON 0.01f
#endif

#ifndef BAND
#define BAND 3
#endif

#if 0

#ifndef BOUNCE_NUM
#define BOUNCE_NUM 3
#endif

#endif

#endif