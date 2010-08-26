/*
 *  grafLaserApp.cpp
 *  GA_Interactive
 *
 *  Created by Chris on 8/25/10.
 *  Copyright 2010 csugrue. All rights reserved.
 *
 */

#include "grafLaserApp.h"

GrafLaserApp::GrafLaserApp()
{
	bSetup = false;
	bEnabled = false;
}

GrafLaserApp::~GrafLaserApp()
{
	if(bSetup)
	{
		ofRemoveListener(ofEvents.mouseMoved, this, &GrafLaserApp::mouseMoved);
		ofRemoveListener(ofEvents.mousePressed, this, &GrafLaserApp::mousePressed);
		ofRemoveListener(ofEvents.mouseReleased, this, &GrafLaserApp::mouseReleased);
		ofRemoveListener(ofEvents.mouseDragged, this, &GrafLaserApp::mouseDragged);
		ofRemoveListener(ofEvents.keyPressed, this, &GrafLaserApp::keyPressed);
		ofRemoveListener(ofEvents.keyReleased, this, &GrafLaserApp::keyReleased);
	}
}

void GrafLaserApp::setup()
{
	bSetup = true;
	bEnabled = true;

	//-- laser specific
	smoothX			= 0.0;
	smoothY			= 0.0;
	
	//--- user settings
	ofxXmlSettings xmlUser;
	xmlUser.loadFile("projects/user.xml");
	string username = xmlUser.getValue("params:user","default");
	
	pathToSettings = "projects/"+username+"/settings/";//"settings/default/";
	myTagDirectory = "projects/"+username+"/tags/";
	
	//--- listeners for mouse, keyboard
	ofAddListener(ofEvents.mouseMoved, this, &GrafLaserApp::mouseMoved);
	ofAddListener(ofEvents.mousePressed, this, &GrafLaserApp::mousePressed);
	ofAddListener(ofEvents.mouseReleased, this, &GrafLaserApp::mouseReleased);
	ofAddListener(ofEvents.mouseDragged, this, &GrafLaserApp::mouseDragged);
	ofAddListener(ofEvents.keyPressed, this, &GrafLaserApp::keyPressed);
	ofAddListener(ofEvents.keyReleased, this, &GrafLaserApp::keyReleased);
	
	mode			= PLAY_MODE_RECORD;
	bShowPanel		= true;
	bUseGravity		= true;
	bUseAudio		= true;
	bUseArchitecture= true;	
	
	prevStroke		= 0;
	currentTagID	= 0;
	waitTime		= 2.f;
	waitTimer		= waitTime;
	rotationY		= -45;
	tagMoveForce	= .1;
	tagPosVel.set(0,0,0);
	z_const			= 16;
	bUseGlobalZ		= false;
	lastUpdatedZ	= -1;
	screenW = ofGetWidth();
	screenH = ofGetHeight();
	
	
	//--- controls
	setupControlPanel();
	updateControlPanel();
	
	//--- particles
	particleDrawer.setup(screenW,screenH);
	
	//--- fbo
	fbo.allocate(screenW,screenH );
	
	//--- warper
	pWarper.initWarp( screenW,screenH,screenW*WARP_DIV,screenH*WARP_DIV );
	pWarper.recalculateWarp();
	pWarper.loadFromXml(pathToSettings+"warper.xml");
	
	//--- audio
	audio.setup();
	
	//--- architecture
	archPhysics.setup(screenW,screenH);
		
	if(bUseArchitecture)
	{
		archPhysics.archImage.loadImage(pathToSettings+"arch.jpg");
		archPhysics.loadFromXML(pathToSettings+"architecture.xml");
		//createWarpedArchitecture();
	}
	
	//--- red blue stuff
	fboLeft.allocate(screenW,screenH );
	fboRight.allocate(screenW,screenH );
	
	grafTagMulti temptTag;
	tags.push_back( temptTag );
	
	//---
	bSetup = true;
		
}

void GrafLaserApp::update()
{
	if( mode == PLAY_MODE_RECORD )
		handleLaserRecord();
	if( mode == PLAY_MODE_PLAY )
		handleLaserPlayback();
		
	updateControlPanel();
}

void GrafLaserApp::draw()
{
	if( panel.getSelectedPanelName() == "Laser Tracker" && bShowPanel ){
		laserTracker.drawPanels(0, 0);
	}
	
	panel.draw();
}

void GrafLaserApp::keyPressed(ofKeyEventArgs & event)
{
	if(!bEnabled) return;
}

void GrafLaserApp::keyReleased(ofKeyEventArgs & event)
{
	if(!bEnabled) return;

}

void GrafLaserApp::mouseMoved(ofMouseEventArgs & event )
{
	if(!bEnabled) return;

}

void GrafLaserApp::mouseDragged(ofMouseEventArgs & event )
{
	if(!bEnabled) return;
	
	if( bShowPanel && !panel.minimize) 
	{
		panel.mouseDragged(event.x,event.y,event.button);
	}
}

