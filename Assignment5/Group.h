#ifndef GROUP_H
#define GROUP_H


#include "Object3D.h"
#include "Ray.h"
#include "Hit.h"
#include <iostream>

using  namespace std;

///TODO: 
///Implement Group
///Add data structure to store a list of Object* 
class Group:public Object3D
{
public:

  Group() : Object3D(NULL) {}
	
  Group ( int num_objects ) {}

  ~Group(){
   
  }

  virtual bool intersect( const Ray& r , Hit& h , float tmin ) {
	  
	  // declare intersect flag variable
	  bool flag; 

	  flag = false; // init intersect flag to false
	  // loop over all objects
	  for (int i = 0; i < objects.size(); i++) {

		  // set flag to true if intersection occurs
		  if (objects[i]->intersect(r, h, tmin)) { 
			  flag = true;
		  }
	  }

	  return flag;
   }
	
  void addObject( int index , Object3D* obj ) {
	  this->objects.push_back(obj);
  }

  int getGroupSize() { 
	  return this->objects.size();
  }

 private:
	 std::vector<Object3D*> objects;
};

#endif
	
