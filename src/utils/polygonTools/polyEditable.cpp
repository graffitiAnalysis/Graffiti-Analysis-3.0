/*
 *  polyEditable.cpp
 *  ensanche_2.0
 *
 *  Created by Chris on 1/15/10.
 *  Copyright 2010 csugrue. All rights reserved.
 *
 */

#include "polyEditable.h"

polyEditable::polyEditable()
{
	polySimple::polySimple();
	polyAdv::polyAdv();
	
	nPts			= 0;
	bEnabled		= false;
	bUseKeyPress	= true;
	bUseRotation	= false;
	mode			= POLY_EDIT_MODE_NONE;
	prevMode		= POLY_EDIT_MODE_NONE;
	bTempMode		= false;
	selectedPoint	= -1;
	selectDist		= 4;
	bUseClosePoly	= true;
	rotation		= 0;
	uId				= ofRandom(0,1000000);
	scale			= 1;
	invScale		= 1.f/scale;
	gRotation		= 0;
	transPreRotate.set(0,0);
	transPstRotate.set(0,0); 
	lastMouse.set(0,0);
	moveOffSet.set(0,0);
	moveBy			= 1;
}

polyEditable::~polyEditable()
{
	
	ofRemoveListener(ofEvents.mousePressed, this, &polyEditable::mousePressed);
	ofRemoveListener(ofEvents.mouseReleased, this, &polyEditable::mouseReleased);
	ofRemoveListener(ofEvents.mouseDragged, this, &polyEditable::mouseDragged);
	ofRemoveListener(ofEvents.keyPressed, this, &polyEditable::keyPressed);
	ofRemoveListener(ofEvents.keyReleased, this, &polyEditable::keyReleased);
}

void polyEditable::setup()
{
	ofAddListener(ofEvents.mousePressed, this, &polyEditable::mousePressed);
	ofAddListener(ofEvents.mouseReleased, this, &polyEditable::mouseReleased);
	ofAddListener(ofEvents.mouseDragged, this, &polyEditable::mouseDragged);
	ofAddListener(ofEvents.keyPressed, this, &polyEditable::keyPressed);
	ofAddListener(ofEvents.keyReleased, this, &polyEditable::keyReleased);
	
}

void polyEditable::clear()
{
	//polySimple::clear();
	pts.clear();
	nPts = 0;
	bClosed = false;
	selectedPoint = -1;
	//bEnabled = false;
}

polyEditable::polyEditable( const polyEditable &  mom )
{
	bDrawnWithPoints = mom.bDrawnWithPoints;
	bClosed = mom.bClosed;
	
	pts.assign(mom.pts.begin(),mom.pts.end());
	nPts = mom.nPts;
 
 	uId = mom.uId;
	
	bEnabled = mom.bEnabled;			
	bUseClosePoly = mom.bUseClosePoly;
	bUseKeyPress = mom.bUseKeyPress;
	bUseRotation = mom.bUseRotation;
	
	selectedPoint = mom.selectedPoint;
	
	selectDist = mom.selectDist;
	lastMouse = mom.lastMouse; 
	lastCenter = mom.lastCenter; 
	moveOffSet = mom.moveOffSet;
	rotation = mom.rotation;			
	
	scale = mom.scale;		
	invScale = mom.invScale;	
	gRotation = mom.gRotation;			
	transPreRotate = mom.transPreRotate;
	transPstRotate = mom.transPstRotate;
	
	mode = mom.mode;		
	prevMode = mom.prevMode;
	bTempMode = mom.bTempMode;
	moveBy = mom.moveBy;
	
}
//--------------------------------------------------------------
//
//	
//
//--------------------------------------------------------------

