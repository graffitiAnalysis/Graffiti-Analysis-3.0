#include "particleSystem.h"

particleSystem::particleSystem()
{
    dropOffRate = 0.18f;
    force       = 1;
    psize       = 3.0;
	
	memset(pos, 0, sizeof(float)*MAX_PARTICLES*3);
    memset(ori, 0, sizeof(float)*MAX_PARTICLES*3);
    memset(col, 0, sizeof(float)*MAX_PARTICLES*4);
    memset(bOn, 0, sizeof(bool)*MAX_PARTICLES);
	memset(sizes,psize, sizeof(float)*MAX_PARTICLES);
	memset(framesOn,0, sizeof(int)*MAX_PARTICLES);
	memset(stopPos,0, sizeof(float)*MAX_PARTICLES*3);
	memset(stopVec,0, sizeof(float)*MAX_PARTICLES*3);
    memset(bDrawOn, 1, sizeof(bool)*MAX_PARTICLES);


    

}

particleSystem::~particleSystem()
{

}

void particleSystem::clear()
{

}

void particleSystem::reset(int fw, int fh)
{
    float scalex = w  / (float)fw;
	float scaley = h / (float)fh;


	for (int i = 0; i < fw; i++)
	{

		for (int j = 0; j < fh; j++)
		{
			int p = j * fw + i;
			if( p < numParticles )
			{
                float x = i*scalex;
                float y = j*scaley;

                ori[p][0] = x;
                ori[p][1] = y;
                ori[p][2] = 0;

                pos[p][0] = x;
                pos[p][1] = y;
                pos[p][2] = 0;

                vel[p][0] = 0;
                vel[p][1] = 0;
                vel[p][2] = 0;

                col[p][0] = 0;
                col[p][1] = 0;
                col[p][2] = 0;
                col[p][3] = 0;

                bOn[p] = false;
				
				sizes[p] = psize;
				framesOn[p] = 0;
				bDrawOn[p] = true;
            }

		}
	}
	
	memset(stopPos,0, sizeof(float)*MAX_PARTICLES*3);
	memset(stopVec,0, sizeof(float)*MAX_PARTICLES*3);

}


void particleSystem::setup(int fieldsize, int fw, int fh, float screenW, float screenH, vectorField * VF)
{
    w = screenW;
    h = screenH;

    numParticles = MIN(fieldsize, MAX_PARTICLES);

    VFptr = VF;

	reset(fw,fh);

	//cout << " total particles: " << numParticles << " max " << MAX_PARTICLES << endl;

}

void particleSystem::update(float dt,float time, float alpha, bool bReset )
{
	float screenW = w;
	float screenH = h;
	 
	int     fieldPosX,fieldPosY,posp;
    ofPoint addV;
    float   addVX,addVY;

    float damp      = dropOffRate*dt;
    float myalpha   = .75*alpha;

    //int totalOn = 0;
    
	// turn on particles that have been moved
	for (int i = 0; i < numParticles; i++)
	{
       
        //-- note: changes to make suio amp part
		if( bOn[i] ){
			framesOn[i]++;
			if( framesOn[i] > 30 && vel[i][0] < .000001 && vel[i][1] < .000001)
			{
				vel[i][0] = 0;
				vel[i][1] = 0;
				stopPos[i][0] = pos[i][0]; stopPos[i][1] = pos[i][1]; stopPos[i][2] = pos[i][2];
				ofxVec3f p1 = ofxVec3f(pos[i][0],pos[i][1],pos[i][2])-ofxVec3f(ori[i][0],ori[i][1],pos[i][2]);
				p1 = p1.normalize();
				stopVec[i][0] = p1.x; stopVec[i][1] = p1.y; stopVec[i][2] = p1.z;
				bOn[i] = false;
			}
		}
		//---
		
		if( framesOn[i] == 0 && pos[i][0] != ori[i][0] && pos[i][1] != ori[i][1] )
        {
            pos[i][2] = time;
            bOn[i]    = true;
			framesOn[i] = 0;
        }
		
		
	}
	
    // add velocity vector to particles
	for (int i = 0; i < numParticles; i++)
	{
        pos[i][0] += vel[i][0]*dt;
        pos[i][1] += vel[i][1]*dt;
	}
	

	
	if(!bReset)
	{

	    for (int i = 0; i < numParticles; i++)
        {

            if( framesOn[i] > 0 && !bOn[i] ) continue;
			
			fieldPosX = (int)((pos[i][0]/screenW) * (float)WIDTH_FIELD);
            fieldPosY = (int)((pos[i][1]/screenH) * (float)HEIGHT_FIELD);


            if( fieldPosX < 0 || fieldPosX > WIDTH_FIELD-1 ||
                fieldPosY < 0 || fieldPosY > HEIGHT_FIELD-1
            )
            ;
            else{

                posp = fieldPosY * WIDTH_FIELD + fieldPosX;

                addVX = dt*(force*VFptr->fx[posp]);//addV.x);
                addVY = dt*(force*VFptr->fy[posp]);

                vel[i][0] += addVX;
                vel[i][1] += addVY;

            }
        }


	}



	for (int i = 0; i < numParticles; i++)
	{
        vel[i][0] -= vel[i][0]*damp;
        vel[i][1] -= vel[i][1]*damp;

	}


}

