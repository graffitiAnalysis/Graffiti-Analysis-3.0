/*
 *  dropParticles.cpp
 *  GA_Interactive
 *
 *  Created by Chris on 5/30/10.
 *  Copyright 2010 csugrue. All rights reserved.
 *
 */

#include "dropParticles.h"

DropParticles::DropParticles()
{
	baseSize		= 10.f;
	baseAlpha		= 0.75f;
	baseGravity		= 20.f;
	baseAlphaFade	= .5f;
	baseGrowSize	= 40.f;
	
	maxParticles	= 1000;
}

DropParticles::~DropParticles()
{
	
}

void DropParticles::setup(int totalParticles)
{
	ga_particle ptemp;
	resetParticle(&ptemp);
	
	for( int i = 0; i < totalParticles; i++)
	{
		particles.push_back(ptemp);
	}
	
}

void DropParticles::resetParticle( ga_particle * p )
{
	p->pos.set(0,0,0);
	p->ori.set(0,0,0);
	p->vel.set(0,0,0);
	p->size = baseSize;
	p->bOn = false;
	p->framesOn = 0;
	p->alpha = 0;
}

void DropParticles::clear()
{
	particles.clear();
}

void DropParticles::reset()
{
	for( int i = 0; i < particles.size(); i++)
	{
		resetParticle(&particles[i]);
	}
	
}

void DropParticles::update(float dt)
{
	
	for( int i = 0; i < particles.size(); i++)
	{
		
		// if still alive, update velocity, framesOn, alpha
		if(particles[i].bOn)
		{
			particles[i].vel.y += baseGravity*dt;
			particles[i].pos += particles[i].vel;
			particles[i].framesOn++;
			
			if( /*particles[i].framesOn > 30 &&*/ particles[i].alpha > 0) //(baseAlphaFade )
				particles[i].alpha -= baseAlphaFade*dt;
				
			if( particles[i].size < baseSize )
				particles[i].size += baseGrowSize*dt;
		}
		
	}
	
	
	for( int i = particles.size()-1; i >=0; i--)
	{
		// update state if faded out
		if( particles[i].alpha <= 0 )
			particles.erase(particles.begin()+i);
		//if( particles[i].alpha <= 0 )
			
		//resetParticle( &particles[i] );
	
	}
}

void DropParticles::turnOnParticle( ga_particle * p, ofPoint pos, float size)
{
	resetParticle(p);
	p->pos		= pos;
	p->size		= size;
	p->alpha	= baseAlpha;
	p->bOn		= true;

}

void DropParticles::draw()
{
	ofFill();
	
	for( int i = 0; i < particles.size(); i++)
	{
		if(!particles[i].bOn) continue;
		
		glPushMatrix();
			glTranslatef(0, 0, particles[i].pos.z);
			float size = particles[i].size;
			float alpha = MIN(baseAlpha,particles[i].alpha);
			ofSetColor(255,255,255,alpha*255);
			ofRect(particles[i].pos.x-(size*.5), particles[i].pos.y-(size*.5), size, size);
		glPopMatrix();
		
	}
	
}


void DropParticles::createRandomDrop( ofPoint startPos, ofPoint vel, float size )
{
	if( particles.size() < maxParticles )
	{
	ga_particle ptemp;
	resetParticle(&ptemp);
	turnOnParticle( &ptemp, startPos, baseSize);
	
	
	float sVel = baseGravity*.25;
	ptemp.vel.set(vel.x*sVel,vel.y*sVel,vel.z*sVel);
	ptemp.size = size;
	///////////////////x
	particles.push_back(ptemp);
	//cout << "new particle at " << startPos.x << " , " << startPos.y << " , " << startPos.z << endl;
	 
	}
	
}


