#include "grafTagMulti.h"


grafTagMulti::grafTagMulti()
{

    startTime       = 0.f;
	maxNumPts       = 5000;
	ptStart         = 0;
	ptEnd           = 0;

    rotation.set(0,0,0);
    position.set(0,0,1);
	rotation_o.set(0,0,0);
    position_o.set(0,0,1);
    z_const     = 12;//8;//4;//16.f;
	
	minPtDist = 0.005;//002;//025;

    bPlayed = false;
	tagname = "graf";
	
	strokes tempStroke;
	myStrokes.push_back(tempStroke);
	
	drawScale = 768;
	
	
	ofAddListener(ofEvents.mousePressed, this, &grafTagMulti::mousePressed);
	ofAddListener(ofEvents.mouseReleased, this, &grafTagMulti::mouseReleased);
	ofAddListener(ofEvents.mouseDragged, this, &grafTagMulti::mouseDragged);
	
	bMouseActive = false;
	lastX = 0;
	lastY = 0;
	
	nPts = 0;
}

grafTagMulti::~grafTagMulti()
{
    //dtor
	ofRemoveListener(ofEvents.mousePressed, this, &grafTagMulti::mousePressed);
	ofRemoveListener(ofEvents.mouseReleased, this, &grafTagMulti::mouseReleased);
	ofRemoveListener(ofEvents.mouseDragged, this, &grafTagMulti::mouseDragged);
}

void grafTagMulti::clear(bool bAdddInitStroke)
{
    myStrokes.clear();
	
	// always have first one ready
	if(bAdddInitStroke)
	{
		strokes tempStroke;
		myStrokes.push_back(tempStroke);
	}
	
	nPts = 0;


}

bool grafTagMulti::addNewPoint(ofPoint pt, float dist, float angle, float time, bool bAverage)
{

	
	//----- ignore 0,0 error
    if( pt.x == 0 && pt.y == 0){ cout << "pt bad "  << endl;  return false; }
	if( myStrokes.size() == 0 ) return false; 

    	
	int id = myStrokes.size()-1;
	
	//----- on the first point, grab the start time
	if ( id <= 0 && myStrokes[ id ].pts.size() == 0){
		if(time == -1 ) startTime = ofGetElapsedTimef(); 
		else			startTime = time;
	}

	//----- combine the position and the time here:
	timePt	myPoint;
	myPoint.pos.x = pt.x;
	myPoint.pos.y = pt.y;
	myPoint.pos.z = pt.z;
	
	if( time == -1 )    myPoint.time	= ofGetElapsedTimef() - startTime;
    else                myPoint.time    = time;

	
	//----- set z from time position
	myPoint.pos.z = ( myPoint.time * 1000.f ) / z_const;
	
	
	bool bPtsEmpty = (myStrokes[ id ].pts.size() <= 0) ? true : false;
	int lastPt = myStrokes[ id ].pts.size() - 1;
				  
	
	//----- check distance and return if too small
    if( !bPtsEmpty && dist == -1 )          dist = ((ofxVec2f)(myStrokes[ id ].pts[lastPt].pos-pt) ).length();
    else if (bPtsEmpty && dist == -1 )   dist = 0;

    if( bPtsEmpty || dist > minPtDist ) myPoint.dist = dist;
    else{ 
		//printf("grafTag: can´t add, dist too small \n"); 
		return false; 
	}

	//----- calc angle from previous point
	if( !bPtsEmpty && angle == -1) angle = atan2(pt.y - myStrokes[ id ].pts[lastPt].pos.y, pt.x - myStrokes[ id ].pts[lastPt].pos.x);
	else if(angle == -1) angle = 0;
	myPoint.angle = angle;

	
	//----- add and check if point is misplaced (in case doing curved smoothing)
	int inPt = -1;
	for( int i = 0; i < myStrokes[ id ].pts.size(); i++)
	{
        if( myStrokes[ id ].pts[i].time > myPoint.time )
        {
            inPt = i;
            break;
        }
    }

    if(myStrokes[ id ].pts.size() == 0) myPoint.ptType = 1;
	else myPoint.ptType = 0;
	
	//----- add the point
	if( inPt == -1 || myStrokes[ id ].pts.size() == 0 )
    {
        myStrokes[ id ].pts.push_back(myPoint);
        myStrokes[ id ].pts_orig.push_back(myPoint);
    }
    else{
         myStrokes[ id ].pts.insert(myStrokes[ id ].pts.begin()+inPt,myPoint);
         myStrokes[ id ].pts_orig.insert(myStrokes[ id ].pts_orig.begin()+inPt,myPoint);
    }
	
	
	//----- delete if overrun limit
	//if (myStrokes[ id ].pts.size() > maxNumPts)
	//{
	//	myStrokes[ id ].pts.erase(myStrokes[ id ].pts.begin());
	//}


    int numPoints = myStrokes[ id ].pts.size()-1;

    //----- average
    if( numPoints > 1 && bAverage)
    {
		myStrokes[ id ].pts[numPoints-1].pos.x = (0.25f * myStrokes[ id ].pts[numPoints-2].pos.x) + (0.50f * myStrokes[ id ].pts[numPoints-1].pos.x) + (0.25f * myStrokes[ id ].pts[numPoints].pos.x);
		myStrokes[ id ].pts[numPoints-1].pos.y = (0.25f * myStrokes[ id ].pts[numPoints-2].pos.y) + (0.50f * myStrokes[ id ].pts[numPoints-1].pos.y) + (0.25f * myStrokes[ id ].pts[numPoints].pos.y);
		myStrokes[ id ].pts[numPoints-1].dist  = (0.25f * myStrokes[ id ].pts[numPoints-2].dist) + (0.50f * myStrokes[ id ].pts[numPoints-1].dist)  + (0.25f * myStrokes[ id ].pts[numPoints].dist);
    }

    ptEnd =  myStrokes[ id ].pts.size()-1;


    //-----  originals
	//if(myStrokes[ id ].pts_orig.size() > maxNumPts)
	//{
	//	myStrokes[ id ].pts_orig.erase(myStrokes[ id ].pts_orig.begin());
	//}

    
	calcMinMax();
	
	//-----  
	nPts++;
	
	return true;
}