void polyEditable::addPoint( ofPoint pt )
{
	//cout << "add point " << endl;
	if( bUseClosePoly && pts.size() > 2)
	{
		if( abs( (int)(pts[0].x-pt.x) ) <= selectDist && abs( (int)(pts[0].y-pt.y) ) <= selectDist )
		{
			bClosed = true;
		}
	
	}
	
	if(!bClosed) pushVertex( pt );
	selectedPoint = pts.size()-1;
	//cout << "editable add " << endl;
	
}

void polyEditable::removePoint()
{
	//cout << "remove " << selectedPoint << " " << pts.size() << endl;
	
	//if( mode == POLY_EDIT_MODE_ADD_PTS ) selectedPoint = pts.size()-1;
	
	if( pts.size() > selectedPoint ){
		pts.erase( pts.begin()+selectedPoint );
		selectedPoint--;
		if( pts.size() <= 1 ) bClosed = false;
	}
	
	if(bClosed) bClosed = false;
}

void polyEditable::nextMode()
{
	
	int nMode = mode + 1;
	nMode  %= POLY_TOTAL_MODES;
	setMode( nMode );
	
	if( mode == POLY_EDIT_MODE_MOVE_PTS )
	{
		selectedPoint = pts.size()-1;
	}
}

void polyEditable::setMode( int md, bool bTemp )
{
	prevMode = mode;	
	mode = md;
	bTempMode = bTemp;
}

//--------------------------------------------------------------
//
//	Events
//
//--------------------------------------------------------------

void polyEditable::keyPressed(ofKeyEventArgs& event)
{
	//cout << "poly e key: "<< bEnabled << " " << bUseKeyPress << endl;
	
	if( !bEnabled || !bUseKeyPress ) return;
	
	if( event.key == ' ')	nextMode();
		
	// Note:; may not work cross-platform, tab through points
	if(event.key == 9 )
	{
		if(  mode == POLY_EDIT_MODE_MOVE_PTS )
		{
			selectedPoint++;
			selectedPoint %= (int)(pts.size());
		} 
			
	}
	
	//cout << "key " << event.key << " " << OF_KEY_BACKSPACE << endl;
	if( event.key == OF_KEY_BACKSPACE ) removePoint();

	
	if( event.key == OF_KEY_UP )
	{
		ofxVec2f mup = ofxVec2f(0,-moveBy);
		mup.rotate(-gRotation);
		if( mode == POLY_EDIT_MODE_MOVE_ALL )		moveAllPointsBy( ofPoint(mup.x,mup.y) );//ofPoint(0,-1) );
		else if( mode == POLY_EDIT_MODE_MOVE_PTS )	movePointBy( selectedPoint, ofPoint(mup.x,mup.y) );//ofPoint(0,-1) );
	}
	else if( event.key == OF_KEY_DOWN)
	{
		ofxVec2f mdwn = ofxVec2f(0,moveBy);
		mdwn.rotate(-gRotation);
		if( mode == POLY_EDIT_MODE_MOVE_ALL )		moveAllPointsBy( ofPoint(mdwn.x,mdwn.y) );
		else if( mode == POLY_EDIT_MODE_MOVE_PTS )	movePointBy( selectedPoint, ofPoint(mdwn.x,mdwn.y) );

	}
	else if( event.key == OF_KEY_LEFT )
	{
		ofxVec2f m = ofxVec2f(-moveBy,0);
		m.rotate(-gRotation);
		
		if( mode == POLY_EDIT_MODE_MOVE_ALL )		moveAllPointsBy( ofPoint(m.x,m.y) );
		else if( mode == POLY_EDIT_MODE_MOVE_PTS )	movePointBy( selectedPoint, ofPoint(m.x,m.y) );
		else if( mode == POLY_EDIT_MODE_ROTATE )	rotate(-.5);

	}
	else if( event.key == OF_KEY_RIGHT )
	{
		ofxVec2f m = ofxVec2f(moveBy,0);
		m.rotate(-gRotation);
		if( mode == POLY_EDIT_MODE_MOVE_ALL )		moveAllPointsBy( ofPoint(m.x,m.y) );
		else if( mode == POLY_EDIT_MODE_MOVE_PTS )	movePointBy( selectedPoint, ofPoint(m.x,m.y));
		else if( mode == POLY_EDIT_MODE_ROTATE )	rotate(.5);

	}
}

