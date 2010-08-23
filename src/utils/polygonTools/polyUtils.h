/*
 *  polyUtils.h
 *  ensanche_2.0
 *
 *  Created by Chris on 1/17/10.
 *  Copyright 2010 csugrue. All rights reserved.
 *
 */
#pragma once

#include "ofMain.h"
#include "ofxVectorMath.h"

bool pointInPolygon(float x, float y, vector<ofPoint>pts);

bool isInsideRect(float x, float y, ofRectangle rect);

bool intersectionTwoLines( ofPoint pt1, ofPoint pt2, ofPoint pt3, ofPoint pt4, ofPoint * iPt );
bool intersectionTwoLines2( ofPoint pt1, ofPoint pt2, ofPoint pt3, ofPoint pt4, ofPoint * iPt );

bool findClosestIntersectionLineAndPoly( ofPoint a, ofPoint b, vector<ofPoint> pts, ofPoint & closestPoint, int & sideId );

bool minLineBetweenLineSegments( ofPoint pt1, ofPoint pt2, ofPoint pt3, ofPoint pt4, ofPoint * pA, ofPoint * pB);

ofPoint distanceToSegment(ofPoint p1, ofPoint p2, ofPoint p3);

bool isIntersectionPoly(ofPoint pt1, ofPoint pt2, vector<ofPoint> pts);
bool isIntersectionPoly(ofPoint pt1, ofPoint pt2, vector<ofPoint> pts, ofPoint & iPt);