void GrafLaserApp::mousePressed(ofMouseEventArgs & event )
{
	if(!bEnabled) return;
	
	if( bShowPanel ) panel.mousePressed(event.x,event.y,event.button);
	
}

void GrafLaserApp::mouseReleased(ofMouseEventArgs & event )
{
	if(!bEnabled) return;
	
	if( bShowPanel && !panel.minimize) 
	{
		panel.mouseReleased();
	}
}


void GrafLaserApp::setupControlPanel()
{
	
	panel.setup("GA 3.0 Laser", ofGetWidth()-320, 20, 300, 600);
	panel.addPanel("App Settings", 1, false);
	panel.addPanel("Draw Settings", 1, false);
	panel.addPanel("Audio Settings", 1, false);
	panel.addPanel("Architecture Drawing", 1, false);
	panel.addPanel("Architecture Settings", 1, false);
	panel.addPanel("FBO Warper", 1, false);
	panel.addPanel("Laser Tracker", 1, false);
	panel.addPanel("Extra Laser Settings", 1, false);
	
	panel.setWhichPanel("Laser Tracker");
	panel.addToggle("Use Laser", "useLaser", false);
	panel.addToggle("Video/Camera", "bCamera", false);
	panel.addToggle("camera settings", "camSettings", false);
	panel.addToggle("show panel", "bShowPanel", false);
	panel.addToggle("laser mode", "laserMode", true);
	panel.addSlider("which camera", "whichCamera", 0, 0, 10, true);
	panel.addSlider("Hue", "hue", 0.0, 0, 1.0, false);
	panel.addSlider("Hue Width", "hueWidth", 0, 0, 1.0, false);
	panel.addSlider("Saturation", "saturation", 0, 0, 1.0, false);
	panel.addSlider("Value", "value", 0, 0, 1.0, false);
	panel.addCustomRect("colors tracked", &laserTracker, 140, 40);
	
	panel.setWhichPanel("Extra Laser Settings");
	panel.addToggle("use clear zone", "bUseClearZone", true);
	panel.addToggle("auto end tag rec", "bAutoEndTag", true);
	panel.addSlider("auto end time", "autoEndTime", 2.2, 1.0, 5.0, false);
	panel.addSlider("no. frames new stroke", "nFramesNewStroke", 12, 5, 30, true);
	panel.addSlider("new stroke jump dist", "jumpDist", 0.2, 0.1, 0.99, false);	
	panel.addToggle("laser move", "laserMove", true);
	panel.addSlider("laser rotation amount", "laserRotAmount", 1.0, 0.1, 10.0, false);
	panel.addSlider("laser rotation slowdown", "laserSlowRate", 0.98, 0.88, 0.9999, false);
	
	//--- load saved
	panel.loadSettings(pathToSettings+"laserSettings.xml");
	
	panel.setValueB("laserMode", false);
	
	panel.update();
}

void GrafLaserApp::updateControlPanel(bool bUpdateAll)
{
	
	panel.update();
	panel.clearAllChanged();
}


//--------
bool GrafLaserApp::checkLaserActive(){
	if( panel.hasValueChanged("useLaser") || panel.hasValueChanged("bCamera") ){
		if( panel.getValueB("bCamera") ){
			laserTracker.setupCamera( panel.getValueI("whichCamera"), 320, 240);
		}else{
			laserTracker.setupVideo("laserTestVideo.mov");
		}
	}
	
	if( panel.getValueB("bCamera") && panel.getValueB("camSettings") ){
		panel.setValueB("camSettings", false);
		laserTracker.openCameraSettings();
	}
	
	return panel.getValueB("useLaser");
}

//--------
void GrafLaserApp::handleLaserPlayback(){
	laserTracker.clearNewStroke();
	
	if( !checkLaserActive() ){
		return;
	}
	
	if( panel.hasValueChanged("laserMode") ){
		resetPlayer(0);
		//tags[currentTagID].center.x  = 0.5;
		//tags[currentTagID].center.y  = 0.5;
		tags[currentTagID].drawScale = screenH;
		smoother.smoothTag(3,&tags[currentTagID]);
		
		drawer.transitionFlatten( 0, 1);
		//particleDrawer.flatten( tags[currentTagID].center.z, 1);		
	}
	
	float preX = laserTracker.laserX;
	float preY = laserTracker.laserY;
	
	laserTracker.processFrame(panel.getValueF("hue"), panel.getValueF("hueWidth"), panel.getValueF("saturation"), panel.getValueF("value"), 2, 12, 0.1, true);
	
	float diff = laserTracker.laserX - preX;
	
	if( panel.getValueB("laserMove") && tags.size() ){
		
		//printf("tags rotation is %f \n", tags[currentTagID].rotation.x);
		
		//tags[currentTagID].rotationSpeed.y	*= panel.getValueF("laserSlowRate");
		
		if( laserTracker.newData() ){
			if( laserTracker.isStrokeNew() ){
				preX = laserTracker.laserX;
				preY = laserTracker.laserY;
				diff = 0.0;
				printf("stroke is new!\n");
			}
			//tags[currentTagID].rotationSpeed.y	+= diff * panel.getValueF("laserRotAmount");
		}
		
		//tags[currentTagID].rotation.y		+= tags[currentTagID].rotationSpeed.y;
		
	}
	
	//drawer.unFlatten(50);
}