void polyEditable::keyReleased(ofKeyEventArgs& event)
{
	if( !bEnabled ) return;

}

ofPoint polyEditable::getMouseAltered(ofPoint m)
{
	

	ofxVec2f mpt = ofxVec2f(m.x-transPreRotate.x,m.y-transPreRotate.y);
	mpt.rotate(-gRotation);
	mpt.x -= transPstRotate.x;
	mpt.y -= transPstRotate.y;
	mpt.x *= invScale;
	mpt.y *= invScale;
	
	m.x = mpt.x;
	m.y = mpt.y;
	
	return m;
}

void polyEditable::mouseMoved(ofMouseEventArgs& event)
{
	
	lastMouse.set( event.x, event.y );
	if( !bEnabled ) return;
}


void polyEditable::mouseDragged(ofMouseEventArgs& event)
{
	
	//cout << uId << " drag enabled:  " << isEnabled() << endl;
	
	if( !bEnabled ) return;
	
	
	
	if(event.button != 0 ) setMode(POLY_EDIT_MODE_MOVE_ALL);
	//else if(event.button != 0 )				setMode(POLY_EDIT_MODE_ROTATE);
	else if( bUseRotation && !bTempMode)					setMode(POLY_EDIT_MODE_MOVE_ALL);
	
	//cout << "edit mode: " << mode << endl;
	
	ofPoint m = getMouseAltered( ofPoint(event.x,event.y) );	
	
	if( mode == POLY_EDIT_MODE_MOVE_PTS )
	{
		if(selectedPoint >= 0)
			pts[ selectedPoint ].set( m.x, m.y);
	
	}else if( mode == POLY_EDIT_MODE_MOVE_ALL )
	{
		
		ofPoint diff = ofPoint( (m.x-lastCenter.x)-moveOffSet.x,(m.y-lastCenter.y)-moveOffSet.y);
		
		if( diff.x == diff.x && diff.y == diff.y )
			moveAllPointsBy( diff );
		
		lastCenter = getCentroid();
		
	}else if( mode == POLY_EDIT_MODE_ROTATE)
	{
		if(bUseRotation) rotation += .5*(event.y-lastMouse.y);
		else			 rotate(.5*(event.y-lastMouse.y));
	}
	
	lastMouse.set( event.x, event.y );

}


void polyEditable::mousePressed(ofMouseEventArgs& event)
{
	
	if( !bEnabled ) return;
	
	lastMouse.set( event.x, event.y );
	ofPoint m = getMouseAltered( ofPoint(event.x,event.y) );//ofPoint( (event.x-offSet.x)*invScale, (event.y-offSet.y)*invScale);

	int modifier = glutGetModifiers();

	if( modifier == GLUT_ACTIVE_SHIFT)
	{
		setMode(POLY_EDIT_MODE_MOVE_PTS);//,true);
	}else if( modifier == GLUT_ACTIVE_CTRL )
	{
		setMode(POLY_EDIT_MODE_ROTATE,true);
	}else if( mode == POLY_EDIT_MODE_MOVE_PTS && bTempMode )
		setMode(prevMode);
	else if( event.button == 0  ) setMode(POLY_EDIT_MODE_ADD_PTS);
	//else setMode(POLY_EDIT_MODE_MOVE_ALL);
	
	
	if( mode == POLY_EDIT_MODE_ADD_PTS && event.button == 0 )
	{
		
		//--- add point here
		addPoint( ofPoint(m.x, m.y) );
	
	}else if( mode == POLY_EDIT_MODE_MOVE_PTS)
	{
		selectedPoint = -1;
		
		//--- select points, chooses first closest (must use tab to go next point for close points)
		for( int i = 0; i < pts.size(); i++)
		{
			if( abs( (int)(m.x-pts[i].x) ) < selectDist && abs( (int)(m.y-pts[i].y) ) < selectDist )
			{
				selectedPoint = i;
				break;
			}
		}
		
		
	}
	
	lastCenter = getCentroid();
	moveOffSet.set(m.x-lastCenter.x,m.y-lastCenter.y);
}


