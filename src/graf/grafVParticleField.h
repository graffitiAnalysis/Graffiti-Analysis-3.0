#ifndef GRAFVIZPARTICLES_H
#define GRAFVIZPARTICLES_H

#include "ofMain.h"
#include "grafTagMulti.h"
#include "particleSystem.h"

class grafVParticleField
{
    public:
        grafVParticleField();
        virtual ~grafVParticleField();

	
		void setup(int w, int h);
		void reset();
	
		void update( ofPoint pt, ofPoint vel, float dt, bool bReset, float centerZ);
		void fall( float dt);
		void flatten(float zDepth, float timeToDoIt);
		
		void setDamping( float val );
		void setParticleSize( float val );
	
		void draw( float zdepth, int screenW, int screenH );
		
		// for particle animations (from audio data)
		void updateParticleSizes(float * vals, float averageVal, int tVals, float maxScale = 4.f);
		void updateDampingFromAudio(float val);
		void updateParticleAmpli(float * vals, float averageVal, int tVals, float maxScale = 4.f);
		
		void flattenInstantly(float zDepth);

		float alpha, xalpha;
		float particle_damping;
		float particle_size;
		float particle_alpha;
		
		int numXtras;
		
		// random new particles that drop
		// need vector of particles: pos, sizes, vel, origin?, alive time etc.
		// vector of bands, if band goes above threshold, turn on, when drops down, create particles and wait time before creating another on that band
		// update gravity on these particles, and physics etc.
		// apply same global aplha for fade out
		// reset/clear all 
		
		// particles with attached lines
		particleSystem  PS;
	
    protected:
		
		int w, h;
	
		
	
		// solo particles
		particleSystem XTRA_PS[4];
	
		// normal vector field
		vectorField     VF;
	
		int transitionCounter;
		float flatTime;
      
    private:
};

#endif // GRAFVIZPARTICLES_H
