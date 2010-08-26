#ifndef _HIT_ZONE_H
#define _HIT_ZONE_H

#include "ofMain.h"
#include "ofxVectorMath.h"

class hitZone{

	public:
	
		hitZone();
		void setActive(bool active);
		bool getActive();
		bool isHit(float x, float y);
		void setPosition(int which, float x, float y);
		void updatePoints();
		
		ofxVec2f zoneDimensions[2];
		vector <ofxVec2f> points;
		bool bActive;
		
		bool bState;
		
};

#endif