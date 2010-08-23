/*
 *  polyGroup.h
 *  ensanche_2.0
 *
 *  Created by Chris on 3/1/10.
 *  Copyright 2010 csugrue. All rights reserved.
 *
 */

#pragma once

#include "polyUtils.h"
#include "polyEditable.h"

/* NOTE: needs copy constructor?? */

class polyGroup{

	public:
	
	polyGroup();
	~polyGroup();
	
	virtual void setup();
	virtual void clear();
	virtual void addPoly();
	virtual void addPoint(ofPoint pt, bool bUseMouseOffset = true);
	//virtual void addPoly( ofPoint m0, ofPoint m1, float width );
	
	virtual void disableAll(bool bDisableMe = false);
	virtual void reEnableLast();
	
	virtual void draw();										
	
	virtual void keyPressed(ofKeyEventArgs& event);
	virtual void keyReleased(ofKeyEventArgs& event);
	virtual void mouseMoved(ofMouseEventArgs& event);
	virtual void mouseDragged(ofMouseEventArgs& event);
	virtual void mousePressed(ofMouseEventArgs& event);
	virtual void mouseReleased(ofMouseEventArgs& event);
	
	virtual void setFillColor(int r, int g, int b, int a);
	virtual void setStrokeColor(int r, int g, int b, int a);
	virtual void setScale( float s );
	virtual void setOffset( ofPoint preR, ofPoint pstR );
	virtual void setGRotation( float r );
	//virtual void setStraight();
	
	vector<polyEditable*> polys;
	
	int selectedId, lastSelected;
	
	float scale;				// a rendering scale
	float invScale;				// for mouse input, if we are scaling globally, need proper mouse coords
	float gRotation;			// for mouse input, if we are rotating in draw, need proper mouse coords
	ofPoint transPreRotate,transPstRotate; // this is to reverse engineer translation outside draw for mouse input...maybe there is a matri math op to do this better
	
	ofColor fillColor;
	ofColor strokeColor;
	
	bool bEnabled;

};