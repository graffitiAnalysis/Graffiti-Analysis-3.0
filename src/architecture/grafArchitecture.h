/*
 *  grafArchitecture.h
 *  GA_Interactive
 *
 *  Created by Chris on 6/5/10.
 *  Copyright 2010 csugrue. All rights reserved.
 *
 *  Many thanks to Todd Vanderlin for ofxBox2d!
 */

#pragma once

#include "ofMain.h"
#include "particleSystem.h"
#include "ofxBox2d.h"
#include "grafArchBox.h"
#include "polyGroup.h"
#include "ofxXmlSettings.h"
#include "grafLineDrawer.h"

enum{
TOOL_TYPE_FREE_DRAW, TOOL_TYPE_LINE, TOOL_TYPE_RECT, TOOL_TYPE_CIRCLE, TOOL_TYPE_POLY
};

#define PS_AT_A_TIME	100
#define PS_TOTAL_ALIVE	300
 
class GrafArchitecture{

	public:
		GrafArchitecture();
		~GrafArchitecture();
		
		void reset();
		void clear();
		void setup(int screenW = 1280, int screenH = 800);
		void update(float dt = 1);
		void draw();
		void drawTool();
		void drawTestImage();
		
		void saveToXML(string filename);
		void loadFromXML(string filename);
		
		// create boxes uot of particles from particlesystem
		void createParticleSet(particleSystem * PS);
		void turnOnParticleBoxes(particleSystem * PS);
		
		// creates box2d elements from polygons in polygroup
		void createArchitectureFromPolys(vector<polySimple>polys);

		void setPhysicsParams(float mass, float bounce, float friction);
		
		//---- the world
		ofxBox2d						box2d;			  //	the box2d world
		ofPoint offSet,offSetPre;
		float scale;
		bool bMakingParticles;
		int totalParticlesMade;
		vector<int> keys;		// to randomize order of creating particles that fall

		//---- box for each particle
		vector		<GrafArchBox>		boxes;			  //	defalut box2d rects
				
		//---- box2d physics object of structure
		vector<ofxBox2dLine>	drawLines;				// drawn lines (free or two points)
		//vector<ofxBox2dPolygon> drawPolys;				// drawn polygons
		//vector<ofxBox2dCircle>	drawCircles;			// drawn circles
	
		//---- display
		bool bShowArchitecture;
		
		//---- drawing tool
		bool bDrawingActive;
		int  toolType;
		polyGroup	pGroup;							// group of editable polygons for drawing
		
		//---- debug and testing options
		ofImage			archImage;
		bool			bUseTestImage;
		
		bool			bMadeAll;
		bool			bPauseKill;
		
		int screenW, screenH, floorH;
		
		bool			bSetup;
		
	protected:
	
		float			boxBounce, boxFriction, boxMass;
		

};