//--------
void GrafLaserApp::handleLaserRecord(){
	
	laserTracker.clearNewStroke();
	
	if( !checkLaserActive() ){
		return;
	}
	
	bool newTag = ( panel.hasValueChanged("laserMode") && panel.getValueB("laserMode") );
	
	if( newTag ){
		particleDrawer.reset();
		printf("clearing tags\n");
		//tags.clear();
	}
	
	if( tags.size() ==  0 ){
		tags.push_back(grafTagMulti());
	}
	
	currentTagID = tags.size()-1;
	
	if( newTag ){
		tags.back().clear(true);
		simpleLine.clear();
		simpleLine.push_back( simpleStroke() );		
	}
	
	bool bNewStroke = false;
	float preX		= laserTracker.laserX;
	float preY		= laserTracker.laserY;	
	
	laserTracker.processFrame(panel.getValueF("hue"), panel.getValueF("hueWidth"), panel.getValueF("saturation"), panel.getValueF("value"), 2, panel.getValueI("nFramesNewStroke"), panel.getValueF("jumpDist"), true);
	
	if( laserTracker.newData() && laserTracker.isStrokeNew() && !newTag ){
		
		//tags.back().nextStroke();
		printf("stroke is new!\n");
		
		if( tags.back().getNPts() == 0 ){
			tags.back().clear(true);
			simpleLine.clear();
			simpleLine.push_back( simpleStroke() );		
		}
		
		preX = laserTracker.laserX;
		preY = laserTracker.laserY;
		bNewStroke = true;
	}
	
	if( laserTracker.newData() ){
		
		if( simpleLine.size() == 0 ){
			simpleLine.push_back( simpleStroke() );
		}
		
		if( bNewStroke ){		
			smoothX = preX = laserTracker.laserX;
			smoothY = preY = laserTracker.laserY;
			
			tags.back().nextStroke();
			tags.back().addNewPoint(ofPoint(smoothX, smoothY) );
			
			simpleLine.push_back( simpleStroke() );
			simpleLine.back().push_back(ofPoint(smoothX, smoothY) );
			
		}else{
			for(int k = 0; k < 4; k++){
				smoothX *= 0.6;
				smoothY *= 0.6;
				
				float ptX = ofMap(k+1, 0, 4, preX, laserTracker.laserX);
				float ptY = ofMap(k+1, 0, 4, preY, laserTracker.laserY);
				
				smoothX += ptX * 0.4;
				smoothY += ptY * 0.4;
				
				tags.back().addNewPoint(ofPoint(smoothX, smoothY));
				simpleLine.back().push_back( ofPoint(smoothX, smoothY) );
			}
		}
		
		lastTimePointAddedF = ofGetElapsedTimef();
	}else{
		
		if( tags.back().getNPts() > 3 && panel.getValueB("bAutoEndTag") && ofGetElapsedTimef() - lastTimePointAddedF > panel.getValueF("autoEndTime")){
			panel.setValueB("laserMode", false);
			laserTracker.clearZone.bState = false;
			resetPlayer(0);
			tags[currentTagID].center.x  = 0.5;
			tags[currentTagID].center.y  = 0.5;
			tags[currentTagID].drawScale = screenH;
			
			smoother.smoothTag(3,&tags[currentTagID]);
			
			drawer.transitionFlatten( tags[currentTagID].center.z, 1);
			particleDrawer.flatten( tags[currentTagID].center.z, 1);
		}
		
	}
	
}

//------------------------------------------
void GrafLaserApp::checkLaserHitState(){
	if( laserTracker.clearZone.bState != panel.getValueB("laserMode") ){
		panel.setValueB("laserMode", laserTracker.clearZone.bState );
	}
}


//--------------------------------------------------------------
void GrafLaserApp::resetPlayer(int next)
{
	if(tags.size() <= 0 ) return;
	
	myTagPlayer.reset();
	
	nextTag(next);
	
	drawer.setup( &tags[currentTagID], tags[currentTagID].distMax );
	
	particleDrawer.reset();
	
	waitTimer = waitTime;
	
	rotationY = 0;
	//tags[currentTagID].rotationSpeed = 0.0;
	tags[currentTagID].rotation = 0.0;
	
	tagPosVel.set(0,0,0);
	
	prevStroke = 0;
	
	
}
//--------------------------------------------------------------
void GrafLaserApp::nextTag(int dir)
{
	
	if(dir==1)
	{
		currentTagID++;
		currentTagID %= tags.size();
	}
	else if(dir==-1){
		currentTagID--;
		if(currentTagID < 0 ) currentTagID = tags.size()-1;
	}
	
	drawer.resetTransitions();
	rotFixTime = 0;
	archPhysics.reset();
	
	//cout << "total pts this tag " << tags[currentTagID].getNPts() << endl;
	
}
