/*
 *  polyGroup.cpp
 *  ensanche_2.0
 *
 *  Created by Chris on 3/1/10.
 *  Copyright 2010 csugrue. All rights reserved.
 *
 */

#include "polyGroup.h"

polyGroup::polyGroup()
{
	selectedId	= -1;
	scale			= 1;
	invScale		= 1.f/scale;
	gRotation		= 0;
	transPreRotate.set(0,0);
	transPstRotate.set(0,0); 
	bEnabled = false;
	
	setFillColor(200,200,200,200);
	setStrokeColor(80,80,80,255);
}

polyGroup::~polyGroup()
{
	
	ofRemoveListener(ofEvents.mousePressed, this, &polyGroup::mousePressed);
	ofRemoveListener(ofEvents.mouseReleased, this, &polyGroup::mouseReleased);
	ofRemoveListener(ofEvents.mouseDragged, this, &polyGroup::mouseDragged);
	ofRemoveListener(ofEvents.keyPressed, this, &polyGroup::keyPressed);
	ofRemoveListener(ofEvents.keyReleased, this, &polyGroup::keyReleased);
	
	for( int i = 0; i < polys.size(); i++)
	{
		delete polys[i];
	}
}

void polyGroup::setup()
{
	ofAddListener(ofEvents.mousePressed, this, &polyGroup::mousePressed);
	ofAddListener(ofEvents.mouseReleased, this, &polyGroup::mouseReleased);
	ofAddListener(ofEvents.mouseDragged, this, &polyGroup::mouseDragged);
	ofAddListener(ofEvents.keyPressed, this, &polyGroup::keyPressed);
	ofAddListener(ofEvents.keyReleased, this, &polyGroup::keyReleased);
	
}

void polyGroup::clear()
{
	for( int i = 0; i < polys.size(); i++)
	{
		delete polys[i];
	}
	
	polys.clear();
}

void polyGroup::addPoly()
{
	
	//if( !bEnabled ) return;
	polys.push_back(  new polyEditable() );
	selectedId	= polys.size()-1;

	polys[selectedId]->setOffset(transPreRotate,transPstRotate);
	polys[selectedId]->setScale(scale);
	polys[selectedId]->setGRotation(gRotation);
	polys[selectedId]->enable();	
	polys[selectedId]->setup();
	
	
	
	//polyFWs[selectedId]->setFillColor(fillColor.r, fillColor.g, fillColor.b, fillColor.a);
	//polyFWs[selectedId]->setStrokeColor(strokeColor.r, strokeColor.g, strokeColor.b, strokeColor.a);

}

void polyGroup::addPoint(ofPoint pt, bool bUseMouseOffset)
{
	if(selectedId<0) return;
	
	if(bUseMouseOffset)
	{
	ofPoint m = polys[selectedId]->getMouseAltered(pt);
	polys[selectedId]->addPoint( ofPoint(m.x,m.y) );
	}
	
	else{
		polys[selectedId]->addPoint( pt );

	}
	
}

void polyGroup::disableAll(bool bDisableMe)
{
	//cout << "pGroup Disabled" << endl;
	
	for( int i = 0; i < polys.size(); i++)
	{
		polys[i]->disable();
	}
	
	if(bDisableMe) bEnabled = false;
	
}

void polyGroup::reEnableLast()
{
	//cout << "pGroup enabled" << endl;
	bEnabled = true;
	if(selectedId < 0 ) return;
}

void polyGroup::draw()
{
	
	for( int i = 0; i < polys.size(); i++)
	{
		
		
		if( i == selectedId && polys[i]->bClosed)
		{
			ofRectangle rect = polys[i]->getBoundingBox();
			ofNoFill();
			ofSetColor(100,100,100,255);
			ofRect( rect.x-4,rect.y-4,rect.width+8, rect.height+8);
		}
		
		if(i == selectedId ) ofSetColor(strokeColor.r,strokeColor.g,strokeColor.b,strokeColor.a);
		else ofSetColor(strokeColor.r,strokeColor.g,strokeColor.b,strokeColor.a*.75);
		
		polys[i]->draw(polys[i]->bClosed);
		
	}
	
}										

void polyGroup::keyPressed(ofKeyEventArgs& event)
{
	if( !bEnabled ) return;
	if( event.key == OF_KEY_BACKSPACE )
	{
		if( polys.size() > selectedId )
		{
			if(polys[selectedId]->pts.size()==1)
			{
			delete polys[selectedId];
			polys.erase(polys.begin()+selectedId);
			selectedId = polys.size()-1;
			}
		}
	}
	
}

void polyGroup::keyReleased(ofKeyEventArgs& event)
{
	if( !bEnabled ) return;

}

void polyGroup::mouseMoved(ofMouseEventArgs& event)
{
	if( !bEnabled ) return;

}

void polyGroup::mouseDragged(ofMouseEventArgs& event)
{
	
	if( !bEnabled ) return;
	//if( selectedId > -1 ) polys[selectedId]->enable();
	//cout << "drag selectedId: " << selectedId << endl;
	//if( selectedId > -1 ) cout << "drag selected enabled: " << polys[selectedId]->isEnabled() << endl;
}