void grafTagMulti::nextStroke()
{
	if( myStrokes.size() > 0 && myStrokes[ myStrokes.size()-1 ].pts.size() > 0 )
	{
		int lastStroke	= myStrokes.size()-1;
		int lastPt		= myStrokes[lastStroke].pts.size()-1;
		myStrokes[lastStroke].pts[lastPt].ptType = 2;
		
		strokes tempStroke;
		myStrokes.push_back(tempStroke);
	}
	
}

void grafTagMulti::cleanUpLastStroke()
{
	int lastStroke = myStrokes.size()-1;
	if( myStrokes[ lastStroke ].pts.size() == 0 )
		myStrokes.erase( myStrokes.begin()+lastStroke );
}

void grafTagMulti::average()
{
	for( int i = 0; i < myStrokes.size(); i++)
		averagePts( &myStrokes[i] );
}

void grafTagMulti::averagePts( strokes * stroke )
{
    for( int i = 2; i < stroke->pts.size(); i++)
    {

        stroke->pts[i-1].pos.x = (0.3f * stroke->pts[i-2].pos.x) + (0.40f * stroke->pts[i-1].pos.x) + (0.3f * stroke->pts[i].pos.x);
        stroke->pts[i-1].pos.y = (0.3f * stroke->pts[i-2].pos.y) + (0.40f * stroke->pts[i-1].pos.y) + (0.3f * stroke->pts[i].pos.y);
        //stroke->pts[i-1].dist  = (0.2f * stroke->pts[i-2].dist)  + (0.60f * stroke->pts[i-1].dist)  + (0.2f * stroke->pts[i].dist);
       
		stroke->pts[i-1].dist  = (0.25f * stroke->pts[i-2].dist)  + (0.50f * stroke->pts[i-1].dist)  + (0.25f * stroke->pts[i].dist);


    }

}

void grafTagMulti::calcMinMax()
{

    center.set(0,0,0);
	max.set(0,0,0);
    min.set(0,0,0);

    distMin = 0;
    distMax = 0;

	for( int i = 0; i < myStrokes.size(); i++)
	{
		for( int j = 0; j < myStrokes[i].pts.size(); j++)
		{
			bool bFirst = (i==0 && j == 0) ? true : false;
			
			if( bFirst ||  myStrokes[i].pts[j].pos.x > max.x ) max.x =  myStrokes[i].pts[j].pos.x;
			if( bFirst ||  myStrokes[i].pts[j].pos.y > max.y ) max.y =  myStrokes[i].pts[j].pos.y;
			if( bFirst ||  myStrokes[i].pts[j].pos.z  > max.z ) max.z =  myStrokes[i].pts[j].pos.z;
			
			if( bFirst ||  myStrokes[i].pts[j].pos.x < min.x ) min.x =  myStrokes[i].pts[j].pos.x;
			if( bFirst ||  myStrokes[i].pts[j].pos.y < min.y ) min.y =  myStrokes[i].pts[j].pos.y;
			if( bFirst ||  myStrokes[i].pts[j].pos.z  < min.z ) min.z =  myStrokes[i].pts[j].pos.z;
			
			if( bFirst ||  myStrokes[i].pts[j].dist  > distMax  ) distMax = myStrokes[i].pts[j].dist;
			if( bFirst ||  myStrokes[i].pts[j].dist  < distMin  ) distMin = myStrokes[i].pts[j].dist;
		}

	}
	
	center.set( .5*(max.x-min.x), .5*(max.y-min.y),.5*(max.z-min.z) );
}

