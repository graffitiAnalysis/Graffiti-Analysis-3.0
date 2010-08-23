/*
 *  polyEditable.h
 *  ensanche_2.0
 *
 *  Created by Chris on 1/15/10.
 *  Copyright 2010 csugrue. All rights reserved.
 *
 */

#include "polyAdv.h"
#include "ofEvents.h"

#define POLY_EDIT_MODE_ADD_PTS	0
#define POLY_EDIT_MODE_MOVE_PTS 1
#define POLY_EDIT_MODE_MOVE_ALL 2
#define POLY_EDIT_MODE_ROTATE	3
#define POLY_EDIT_MODE_NONE		4

#define POLY_TOTAL_MODES		3

/*
TO DO: 
- add rotation option
- copy constructor
*/

#pragma once

class polyEditable: public polyAdv{
	
public:
	
	polyEditable();
	virtual ~polyEditable();
	polyEditable(  const polyEditable &  mom );
	
	virtual void setup();
	virtual void clear();
	
	virtual void addPoint( ofPoint pt );						// add new point to poly and closes if near start point
	virtual void removePoint();
	virtual void nextMode();									// skip to next edit mode (add point, edit point, move all, rotate)
	virtual void setMode( int md, bool bTemp = false );				// sets which edit mode we are in
	virtual void moveAllPointsBy( ofPoint pt );					// moves all points by pt.x, pt.y amount
	virtual void movePointBy( int index, ofPoint pt);			// moves one point at position index by pt.x,pt.y
	virtual void rotate(float r);								// rotates entire polygon around its center (bounding box center)
	
	virtual void draw(bool bDrawClosed=false);										
	
	virtual void keyPressed(ofKeyEventArgs& event);
	virtual void keyReleased(ofKeyEventArgs& event);
	virtual void mouseMoved(ofMouseEventArgs& event);
	virtual void mouseDragged(ofMouseEventArgs& event);
	virtual void mousePressed(ofMouseEventArgs& event);
	virtual void mouseReleased(ofMouseEventArgs& event);
	
	virtual void enable(){ bEnabled = true;}
	virtual void disable(){ bEnabled = false;}
	
	virtual void setScale( float s );
	virtual void setOffset( ofPoint preR, ofPoint pstR );
	virtual void setGRotation( float r ){ gRotation = r; }
	virtual ofPoint getMouseAltered(ofPoint m);
	
	int getMode(){ return mode; }
	bool isEnabled(){ return bEnabled; }
	
	int uId;
	
	bool bUseClosePoly;		// if true this will automatically close polygon if click close to start point
	bool bUseKeyPress;
	bool bUseRotation;
	
	// for editing
	int selectedPoint;
	
	// params
	float selectDist;	// maximum distance from a point/node for it to be selected
	ofPoint lastMouse, lastCenter, moveOffSet;
	float rotation;		// this is used to perform rendring rotations. should make func to apply actual transformation from this
	
	float scale;				// a rendering scale
	float invScale;				// for mouse input, if we are scaling globally, need proper mouse coords
	float gRotation;			// for mouse input, if we are rotating in draw, need proper mouse coords
	ofPoint transPreRotate,transPstRotate; // this is to reverse engineer translation outside draw for mouse input...maybe there is a matri math op to do this better
	float moveBy;
	
	protected:
		int mode;		// current editing mode
		int prevMode;
		bool bTempMode;
		bool bEnabled;			// enable or disable events

};