void polyEditable::mouseReleased(ofMouseEventArgs& event)
{
	
	if( !bEnabled ) return;
	lastMouse.set( event.x, event.y );
	moveOffSet.set(0,0);
	
	if( bTempMode ) setMode(prevMode);
}

//--------------------------------------------------------------
//
//	Drawing
//
//--------------------------------------------------------------

void polyEditable::draw(bool bDrawClosed)
{	
		
	//cout << uId << " : " << bEnabled << endl;
	
	//polySimple::draw();
	ofBeginShape();
	for( int i = 0; i < pts.size(); i++)
	{
		ofVertex(pts[i].x,pts[i].y);
	}
	ofEndShape(bDrawClosed);
	
	if( mode == POLY_EDIT_MODE_MOVE_ALL)
	{
		//ofRectangle boundingBox = getBoundingBox();
		//ofRect( boundingBox.x, boundingBox.y, boundingBox.width, boundingBox.height);
	}
	
	
	//if( mode == POLY_EDIT_MODE_MOVE_PTS )
	//{
	//-- draw selected point
	if( selectedPoint >= 0 )
	{
		ofNoFill();
		if(pts.size() > selectedPoint) ofCircle(pts[selectedPoint].x, pts[selectedPoint].y, 4);
		if(mode == POLY_EDIT_MODE_MOVE_PTS )
		{
		ofFill();
		ofSetColor(255, 255, 255,255);
		if(pts.size() > selectedPoint) ofCircle(pts[selectedPoint].x, pts[selectedPoint].y, 1);
		
		ofNoFill();
		ofSetColor(255, 0, 0,0);
		if(pts.size() > selectedPoint) ofCircle(pts[selectedPoint].x, pts[selectedPoint].y, 1);
		}
		
		/*if(mode == POLY_EDIT_MODE_MOVE_PTS )
		{
			ofNoFill();
			ofSetColor(100,100,100,200);
			for( int i = 0; i < pts.size(); i++)
			{
				
				if( i != selectedPoint) ofCircle(pts[i].x, pts[i].y, 4);
			}
		}*/
	}
	
	//}else 
	
}
//--------------------------------------------------------------
//
//	Editing points
//
//--------------------------------------------------------------

void polyEditable::moveAllPointsBy( ofPoint pt )
{
	for( int i = 0; i < pts.size(); i++)
	{
		pts[i].x += pt.x;
		pts[i].y += pt.y;
	}
}

void polyEditable::movePointBy( int index, ofPoint pt)
{
	if( index < pts.size() && index >= 0 )
	{
		pts[index].x += pt.x;
		pts[index].y += pt.y;
	}
}

void polyEditable::rotate( float r )
{
	// find center
	ofRectangle rect = getBoundingBox();
	ofPoint cpt;
	cpt.x = rect.x + (rect.width*.5);
	cpt.y = rect.y + (rect.height*.5);
	
	for( int i = 0; i < pts.size(); i++)
	{
		ofxVec2f vPt = ofxVec2f(pts[i].x,pts[i].y);
		vPt.rotate(r,cpt);
		pts[i].set(vPt.x,vPt.y);
	}
	
}

void polyEditable::setScale( float s )
{
	if( s == 0 ) return;
	scale = s;
	invScale = 1.f / scale;
}

void polyEditable::setOffset( ofPoint preR, ofPoint pstR )
{
	transPreRotate.set(preR.x,preR.y);
	transPstRotate.set(pstR.x,pstR.y);
}