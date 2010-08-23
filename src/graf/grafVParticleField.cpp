#include "grafVParticleField.h"

grafVParticleField::grafVParticleField()
{
    
    alpha				= 1;
	xalpha				= 1;
    particle_damping	= .12f;
    particle_size		= 2.f;
    particle_alpha		= .75f;
}

grafVParticleField::~grafVParticleField()
{
    //dtor
}

void grafVParticleField::reset()
{
	
    // set up original set with lines
    PS.reset(WIDTH_FIELD,HEIGHT_FIELD);
	
    // set up extras for flare
    for( int i = 0; i < numXtras; i++)
        XTRA_PS[i].reset(WIDTH_FIELD,HEIGHT_FIELD);
	
    // set vector field
    VF.bReset = true;
    VF.clearField();
	
    // reset vars
    alpha = 1;
	xalpha = 1;
    transitionCounter = 0;
	numXtras = 1;
	flatTime = 0;
}

void grafVParticleField::setup(int w_, int h_)
{
	
    w = w_;
    h = h_;
	
	PS.setup(FIELD_SIZE,WIDTH_FIELD,HEIGHT_FIELD,w,h,&VF);
	
	//particleSystem psx;
	int toatlXtra = 4;
	for( int i = 0; i < toatlXtra; i++)
	{
        //XTRA_PS.push_back(psx);
        XTRA_PS[i].setup(FIELD_SIZE,WIDTH_FIELD,HEIGHT_FIELD,w,h,&VF);
		
	}
	
	setDamping( particle_damping);
	setParticleSize( particle_size );
}

void grafVParticleField::flatten( float zDepth, float timeToDoIt)
{
	if( flatTime == 0 ) 
		flatTime= ofGetElapsedTimef();
	
	float pct = 1 - ((ofGetElapsedTimef()-flatTime) / timeToDoIt);

	for( int j= 0; j< PS.numParticles; j++)
	{
		PS.pos[j][2] = pct*PS.pos[j][2]  + (1-pct)*zDepth;
		PS.stopPos[j][2]  = pct*PS.stopPos[j][2]  + (1-pct)*zDepth;
		PS.stopVec[j][2]  = pct*PS.stopVec[j][2]  + (1-pct);//*zDepth;
	}
	
}

void grafVParticleField::fall(float dt)
{
	for( int j= 0; j< PS.numParticles; j++)
	{
		//if( PS.bOn[j] )
		//{
			PS.vel[j][1] += (dt) * (40.0);
			PS.pos[j][1] += PS.vel[j][1] * (dt);
		//}	 
	 }
	
	for( int i= 0; i< numXtras; i++)
	{
		for( int j= 0; j< XTRA_PS[i].numParticles; j++)
		{
		//if( XTRA_PS[i].bOn[j] )
		//{
			XTRA_PS[i].vel[j][1] += (dt) * (40.0);
			XTRA_PS[i].pos[j][1] += XTRA_PS[i].vel[j][1] * (dt);
		//}	 
		}
	}
}

void grafVParticleField::setDamping( float val )
{
	
    particle_damping = val;
    PS.setParticleDropOffRate(particle_damping);
	
    for( int i = 0; i < numXtras; i++) 
		XTRA_PS[i].dropOffRate = (.8*particle_damping) - (i*.01f );
    //setParticleDropOffRate( (.8*particle_damping) - (i*.01f ));
	
}
void grafVParticleField::setParticleSize( float val )
{
	
    particle_size = val;
    PS.setParticleSize(particle_size);
	
    for( int i = 0; i < numXtras; i++) XTRA_PS[i].setParticleSize(particle_size*.75f);
	
}

void grafVParticleField::update( ofPoint pt, ofPoint vel, float dt, bool bReset)
{
	
	
	
	//if(vel.x == 0 && vel.y == 0 ) 
	float zdepth	= pt.z;
	
    PS.update(dt*100,zdepth, alpha*particle_alpha, bReset);
	
    for( int i = 0; i < numXtras; i++)
		XTRA_PS[i].update(dt*100,zdepth, alpha, bReset);
	
    VF.bReset = bReset;
	
	VF.fadeField(0.9f);
	
	vel.x = dt;
	//if(!bReset)
		VF.update( pt, vel, h, w, h);
	
	
}