void particleSystem::setParticleForce(float val)
{
    force = val;
}

void particleSystem::setParticleDropOffRate(float val)
{
    dropOffRate = val;
}

void particleSystem::setParticleSize(float val)
{
    psize = val;
}

void particleSystem::draw(float time, float alpha, float size,  bool bDrawLines)
{


    for (int i = 0; i < numParticles; i++)
	{
        if(framesOn[i] <= 0 ){
		//!bOn[i]){
			col[i][3] = 0;
		}/*else if(!bOn[i])
		{
			col[i][0] = 1;
			col[i][1] = 0;
			col[i][2] = 0;
			col[i][3] = alpha;
		
		}*/else{ 
			col[i][0] = 1;
			col[i][1] = 1;
			col[i][2] = 1;
			col[i][3] = alpha;
		}
	}

	float linePoints[2][3];
    float lineCol[2][4];

    if(bDrawLines)
    {

        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_COLOR_ARRAY);

        for (int i = 0; i < numParticles; i++)
		{
		    //if(bDrawOn[i])
			
			if(framesOn[i] <= 0 || !bDrawOn[i]) continue;

		    linePoints[0][0] = pos[i][0];
            linePoints[0][1] = pos[i][1];
            linePoints[0][2] = pos[i][2];
            linePoints[1][0] = ori[i][0];
            linePoints[1][1] = ori[i][1];
            linePoints[1][2] = pos[i][2];

            lineCol[0][0] = .7;
            lineCol[0][1] = .7;
            lineCol[0][2] = .7;
            lineCol[0][3] = .95*alpha;

            lineCol[1][0] = 0;
            lineCol[1][1] = 0;
            lineCol[1][2] = 0;
            lineCol[1][3] = 0;


            glVertexPointer(3, GL_FLOAT, 0, &linePoints[0]);
            glColorPointer(4, GL_FLOAT, 0, lineCol);
            glDrawArrays(GL_LINES, 0, 2);

		}
		glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_COLOR_ARRAY);
    }


	float rectPoints[12];
	int w = 1;//size;
	int h = 1;//size;
	
    
	//glEnable(GL_DEPTH_TEST);
	
	glColor4f(1,1,1,alpha);
	
	glEnableClientState(GL_VERTEX_ARRAY);
	//glEnableClientState(GL_COLOR_ARRAY);

	for (int i = 0; i < numParticles; i++)
	{
		if(col[i][3] <= 0 || !bDrawOn[i] ) continue;
		
		rectPoints[0] = pos[i][0]-(w*sizes[i])*.5;
		rectPoints[1] = pos[i][1]-(w*sizes[i])*.5;
		rectPoints[2] = pos[i][2];
		
		rectPoints[3] = pos[i][0]+(w*sizes[i])*.5;
		rectPoints[4] = pos[i][1]-(w*sizes[i])*.5;
		rectPoints[5] = pos[i][2];
		
		rectPoints[6] = pos[i][0]+(w*sizes[i])*.5;
		rectPoints[7] = pos[i][1]+(w*sizes[i])*.5;
		rectPoints[8] = pos[i][2];
		
		rectPoints[9] = pos[i][0]-(w*sizes[i])*.5;
		rectPoints[10] = pos[i][1]+(w*sizes[i])*.5;
		rectPoints[11] = pos[i][2];
		
		//glColorPointer(4, GL_FLOAT, 0, col[i]);
		glVertexPointer(3, GL_FLOAT, 0, &rectPoints[0]);
		glDrawArrays( GL_TRIANGLE_FAN, 0, 4);
		
	}

	//glDisable(GL_DEPTH_TEST);
	glDisableClientState(GL_VERTEX_ARRAY);
}


int particleSystem::getIndexOfRandomAliveParticle()
{
	vector<int> rIds;
	for( int i = 0; i < numParticles; i++)
	{
	if(framesOn[i] > 0) rIds.push_back(i);
	}
	
	if( rIds.size() > 0 )
	{
	int rNum = ofRandom(0,rIds.size()-1);
	return rIds[rNum];
	}else
		return 0;
}
