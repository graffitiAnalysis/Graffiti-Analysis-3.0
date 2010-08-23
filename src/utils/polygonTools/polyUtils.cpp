/*
 *  polyUtils.cpp
 *  ensanche_2.0
 *
 *  Created by Chris on 1/17/10.
 *  Copyright 2010 csugrue. All rights reserved.
 *
 */

#include "polyUtils.h"



bool pointInPolygon(float x, float y, vector<ofPoint>pts)
{
	
	// code via Zach Lieberman via Randolph Franklin...
	int i, j, c = 0;
	int nPts = pts.size();
	
	for (i = 0, j = nPts-1; i < nPts; j = i++) {
		if ((((pts[i].y <= y) && (y < pts[j].y)) ||
			 ((pts[j].y <= y) && (y < pts[i].y))) &&
			(x < (pts[j].x - pts[i].x) * (y - pts[i].y) / (pts[j].y - pts[i].y) + pts[i].x))
			c = !c;
	}
	return (bool) c;
}


bool isInsideRect(float x, float y, ofRectangle rect){
    return ( x >= rect.x && x <= rect.x + rect.width && y >= rect.y && y <= rect.y + rect.height );
}


bool intersectionTwoLines( ofPoint pt1, ofPoint pt2, ofPoint pt3, ofPoint pt4, ofPoint * iPt )
{
	
    iPt->x = 0;
    iPt->y = 0;
	
    float ua =  ( (pt4.x-pt3.x)*(pt1.y-pt3.y) - (pt4.y-pt3.y)*(pt1.x-pt3.x) ) /
	( (pt4.y-pt3.y)*(pt2.x-pt1.x) - (pt4.x-pt3.x)*(pt2.y-pt1.y) );
	
    float ub =  ( (pt2.x-pt1.x)*(pt1.y-pt3.y) - (pt2.y-pt1.y)*(pt1.x-pt3.x) ) /
	( (pt4.y-pt3.y)*(pt2.x-pt1.x) - (pt4.x-pt3.x)*(pt2.y-pt1.y) );
	
	
    if( ua > 0 && ua < 1 && ub > 0 && ub < 1)
        ;
    else
        return false;
	
    iPt->x = pt1.x + ua * (pt2.x-pt1.x);
    iPt->y = pt1.y + ua * (pt2.y-pt1.y);
	
    return true;
}

bool intersectionTwoLines2( ofPoint pt1, ofPoint pt2, ofPoint pt3, ofPoint pt4, ofPoint * iPt )
{
	
    iPt->x = 0;
    iPt->y = 0;
	
    float ua =  ( (pt4.x-pt3.x)*(pt1.y-pt3.y) - (pt4.y-pt3.y)*(pt1.x-pt3.x) ) /
	( (pt4.y-pt3.y)*(pt2.x-pt1.x) - (pt4.x-pt3.x)*(pt2.y-pt1.y) );
	
    float ub =  ( (pt2.x-pt1.x)*(pt1.y-pt3.y) - (pt2.y-pt1.y)*(pt1.x-pt3.x) ) /
	( (pt4.y-pt3.y)*(pt2.x-pt1.x) - (pt4.x-pt3.x)*(pt2.y-pt1.y) );
	
	
    if( ua >= 0 && ua <= 1 && ub >= 0 && ub <= 1)
        ;
    else
        return false;
	
    iPt->x = pt1.x + ua * (pt2.x-pt1.x);
    iPt->y = pt1.y + ua * (pt2.y-pt1.y);
	
    return true;
}

bool findClosestIntersectionLineAndPoly( ofPoint a, ofPoint b, vector<ofPoint> pts, ofPoint & closestPoint, int & sideId )
{
    if(pts.size() <= 0 ) return false;
	
	vector<float> dist;
    vector<ofPoint> ipts;
	vector<int> ids;
	
	//if( pts[0].x != pts[ pts.size()-1].x && pts[0].y != pts[ pts.size()-1].y)
	pts.push_back(pts[0]);
		
    for( int i = 1; i < pts.size(); i++)
    {
		ofPoint iPt;
		if ( intersectionTwoLines( pts[i-1], pts[i], a, b, &iPt ) )
		{
			dist.push_back( ( (a.x-iPt.x) *(a.x-iPt.x) + (a.y-iPt.y)*(a.y-iPt.y) ) );
			ipts.push_back( iPt );
			ids.push_back(i-1);
		}
    }
	
	closestPoint.set(0,0);
	if( ipts.size() <= 0 ) return false;
	
    ofPoint p = ofPoint(0,0);
    float cdist = 0;
	
    for( int i = 0; i < dist.size(); i++)
    {
        if( i == 0 || dist[i] < cdist )
        {
            cdist = dist[i];
            p.set( ipts[i].x,ipts[i].y );
			sideId = ids[i];
        }
    }
	
	closestPoint.set(p.x,p.y);
    
	return true;
	
}


