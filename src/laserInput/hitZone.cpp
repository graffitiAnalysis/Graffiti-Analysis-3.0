#include "hitZone.h"

//--------------------------------------------------
bool bInsidePoly(float x, float y, const vector<ofxVec2f> & polygon){
	int counter = 0;
	int i;
	double xinters;
	ofPoint p1,p2;

	int N = polygon.size();

	p1 = polygon[0];
	for (i=1;i<=N;i++) {
		p2 = polygon[i % N];
		if (y > MIN(p1.y,p2.y)) {
		  if (y <= MAX(p1.y,p2.y)) {
			if (x <= MAX(p1.x,p2.x)) {
			  if (p1.y != p2.y) {
				xinters = (y-p1.y)*(p2.x-p1.x)/(p2.y-p1.y)+p1.x;
				if (p1.x == p2.x || x <= xinters)
				  counter++;
			  }
			}
		  }
		}
		p1 = p2;
	}

	if (counter % 2 == 0) return false;
	else return true;
}


//---------------------------
hitZone::hitZone(){
	bActive = true;		
	points.assign(4, ofxVec2f());	
	bState = false;
}

//---------------------------
void hitZone::setActive(bool active){
	bActive = active;			
}

//---------------------------
bool hitZone::getActive(){
	return bActive;			
}

//---------------------------
bool hitZone::isHit(float x, float y){
	if( !bActive  ){
		bState = false;
		return false;
	}

	bool bHit = bInsidePoly(x, y, points);
	
	printf("%f %f bHit is %i\n",x, y, bHit);
	
	if( bHit ){
		bState = !bState;
	}
	
	return bHit;
}

//---------------------------
void hitZone::setPosition(int which, float x, float y){
	
	if( which >= 0 && which < points.size() ){
		points[which].set(x, y);
	}

	updatePoints();			
}

//---------------------------
void hitZone::updatePoints(){

	//lets check that the points don't lie outside our range 0-1			
	for(int i = 0; i < 4; i++){
		if(points[i].x > 1.0) points[i].x = 1.0;
		if(points[i].x < 0.0) points[i].x = 0.0;
		if(points[i].y > 1.0) points[i].y = 1.0;
		if(points[i].y < 0.0) points[i].y = 0.0;
	}			
							
}

