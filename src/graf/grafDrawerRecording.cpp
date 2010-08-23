#include "grafDrawerRecording.h"


grafDrawerRecording::grafDrawerRecording()
{
	alpha = 1.f;
	minLen = .05;
	maxLen = .05;
	lineWidth = 2.0;
	lineAlpha = .92;
	lineScale = .05;
	bSetupDrawer = false;
}

grafDrawerRecording::~grafDrawerRecording()
{
	for( int i = 0; i < lines.size(); i++)
		delete lines[i];
}

void grafDrawerRecording::setup(grafTagMulti * myTag, int startStroke, int startPt, int endStroke, int endPt)
{
	alpha = 1.f;
	
	if( myTag->myStrokes.size() == 0 ) return;
	
	if( lines.size() > 0 )
	{
		for( int i = 0; i < lines.size(); i++)
		delete lines[i];
	}
	
	lines.clear();
	
	for( int i = startStroke; i < endStroke; i++)
	{
		lines.push_back( new grafLineDrawer() );
	}
	
	vector<timePt> pts;
	
	int ln = 0;
	
	for( int i = startStroke; i < endStroke; i++)
	{
		lines[ln]->globalAlpha		= lineAlpha;
		lines[ln]->lineScale		= lineScale;
		lines[ln]->bUseFadeInOut	= false;
		
		pts.clear();
		int start	= (i == startStroke ) ? startPt : 0;
		int end		= (i == endStroke	) ? endPt : myTag->myStrokes[i].pts.size();
		
		for( int j = start; j < end; j++)
		{
			pts.push_back(myTag->myStrokes[i].pts[j]);
		}
		
		lines[ln]->setupUniform( pts, .025f);
		
	}
	
	bSetupDrawer = false;
	
}

void grafDrawerRecording::setAlpha(float a)
{
	lineAlpha = a;
	for( int i = 0; i < lines.size(); i++)
	{
		lines[i]->globalAlpha = lineAlpha;
	}
}

void grafDrawerRecording::setLineScale(float val)
{
	if( lineScale != val )
	{
		lineScale = val;
		for( int i = 0; i < lines.size(); i++)
		{
		lines[i]->lineScale = val;
		}
		bSetupDrawer = true;
	}
}


void grafDrawerRecording::transition( float dt, float pct )
{
	average(pct);
	alpha -= .35*dt;
}

void grafDrawerRecording::average( float pct )
{
	for( int i = 0; i < lines.size(); i++)
		lines[i]->average(pct);
	
}

void grafDrawerRecording::draw(int startStroke, int startPoint, int lastStroke, int lastPoint)
{
	
	glEnable( GL_DEPTH_TEST);
	
	for( int i = startStroke; i < lines.size(); i++)
	{
		int startP = (i==startStroke) ? startPoint : 1;
		if( i < lastStroke )		lines[i]->draw(-1,alpha,startPoint);
		else if( i == lastStroke )	lines[i]->draw(lastPoint,alpha,startPoint);
		
		if( i < lastStroke )		lines[i]->drawOutline(-1,alpha,lineWidth,startPoint);
		else if( i == lastStroke )	lines[i]->drawOutline(lastPoint,alpha,lineWidth,startPoint);
	}
	
	glDisable( GL_DEPTH_TEST );
	
	
}


void grafDrawerRecording::drawTimeLine(ofPoint center, float currentTime, float startTime, float z_const, ofTrueTypeFont * font, float scale  )
{
    ofSetColor(255,255,255,255);
	
	float timeStart = startTime;
	float printTime = currentTime;
	currentTime = (1000 * currentTime )/ z_const;
    
	float linePoints[6];
	
	linePoints[0] = center.x;
	linePoints[1] = center.y;
	linePoints[2] = startTime;
	linePoints[3] = center.x;
	linePoints[4] = center.y;
	linePoints[5] = currentTime;
	
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, &linePoints[0]);
	glDrawArrays(GL_LINES, 0, 2);
	
    float h = .010;
    int count = 0;
	
    for( float t = timeStart; t < currentTime; t += (300 / z_const))
    {
		
		if( count % 2 == 0) h = .020;
		else h = .010;
		
		linePoints[2] = t;
		linePoints[4] = center.y-h;
		linePoints[5] = t;
		
		glVertexPointer(3, GL_FLOAT, 0, &linePoints[0]);
		glDrawArrays(GL_LINES, 0, 2);
		
		count++;
    }
	
	
	
    glDisableClientState(GL_VERTEX_ARRAY);
	
	glPushMatrix();
		glTranslatef(center.x,center.y,currentTime);
		glScalef(.5 * (1.0/scale),.5 * (1.0/scale),0);
		font->drawString( ofToString( printTime, 2 ) , 2,0);
    glPopMatrix();
	
}


void grafDrawerRecording::drawBoundingBox( ofPoint pmin, ofPoint pmax, ofPoint pcenter )
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
	
	
	glDisable(GL_VERTEX_ARRAY);
	
	
}


void grafDrawerRecording::drawWireFrame(grafTagMulti * myTag,int startStroke, int startPoint, int lastStroke, int lastPoint)
{
	ofNoFill();
	ofSetColor(0xffffff);
	
	for( int i = 0; i < myTag->myStrokes.size(); i++)
	{
		int start = (i==startStroke) ? startPoint : 0;
		int end = (i==lastStroke) ? lastPoint:  myTag->myStrokes[i].pts.size();
		
		glColor4f(1,1,1,1);
		
		glBegin(GL_LINE_STRIP);
		
		
		for( int j = 0; j < myTag->myStrokes[i].pts.size(); j++)
		{
			glVertex3f(myTag->myStrokes[i].pts[j].pos.x, myTag->myStrokes[i].pts[j].pos.y,myTag->myStrokes[i].pts[j].pos.z);
		}
		
		glEnd();
		
	}
	
}
