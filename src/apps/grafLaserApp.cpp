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
	bSetup		= false;
	bEnabled	= false;
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
	
	//-- laser specific
	smoothX			= 0.0;
	smoothY			= 0.0;
	
	//--- user settings
	ofxXmlSettings xmlUser;
	xmlUser.loadFile("appSettings.xml");
	string username = xmlUser.getValue("project_folder","default");
	
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
	//pWarper.initWarp( screenW,screenH,screenW*WARP_DIV,screenH*WARP_DIV );
	//pWarper.recalculateWarp();
	//pWarper.loadFromXml(pathToSettings+"warper.xml");
	
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
	//fboLeft.allocate(screenW,screenH );
	//fboRight.allocate(screenW,screenH );
	
	grafTagMulti temptTag;
	tags.push_back( temptTag );
	
	lastTime = ofGetElapsedTimef();
	
	//--- saving tags
	gIO.setup("0","Graffiti Analysis","3.0");
	
	//---
	bSetup = true;
	bEnabled = true;

	panel.setSelectedPanel("Laser Tracker");
	
	

}

/*void GrafLaserApp::update()
{
	if( mode == PLAY_MODE_RECORD )
		handleLaserRecord();
	if( mode == PLAY_MODE_PLAY )
		handleLaserPlayback();
		
	updateControlPanel();
}*/

//--------------------------------------------------------------
void GrafLaserApp::update(){
	
	dt  = ofGetElapsedTimef()-lastTime;
	lastTime  = ofGetElapsedTimef();
	
	int lastMode = mode;
	
	if( panel.getValueB("useLaser") ){
		if( panel.getValueB("bUseClearZone") ){
			laserTracker.setUseClearZone(true);			
			checkLaserHitState();		
		}else{
			laserTracker.setUseClearZone(false);
		}
		
		if( panel.getValueB("laserMode") ){
			mode = PLAY_MODE_RECORD;
		}else{
			mode = PLAY_MODE_PLAY;
		}
	}else{
		mode = PLAY_MODE_PLAY;
	}
	
	if( mode == PLAY_MODE_PLAY && mode != lastMode ) saveTag();
	
	bool bTrans = false;
	if( currentTagID >= tags.size() ){
		currentTagID = tags.size()-1;
		if( currentTagID < 0 ){
			currentTagID = 0;
			//loadTags();
		}
	}
	
	
	if( mode == PLAY_MODE_PLAY && tags.size() > 0 )
	{
		
		//---- set drawing data for render
		if( drawer.bSetupDrawer )
			drawer.setup( &tags[currentTagID], tags[currentTagID].distMax );
		
		//---- update tag playing state
		if( !myTagPlayer.bDonePlaying )					
		{
			myTagPlayer.update(&tags[currentTagID]);	// normal play, update tag
			
		}else if( !myTagPlayer.bPaused && myTagPlayer.bDonePlaying && waitTimer > 0)			   
		{
			waitTimer -= dt;	// pause time after drawn, before fades out
		}
		else if ( !myTagPlayer.bPaused && myTagPlayer.bDonePlaying && (drawer.alpha > 0 || particleDrawer.alpha > 0))			  		
		{
			updateTransition(0);
		}
		else if (  !myTagPlayer.bPaused && myTagPlayer.bDonePlaying )							
		{
			if( panel.getValueB("useLaser") ){
				resetPlayer(0);	// setup for next tag
			}else{
				resetPlayer(1);	// setup for next tag
			}
		}
		
		
		//---------- AUDIO applied
		if( bUseAudio) updateAudio();
		
		
		//--------- ARCHITECTURE
		//if( bUseArchitecture ) updateArchitecture();
		
		
		//--------- PARTICLES
		updateParticles();
		
		
		//THEO
		
		if( panel.getValueB("useLaser") ){		
			handleLaserPlayback();
		}else{
			//--------- TAG ROTATION + POSITION
			if(bRotating && !myTagPlayer.bPaused ) rotationY += panel.getValueF("ROT_SPEED")*dt;
			
			// update pos / vel
			tags[currentTagID].position.x += tagPosVel.x;
			tags[currentTagID].position.y += tagPosVel.y;
			
			tagPosVel.x -= .1*tagPosVel.x;
			tagPosVel.y -= .1*tagPosVel.y;
		}
	}
	else if( mode == PLAY_MODE_RECORD ){
		handleLaserRecord();
	}
	
	// controls
	if( bShowPanel ) updateControlPanel();
	
	panel.clearAllChanged();
}

