/*
 *  dropParticles.h
 *  GA_Interactive
 *
 *  Created by Chris on 5/30/10.
 *  Copyright 2010 csugrue. All rights reserved.
 *
 */
#pragma once

#include "ofMain.h"

typedef struct ga_particle{
	ofPoint pos;
	ofPoint vel;
	ofPoint ori;
	float size;
	float alpha;
	bool bOn;
	int framesOn;
};

class DropParticles{

	public:
		DropParticles();
		~DropParticles();
		
		void setup(int totalParticles = 0);
		void reset();
		void clear();
		
		void resetParticle( ga_particle * p );
		void turnOnParticle( ga_particle * p, ofPoint pos, float size);
		
		void update(float dt = 1);
		void draw();
		
		void createRandomDrop( ofPoint startPos, ofPoint vel, float size );
		
		vector<ga_particle> particles;
		
		float baseSize;
		float baseAlpha;
		float baseGravity;
		float baseAlphaFade;
		float baseGrowSize;
		int maxParticles;
		
	protected:

};

