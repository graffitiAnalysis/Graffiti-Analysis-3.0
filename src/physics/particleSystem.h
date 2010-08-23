#ifndef PARTICLESYSTEM_H
#define PARTICLESYSTEM_H

//#include "particle.h"
#include "vectorField.h"
#include "ofxVectorMath.h"

#define MAX_PARTICLES 10000



class particleSystem
{
    public:
        particleSystem();
        virtual ~particleSystem();

        void clear();
        void reset(int w, int h);
        void setup(int fieldsize, int w, int h, float screenW, float screenH, vectorField * VF);
        void update(float dt,float time, float alpha, bool bReset=false);
        void draw(float time, float alpha, float size=1.5, bool bDrawLines = true);

        void setParticleForce(float val);
        void setParticleDropOffRate(float val);
        void setParticleSize( float val );
		
		int getIndexOfRandomAliveParticle();
		
        int w,h;

        float	pos[MAX_PARTICLES][3];
        float	vel[MAX_PARTICLES][3];
        float	col[MAX_PARTICLES][4];
        float   ori[MAX_PARTICLES][3];
        bool    bOn[MAX_PARTICLES];
		bool    bDrawOn[MAX_PARTICLES];
		int    framesOn[MAX_PARTICLES];
		float    stopPos[MAX_PARTICLES][3];
        float    stopVec[MAX_PARTICLES][3];
		
		vectorField * VFptr;

        float   dropOffRate;
        float   force;
        int     numParticles;
        float		psize;
		
		// size data for each particle
		float sizes[MAX_PARTICLES];

    protected:
    private:
};

#endif // PARTICLESYSTEM_H
