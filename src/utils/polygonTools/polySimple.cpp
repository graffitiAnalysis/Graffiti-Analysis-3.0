/*
 *  polySimple.cpp
 *  ensanche_2.0
 *
 *  Created by Chris Sugrue on 1/14/10.
 *  Copyright 2010 csugrue. All rights reserved.
 *
 */

#include "polySimple.h"

polySimple::polySimple()
{
	nPts = 0;
	bDrawnWithPoints = true;
	bClosed = false;
	area = 0;
}

polySimple::~polySimple()
{
}

polySimple::polySimple(  const polySimple &  mom )
{
	bDrawnWithPoints = mom.bDrawnWithPoints;
	bClosed = mom.bClosed;
	pts.assign(mom.pts.begin(),mom.pts.end());
	nPts = mom.nPts;
	area = mom.area;

}

void polySimple::pushVertex( ofPoint pt )
{
	pts.push_back(pt);
}

void polySimple::popVertex( ofPoint pt )
{
	if(pts.size() > 0)
		pts.pop_back();
}

void polySimple::insertVertex( ofPoint pt, int ptBefore )
{
	if( pts.size() > ptBefore)
		pts.insert(pts.begin()+ptBefore,pt);
}

void polySimple::clear()
{
	pts.clear();
	nPts = 0;
}

void polySimple::draw()
{
	ofBeginShape();
	for( int i = 0; i < pts.size(); i++)
	{
		ofVertex(pts[i].x,pts[i].y);
	}
	ofEndShape(bClosed);
	
	if( bDrawnWithPoints )
	{
		// NOTE: change to points not circles (this is slow)
		for( int i = 0; i < pts.size(); i++)
		{
			ofCircle(pts[i].x,pts[i].y,1);
		}
	}
	
}

void polySimple::setWinding( int direction)
{
	bool bCounterClockwise = true;
	
	float area = getSignedArea();
    if( area < 0 ) bCounterClockwise = false;
    
	if( (bCounterClockwise && direction==WIND_CLOCKWISE) || (!bCounterClockwise && direction==WIND_COUNTER_CLOCKWISE) )
	{
		vector<ofPoint> temp;
		
		for( int i = 0; i < pts.size(); i++) temp.push_back(pts[i]);
		
		int j = 0;
		for( int i = pts.size()-1; i >=0; i--)
		{
			pts[i] = temp[j]; j++;
		}
	}
	
}

ofPoint polySimple::getCentroid()
{
	float cx=0;
	float cy=0;
	float A =getSignedArea();
	
	ofPoint res;
	int j = 0;
    int N = pts.size();
	
	float factor=0;
	for (int i=0; i<N; i++)
	{
		j = (i + 1) % N;
		factor=(pts[i].x*pts[j].y-pts[j].x*pts[i].y);
		cx+=(pts[i].x+pts[j].x)*factor;
		cy+=(pts[i].y+pts[j].y)*factor;
	}
	
	A*=6.0f;
	factor=1.f/A;
	cx*=factor;
	cy*=factor;
	res.x=cx;
	res.y=cy;
	
	return res;
}

ofRectangle polySimple::getBoundingBox()
{
	if( pts.size() <= 0 ) return ofRectangle(0,0,0,0);
	
	ofPoint min = pts[0];
    ofPoint max = pts[0];
	
    for( int i = 1; i < pts.size(); i++)
    {
        if(pts[i].x<min.x) min.x = pts[i].x;
        if(pts[i].y<min.y) min.y = pts[i].y;
        if(pts[i].x>max.x) max.x = pts[i].x;
        if(pts[i].y>max.y) max.y = pts[i].y;
    }
	
    return ofRectangle( min.x, min.y, max.x-min.x, max.y-min.y);
	
}

float polySimple::getArea()
{
	double area = 0;
	
	int n = pts.size();
	
	for (int i=0; i < n; i++)
	{
		int j = (i + 1) % n;
		area += pts[i].x * pts[j].y;
		area -= pts[i].y * pts[j].x;
	}
	
	area /= 2.f;
	return (area < 0 ? -area : area);
}

float polySimple::getSignedArea()
{
    double area = 0;
	
	int n = pts.size();
	
	for (int i=0; i < n; i++)
	{
		int j = (i + 1) % n;
		area += pts[i].x * pts[j].y;
		area -= pts[i].y * pts[j].x;
	}
	
	area /= 2.f;
	return area;
}

int polySimple::getNumVertices()
{
	return pts.size();
}

ofPoint polySimple::getPt( int id )
{
	if( id < pts.size() ) return pts[id];
	else return NULL;
}


void polySimple::setArea()
{
area = getArea();
}