void grafTagMulti::draw()
{
	ofNoFill();
	ofSetColor(0xffffff);
	
	for( int i = 0; i < myStrokes.size(); i++)
	{
		ofBeginShape();
		for( int j = 0; j < myStrokes[i].pts.size(); j++)
		{
			ofVertex(myStrokes[i].pts[j].pos.x*drawScale, myStrokes[i].pts[j].pos.y*drawScale);
		}
		ofEndShape(false);
		
	}
	
}

grafTagMulti::grafTagMulti( const grafTagMulti &  mom )
{
	myStrokes.clear();
	myStrokes.assign( mom.myStrokes.begin(), mom.myStrokes.end() );
	
	rotation=mom.rotation;
	position=mom.position;
	rotation_o=mom.rotation_o;
	position_o=mom.position_o;
	z_const=mom.z_const;
	drawScale=mom.drawScale;
	
	min=mom.min;
	max=mom.max;
	center=mom.center;
	distMin=mom.distMin; 
	distMax=mom.distMax;
	minPtDist=mom.minPtDist;
	
	// tag info
	tagname=mom.tagname;
	bPlayed=mom.bPlayed;
	startTime=mom.startTime;
	maxNumPts=mom.maxNumPts;
	ptStart=mom.ptStart;
	ptEnd=mom.ptEnd;
	
	nPts = mom.nPts;

	
}



void grafTagMulti::mouseDragged(ofMouseEventArgs& event)
{
	
	if(!bMouseActive) return;
	
	
	
	
	if( event.button == 0  )
	{
		if(bShift)
		{
			position.x += (event.x-lastX);
			position.y += (event.y-lastY);
		}
		else
			rotation.x += (event.x-lastX);
				
	}else{
		
		position.z += (event.y-lastY);//*.01f;
		
	}
	
	lastX = event.x;
	lastY = event.y;
}

void grafTagMulti::mousePressed(ofMouseEventArgs& event)
{
	lastX = event.x;
	lastY = event.y;
	
	if(!bMouseActive) return;
	
	int modifier = glutGetModifiers();
	if( modifier == GLUT_ACTIVE_SHIFT) bShift = true;
	else bShift = false;
}

void grafTagMulti::mouseReleased(ofMouseEventArgs& event)
{
	if(!bMouseActive) return;
	bShift = false;
}

void grafTagMulti::drawSkeleton(bool bCentered)
{
	if(bCentered)calcMinMax();
	
	//----- make vertices
	int tPts = 0;
	for( int i = 0; i < myStrokes.size(); i++) tPts += myStrokes[i].pts.size();
	
	float * vertices = new float[tPts*3];
	
	int t = 0;
	for( int i = 0; i < myStrokes.size(); i++)
	{
		for( int j = 0; j < myStrokes[i].pts.size(); j++)
		{
			vertices[t]		= myStrokes[i].pts[j].pos.x;
			vertices[t+1]	= myStrokes[i].pts[j].pos.y;
			vertices[t+2]	= myStrokes[i].pts[j].pos.z;
			t+=3;
		}
	}
	//----- 
	
	
	glPushMatrix();
		
		glTranslatef(ofGetWidth()*.5f, ofGetHeight()*.5, 0);	
	
		glTranslatef(position.x,position.y,position.z);
	
		// rotate
		glRotatef(rotation.x,0,1,0);
		glRotatef(rotation.y,1,0,0);
		glRotatef(rotation.z,0,0,1); 
	
	
		// translate to draw offset		
		glScalef(drawScale,drawScale,1);
		glTranslatef(-min.x,-min.y,-min.z);
		glTranslatef(-center.x,-center.y,-center.z);

		
		glEnableClientState(GL_VERTEX_ARRAY);

			glVertexPointer(3, GL_FLOAT, 0, vertices);
			glDrawArrays(GL_LINE_STRIP, 0, tPts);
	
		glDisableClientState(GL_VERTEX_ARRAY);
	
		drawBoundingBox(min,max,center);

	glPopMatrix();
	
	
	delete [] vertices;
}

