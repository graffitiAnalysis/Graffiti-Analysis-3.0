/*
 *  grafLaserApp.h
 *  GA_Interactive
 *
 *  Created by Chris on 8/25/10.
 *  Copyright 2010 csugrue. All rights reserved.
 *
 */

#pragma once

#include "ofMain.h"
#include "ofxControlPanel.h"
#include "laserTracking.h"
#include "audioAnalyzer.h"
#include "dropParticles.h"
#include "grafArchitecture.h"
#include "perspectiveWarper.h"
#include "grafTagMulti.h"
#include "grafIO.h"
#include "grafPlayer.h"
#include "grafCurveSmoother.h"
#include "grafDrawer.h"
#include "grafVParticleField.h"
#include "ofxFBOTexture.h"
#include "grafConstants.h"

typedef vector <ofPoint> simpleStroke;

class GrafLaserApp{

	public:
		GrafLaserApp();
		~GrafLaserApp();
		
		void setup();
		void update();
		void draw();
		void drawTagLaser();
		void drawControls();
		
		void keyPressed(ofKeyEventArgs & event);
		void keyReleased(ofKeyEventArgs & event);
		void mouseMoved(ofMouseEventArgs & event );
		void mouseDragged(ofMouseEventArgs & event );
		void mousePressed(ofMouseEventArgs & event );
		void mouseReleased(ofMouseEventArgs & event );
		
		void enable(){ bEnabled = true; }
		void disable(){ bEnabled = false; }
		
		void saveTag();
		string getTimeString();

		ofxControlPanel		panel;
		bool	bSetup;
		bool	bUseAudio;
		bool	bUseArchitecture;
		bool	bShowPanel;
		
		//---------- audio analysis
		AudioAnalyzer	audio;
	
	protected:
		
		//---------- laser io 
		bool checkLaserActive();
		void handleLaserRecord();
		void handleLaserPlayback();
		void checkLaserHitState();
		
		laserTracking laserTracker;
		vector <simpleStroke> simpleLine;
		float smoothX, smoothY;
		float lastTimePointAddedF;
	
	
		//----- updates
		void updateParticles();
		void updateTransition( int type);
		void updateAudio();
		void updateZDepth(float z_const);
		
		//---- tag saving
		void saveTagPositions();
		void saveAllTagPositions();
		
		//---- tag playback management
		void resetPlayer(int next);
		void nextTag(int dir=1);
		void clearAll();
		
		//--- control panel, interface etc
		void setupControlPanel();
		void updateControlPanel(bool bUpdateAll = false);
		string getCurrentTagName();

		//---------- interactive architecture
		//void createWarpedArchitecture();
		GrafArchitecture		archPhysics;		// physics / architecture effects
		vector<polySimple> wPolys;
		
		//---------- audio particle effects
		DropParticles	drops;
		
		//---------- gml tags
		vector<grafTagMulti> tags;						// vector of all loaded tags
		int currentTagID;								// id of the current tag being played 
		float	z_const;
		bool	bUseGlobalZ;
		int		lastUpdatedZ;
		grafIO	gIO;
		
		//---------- player to animate the tag 
		grafPlayer	myTagPlayer;						// manages playback of tag in time
	
		//---------- smooths curve for nice drawing
		grafCurveSmoother smoother;						// adds points to smooth tag
	
		//---------- drawing
		grafDrawer			drawer;						// draws thick time stroked line
		grafVParticleField	particleDrawer;				// draws and animates particles
		float				rotationY;					// y rotation for current tag
		ofPoint				tagPosVel;					// vel for moving tag around on screen
		float				tagMoveForce;				// force for moving tag around
		int					prevStroke;					// for multi stroke tags, to help particle animation
		float				rotFixTime;					// timer for making rotation move to particular position in a fixed time frame
		
		//---------- fbo + warping
		ofxFBOTexture		fbo;
		PerspectiveWarper	pWarper;
		//ofxFBOTexture		fboLeft;
		//ofxFBOTexture		fboRight;
	
	
		//---------- application settings
		int						screenW,screenH;	
		float					lastX,lastY;
		float					dt;					// difference time
		float					waitTimer;			// counter to pause after animation finished
		float					waitTime;			// time to pause
		string					pathToSettings;
		string					myTagDirectory;			// directory to load from (changeable in data/settings/directorySettings.xml
		int						mode;				// current play mode PLAY_MODE_LOAD for loading, PLAY_MODE_PLAY for normal playback
		bool					bEnabled;
		bool					bRotating;			// toggle auto rotation on/off
		bool					bUseGravity;
		
		float lastTime;

		
};
