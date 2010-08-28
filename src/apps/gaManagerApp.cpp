/*
 *  gaManagerApp.cpp
 *  GA_Interactive
 *
 *  Created by Chris on 8/22/10.
 *  Copyright 2010 csugrue. All rights reserved.
 *
 */

#include "gaManagerApp.h"

GaManagerApp::GaManagerApp()
{
	mode = GA_MODE_START;
	
	titleImage.loadImage("images/title_bg3.png");
	fontS.loadFont("fonts/frabk.ttf",12);
	fontSS.loadFont("fonts/frabk.ttf",10);
	
	bSetSplashTag = false;
	tModes = 2;
}

GaManagerApp::~GaManagerApp()
{
}

void GaManagerApp::setup()
{
	screenW = ofGetWidth();
	screenH = ofGetHeight();
	
	xp = ofGetWidth()-320;//(screenW/2) - (380/2);
	yp = 405;
	
	polyEditable polyTemp;
	for( int i = 0; i < 3; i++)
	{
		polyButtons.push_back(polyTemp);
		
		polyButtons[i].pushVertex( ofPoint(xp,yp+0) );
		polyButtons[i].pushVertex( ofPoint(xp+380,yp+0) );
		polyButtons[i].pushVertex( ofPoint(xp+380,yp+40) );	
		polyButtons[i].pushVertex( ofPoint(xp,yp+40) );
		
		yp += 40;
	}
	
	
	gIO.loadTag("settings/she-04.gml", &tag);
	particleDrawer.setup(ofGetWidth(),ofGetHeight());
	
	rotationY = 0;
	drawer.alpha = .9;
	drawer.lineAlpha = .99;
	particleDrawer.numXtras = 3;
	myTagPlayer.bReset = true;
	
	if( tag.myStrokes.size() > 0 ) bSetSplashTag = true;
	
	if(bSetSplashTag)
	{
		smoother.smoothTag(4, &tag);
		tag.average();
		tag.average();
	}
	
}

float lastTime = 0;

void GaManagerApp::update()
{
	float dt  = ofGetElapsedTimef()-lastTime;
	lastTime  = ofGetElapsedTimef();
	
	if(bSetSplashTag)
	{
	drawer.setup( &tag, tag.distMax );
	
	//---- update tag playing state
	if( !myTagPlayer.bDonePlaying )					
		myTagPlayer.update(&tag);	// normal play, update tag
			
	rotationY += dt;
	
	particleDrawer.update( myTagPlayer.getCurrentPoint(),myTagPlayer.getVelocityForTime(&tag),  dt, myTagPlayer.bReset);
	myTagPlayer.bReset = false;
	
	}
	
}

void GaManagerApp::draw()
{
	
	if(screenW != ofGetWidth() )
	{
		int diffX = ofGetWidth()- screenW;
		for( int i = 0; i < 3; i++)
			polyButtons[i].moveAllPointsBy( ofPoint(diffX,0) );
		
		screenW = ofGetWidth();
		
	}
	
	if(bSetSplashTag)
	{
	
	glEnable(GL_DEPTH_TEST);
	
	glPushMatrix();
	
		glTranslatef(tag.position.x,tag.position.y, 0);
		glTranslatef(ofGetWidth()/2.f, ofGetHeight()/2.f, 0);
		glScalef(tag.position.z,tag.position.z,tag.position.z);
		glRotatef(tag.rotation.x,1,0,0);
		glRotatef(tag.rotation.y+rotationY,0,1,0);
		glRotatef(tag.rotation.z,0,0,1);
	
		glTranslatef(-tag.min.x*tag.drawScale,-tag.min.y*tag.drawScale,-tag.min.z);
		glTranslatef(-tag.center.x*tag.drawScale,-tag.center.y*tag.drawScale,-tag.center.z);
		
		glDisable(GL_DEPTH_TEST);
		particleDrawer.draw(myTagPlayer.getCurrentPoint().z,  ofGetWidth(),  ofGetHeight());
		glEnable(GL_DEPTH_TEST);
						  
						  
		glPushMatrix();
			glScalef( tag.drawScale, tag.drawScale, 1);
			drawer.draw( myTagPlayer.getCurrentStroke(), myTagPlayer.getCurrentId() );
		glPopMatrix();
		
	glPopMatrix();
	
	}
	
	glDisable(GL_DEPTH_TEST);
	
	xp = ofGetWidth()-320;
	yp = 120;
	
	ofEnableAlphaBlending();
	
	ofFill();
	ofSetColor(0, 0, 0, 220);
	ofRect(xp,-10,380,ofGetHeight()+20);
	
	ofNoFill();
	ofSetColor(100, 100, 100);
	ofRect(xp,-10,380,ofGetHeight()+20);
	
	xp += 20;
	
	ofSetColor(255, 255, 255);
	titleImage.draw(xp-3,yp);
	
	yp += titleImage.height + 40;
	
	fontSS.drawString("Graffiti Analysis 3.0 by Evan Roth.",xp,yp);
	fontSS.drawString("Software development by Chris Sugrue.",xp,yp+=20);
	fontSS.drawString("Laser input integration by Theo Watson.",xp,yp+=20);
	fontSS.drawString("Complies with all <GML> standards.",xp,yp+=20);
	fontSS.drawString("GNU General Public License.",xp,yp+=20);
	fontSS.drawString("graffitianalysis.com", xp, yp+=30);
	
	
	yp+=90;
	fontSS.drawString("LAUNCH", xp, yp);
	fontS.drawString("PLAYBACK", xp+60, yp);
	
	fontSS.drawString("LAUNCH", xp, yp+=40);
	fontS.drawString("LASER INPUT ", xp+60, yp);
	
	//fontSS.drawString("LAUNCH", xp, yp+80);
	//fontS.drawString("RECORDER", xp+60, yp+80);
	
	for( int i = 0; i < tModes; i++) polyButtons[i].draw();
	
	
	fontSS.drawString("F1: home", xp, yp+=80);
	fontSS.drawString("x: toggle controls", xp, yp+=20);
	fontSS.drawString("f: toggle fullscreen", xp, yp+=20);
}


int GaManagerApp::hitTest(int x, int y)
{
	if( polyButtons[0].bHitTest(x,y) ) return GA_MODE_PLAYER;
	else if( polyButtons[1].bHitTest(x,y) ) return GA_MODE_LASER;
	//else if( polyButtons[2].bHitTest(x,y) ) return GA_MODE_RECORDER;
	
	return -1;
}