void polyGroup::mousePressed(ofMouseEventArgs& event)
{
		
	if( !bEnabled ) return;
	
	disableAll();
	
	//--- check shift key
	int modifier = glutGetModifiers();
		
	//--- if left-click and no modifiers, create new, add points. if r-click choose the closest if inside boundingbox to move it
	if( event.button == 0 && modifier != GLUT_ACTIVE_SHIFT && modifier != GLUT_ACTIVE_CTRL )
	{
			if( selectedId == -1 )
			{
				// make new if we have none
				addPoly();
				addPoint( ofPoint(event.x,event.y) );
				//ofPoint m = polys[selectedId]->getMouseAltered(ofPoint(event.x,event.y));
				//polys[selectedId]->addPoint( ofPoint(m.x,m.y) );
				
			}else if( polys.size() > selectedId )
			{
				if( !polys[selectedId]->bClosed ) 
				{
					//addPoint( ofPoint(event.x,event.y) );
					//ofPoint m = polys[selectedId]->getMouseAltered(ofPoint(event.x,event.y));
					//polys[selectedId]->addPoint( ofPoint(m.x,m.y) );// add pt
				}else{
					//polys[selectedId]->disable();
					
					addPoly(); // make new if last is finished
					selectedId = polys.size()-1;
					//addPoint( ofPoint(event.x,event.y) );
					//ofPoint m = polys[selectedId]->getMouseAltered(ofPoint(event.x,event.y));
					//polys[selectedId]->addPoint( ofPoint(m.x,m.y) );
				}
				
			}
		
			if(polys.size() > selectedId) polys[selectedId]->enable();
			
	}else{
				
		// reset selectedId to none but remember original to restore after
		int lastId = selectedId;
		selectedId = -1;
		float cDist = 0;
		
		// find closest and set as selected
		for( int i = 0; i < polys.size(); i++)
		{
			ofPoint m = polys[i]->getMouseAltered( ofPoint(event.x,event.y) );
			ofRectangle boundingbox = polys[i]->getBoundingBox();
			
			// make bounding area bigger so we can select more easily
			boundingbox.x -= 4;
			boundingbox.y -= 4;
			boundingbox.width	+= 8;
			boundingbox.height	+= 8;
			
			// dist to point
			bool bCloseToCenter = false;
			bool bCloseToEndPt  = false;
			
			// if we are editing points, also check if we are close to point (but out of bounding box ok)
			if( modifier == GLUT_ACTIVE_SHIFT )
			{
				for( int j = 0; j < polys[i]->pts.size() ; j++)
				{
					if( abs(m.x-polys[i]->pts[j].x) < polys[i]->selectDist && 
					    abs(m.y-polys[i]->pts[j].y) < polys[i]->selectDist ) 
						bCloseToEndPt = true;
				}
			}
			
			// check dist to centroid in case bounding very small
			ofPoint c = polys[i]->getCentroid();
			
			for( int j = 0; j < polys[i]->pts.size(); j++)
			{
				
				if( abs(m.x-c.x) < polys[i]->selectDist && 
				    abs(m.y-c.y) < polys[i]->selectDist ) 
					 bCloseToCenter = true;
				
			}	
			
			// if meet requirements see if we are closer to this than last and remember
			if( isInsideRect(m.x, m.y, boundingbox) || bCloseToEndPt || bCloseToCenter)
			{
				float distSq = ( (m.x-c.x)*(m.x-c.x) + (m.y-c.y)*(m.y-c.y) );
				if( selectedId == -1 || distSq < cDist )
				{
					selectedId = i;
					cDist = distSq;
				}
			}
			
		}
		
		// pick the closest
		if( selectedId >= 0 && polys.size() > selectedId )
		{
			polys[selectedId]->enable();
			polys[selectedId]->mousePressed(event);
		}
		
		// if nobody is close, keep previous
		if(selectedId == -1) selectedId = lastId;
	}
	
	//cout << "selectedId: " << selectedId << endl;
	//cout << "selected enabled: " << polys[selectedId]->isEnabled() << endl;
	//polys[selectedId]->enable();
}

void polyGroup::mouseReleased(ofMouseEventArgs& event)
{
	if( !bEnabled ) return;
}

void polyGroup::setScale( float s )
{
	scale = s;
	for( int i = 0; i < polys.size(); i++)
	{
		polys[i]->setScale(s);
	}
	
}

void polyGroup::setOffset( ofPoint preR, ofPoint pstR )
{
	transPreRotate = preR;
	transPstRotate = pstR;
	
	for( int i = 0; i < polys.size(); i++)
	{
		polys[i]->setOffset(preR,pstR);
	}
	
}

void polyGroup::setGRotation( float r )
{
	gRotation = r;
	for( int i = 0; i < polys.size(); i++)
	{
		polys[i]->setGRotation(r);
	}
	
}


/*
void polyGroup::setStraight()
{
	if(selectedId >= 0 && polyFWs.size() > selectedId ){
		polyFWs[selectedId]->setStraight();
	}
}
*/
void polyGroup::setFillColor(int r, int g, int b, int a)
{
	fillColor.r = r;
	fillColor.g = g;
	fillColor.b = b;
	fillColor.a = a;
}

void polyGroup::setStrokeColor(int r, int g, int b, int a)
{
	strokeColor.r = r;
	strokeColor.g = g;
	strokeColor.b = b;
	strokeColor.a = a;
}

