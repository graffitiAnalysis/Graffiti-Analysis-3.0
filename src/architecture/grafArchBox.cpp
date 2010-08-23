/*
 *  grafArchBox.cpp
 *  GA_Interactive
 *
 *  Created by Chris on 6/6/10.
 *  Copyright 2010 csugrue. All rights reserved.
 *
 */

#include "grafArchBox.h"

void GrafArchBox::draw(){
	
	if(dead) return;
	
	//wow this is a pain
	b2Shape* s = body->GetShapeList();
	const b2XForm& xf = body->GetXForm();
	b2PolygonShape* poly = (b2PolygonShape*)s;
	int count = poly->GetVertexCount();
	const b2Vec2* localVertices = poly->GetVertices();
	b2Assert(count <= b2_maxPolygonVertices);
	b2Vec2 verts[b2_maxPolygonVertices];
	for(int32 i=0; i<count; ++i) {
		verts[i] = b2Mul(xf, localVertices[i]);
	}
	
	
	ofEnableAlphaBlending();
	ofSetColor(255,255,255,alpha*255);
	ofFill();
	ofBeginShape();
	for (int32 i = 0; i <count; i++) {
		ofVertex(verts[i].x*OFX_BOX2D_SCALE, verts[i].y*OFX_BOX2D_SCALE);
	}
	ofEndShape();
	
	/*ofSetColor(0, 255, 255);
	ofNoFill();
	ofBeginShape();
	for (int32 i = 0; i <count; i++) {
		ofVertex(verts[i].x*OFX_BOX2D_SCALE, verts[i].y*OFX_BOX2D_SCALE);
	}
	ofEndShape(true);	*/
	ofDisableAlphaBlending();
}

void GrafArchBox::setAlpha(float a )
{
	alpha = a;
}

void GrafArchBox::updateAlive(float dt)
{
	timeAlive += dt;
	framesAlive++;
}

