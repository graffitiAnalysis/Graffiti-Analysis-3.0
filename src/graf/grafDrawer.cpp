#include "grafDrawer.h"


grafDrawer::grafDrawer()
{
	alpha = 1.f;
	minLen = .005;
	lineWidth = 2.0;
	lineAlpha = .92;
	lineScale = .05;
	bSetupDrawer = false;
	pctTransLine = .001;
	flatTime = 0;
}

grafDrawer::~grafDrawer()
{
	for( int i = 0; i < lines.size(); i++)
		delete lines[i];
}

void grafDrawer::setup(grafTagMulti * myTag, float maxLen )
{
	alpha = 1.f;
	
	if( myTag->myStrokes.size() == 0 ) return;
	
	if( lines.size() > 0 )
	{
		for( int i = 0; i < lines.size(); i++)
		delete lines[i];
	}
	
	lines.clear();
	
	for( int i = 0; i < myTag->myStrokes.size(); i++)
	{
		lines.push_back( new grafLineDrawer() );
	}
	
		
	for( int i = 0; i < myTag->myStrokes.size(); i++)
	{
		lines[i]->globalAlpha = lineAlpha;
		lines[i]->lineScale = lineScale;
		lines[i]->setup( myTag->myStrokes[i].pts, minLen, maxLen, .5f);		
	}
	
	bSetupDrawer = false;
	pctTransLine = .001;
	
}

void grafDrawer::setAlpha(float a)
{
	lineAlpha = a;
	for( int i = 0; i < lines.size(); i++)
	{
		lines[i]->globalAlpha = lineAlpha;
	}
}

void grafDrawer::setLineScale(float val)
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


void grafDrawer::transition( float dt, float pct )
{
	average(pct);
	//alpha -= .35*dt;
}


void grafDrawer::transitionDeform( float dt, float pct, float * amps, int numAmps  )
{
	float pctMe = 1-pctTransLine;
	float pctLn = pctTransLine;
	
	
	for( int i = 0; i < lines.size(); i++)
	{
		for( int j = 0; j < lines[i]->pts_l.size(); j++)
		{
			int ps = 1+(j % (numAmps-1));
			float bandH = pct*(amps[ps]);
			
			lines[i]->pts_l[j].y = pctMe*lines[i]->pts_l[j].y+pctLn*(lines[i]->pts_lo[j].y+bandH);//.9*lines[i]->pts_l[j].y+.1*bandH;
			lines[i]->pts_r[j].y = pctMe*lines[i]->pts_r[j].y+pctLn*(lines[i]->pts_ro[j].y+bandH);//.9*lines[i]->pts_r[j].y+.1*bandH;
			lines[i]->pts_lout[j].y = pctMe*lines[i]->pts_lout[j].y+pctLn*(lines[i]->pts_lo[j].y+bandH);//.9*lines[i]->pts_l[j].y+.1*bandH;
			lines[i]->pts_rout[j].y = pctMe*lines[i]->pts_rout[j].y+pctLn*(lines[i]->pts_ro[j].y+bandH);//.9*lines[i]->pts_r[j].y+.1*bandH;
			
		}
		
	}
	
	//if( pctTransLine < .1 ) pctTransLine += .001;
	
}

