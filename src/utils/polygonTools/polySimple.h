/*
 *  polySimple.h
 *  ensanche_2.0
 *
 *  Created by Chris Sugrue on 1/14/10.
 *  Copyright 2010 csugrue. All rights reserved.
 *
 */

#pragma once

#include "ofMain.h"

#define	WIND_COUNTER_CLOCKWISE	0
#define WIND_CLOCKWISE	1

class polySimple{
	
public:
	polySimple();
	~polySimple();
	polySimple(  const polySimple &  mom );
	
	virtual void pushVertex( ofPoint pt );
	virtual void popVertex( ofPoint pt );
	virtual void insertVertex( ofPoint pt, int ptBefore );
	virtual void clear();
	
	virtual void draw();
	
	virtual void setWinding( int direction = WIND_COUNTER_CLOCKWISE);
	
	virtual ofPoint			getCentroid();
	virtual ofRectangle		getBoundingBox();
	virtual float			getArea();
	virtual float			getSignedArea();
	virtual int				getNumVertices();
	virtual ofPoint			getPt( int id );
	
	vector<ofPoint>		pts;				
	int                 nPts;
	bool				bDrawnWithPoints, bClosed;
	
	// mostly for sorting
	float				area;
	void				setArea();
	
	
};