void GrafLaserApp::updateParticles(){
	
	int lastStroke = myTagPlayer.getCurrentStroke();
	int lastPoint  = myTagPlayer.getCurrentId();
	
	if( prevStroke != lastStroke )	myTagPlayer.bReset = true; 
	if( lastPoint <= 0 )			myTagPlayer.bReset = true;
	if( tags[currentTagID].myStrokes[ lastStroke].pts.size()-1 == lastPoint ) myTagPlayer.bReset = true;
	
	particleDrawer.update( myTagPlayer.getCurrentPoint(),myTagPlayer.getVelocityForTime(&tags[currentTagID]),  dt,  myTagPlayer.bReset, tags[currentTagID].center.z);
	
	myTagPlayer.bReset = false; // important so no particle error on first stroke
	prevStroke		= myTagPlayer.getCurrentStroke();	
}

void GrafLaserApp::draw()
{
	
	
	screenW = ofGetWidth();
	screenH = ofGetHeight();
	
	if(fbo.texData.width != ofGetWidth())
	{
		fbo.allocate(ofGetWidth(),ofGetHeight());
	}

	
	drawTagLaser();
	
	if( panel.getSelectedPanelName() == "Laser Tracker" && bShowPanel ){
		laserTracker.drawPanels(0, 0);
	}
	
	drawControls();
	//panel.draw();
}

//--------------------------------------------------------------
void GrafLaserApp::drawControls()
{
	if(bShowPanel)
	{
		
		if( panel.getSelectedPanelName() == "FBO Warper" )
		{
			if( panel.getValueB("toggle_fbo_preview") )
				pWarper.drawEditInterface(10, 10,.25);
			else 
				pWarper.drawEditInterface(0, 0, 1);
			
			ofSetColor(255,255,255,100);
			pWarper.drawUV(0, 0, 1);
		}
		
		panel.draw();
		ofSetColor(255,255,255,200);
		
		if( bUseAudio && panel.getSelectedPanelName() == "Audio Settings" )
			audio.draw();

	}
	
	
}

void GrafLaserApp::updateTransition( int type)
{
	//----------  TRANSITIONS
	// fade away, dissolve, deform etc.
	
	drawer.prelimTransTime += dt;
	
	if(bUseAudio)
	{
		
		float deform_frc = panel.getValueF("wave_deform_force");
		//float line_amp_frc = panel.getValueF("line_width_force");
		float bounce_frc = panel.getValueF("bounce_force");
		
		if( panel.getValueB("use_wave_deform") ) drawer.transitionDeform( dt,deform_frc, audio.audioInput, NUM_BANDS);
		//if( panel.getValueB("use_line_width") ) drawer.transitionLineWidth( dt,audio.averageVal*line_amp_frc);
		if( panel.getValueB("use_bounce") ) drawer.transitionBounce( dt,audio.averageVal*bounce_frc);
		if( drawer.pctTransLine < .1 ) drawer.pctTransLine += .001;
		
	}
	
	
	
	if(bUseArchitecture)
	{
		
		if( drawer.prelimTransTime > panel.getValueI("wait_time") )
		{
			
			bRotating = false;
			
			drawer.transitionFlatten( tags[currentTagID].center.z, 50);
			particleDrawer.flatten( tags[currentTagID].center.z, 52);
			
			if(rotFixTime == 0) rotFixTime = ofGetElapsedTimef();
			float pct = 1 - ((ofGetElapsedTimef()-rotFixTime) / 45.f);
			rotationY = pct*rotationY + (1-pct)*(0);
			
			if( pct < .9 && !archPhysics.bMakingParticles) 
				archPhysics.turnOnParticleBoxes(&particleDrawer.PS);
			
			if(particleDrawer.xalpha  > 0 ) particleDrawer.xalpha -= .5*dt;
			
			// if all particles have fallen and 
			if(archPhysics.bMadeAll)
			{
				// do average transition
				drawer.transition(dt,.15);
				drawer.alpha -= .35*dt;
				if(particleDrawer.alpha  > 0 ) particleDrawer.alpha -= .5*dt;
			}
			
			//if( bUseGravity ) particleDrawer.fall(dt);
			
		}
		
	}else{
		
		// do average transition
		//drawer.transition(dt,.99);
		
		drawer.alpha -= .35*dt;
		//if(!bUseAudio) 
		drawer.transition(dt,.15);
		if( bUseGravity ) particleDrawer.fall(dt);
		if(particleDrawer.alpha  > 0 ) particleDrawer.alpha -= .5*dt;
		
	}
	
	//---------- 
}