void grafDrawer::transitionLineWidth( float dt, float avg )
{
	float pctMe = 1-pctTransLine;
	float pctLn = pctTransLine;
	
	
	for( int i = 0; i < lines.size(); i++)
	{
		for( int j = 0; j < lines[i]->pts_l.size(); j++)
		{			
			lines[i]->pts_l[j].x = pctMe*lines[i]->pts_l[j].x+pctLn*(lines[i]->pts_lo[j].x+avg*-lines[i]->vecs[j].x);
			lines[i]->pts_l[j].y = pctMe*lines[i]->pts_l[j].y+pctLn*(lines[i]->pts_lo[j].y+avg*-lines[i]->vecs[j].y);
			
			lines[i]->pts_r[j].x = pctMe*lines[i]->pts_r[j].x+pctLn*(lines[i]->pts_ro[j].x+avg*lines[i]->vecs[j].x);
			lines[i]->pts_r[j].y = pctMe*lines[i]->pts_r[j].y+pctLn*(lines[i]->pts_ro[j].y+avg*lines[i]->vecs[j].y);
		
			lines[i]->pts_lout[j].x = pctMe*lines[i]->pts_lout[j].x+pctLn*(lines[i]->pts_lo[j].x+avg*-lines[i]->vecs[j].x);
			lines[i]->pts_lout[j].y = pctMe*lines[i]->pts_lout[j].y+pctLn*(lines[i]->pts_lo[j].y+avg*-lines[i]->vecs[j].y);
			
			lines[i]->pts_rout[j].x = pctMe*lines[i]->pts_rout[j].x+pctLn*(lines[i]->pts_ro[j].x+avg*lines[i]->vecs[j].x);
			lines[i]->pts_rout[j].y = pctMe*lines[i]->pts_rout[j].y+pctLn*(lines[i]->pts_ro[j].y+avg*lines[i]->vecs[j].y);
			
		}
		
	}
	
	//if( pctTransLine < .1 ) pctTransLine += .001;
	//alpha -= .35*dt;
	
}

void grafDrawer::transitionBounce( float dt, float avg )
{
	float pctMe = 1-pctTransLine;
	float pctLn = pctTransLine;
	
	
	for( int i = 0; i < lines.size(); i++)
	{
		for( int j = 0; j < lines[i]->pts_l.size(); j++)
		{			
			lines[i]->pts_l[j].y = pctMe*lines[i]->pts_l[j].y+pctLn*(lines[i]->pts_lo[j].y+avg);
			lines[i]->pts_r[j].y = pctMe*lines[i]->pts_r[j].y+pctLn*(lines[i]->pts_ro[j].y+avg);
			
			lines[i]->pts_lout[j].y = pctMe*lines[i]->pts_lout[j].y+pctLn*(lines[i]->pts_lo[j].y+avg);
			lines[i]->pts_rout[j].y = pctMe*lines[i]->pts_rout[j].y+pctLn*(lines[i]->pts_ro[j].y+avg);
		}
		
	}
	
	//if( pctTransLine < .1 ) pctTransLine += .001;
	//alpha -= .35*dt;
	
}

void grafDrawer::transitionFlatten(  float zDepth, float timeToDoIt  )
{
	if( flatTime == 0 )
		flatTime = ofGetElapsedTimef();
		
	float pct = 1 - ((ofGetElapsedTimef()-flatTime) / timeToDoIt);
	
	for( int i = 0; i < lines.size(); i++)
	{
		for( int j = 0; j < lines[i]->pts_l.size(); j++)
		{
			lines[i]->pts_l[j].z	= pct*lines[i]->pts_l[j].z		+ (1-pct)*zDepth;
			lines[i]->pts_r[j].z	= pct*lines[i]->pts_r[j].z		+ (1-pct)*zDepth;
			lines[i]->pts_lout[j].z = pct*lines[i]->pts_lout[j].z	+ (1-pct)*zDepth;
			lines[i]->pts_rout[j].z = pct*lines[i]->pts_rout[j].z	+ (1-pct)*zDepth;
		}
	}

}

void grafDrawer::resetTransitions()
{
	pctTransLine	=.001;
	prelimTransTime = 0;
	flatTime		= 0;
}

void grafDrawer::average( float pct )
{
	for( int i = 0; i < lines.size(); i++)
		lines[i]->average(pct);
	
}

void grafDrawer::draw( int lastStroke, int lastPoint)
{
	glEnable( GL_DEPTH_TEST);
	
	for( int i = 0; i < lines.size(); i++)
	{
		if( i < lastStroke )		lines[i]->draw(-1,alpha,-1,lineWidth);
		else if( i == lastStroke )	lines[i]->draw(lastPoint,alpha,-1,lineWidth);
		
		//if( i < lastStroke )		lines[i]->drawOutline(-1,alpha,lineWidth);
		//else if( i == lastStroke )	lines[i]->drawOutline(lastPoint,alpha,lineWidth);
	}
	glDisable( GL_DEPTH_TEST );
	
	
}