ofPoint distanceToSegment(ofPoint p1, ofPoint p2, ofPoint p3){
	
	double xDelta = p2.x - p1.x;
	double yDelta = p2.y - p1.y;
	
	if ((xDelta == 0) && (yDelta == 0)) {
	    printf("p1 and p2 cannot be the same point");
	}
	
	double u = ((p3.x - p1.x) * xDelta + (p3.y - p1.y) * yDelta) / (xDelta * xDelta + yDelta * yDelta);
	
	ofPoint closestPoint;
	if (u < 0){
	    closestPoint = p1;
	}else if (u > 1){
	    closestPoint = p2;
	}else{
	    closestPoint = ofPoint(p1.x + u * xDelta, p1.y + u * yDelta);
	}
	
	return closestPoint;
}


bool minLineBetweenLineSegments( ofPoint pt1, ofPoint pt2, ofPoint pt3, ofPoint pt4, ofPoint * pA, ofPoint * pB)
{
	
	// check if they intersect
	if( intersectionTwoLines(pt1, pt2, pt3, pt4, pA) )
	{
		pB->x = pA->x;
		pB->y = pA->y;
		return true;
	}else{
		
		vector<float>distSq(4);
		
		ofPoint s[4];
		ofPoint e[4];
		
		e[0] = pt3;
		e[1] = pt4;
		e[2] = pt1;
		e[3] = pt2;
		
		s[0] = distanceToSegment(pt1, pt2, pt3);
		s[1] = distanceToSegment(pt1, pt2, pt4);
		s[2] = distanceToSegment(pt3, pt4, pt1);
		s[3] = distanceToSegment(pt3, pt4, pt2);
		
		distSq[0] = (s[0].x-pt3.x)*(s[0].x-pt3.x)+(s[0].y-pt3.y)*(s[0].y-pt3.y);
		distSq[1] = (s[1].x-pt4.x)*(s[1].x-pt4.x)+(s[1].y-pt4.y)*(s[1].y-pt4.y);
		distSq[2] = (s[2].x-pt1.x)*(s[2].x-pt1.x)+(s[2].y-pt1.y)*(s[2].y-pt1.y);
		distSq[3] = (s[3].x-pt2.x)*(s[3].x-pt2.x)+(s[3].y-pt2.y)*(s[3].y-pt2.y);
		
		int close		= 0;
		float closeD	= 0;
		
		for( int i = 0; i < 4; i++)
		{
			if( i == 0 || distSq[i] < closeD)
			{
				close = i;
				closeD = distSq[i]; 
			}
			
		}
		
		pA->x = s[close].x;
		pA->y = s[close].y;
		pB->x = e[close].x;
		pB->y = e[close].y;
		
		return false;
		
	}
	
	
	
}


bool isIntersectionPoly(ofPoint pt1, ofPoint pt2, vector<ofPoint> pts)
{
	bool bIntersection = false;
	ofPoint iPt = ofPoint(0,0);
	
	//pts.push_back(pts[0]);
	
	for( int i = 0; i < pts.size()-1; i++)
	{
			if( intersectionTwoLines(pt1, pt2, pts[i], pts[i+1], &iPt) )
				bIntersection = true;
	}
	
	return bIntersection;
}

bool isIntersectionPoly(ofPoint pt1, ofPoint pt2, vector<ofPoint> pts, ofPoint & iPt)
{
	bool bIntersection = false;
	
	//pts.push_back(pts[0]);
	
	for( int i = 0; i < pts.size()-1; i++)
	{
		if( intersectionTwoLines(pt1, pt2, pts[i], pts[i+1], &iPt) )
			bIntersection = true;
	}
	
	return bIntersection;
}