void GrafLaserApp::updateAudio()
{
	if(panel.getValueB("use_p_size") ) 
		particleDrawer.updateParticleSizes(audio.ifftOutput,audio.averageVal, NUM_BANDS,panel.getValueF("particle_size_force") );
	
	particleDrawer.setDamping( panel.getValueF("P_DAMP") );
	
	if( /*drawer.prelimTransTime < panel.getValueI("wait_time")  &&*/ panel.getValueB("use_p_amp") ) 
		particleDrawer.updateParticleAmpli(audio.ifftOutput,audio.averageVal, NUM_BANDS,panel.getValueF("outward_amp_force") );
	
	// create drops
	if( panel.getValueB("use_drop") )
	{
		
		for( int i = 0; i < audio.peakFades.size(); i++)
		{
			if( audio.peakFades[i] == 1 )
			{
				int randomP = particleDrawer.PS.getIndexOfRandomAliveParticle();//ofRandom( 0, particleDrawer.PS.numParticles );
				ofPoint pPos = ofPoint(particleDrawer.PS.pos[randomP][0],particleDrawer.PS.pos[randomP][1],particleDrawer.PS.pos[randomP][2]);
				ofPoint pVel = ofPoint(particleDrawer.PS.vel[randomP][0],particleDrawer.PS.vel[randomP][1],particleDrawer.PS.vel[randomP][2]);
				drops.createRandomDrop( pPos, pVel, particleDrawer.PS.sizes[randomP] );
			}
		}
		
		// update particle drops (audio stuff);
		drops.update(dt);
	}
	
	// update audio
	audio.update();
}