void grafDrawer::drawTimeLine(ofPoint center, float currentTime, float startTime, float z_const, ofTrueTypeFont * font, float scale  )
{
    ofSetColor(255,255,255,255);
	
	float timeStart = startTime;
	float printTime = currentTime;
	currentTime = (1000 * currentTime )/ z_const;
    
	float linePoints[6];
	
	linePoints[0] = center.y;
	linePoints[1] = center.x;
	linePoints[2] = startTime;
	linePoints[3] = center.y;
	linePoints[4] = center.x;
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
		linePoints[3] = center.y+h;
		linePoints[5] = t;
		
		glVertexPointer(3, GL_FLOAT, 0, &linePoints[0]);
		glDrawArrays(GL_LINES, 0, 2);
		
		count++;
    }
	
	
    // h = .030;
	
	
	
    glDisableClientState(GL_VERTEX_ARRAY);
	
	glPushMatrix();
		glTranslatef(center.y,center.x,currentTime);
		glRotatef(90,0,0,1);
		glScalef(.5 * (1.0/scale),.5 * (1.0/scale),0);
		font->drawString( ofToString( printTime, 2 ) , 2,0);
    glPopMatrix();
	
}


void grafDrawer::drawBoundingBox( ofPoint pmin, ofPoint pmax, ofPoint pcenter )
{
	glColor4f(1,1,1,1);
	
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
	pos[7] = pmin.y;
	pos[8] = pmin.z;
	
	pos[9]  = pmax.x;
	pos[10] = pmin.y;
	pos[11] = pmax.z;
	
	
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
	// centered
	/*pos[0] = pcenter.x-100;
	pos[1] = pcenter.y;
	pos[2] = pcenter.z;
	
	pos[3] = pcenter.x+100;
	pos[4] = pcenter.y;
	pos[5] = pcenter.z;
	
	pos[6] = pcenter.x;
	pos[7] = pcenter.y-100;
	pos[8] = pcenter.z;
	
	pos[9]  = pcenter.x;
	pos[10] = pcenter.y+100;
	pos[11] = pcenter.z;*/
	
	//glVertexPointer(3, GL_FLOAT, 0, pos);
	//glDrawArrays(GL_LINES, 0, 4);
	
	glDisable(GL_VERTEX_ARRAY);
	
	
}


void grafDrawer::alterZDepth( grafTagMulti * myTag)
{
	
	for( int i = 0; i < myTag->myStrokes.size(); i++)
	{
		for( int j = 0; j < myTag->myStrokes[i].pts.size(); j++)
		{
			lines[i]->pts_l[j].z = myTag->myStrokes[i].pts[j].pos.z;	
			lines[i]->pts_r[j].z = myTag->myStrokes[i].pts[j].pos.z;
			
			lines[i]->pts_lo[j].z = myTag->myStrokes[i].pts[j].pos.z;	
			lines[i]->pts_ro[j].z = myTag->myStrokes[i].pts[j].pos.z;
			
			lines[i]->pts_lout[j].z = myTag->myStrokes[i].pts[j].pos.z;	
			lines[i]->pts_rout[j].z = myTag->myStrokes[i].pts[j].pos.z;
		}
	}
}


void grafDrawer::flattenInstantly(float zDepth){
	for( int i = 0; i < lines.size(); i++){
		for( int j = 0; j < lines[i]->pts_l.size(); j++){
			lines[i]->pts_l[j].z	= zDepth;
			lines[i]->pts_r[j].z	= zDepth;
			lines[i]->pts_lout[j].z = zDepth;
			lines[i]->pts_rout[j].z = zDepth;
		}
	}
}
