#ifndef RAY_TRACER_H
#define RAY_TRACER_H

#include <cassert>
#include <vector>
#include "SceneParser.h"
#include "Ray.h"
#include "Hit.h"

class SceneParser;


class RayTracer
{
public:
  
  RayTracer() {
      assert( false );
  }

  RayTracer( SceneParser* scene, int max_bounces, bool shadow_toggle ); //more arguments as you need...
  ~RayTracer();
  
  Vector3f traceRay( Ray& ray, float tmin, int bounces, float refr_index, Hit& hit ) const;


private:
  SceneParser* m_scene;

  int m_maxBounces;

  bool shadow_toggle = false;

  Group* g;

};

#endif // RAY_TRACER_H