//--------------------------------------------------------------
void GrafLaserApp::drawTagLaser(){
	
	if( mode == PLAY_MODE_LOAD )
	{
		//nothing while loading
		;
	}
	else if( mode == PLAY_MODE_RECORD ){
		
		ofPushStyle();
		ofSetColor(255, 255, 255, 255);
		ofNoFill();
		
		for(int k = 0; k < simpleLine.size(); k++){
			if( simpleLine[k].size() < 2 )continue;
			
			
			ofSetLineWidth(2);
			glBegin(GL_TRIANGLE_STRIP);
			for(int i = 0; i < simpleLine[k].size(); i++){
				
				float pctTaper = 1.0;
				
				int numTaper = 6;
				
				if( i < numTaper ){
					pctTaper = ofMap(i, 0, numTaper, 0.0, 1.0, true);
				}else if( i > simpleLine[k].size()-numTaper ){
					pctTaper = ofMap(i, simpleLine[k].size()-numTaper, simpleLine[k].size()-1, 1.0, 0.0);
				}
				
				if( i > 0 && i < simpleLine[k].size() -1 ){
					
					ofxVec2f diff = simpleLine[k].at(i)-simpleLine[k].at(i-1);
					diff.x *= screenW;
					diff.y *= screenH;
					diff.normalize();
					
					diff *= 9.0 * pctTaper;
					
					glVertex2f( diff.y  + simpleLine[k].at(i).x * (float)screenW, -diff.x + simpleLine[k].at(i).y * (float)screenH);
					glVertex2f( -diff.y + simpleLine[k].at(i).x * (float)screenW,  diff.x + simpleLine[k].at(i).y * (float)screenH);
					
				}else{
					glVertex2f( simpleLine[k].at(i).x * (float)screenW, simpleLine[k].at(i).y * (float)screenH);
				}
			}
			glEnd();
		}
		
		ofPopStyle();		
		
		ofSetColor(255, 255, 255, 255);
		
	}
	else if( mode == PLAY_MODE_PLAY && tags.size() )
	{
		
	glPushMatrix();
		
		glTranslatef(screenW/2, screenH/2, 0);
		//glScalef(tags[currentTagID].position.z,tags[currentTagID].position.z,tags[currentTagID].position.z);
		
		glPushMatrix();
		
			glRotatef(tags[currentTagID].rotation.x,1,0,0);
			glRotatef(tags[currentTagID].rotation.y+rotationY,0,1,0);
			glRotatef(tags[currentTagID].rotation.z,0,0,1);
		
			//glTranslatef(-tags[currentTagID].min.x*tags[currentTagID].drawScale,-tags[currentTagID].min.y*tags[currentTagID].drawScale,-tags[currentTagID].min.z);
			glTranslatef(-tags[currentTagID].center.x*tags[currentTagID].drawScale,-tags[currentTagID].center.y*tags[currentTagID].drawScale,-tags[currentTagID].center.z);
		
			glDisable(GL_DEPTH_TEST);
		
			// draw particles
			particleDrawer.draw(myTagPlayer.getCurrentPoint().z,  screenW,  screenH);
		
			// draw audio particles
			//if( bUseAudio && panel.getValueB("use_drop") ) drops.draw();
		
			glEnable(GL_DEPTH_TEST);
		
			// draw tag
			glPushMatrix();
				glScalef( tags[currentTagID].drawScale, tags[currentTagID].drawScale, 1);
				drawer.draw( myTagPlayer.getCurrentStroke(), myTagPlayer.getCurrentId() );
			glPopMatrix();
		
		glPopMatrix();
		
	glPopMatrix();
		
		
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_FOG);		
		
	}
	
	
	if( panel.getValueB("useLaser") && panel.getValueB("bUseClearZone") ){
		laserTracker.drawClearZone(0, 0, screenW, screenH);
	}
	
	if(bUseArchitecture && mode == PLAY_MODE_PLAY )
	{
		
		glViewport(0,0,fbo.texData.width,fbo.texData.height);
		// set translation in polygon tool so drwaing happens in correct place
		//archPhysics.offSetPre.x = (tags[currentTagID].position.x);
		//archPhysics.offSetPre.y = (tags[currentTagID].position.y);
		//archPhysics.offSet.x = (-tags[currentTagID].min.x*tags[currentTagID].drawScale) + (-tags[currentTagID].center.x*tags[currentTagID].drawScale);
		//archPhysics.offSet.y = (-tags[currentTagID].min.y*tags[currentTagID].drawScale) + (-tags[currentTagID].center.y*tags[currentTagID].drawScale);
		//archPhysics.scale = tags[currentTagID].position.z;
		
		archPhysics.draw();
		
	}
	
	
	
	
	
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
	//panel.addPanel("Architecture Settings", 1, false);
	//panel.addPanel("FBO Warper", 1, false);
	panel.addPanel("Laser Tracker", 1, false);
	panel.addPanel("Extra Laser Settings", 1, false);
	
	panel.setWhichPanel("Laser Tracker");
	panel.addToggle("Enable laser input", "useLaser", false);
	panel.addToggle("Video / Camera", "bCamera", false);
	panel.addToggle("Open camera settings", "camSettings", false);
	//panel.addToggle("Show laser panel", "bShowPanel", false);
	panel.addToggle("Switch laser mode", "laserMode", true);
	panel.addSlider("Which camera", "whichCamera", 0, 0, 10, true);
	panel.addSlider("Hue", "hue", 0.0, 0, 1.0, false);
	panel.addSlider("Hue Width", "hueWidth", 0, 0, 1.0, false);
	panel.addSlider("Saturation", "saturation", 0, 0, 1.0, false);
	panel.addSlider("Value", "value", 0, 0, 1.0, false);
	panel.addCustomRect("Colors tracked", &laserTracker, 140, 40);
	
	panel.setWhichPanel("Extra Laser Settings");
	panel.addToggle("Use clear zone", "bUseClearZone", true);
	panel.addToggle("Auto end tag rec", "bAutoEndTag", true);
	panel.addSlider("Auto end time", "autoEndTime", 2.2, 1.0, 5.0, false);
	panel.addSlider("No. frames new stroke", "nFramesNewStroke", 12, 5, 30, true);
	panel.addSlider("New stroke jump dist", "jumpDist", 0.2, 0.1, 0.99, false);	
	panel.addToggle("Laser move", "laserMove", true);
	panel.addSlider("Laser rotation amount", "laserRotAmount", 1.0, 0.1, 10.0, false);
	panel.addSlider("Laser rotation slowdown", "laserSlowRate", 0.98, 0.88, 0.9999, false);
	
	//---- application sttings
	panel.setWhichPanel("App Settings");
	//panel.addToggle("Load tags", "LOAD_TAGS", false);
	//panel.addToggle("Auto load on start", "AUTO_LOAD", true);
	//panel.addSpace();
	
	panel.addToggle("Use Audio", "use_audio",true);
	//panel.addToggle("Use Architecture", "use_arch",true);
	//panel.addToggle("Use Red / Blue", "use_rb",true);
	//panel.addSpace();
	
	panel.addToggle("Play / Pause", "PLAY", true);
	panel.addToggle("FullScreen", "FULL_SCREEN", false);
	panel.addToggle("Rotate", "ROTATE", true);
	panel.addSlider("Rotation Speed","ROT_SPEED",.65,0,4,false);
	panel.addSlider("Change wait time","wait_time",30,0,120,true);
	//panel.addToggle("Display filename", "SHOW_NAME", true);
	//panel.addToggle("Display time", "SHOW_TIME", true);
	//panel.addToggle("Save Tag Position/Rotation", "save_Tag_pos", false);
	//panel.addSpace();
	
	//--- draw settings
	panel.setWhichPanel("Draw Settings");
	panel.addSlider("Line Alpha","LINE_ALPHA",.92,0,1,false);
	panel.addSlider("Outline Width","LINE_WIDTH",2,1,10,false);
	panel.addSlider("Line Scale","LINE_SCALE",.05,0,.1,false);
	panel.addSlider("Particle Size","P_SIZE",2,0,10,false);
	panel.addSlider("Particle Alpha","P_ALPHA",.75,0,1,false);
	panel.addSlider("Particle Damping","P_DAMP",.15,0,.25,false);
	panel.addSlider("Number Particles","P_NUM",1,0,4,true);
	panel.addToggle("Use gravity", "USE_GRAVITY", true);
	panel.addToggle("Use edge mask", "USE_MASK", false);
	panel.addSlider("Z Depth","Z_DEPTH",16,0,50,false);
	panel.addToggle("Apply to Z to all","Z_ALL",false);
	
	
	
	//--- audio settings
	panel.setWhichPanel("Audio Settings");
	panel.addSlider("Outward amp force","outward_amp_force",8,0,200,false);
	panel.addSlider("Particle size force","particle_size_force",22,0,200,false);
	panel.addSlider("Wave deform force","wave_deform_force",1,0,10,false);
	panel.addSlider("Bounce force","bounce_force",.25,0,2,false);
	panel.addSlider("Drop p threshold","drop_p_thresh",.1,0,2,false);
	// toggles to apply what to what...
	panel.addToggle("Use particle amp", "use_p_amp", true);
	panel.addToggle("Use particle size", "use_p_size", true);
	panel.addToggle("Use wave deform", "use_wave_deform", true);
	panel.addToggle("Use bounce", "use_bounce", true);
	panel.addToggle("Use drop particel", "use_drop", true);
	
	//--- arch settings
	/*
	panel.setWhichPanel("Architecture Settings");
	panel.addToggle("new structure", "new_structure",false);
	panel.addToggle("done","arch_done",false);
	panel.addToggle("save xml", "arch_save", false);
	panel.addToggle("load xml", "arch_load", false);
	panel.addToggle("clear", "arch_clear", false);
	panel.addSpace();
	
	panel.addToggle("show drawing tool", "show_drawing_tool",false);
	panel.addToggle("show image", "show_image",true);
	panel.addSlider("mass","box_mass",1,0,20,false);
	panel.addSlider("bounce","box_bounce",.53,0,2,false);
	panel.addSlider("friction","box_friction",.41,0,2,false);
	panel.addSlider("gravity","gravity",6,0,20,false);
	*/
	
	//--- fbo settings	
	/*panel.setWhichPanel("FBO Warper");
	panel.addToggle("Load Warping", "load_warping", true);
	panel.addToggle("Save Warping", "save_warping", false);
	panel.addToggle("Reset Warping", "reset_warping", false);
	panel.addToggle("Toggle Preview","toggle_fbo_preview",false);
	*/
	
	//--- load saved
	panel.loadSettings(pathToSettings+"laserSettings.xml");
	
	panel.setValueB("laserMode", false);
	
	panel.update();
	panel.setWhichPanel("Laser Tracker");
}

