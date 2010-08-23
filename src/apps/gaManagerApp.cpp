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
}

GaManagerApp::~GaManagerApp()
{
}

void GaManagerApp::setup()
{
}

void GaManagerApp::update()
{
}

void GaManagerApp::draw()
{
	ofSetColor(255, 255, 255);
	
	glPushMatrix();
		glTranslatef(ofGetWidth()/2,ofGetHeight()/2,0);
		ofDrawBitmapString("Temporary menu (will be buttons)\n\nF1: start\nF2: recorder\nF3: player\nF4: laser tag", 0, 0);
	glPopMatrix();
}