void grafVParticleField::updateParticleSizes(float * vals, float averageVal, int tVals, float maxScale)
{
	
	// loop through all particles and change scaling of size based on value in array (fft value)
	for( int i = 0; i < PS.numParticles; i++)
	{
		int ps = i % tVals;
		float force = PS.psize * ( 1 +  (vals[ps] * maxScale) );//+  (vals[ps] * maxScale) ) );
		if(force!=force) force = 0;
		PS.sizes[i] = .9*PS.sizes[i]+.1*force;//1 + maxScale * vals[ps];
		
	}
	
	
	for( int i = 0; i < numXtras; i++)
	{
		for( int j = 0; j < XTRA_PS[i].numParticles; j++)
		{
			int ps = j % tVals;
			float force = averageVal * maxScale +  vals[ps] * (maxScale*.5) + 1;
			//XTRA_PS[i].sizes[j] = force;//1 + maxScale * vals[ps];
		}
	}
}

void grafVParticleField::updateParticleAmpli(float * vals, float averageVal, int tVals, float maxScale)
{
	for( int i = 0; i < PS.numParticles; i++)
	{
		int ps = (i % (tVals-1))+1;
		float pct = 1;//1.5*powf((ps/(float)tVals),1.5);//.5+((ps/(float)tVals));
		float force = (vals[ps]*maxScale) + (averageVal*maxScale*2);//((vals[ps]*vals[ps])*pct) * (maxScale*.5);//averageVal * (maxScale*.5) +  ((vals[ps]*vals[ps])*pct) * (maxScale*.5);//maxScale*(vals[ps]);//(averageVal * 10 +  vals[ps] * 5 + .5);
		
		if( force!=force ) force = 0;
		
		
		//PS.sizes[i] = force;//1 + maxScale * vals[ps];
		if(!PS.bOn[i] && PS.framesOn[i] > 0)
		{
			//PS.pos[i][0] = .9*PS.stopPos[i][0] + .1*(PS.stopPos[i][0] +force*PS.stopVec[i][0]);
			//PS.pos[i][1] = .9*PS.stopPos[i][1] + .1*(PS.stopPos[i][1] +force*PS.stopVec[i][1]);
			//PS.pos[i][2] = .9*PS.stopPos[i][2] + .1*(PS.stopPos[i][2] +force*PS.stopVec[i][2]);
			PS.pos[i][0] = .9*PS.pos[i][0] + .1*(PS.stopPos[i][0]);// +force*PS.stopVec[i][0]);
			PS.pos[i][1] = .9*PS.pos[i][1] + .1*(PS.stopPos[i][1]);// +force*PS.stopVec[i][1]);
			PS.pos[i][2] = .9*PS.pos[i][2] + .1*(PS.stopPos[i][2]);//+force*PS.stopVec[i][2]);
			PS.vel[i][0] += force*PS.stopVec[i][0];// +force*PS.stopVec[i][0]);
			PS.vel[i][1] += force*PS.stopVec[i][1];// +force*PS.stopVec[i][1]);
			PS.vel[i][2] += force*PS.stopVec[i][2];
		}
	}
}

void grafVParticleField::updateDampingFromAudio(float val)
{
	if( val > 0 ) val = particle_damping + val;
	else val = particle_damping;
	
	PS.setParticleDropOffRate(val);
	
    //for( int i = 0; i < numXtras; i++) 
	//	XTRA_PS[i].dropOffRate = (.8*val) - (i*.01f );
}

void grafVParticleField::draw( float zdepth, int screenW, int screenH )
{
   // glEnable(GL_DEPTH_TEST);
	
	ofNoFill();
	
    glPushMatrix();
	
	for( int i = 0; i <numXtras; i++)
	{
		XTRA_PS[i].draw(zdepth, xalpha*particle_alpha, particle_size*.75f, false );
	}
	
	PS.draw(  zdepth, alpha*particle_alpha, particle_size );
	
	
    glPopMatrix();
	
	//glDisable(GL_DEPTH_TEST);
	
	
	
}