void GrafLaserApp::updateControlPanel(bool bUpdateAll)
{
	
	panel.update();
	panel.clearAllChanged();
	
	bUseAudio = panel.getValueB("use_audio");
	
	if( panel.getSelectedPanelName() == "App Settings" || bUpdateAll)
	{
		
		myTagPlayer.bPaused = !panel.getValueB("PLAY");
		if( panel.getValueB("FULL_SCREEN") )
		{
			panel.setValueB("FULL_SCREEN",false);
			ofToggleFullscreen();
		}
		bRotating = panel.getValueB("ROTATE");
		
		if( panel.getValueB("save_Tag_pos") )
		{
			panel.setValueB("save_Tag_pos",false);
			//saveAllTagPositions();
		}
		
	}
	
	if( panel.getSelectedPanelName() == "Draw Settings" || bUpdateAll)
	{
		
		drawer.setAlpha(panel.getValueF("LINE_ALPHA"));
		drawer.lineWidth = panel.getValueF("LINE_WIDTH");
		drawer.setLineScale( panel.getValueF("LINE_SCALE") );
		
		particleDrawer.setParticleSize( panel.getValueF("P_SIZE") );
		particleDrawer.particle_alpha = panel.getValueF("P_ALPHA") ;
		particleDrawer.numXtras = panel.getValueI("P_NUM");
		bUseGravity = panel.getValueB("USE_GRAVITY");
		
		bUseGlobalZ = panel.getValueB("Z_ALL");
		if(bUseGlobalZ)
		{
			z_const = panel.getValueF("Z_DEPTH");
			if(tags.size() > 0 && (lastUpdatedZ != currentTagID || tags[currentTagID].z_const != z_const) )
			{
				//updateZDepth(z_const);
				lastUpdatedZ = currentTagID;
			}
		}else{
			if(tags.size() > 0)
			{
				if(lastUpdatedZ != currentTagID)
				{
					panel.setValueF("Z_DEPTH",tags[currentTagID].z_const);
					z_const = panel.getValueF("Z_DEPTH");
					lastUpdatedZ = currentTagID;
					
				}else if(tags[currentTagID].z_const != panel.getValueF("Z_DEPTH")){
					//updateZDepth(z_const);
					z_const = panel.getValueF("Z_DEPTH");
				}
			}
		}		
	}
	
	if( bUseAudio && panel.getSelectedPanelName() == "Audio Settings" )
	{
		audio.peakThreshold = panel.getValueF("drop_p_thresh");
		audio.panel.show();
	}else{
		audio.panel.hide();
	}
	
	/*if( panel.getSelectedPanelName() == "FBO Warper")
	{
		pWarper.enableEditing();
		
		if( panel.getValueB("load_warping") )
		{
			panel.setValueB("load_warping",false);
			pWarper.loadFromXml(pathToSettings+"warper.xml");
		}
		
		if( panel.getValueB("save_warping") )
		{
			panel.setValueB("save_warping",false);
			pWarper.saveToXml(pathToSettings+"warper.xml");
		}
		
		if( panel.getValueB("reset_warping") )
		{
			panel.setValueB("reset_warping",false);
			pWarper.initWarp( screenW,screenH,screenW*WARP_DIV,screenH*WARP_DIV );
			pWarper.recalculateWarp();
		}
	}*/
	
	
	//--- disable things
	if( panel.getSelectedPanelName() != "FBO Warper" )
	{
		pWarper.disableEditing();
	}

	
}

