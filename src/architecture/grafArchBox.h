/*
 *  grafArchBox.h
 *  GA_Interactive
 *
 *  Created by Chris on 6/6/10.
 *  Copyright 2010 csugrue. All rights reserved.
 *
 */

#pragma once
#include "ofMain.h"
#include "ofxBox2dRect.h"

class GrafArchBox : public ofxBox2dRect {
	
public:
	
	
	GrafArchBox(){
		alpha = 200;
		timeAlive = 0;
		framesAlive = 0;
		lifeSpanTime = 10;
	}
	
	void draw();
	
	void setAlpha(float a );
	void updateAlive(float dt);
	void fadeAlpha(float dt){ alpha -= dt; if(alpha < 0 ) alpha = 0; }
	
	void resetAliveTime()
	{
		timeAlive	= 0;
		framesAlive = 0;
	}
	
	void setRandomLifespan(float lo, float hi)
	{
		lifeSpanTime = ofRandom(lo,hi);
	}
	
	
	float	getTimeAlive(){ return timeAlive;}
	int		getFramesAlive(){ return framesAlive;}
	
	
	
	float lifeSpanTime;
	float timeAlive;
	int framesAlive;
	float alpha;
	
};