void grafTagMulti::drawBoundingBox()
{
drawBoundingBox(min, max, center );
}

void grafTagMulti::drawBoundingBox( ofPoint pmin, ofPoint pmax, ofPoint pcenter )
{
	
	// draw cube
	float pos[18];
	
	glEnableClientState(GL_VERTEX_ARRAY);
	
	// front
	pos[0] = pmin.x;
	pos[1] = pmax.y;
	pos[2] = pmin.z;
	
	pos[3] = pmax.x;
	pos[4] = pmax.y;
	pos[5] = pmin.z;
	
	pos[6] = pmax.x;
	pos[7] = pmin.y;
	pos[8] = pmin.z;
	
	pos[9]  = pmin.x;
	pos[10] = pmin.y;
	pos[11] = pmin.z;
	
	glVertexPointer(3, GL_FLOAT, 0, pos);
	glDrawArrays(GL_LINE_LOOP, 0, 4);
	
	
	// back
	pos[0] = pmin.x;
	pos[1] = pmax.y;
	pos[2] = pmax.z;
	
	pos[3] = pmax.x;
	pos[4] = pmax.y;
	pos[5] = pmax.z;
	
	pos[6] = pmax.x;
	pos[7] = pmin.y;
	pos[8] = pmax.z;
	
	pos[9]  = pmin.x;
	pos[10] = pmin.y;
	pos[11] = pmax.z;
	
	glVertexPointer(3, GL_FLOAT, 0, pos);
	glDrawArrays(GL_LINE_LOOP, 0, 4);
	
	// top
	pos[0] = pmin.x;
	pos[1] = pmax.y;
	pos[2] = pmin.z;
	
	pos[3] = pmin.x;
	pos[4] = pmax.y;
	pos[5] = pmax.z;
	
	pos[6] = pmax.x;
	pos[7] = pmax.y;
	pos[8] = pmax.z;
	
	pos[9]  = pmax.x;
	pos[10] = pmax.y;
	pos[11] = pmin.z;
	
	
	glVertexPointer(3, GL_FLOAT, 0, pos);
	glDrawArrays(GL_LINE_LOOP, 0, 4);
	
	// bottom
	pos[0] = pmin.x;
	pos[1] = pmin.y;
	pos[2] = pmin.z;
	
	pos[3] = pmin.x;
	pos[4] = pmin.y;
	pos[5] = pmax.z;
	
	pos[6] = pmax.x;
	pos[7] = pmin.y;
	pos[8] = pmax.z;
	
	pos[9]  = pmax.x;
	pos[10] = pmin.y;
	pos[11] = pmin.z;
	
	pos[12] = pmin.x;
	pos[13] = pmin.y;
	pos[14] = pmin.z;
	
	glVertexPointer(3, GL_FLOAT, 0, pos);
	glDrawArrays(GL_LINE_LOOP, 0, 5);	
	
	// center
	pos[0] = pmin.x;
	pos[1] = pmax.y;
	pos[2] = pmin.z+center.z;
	
	pos[3] = pmax.x;
	pos[4] = pmax.y;
	pos[5] = pmin.z+center.z;
	
	pos[6] = pmax.x;
	pos[7] = pmin.y;
	pos[8] = pmin.z+center.z;
	
	pos[9]  = pmin.x;
	pos[10] = pmin.y;
	pos[11] = pmin.z+center.z;
	
	glVertexPointer(3, GL_FLOAT, 0, pos);
	glDrawArrays(GL_LINE_LOOP, 0, 4);
	
	glDisable(GL_VERTEX_ARRAY);
	
	
}

void grafTagMulti::applyDrawScale()
{
	if( drawScale == 1) return;
	
	for( int i = 0; i < myStrokes.size(); i++)
	{
		if( myStrokes[i].pts.size() == 0 ) continue;
		
		for( int j = 0; j < myStrokes[i].pts.size(); j++)
		{
			myStrokes[i].pts[j].pos.x *= drawScale;
			myStrokes[i].pts[j].pos.y *= drawScale;
		}
	}
	
	drawScale = 1;
}