//--------
bool GrafLaserApp::checkLaserActive(){
	if( panel.hasValueChanged("useLaser") || panel.hasValueChanged("bCamera") ){
		if( panel.getValueB("bCamera") ){
			laserTracker.setupCamera( panel.getValueI("whichCamera"), 320, 240);
		}else{
			laserTracker.setupVideo("video/laserTestVideo.mov");
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
		tags[currentTagID].center.x  = 0.5;
		tags[currentTagID].center.y  = 0.5;
		tags[currentTagID].drawScale = screenH;
		
		drawer.flattenInstantly( tags[currentTagID].center.z);
		particleDrawer.flattenInstantly( tags[currentTagID].center.z );		
	}
	
	float preX = laserTracker.laserX;
	float preY = laserTracker.laserY;
	
	laserTracker.processFrame(panel.getValueF("hue"), panel.getValueF("hueWidth"), panel.getValueF("saturation"), panel.getValueF("value"), 2, 12, 0.1, true);
	
	float diff = laserTracker.laserX - preX;
	
	if( !panel.hasValueChanged("laserMode") && panel.getValueB("bAutoEndTag") 
	   && laserTracker.newData() && ofGetElapsedTimef() - lastTimePointAddedF  > 2.0 ){
		if( laserTracker.isStrokeNew() ){
			preX = laserTracker.laserX;
			preY = laserTracker.laserY;
			diff = 0.0;
			printf("recording!\n");
			lastTimePointAddedF = ofGetElapsedTimef();
			panel.setValueB("laserMode", true);
			laserTracker.clearZone.bState = true;
			
			tags.back().clear(true);
			simpleLine.clear();
			simpleLine.push_back( simpleStroke() );		
			
		}
		
	}
	
	//	if( panel.getValueB("laserMove") && tags.size() ){
	//		
	//		//printf("tags rotation is %f \n", tags[currentTagID].rotation.x);
	//		
	////		tags[currentTagID].rotationSpeed.y	*= panel.getValueF("laserSlowRate");
	//		
	//		if( laserTracker.newData() ){
	//			if( laserTracker.isStrokeNew() ){
	//				preX = laserTracker.laserX;
	//				preY = laserTracker.laserY;
	//				diff = 0.0;
	//				printf("stroke is new!\n");
	//			}
	////			if( bRotating ){
	////				tags[currentTagID].rotationSpeed.y	+= diff * panel.getValueF("laserRotAmount");
	////			}else{
	////				tags[currentTagID].rotationSpeed.y *= 0.96;
	////			}
	//		}
	//
	//		
	////		if( bRotating == false ){
	////			tags[currentTagID].rotation.y *= 0.92;
	////		}else{
	////			tags[currentTagID].rotation.y		+= tags[currentTagID].rotationSpeed.y;
	////		}
	//
	//	}
	
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
	bRotating = true;
	panel.setValueB("ROTATE", true);
	
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
				//tags.back().z_const   = panel.getValueF("zScale");
				
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
			//tags[currentTagID].z_const   = panel.getValueF("zScale");
			
			drawer.flattenInstantly( tags[currentTagID].center.z);
			particleDrawer.flattenInstantly( tags[currentTagID].center.z);
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


void GrafLaserApp::saveTag()
{
	string filename = myTagDirectory+"laser_"+getTimeString()+".gml";
	//pathToSettings = "projects/"+username+"/settings/";//"settings/default/";
	//myTagDirectory = "projects/"+username+"/tags/";
	gIO.constructGML(&tags[currentTagID] );
	gIO.saveTag(filename);
}


string GrafLaserApp::getTimeString()
{
	return ofToString( ofGetDay() ) + "_" + ofToString( ofGetMonth() )  + "_" + ofToString( ofGetYear() )  + "_" + ofToString( ofGetHours() )  + "_" + ofToString( ofGetMinutes() )  + "_" + ofToString( ofGetSeconds() ); 
}