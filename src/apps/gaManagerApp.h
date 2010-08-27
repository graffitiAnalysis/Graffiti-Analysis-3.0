/*
 *  gaManagerApp.h
 *  GA_Interactive
 *
 *  Created by Chris on 8/22/10.
 *  Copyright 2010 csugrue. All rights reserved.
 *
 */

#pragma once

#include "ofMain.h"
#include "polyEditable.h"
#include "grafTagMulti.h"
#include "grafTagMulti.h"
#include "grafIO.h"
#include "grafPlayer.h"
#include "grafCurveSmoother.h"
#include "grafDrawer.h"
#include "grafVParticleField.h"

#define	GA_MODE_RECORDER	0
#define GA_MODE_PLAYER		1
#define GA_MODE_LASER		2
#define GA_MODE_HELP		3
#define GA_MODE_START		4

class GaManagerApp{

	public:
		GaManagerApp();
		~GaManagerApp();
		
		void setup();
		void update();
		void draw();
		
		void setMode(int newMode){ mode = newMode; }
		int getMode(){ return mode; }
		int hitTest(int x, int y);
		
		int mode;
		
	protected:
	
		ofImage	titleImage;
		ofTrueTypeFont		fontS;					// fonts for drawing info to screen
		ofTrueTypeFont		fontSS;					// fonts for drawing info to screen

		vector<polyEditable> polyButtons;
		int xp,yp;
		
		grafTagMulti			tag;
		grafIO					gIO;					// gml loader/saver
		grafCurveSmoother		smoother;				// adds points to smooth tag
		grafDrawer				drawer;					// draws thick time stroked line
		grafVParticleField		particleDrawer;			// draws and animates particles
		grafPlayer				myTagPlayer;			// manages playback of tag in time
		float					rotationY;				// y rotation for current tag
		int screenW, screenH;
		
		bool bSetSplashTag;
		int tModes;
		
};