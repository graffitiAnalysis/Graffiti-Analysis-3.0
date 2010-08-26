#include "grafPlayerApp.h"

//--------------------------------------------------------------
GrafPlayerApp::GrafPlayerApp(){

	bSetup = false;
	
}

GrafPlayerApp::~GrafPlayerApp(){

	ofRemoveListener(ofEvents.mouseMoved, this, &GrafPlayerApp::mouseMoved);
	ofRemoveListener(ofEvents.mousePressed, this, &GrafPlayerApp::mousePressed);
	ofRemoveListener(ofEvents.mouseReleased, this, &GrafPlayerApp::mouseReleased);
	ofRemoveListener(ofEvents.mouseDragged, this, &GrafPlayerApp::mouseDragged);
	ofRemoveListener(ofEvents.keyPressed, this, &GrafPlayerApp::keyPressed);
	ofRemoveListener(ofEvents.keyReleased, this, &GrafPlayerApp::keyReleased);

}

//--------------------------------------------------------------
void GrafPlayerApp::setup(){

	
	ofxXmlSettings xmlUser;
	xmlUser.loadFile("projects/user.xml");
	string username = xmlUser.getValue("params:user","default");
	
	pathToSettings = "projects/"+username+"/settings/";//"settings/default/";
	myTagDirectory = "projects/"+username+"/tags/";
	
	ofAddListener(ofEvents.mouseMoved, this, &GrafPlayerApp::mouseMoved);
	ofAddListener(ofEvents.mousePressed, this, &GrafPlayerApp::mousePressed);
	ofAddListener(ofEvents.mouseReleased, this, &GrafPlayerApp::mouseReleased);
	ofAddListener(ofEvents.mouseDragged, this, &GrafPlayerApp::mouseDragged);
	ofAddListener(ofEvents.keyPressed, this, &GrafPlayerApp::keyPressed);
	ofAddListener(ofEvents.keyReleased, this, &GrafPlayerApp::keyReleased);

	screenW			= 1024;
	screenH			= 768;
	loadScreenSettings();
	

	mode			= PLAY_MODE_LOAD;
	lastX			= 0;
	lastY			= 0;
	bShowPanel		= true;
	bRotating		= false;
	bShowName		= false;
	bShowTime		= false;
	bUseFog			= true;
	bUseMask		= true;
	bTakeScreenShot = false;
	bUseGravity		= true;
	bUseAudio		= true;
	bUseArchitecture= true;
	modeRender		= GA_RMODE_NORMAL;
	modeDualScreen	= GA_SCREENS_NORMAL;
	
	
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
	
	fontSS.loadFont("fonts/frabk.ttf",9);
	fontS.loadFont("fonts/frabk.ttf",14);
	fontL.loadFont("fonts/frabk.ttf",22);
	imageMask.loadImage("images/mask.jpg");
	
	float FogCol[3]={0,0,0};
    glFogfv(GL_FOG_COLOR,FogCol);
    glFogi(GL_FOG_MODE, GL_LINEAR);
    glFogf(GL_FOG_DENSITY, 0.05f);
	fogStart = 370;
    fogEnd   = 970;
	
	// controls
	setupControlPanel();
	updateControlPanel();
	
	if(panel.getValueB("use_rb") ) modeRender = GA_RMODE_RB;

	particleDrawer.setup(screenW,screenH);
	
	// fbo
	#ifdef GA_STACK_SCREENS
		fbo.allocate(screenW*2,screenH*2 );
	#else
		fbo.allocate(screenW,screenH );
	#endif
	
	pWarper.initWarp( screenW,screenH,screenW*WARP_DIV,screenH*WARP_DIV );
	pWarper.recalculateWarp();
	pWarper.loadFromXml(pathToSettings+"warper.xml");
	
	// audio
	audio.setup();
	
	#ifdef GA_STACK_SCREENS
		archPhysics.setup(screenW,screenH);
		archPhysics.floorH = screenH*2;
	#else
		archPhysics.setup(screenW,screenH);
	#endif
	
	if(bUseArchitecture)
	{
		archPhysics.archImage.loadImage(pathToSettings+"arch.jpg");
		archPhysics.loadFromXML(pathToSettings+"architecture.xml");
		createWarpedArchitecture();
	}
	
	// red blue stuff
	fboLeft.allocate(screenW,screenH );
	fboRight.allocate(screenW,screenH );
	
	loadStatus = "No tags loaded.";
	
	bSetup = true;
	
}

static float lastTime = 0.f;

//--------------------------------------------------------------
void GrafPlayerApp::update(){

	dt  = ofGetElapsedTimef()-lastTime;
	lastTime  = ofGetElapsedTimef();
	
	
	bool bTrans = false;
	
	if( mode == PLAY_MODE_LOAD )
	{
		loadTags();
	}
	else if( mode == PLAY_MODE_PLAY && tags.size() > 0 )
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
			bTrans = true;
		}
		else if (  !myTagPlayer.bPaused && myTagPlayer.bDonePlaying )							
		{
			resetPlayer(1);	// setup for next tag
		}
	
		
		//---------- AUDIO applied
		if( bUseAudio) updateAudio();
		
		
		//--------- ARCHITECTURE
		if( bUseArchitecture ) updateArchitecture();
		
		
		//--------- PARTICLES
		updateParticles();
			
		
		//--------- TAG ROTATION + POSITION
		if(bRotating && !myTagPlayer.bPaused ) rotationY += panel.getValueF("ROT_SPEED")*dt;
		
		// update pos / vel
		tags[currentTagID].position.x += tagPosVel.x;
		tags[currentTagID].position.y += tagPosVel.y;
		
		tagPosVel.x -= .1*tagPosVel.x;
		tagPosVel.y -= .1*tagPosVel.y;
		
		if(!bTrans)
		{
		tags[currentTagID].rotation_o = tags[currentTagID].rotation;
		tags[currentTagID].position_o = tags[currentTagID].position;
		}

	}
	
	
	
	// controls
	if( bShowPanel ) updateControlPanel();
	
	
}

void GrafPlayerApp::updateParticles()
{
	
	int lastStroke = myTagPlayer.getCurrentStroke();
	int lastPoint  = myTagPlayer.getCurrentId();
	
	if( prevStroke != lastStroke )	myTagPlayer.bReset = true; 
	if( lastPoint <= 0 )			myTagPlayer.bReset = true;
	if( tags[currentTagID].myStrokes[ lastStroke].pts.size()-1 == lastPoint ) myTagPlayer.bReset = true;
	
	particleDrawer.update( myTagPlayer.getCurrentPoint(),myTagPlayer.getVelocityForTime(&tags[currentTagID]),  dt,  myTagPlayer.bReset);
	
	myTagPlayer.bReset = false; // important so no particle error on first stroke
	prevStroke		= myTagPlayer.getCurrentStroke();	
}

void GrafPlayerApp::updateTransition( int type)
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
			
			tags[currentTagID].rotation.x = pct*tags[currentTagID].rotation.x + (1-pct)*(0);
			tags[currentTagID].rotation.y = pct*tags[currentTagID].rotation.y  + (1-pct)*(0);
			tags[currentTagID].rotation.z = pct*tags[currentTagID].rotation.z + (1-pct)*(0);
			
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
			
			
		 }
	
	}else{
	
		// do average transition
		//drawer.transition(dt,.99);
		
		if( drawer.prelimTransTime > panel.getValueI("wait_time") )
		{
			drawer.alpha -= .35*dt;
			drawer.transition(dt,.15);
			if( bUseGravity ) particleDrawer.fall(dt);
			if(particleDrawer.alpha  > 0 ) particleDrawer.alpha -= .5*dt;
		}
	}
	
	//---------- 
}

void GrafPlayerApp::updateAudio()
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

void GrafPlayerApp::updateArchitecture()
{
	//if( drawer.prelimTransTime < panel.getValueI("wait_time") && !archPhysics.bMakingParticles)
	//	archPhysics.turnOnParticleBoxes(&particleDrawer.PS);
	
	archPhysics.update(dt);
	if(archPhysics.bMakingParticles)
	{
		archPhysics.createParticleSet(&particleDrawer.PS);
	}
	
	
}

//--------------------------------------------------------------
void GrafPlayerApp::drawTagNormal(){
	
	
	
	
	
	fbo.clear();
	fbo.begin();
	
	if( mode == PLAY_MODE_PLAY && tags.size() > 0)
	{
	
	glViewport(tags[currentTagID].position.x,-tags[currentTagID].position.y,fbo.texData.width,fbo.texData.height);
		
	ofEnableAlphaBlending();
	ofSetColor(255,255,255,255);

	glPushMatrix();
	
	glTranslatef(screenW/2, screenH/2, 0);
	glScalef(tags[currentTagID].position.z,tags[currentTagID].position.z,tags[currentTagID].position.z);
	
	glPushMatrix();
	
		glRotatef(tags[currentTagID].rotation.x,1,0,0);
		glRotatef(tags[currentTagID].rotation.y+rotationY,0,1,0);
		glRotatef(tags[currentTagID].rotation.z,0,0,1);
	
		glTranslatef(-tags[currentTagID].min.x*tags[currentTagID].drawScale,-tags[currentTagID].min.y*tags[currentTagID].drawScale,-tags[currentTagID].min.z);
		glTranslatef(-tags[currentTagID].center.x*tags[currentTagID].drawScale,-tags[currentTagID].center.y*tags[currentTagID].drawScale,-tags[currentTagID].center.z);
	
		glDisable(GL_DEPTH_TEST);
	
		// draw particles
		particleDrawer.draw(myTagPlayer.getCurrentPoint().z,  screenW,  screenH);
	
		// draw audio particles
		if( bUseAudio && panel.getValueB("use_drop") ) drops.draw();
	
		glEnable(GL_DEPTH_TEST);
	
		// draw tag
		glPushMatrix();
			glScalef( tags[currentTagID].drawScale, tags[currentTagID].drawScale, 1);
			drawer.draw( myTagPlayer.getCurrentStroke(), myTagPlayer.getCurrentId() );
		glPopMatrix();
	
		// draw bounding box
		glPushMatrix();
			ofSetColor(255, 255, 255);
			glScalef( tags[currentTagID].drawScale, tags[currentTagID].drawScale, 1);
			//tags[currentTagID].drawBoundingBox( tags[currentTagID].min, tags[currentTagID].max, tags[currentTagID].center );
		glPopMatrix();
	
	glPopMatrix();
	
	
	
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_FOG);
	
	glPopMatrix();
	
	if(bUseArchitecture)
	{
		
		glViewport(0,0,fbo.texData.width,fbo.texData.height);
		
		// set translation in polygon tool so drwaing happens in correct place
		archPhysics.offSetPre.x = (tags[currentTagID].position.x);
		archPhysics.offSetPre.y = (tags[currentTagID].position.y);
		archPhysics.offSet.x = (-tags[currentTagID].min.x*tags[currentTagID].drawScale) + (-tags[currentTagID].center.x*tags[currentTagID].drawScale);
		archPhysics.offSet.y = (-tags[currentTagID].min.y*tags[currentTagID].drawScale) + (-tags[currentTagID].center.y*tags[currentTagID].drawScale);
		archPhysics.scale = tags[currentTagID].position.z;
		
		archPhysics.draw();
		
	}
	
	
	// image mask for edges
	if(bUseMask)
	{
		ofEnableAlphaBlending();
		glBlendFunc(GL_DST_COLOR, GL_ZERO);
		imageMask.draw(0,0,screenW,screenH);
	}
	
	
	// data
	ofSetColor(255,255,255,255);
	if( bShowName && tags.size() > 0 ) fontL.drawString( tags[ currentTagID ].tagname, 10,ofGetHeight()-30 );
	if( bShowTime && tags.size() > 0 )
	{
		float time = myTagPlayer.getCurrentTime();
		float wd = fontL.stringWidth( ofToString( time,0) ) / 10.f;
		wd = 10*(int)(wd);
			
		fontL.drawString(ofToString(time,2), ofGetWidth()-wd-70, ofGetHeight()-30);
	}
		
	
	
	}
	fbo.end();
	//---- end fbo render

}

//--------------------------------------------------------------
void GrafPlayerApp::draw(){

	
	screenW = ofGetWidth();
	screenH = ofGetHeight();
	
	// architecture test image
	if( mode == PLAY_MODE_LOAD )
	{
		ofDrawBitmapString(loadStatus, 200, 200);
		if( tags.size() > 0 )
		{
			//ofDrawBitmapString(ofToString( (int)(tags.size()-1) ) + " of " + ofToString((totalToLoad-1)), 280, 200);
			ofDrawBitmapString(tags[tags.size()-1].tagname, 200, 220);
		}
	}else if( mode == PLAY_MODE_PLAY )
	{
		if(fbo.texData.width != ofGetWidth())
		{
			fbo.allocate(ofGetWidth(),ofGetHeight());
		}
	
			ofSetColor(150,150,150,255);
		if( bUseArchitecture && panel.getValueB("show_image") )
			archPhysics.drawTestImage();
	
		//---------  fbo renders
		if( modeRender == GA_RMODE_NORMAL)
		{
			drawTagNormal();
			
			ofEnableAlphaBlending();
			ofSetColor(255,255,255,255);
			
			fbo.drawWarped(0, 0,screenW, screenH,pWarper.u,pWarper.v,pWarper.num_x_sections,pWarper.num_y_sections);
			fbo.drawWarped(screenW, 0,screenW, screenH,pWarper.u,pWarper.v,pWarper.num_x_sections,pWarper.num_y_sections);
			
			/*if( modeDualScreen == GA_SCREENS_STACKED)
			{
				fbo.draw(1024, 0,1024*2,768*2);
				fbo.draw(1024*2, -768,1024*2,768*2);
			}else{
				fbo.drawWarped(0, 0,screenW, screenH,pWarper.u,pWarper.v,pWarper.num_x_sections,pWarper.num_y_sections);
				fbo.drawWarped(screenW, 0,screenW, screenH,pWarper.u,pWarper.v,pWarper.num_x_sections,pWarper.num_y_sections);
			}*/
		
		}else if( modeRender == GA_RMODE_RB )
		{
			float eyeDist = panel.getValueF("eye_dist");
			
			glColorMask(GL_TRUE,GL_FALSE,GL_FALSE,GL_FALSE); 
			drawStereoEye(-eyeDist);
			
			glClear( GL_DEPTH_BUFFER_BIT);
			glColorMask(GL_FALSE,GL_TRUE,GL_TRUE,GL_TRUE);
			drawStereoEye(eyeDist);
			
			glColorMask(1,1,1,1);
			glClear( GL_DEPTH_BUFFER_BIT);
			
			glViewport( 0, 0, ofGetWidth(), ofGetHeight() );
			ofSetupScreen();
		}
	}
	
	//---- controls renders
	drawControls();
	
}

//--------------------------------------------------------------
void GrafPlayerApp::drawStereoEye(float eyeDist)
{
	
	ofEnableAlphaBlending();
	ofSetColor(255,255,255,255);
	
	if( mode == PLAY_MODE_PLAY )
	{
		
		glViewport(tags[currentTagID].position.x,-tags[currentTagID].position.y,fbo.texData.width,fbo.texData.height);
		
		glPushMatrix();
		
			glTranslatef(screenW/2, screenH/2, 0);
			
			glTranslatef(eyeDist,0, 0);
			
			glScalef(tags[currentTagID].position.z,tags[currentTagID].position.z,tags[currentTagID].position.z);
		
			glPushMatrix();
		
				glRotatef(tags[currentTagID].rotation.x,1,0,0);
				glRotatef(tags[currentTagID].rotation.y+rotationY,0,1,0);
				glRotatef(tags[currentTagID].rotation.z,0,0,1);
		
				glTranslatef(-tags[currentTagID].min.x*tags[currentTagID].drawScale,-tags[currentTagID].min.y*tags[currentTagID].drawScale,-tags[currentTagID].min.z);
				glTranslatef(-tags[currentTagID].center.x*tags[currentTagID].drawScale,-tags[currentTagID].center.y*tags[currentTagID].drawScale,-tags[currentTagID].center.z);
		
				glDisable(GL_DEPTH_TEST);
		
				// draw particles
				particleDrawer.draw(myTagPlayer.getCurrentPoint().z,  screenW,  screenH);
		
				// draw audio particles
				if( bUseAudio && panel.getValueB("use_drop") ) drops.draw();
		
				glEnable(GL_DEPTH_TEST);
		
				// draw tag
				glPushMatrix();
					glScalef( tags[currentTagID].drawScale, tags[currentTagID].drawScale, 1);
					drawer.draw( myTagPlayer.getCurrentStroke(), myTagPlayer.getCurrentId() );
				glPopMatrix();
		
				// draw bounding box
				glPushMatrix();
					ofSetColor(255, 255, 255);
					glScalef( tags[currentTagID].drawScale, tags[currentTagID].drawScale, 1);
					//tags[currentTagID].drawBoundingBox( tags[currentTagID].min, tags[currentTagID].max, tags[currentTagID].center );
				glPopMatrix();
		
			glPopMatrix();
		
			glDisable(GL_DEPTH_TEST);
		
		glPopMatrix();
		
		if(bUseArchitecture)
		{
			glViewport(0,0,fbo.texData.width,fbo.texData.height);
				
			// set translation in polygon tool so drwaing happens in correct place
			archPhysics.offSetPre.x = (tags[currentTagID].position.x);
			archPhysics.offSetPre.y = (tags[currentTagID].position.y);
			archPhysics.offSet.x = (-tags[currentTagID].min.x*tags[currentTagID].drawScale) + (-tags[currentTagID].center.x*tags[currentTagID].drawScale);
			archPhysics.offSet.y = (-tags[currentTagID].min.y*tags[currentTagID].drawScale) + (-tags[currentTagID].center.y*tags[currentTagID].drawScale);
			archPhysics.scale = tags[currentTagID].position.z;
				
			archPhysics.draw();
				
		}
	
	}
		
}

//--------------------------------------------------------------
void GrafPlayerApp::drawControls()
{
	// -- arch drawing tool
	if(bUseArchitecture)
	{
		if( archPhysics.bDrawingActive || panel.getValueB("show_drawing_tool") )
			archPhysics.drawTool();
	}
	
	
	//--- control panel
	if(bShowPanel){
		
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
		if( panel.getSelectedPanelName() != "Architecture Drawing" )
		{
			fontSS.drawString("x: toggle control panel  |  p: pause/play  |  s: screen capture  |  m: toggle mouse  |  f: toggle fullscreen  |  R: reset pos/rot  |  arrows: next/prev  |  esc: quit", 90, ofGetHeight()-50);
			fontSS.drawString("left mouse: alter position  |  left+shift mouse: zoom  |  right mouse: rotate y  |  right+shift mouse: rotate x", 220, ofGetHeight()-30);
		}else{
			fontSS.drawString("left-mouse: add point  |  right-mouse: move / select all  |  left+shift mouse: move single point  |  Return: new shape  | Delete: remove last point", 90, ofGetHeight()-50);
			
		}
		
		
		if( bUseAudio && panel.getSelectedPanelName() == "Audio Settings" )
			audio.draw();
		
	}
}

//--------------------------------------------------------------
void GrafPlayerApp::keyPressed (ofKeyEventArgs & event){


	if( panel.isAnyTextBoxActive() )
	{
		panel.addChar(event.key);
		return;
	}
	
    switch(event.key){

  		case 'x': bShowPanel=!bShowPanel; break;
		case 'p': 
			myTagPlayer.bPaused = !myTagPlayer.bPaused;
			//bRotating = !myTagPlayer.bPaused;
			panel.setValueB("ROTATE",bRotating);
			panel.setValueB("PLAY",!myTagPlayer.bPaused);
			break;
			
			
		case OF_KEY_RIGHT:  resetPlayer(1); break;
        case OF_KEY_LEFT:   resetPlayer(-1); break;
		
		case 's': bTakeScreenShot = true; break;
			
		case 'R':
			tags[currentTagID].rotation.set(0,0,0);
			tags[currentTagID].position.set(0,0,1);
			rotationY = 0;
			tagPosVel.set(0,0,0);
			break;
		case OF_KEY_RETURN:
			if( panel.getSelectedPanelName() == "Architecture Drawing" )
				archPhysics.pGroup.addPoly();
			break;
		default:
  			break;

  }

	
	

}

//--------------------------------------------------------------
void GrafPlayerApp::keyReleased(ofKeyEventArgs & event){

}

//--------------------------------------------------------------
void GrafPlayerApp::mouseMoved(ofMouseEventArgs & event ){

	lastX   = event.x;
	lastY   = event.y;
}

//--------------------------------------------------------------
void GrafPlayerApp::mouseDragged(ofMouseEventArgs & event ){

	if( bShowPanel && !panel.minimize) 
	{
		panel.mouseDragged(event.x,event.y,event.button);
	}
	
	if(bUseAudio)
	{
		audio.panel.mouseDragged(event.x,event.y,event.button);
	}
	
	bool bMoveTag = true;
	
	if( panel.isMouseInPanel(event.x, event.y) )						bMoveTag = false;
	else if( panel.getSelectedPanelName() == "Architecture Drawing")	bMoveTag = false;
	else if( pWarper.isEditing() && pWarper.getMouseIndex() != -1)		bMoveTag = false;
	else if(panel.getSelectedPanelName() == "Audio Settings" && audio.panel.isMouseInPanel(event.x, event.y) )				bMoveTag = false;
	
	if( bMoveTag )
	{
		if( event.button == 0 )
		{
			if(!bShiftOn)
			{
				tagPosVel.x +=  tagMoveForce * (event.x-lastX);
				tagPosVel.y +=  tagMoveForce * (event.y-lastY);
			}else if(tags.size() > 0){
				tags[currentTagID].position.z += .01 * (event.y-lastY);
				tags[currentTagID].position.z = MAX(tags[currentTagID].position.z,.01);
			}
		}else{
			if( tags.size() > 0 && !bShiftOn)		tags[currentTagID].rotation.y += (event.x-lastX);
			else if( tags.size() > 0 && bShiftOn)	tags[currentTagID].rotation.x += (event.y-lastY);
		}
	}	
	
	
	lastX   = event.x;
	lastY   = event.y;

}

//--------------------------------------------------------------
void GrafPlayerApp::mousePressed(ofMouseEventArgs & event ){

    if( bShowPanel ) panel.mousePressed(event.x,event.y,event.button);
	
	
	if(bUseArchitecture)
	{
		if( panel.isMouseInPanel(event.x, event.y) ) archPhysics.pGroup.disableAll(true);
		else if( panel.getSelectedPanelName() == "Architecture Drawing") archPhysics.pGroup.reEnableLast();
	}
	
	if(bUseAudio)
	{
		audio.panel.mousePressed(event.x,event.y,event.button);
	}
}

//--------------------------------------------------------------
void GrafPlayerApp::mouseReleased(ofMouseEventArgs & event ){

	if( bShowPanel && !panel.minimize) 
	{
		panel.mouseReleased();
	}
	 
	if(bUseAudio)
	{
		audio.panel.mouseReleased();
	}


}

//--------------------------------------------------------------
void GrafPlayerApp::resetPlayer( int next)
{
	if(tags.size() <= 0 ) return;
	
	myTagPlayer.reset();
	
	nextTag(next);
	
	drawer.setup( &tags[currentTagID], tags[currentTagID].distMax );
	
	particleDrawer.reset();
	
	waitTimer = waitTime;
	
	rotationY = 0;
	
	tagPosVel.set(0,0,0);
	
	prevStroke = 0;
	
	if(bRotating) bRotating = true;
	
}
//--------------------------------------------------------------
void GrafPlayerApp::nextTag(int dir)
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
		
	tags[currentTagID].rotation = tags[currentTagID].rotation_o;
	tags[currentTagID].position = tags[currentTagID].position_o;
	
	drawer.resetTransitions();
	rotFixTime = 0;
	archPhysics.reset();
	
	cout << "total pts this tag " << tags[currentTagID].getNPts() << endl;
	
}
//--------------------------------------------------------------
void GrafPlayerApp::loadTags()
{
		
	if( threadedLoader.bResponseReady )
	{
		
		if( threadedLoader.getResponse() == 1 && threadedLoader.totalLoaded > 0 && tags.size() < threadedLoader.totalToLoad)
		{
			
			loadStatus = "Loading...";
			if( tags.size() < threadedLoader.totalLoaded ){
				grafTagMulti temptTag;
				tags.push_back( temptTag );
				int toLoad = tags.size()-1;
				tags[ toLoad ] = threadedLoader.tags[ toLoad ];
				smoother.smoothTag(4, &tags[ toLoad ]);
				tags[toLoad].average();
				tags[toLoad].average();
			}
			
		}else if( tags.size() > 0 && tags.size() >= threadedLoader.totalToLoad ){
			
			threadedLoader.stop();
			mode = PLAY_MODE_PLAY;
			resetPlayer(0);
		}
		
	}
	
	threadedLoader.update();
	
}
//--------------------------------------------------------------
void GrafPlayerApp::preLoadTags(int myMode)
{
	if( myMode == 0)
	{
		threadedLoader.setup( ofToDataPath(myTagDirectory,true) );
		loadStatus = "Loading...";

	}
	else{
		string keywd = panel.getValueS("Rss Keyword");
		threadedLoader.setup( keywd, ofToDataPath(myTagDirectory,true) );
		loadStatus = "Downloading files...";
	}
	
}

//--------------------------------------------------------------
void GrafPlayerApp::saveAllTagPositions()
{
	if(currentTagID < 0) return;
	if( !threadedLoader.bResponseReady ) return;
	
	for( int i = 0; i < tags.size(); i++)
	{
	ofxXmlSettings xml;
	xml.loadFile( threadedLoader.getFileName(i) );
	xml.setValue("GML:tag:environment:offset:x", tags[i].position_o.x );
	xml.setValue("GML:tag:environment:offset:y", tags[i].position_o.y );
	xml.setValue("GML:tag:environment:offset:z", tags[i].position_o.z );
	xml.setValue("GML:tag:environment:rotation:x", tags[i].rotation_o.x );
	xml.setValue("GML:tag:environment:rotation:y", tags[i].rotation_o.y );
	xml.setValue("GML:tag:environment:rotation:z", tags[i].rotation_o.z );
	xml.saveFile( threadedLoader.getFileName(i) );
	}
}

//--------------------------------------------------------------
void GrafPlayerApp::saveTagPositions()
{
	if(currentTagID < 0) return;
	if( !threadedLoader.bResponseReady ) return;
	
	ofxXmlSettings xml;
	xml.loadFile( threadedLoader.getFileName(currentTagID) );
	xml.setValue("GML:tag:environment:offset:x", tags[currentTagID].position.x );
	xml.setValue("GML:tag:environment:offset:y", tags[currentTagID].position.y );
	xml.setValue("GML:tag:environment:offset:z", tags[currentTagID].position.z );
	xml.setValue("GML:tag:environment:rotation:x", tags[currentTagID].rotation.x );
	xml.setValue("GML:tag:environment:rotation:y", tags[currentTagID].rotation.y );
	xml.setValue("GML:tag:environment:rotation:z", tags[currentTagID].rotation.z );
	xml.saveFile(threadedLoader.getFileName(currentTagID) );
}

//--------------------------------------------------------------
void GrafPlayerApp::setupControlPanel()
{
	
	panel.setup("GA 3.0", ofGetWidth()-320, 20, 300, 600);
	panel.addPanel("App Settings", 1, false);
	panel.addPanel("Draw Settings", 1, false);
	panel.addPanel("Audio Settings", 1, false);
	panel.addPanel("Architecture Drawing", 1, false);
	panel.addPanel("Architecture Settings", 1, false);
	panel.addPanel("FBO Warper", 1, false);

	//---- application sttings
	panel.setWhichPanel("App Settings");
	panel.addToggle("Use Audio", "use_audio",true);
	panel.addToggle("Use Architecture", "use_arch",true);
	panel.addToggle("Use Analglyph", "use_rb",true);
	panel.addToggle("Play / Pause", "PLAY", true);
	panel.addToggle("FullScreen", "FULL_SCREEN", false);
	panel.addToggle("Rotate", "ROTATE", true);
	panel.addSlider("Rotation Speed","ROT_SPEED",.65,0,4,false);
	panel.addToggle("Display filename", "SHOW_NAME", true);
	panel.addToggle("Display time", "SHOW_TIME", true);
	panel.addToggle("Save Tag Position/Rotation", "save_Tag_pos", false);
	panel.addSlider("Eye Distance","eye_dist",5,0,20,false);
	panel.addSlider("Change wait time","wait_time",30,0,120,true);
	panel.addToggle("Load tags", "LOAD_TAGS", false);
	rssTextBox = panel.addTextInput("Rss Keyword", "RSS_KEYWORD", "", 100, 20 );
	panel.addToggle("Load from rss", "LOAD_RSS", false);


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
	panel.addSlider("Wave deform force","wave_deform_force",.25,0,2,false);
	panel.addSlider("Bounce force","bounce_force",.25,0,2,false);
	panel.addSlider("Drop p threshold","drop_p_thresh",.1,0,2,false);
	// toggles to apply what to what...
	panel.addToggle("Use particle amp", "use_p_amp", true);
	panel.addToggle("Use particle size", "use_p_size", true);
	panel.addToggle("Use wave deform", "use_wave_deform", true);
	panel.addToggle("Use bounce", "use_bounce", true);
	panel.addToggle("Use drop particel", "use_drop", true);

	panel.setWhichPanel("Architecture Settings");
	panel.addToggle("show drawing tool", "show_drawing_tool",false);
	//panel.addToggle("show image", "show_image",true);
	panel.addSlider("mass","box_mass",1,0,20,false);
	panel.addSlider("bounce","box_bounce",.53,0,2,false);
	panel.addSlider("friction","box_friction",.41,0,2,false);
	panel.addSlider("gravity","gravity",6,0,20,false);
	
	panel.setWhichPanel("Architecture Drawing");
	panel.addToggle("new structure", "new_structure",false);
	panel.addToggle("done","arch_done",false);
	panel.addToggle("save xml", "arch_save", false);
	panel.addToggle("load xml", "arch_load", false);
	panel.addToggle("clear", "arch_clear", false);
	
	panel.setWhichPanel("FBO Warper");
	panel.addToggle("Load Warping", "load_warping", true);
	panel.addToggle("Save Warping", "save_warping", false);
	panel.addToggle("Reset Warping", "reset_warping", false);
	panel.addToggle("Toggle Preview","toggle_fbo_preview",false);
	
	//--- load saved
	panel.loadSettings(pathToSettings+"appSettings.xml");
	
	updateControlPanel(true);
}
//--------------------------------------------------------------
void GrafPlayerApp::updateControlPanel(bool bUpdateAll)
{
	
	panel.update();
	
	bUseAudio = panel.getValueB("use_audio");
	bUseArchitecture = panel.getValueB("use_arch");
	bUseRedBlue = panel.getValueB("use_rb");
	
	if(bUseRedBlue) modeRender = GA_RMODE_RB;
	else modeRender = GA_RMODE_NORMAL;
	
	
	if( panel.getSelectedPanelName() == "App Settings" || bUpdateAll)
	{
		
		myTagPlayer.bPaused = !panel.getValueB("PLAY");
		if( panel.getValueB("FULL_SCREEN") )
		{
			panel.setValueB("FULL_SCREEN",false);
			ofToggleFullscreen();
		}
		bRotating = panel.getValueB("ROTATE");
		bShowName = panel.getValueB("SHOW_NAME");
		bShowTime = panel.getValueB("SHOW_TIME");
		
		if( panel.getValueB("save_Tag_pos") )
		{
			panel.setValueB("save_Tag_pos",false);
			//saveTagPositions();
			saveAllTagPositions();
		}

		//
		if( panel.getValueB("LOAD_TAGS") )
		{
			panel.setValueB("LOAD_TAGS",false);
			mode = PLAY_MODE_LOAD;
			if(tags.size()>0) tags.clear();
			preLoadTags(0);
		}
		
		if( panel.getValueB("LOAD_RSS") )
		{
			panel.setValueB("LOAD_RSS",false);
			mode = PLAY_MODE_LOAD;
			if(tags.size()>0) tags.clear();
			preLoadTags(1);
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
		bUseMask = panel.getValueB("USE_MASK");
		
		
		//bUseFog = panel.getValueB("USE_FOG");
		//fogStart = panel.getValueI("FOG_START");
		//fogEnd = panel.getValueI("FOG_END");
		
		bUseGlobalZ = panel.getValueB("Z_ALL");
		if(bUseGlobalZ)
		{
			z_const = panel.getValueF("Z_DEPTH");
			if(tags.size() > 0 && (lastUpdatedZ != currentTagID || tags[currentTagID].z_const != z_const) )
			{
			updateZDepth(z_const);
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
					updateZDepth(z_const);
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
	
	if( bUseArchitecture && panel.getSelectedPanelName() == "Architecture Drawing" )
	{
		if(panel.bNewPanelSelected)
		{
			archPhysics.bDrawingActive = true;
			archPhysics.pGroup.reEnableLast();
		}
		
		if( panel.getValueB("arch_done") )
		{
			panel.setValueB("arch_done",false);
			createWarpedArchitecture();
		}
		
		if( panel.getValueB("new_structure") )
		{
			panel.setValueB("new_structure",false);
			archPhysics.pGroup.addPoly();
		}
		
		if( panel.getValueB("arch_save") )
		{
			panel.setValueB("arch_save",false);
			archPhysics.saveToXML(pathToSettings+"architecture.xml");
		}
		
		if( panel.getValueB("arch_load") )
		{
			panel.setValueB("arch_load",false);
			archPhysics.loadFromXML(pathToSettings+"architecture.xml");
			createWarpedArchitecture();
		}
		
		if( panel.getValueB("arch_clear") )
		{
			panel.setValueB("arch_clear",false);
			archPhysics.pGroup.clear();
			archPhysics.pGroup.addPoly();
		}
	}
	
	if(  bUseArchitecture &&  panel.getSelectedPanelName() == "Architecture Settings" )
	{
		archPhysics.bShowArchitecture = panel.getValueB("show_drawing_tool");
		
		if( panel.isMouseInPanel(lastX, lastY) )
		{
			archPhysics.setPhysicsParams( panel.getValueF("box_mass"), panel.getValueF("box_bounce"), panel.getValueF("box_friction"));
			archPhysics.box2d.setGravity(0,panel.getValueI("gravity") );
		}
	}
	
	if( panel.getSelectedPanelName() == "FBO Warper")
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
			#ifdef GA_STACK_SCREENS
			pWarper.initWarp( screenW,screenH,screenW*WARP_DIV,screenH*WARP_DIV );
			pWarper.recalculateWarp();
			#else
			pWarper.initWarp( screenW,screenH,screenW*WARP_DIV,screenH*WARP_DIV );
			pWarper.recalculateWarp();
			#endif
		}
		
		
		
	}
	

	//--- disable things
	if( panel.getSelectedPanelName() != "FBO Warper" )
	{
		pWarper.disableEditing();
	}
	
	if( bUseArchitecture && panel.getSelectedPanelName() != "Architecture Drawing")
	{
		archPhysics.bDrawingActive = false;
		archPhysics.pGroup.disableAll(true);
	}
	
	
	
	if(bUseArchitecture && !archPhysics.bSetup )
		archPhysics.setup(screenW,screenH);

}

string GrafPlayerApp::getCurrentTagName()
{
	if(tags.size() <= 0 ) return " ";
	else if( mode == PLAY_MODE_LOAD) return tags[tags.size()-1].tagname;
	else return tags[currentTagID].tagname;
}


void GrafPlayerApp::createWarpedArchitecture()
{
	wPolys.clear();
	
	for( int i = 0; i < archPhysics.pGroup.polys.size(); i++)
	{
		polySimple tPoly;
		tPoly.pts.assign( archPhysics.pGroup.polys[i]->pts.begin(),archPhysics.pGroup.polys[i]->pts.end() );
		wPolys.push_back(tPoly);
	}
	
	#ifdef GA_STACK_SCREENS
		for( int i = 0; i < wPolys.size(); i++)
		{
			for( int j = 0; j < wPolys[i].pts.size(); j++)
			{
				//ofPoint wPoint = pWarper.warpPoint(wPolys[i].pts[j]);
				if(wPolys[i].pts[j].x > 1024*2){
				 wPolys[i].pts[j].x -= 1024*2;
				 wPolys[i].pts[j].y += 768;
				}else if(wPolys[i].pts[j].x > 1024){
					wPolys[i].pts[j].x -= 1024;
				}
			}
		}
	#else
		for( int i = 0; i < wPolys.size(); i++)
		{
			for( int j = 0; j < wPolys[i].pts.size(); j++)
			{
				ofPoint wPoint = pWarper.warpPoint(wPolys[i].pts[j]);
				wPolys[i].pts[j] = wPoint;
			}
		}
	#endif	
	archPhysics.createArchitectureFromPolys(wPolys);
}


void GrafPlayerApp::loadScreenSettings()
{
	ofxXmlSettings xml;
	xml.loadFile(pathToSettings+"screenSettings.xml");
	xml.pushTag("screen");
		screenW = xml.getValue("width",1024);
		screenH = xml.getValue("height",768);
	xml.popTag();
}


void GrafPlayerApp::updateZDepth(float z_const)
{
	for( int i = 0; i < tags[ currentTagID ].myStrokes.size(); i++)
	{
		for( int j = 0; j < tags[ currentTagID ].myStrokes[i].pts.size(); j++)
		{
		
		tags[ currentTagID ].myStrokes[i].pts[j].pos.z = ( tags[ currentTagID ].myStrokes[i].pts[j].time * 1000.f ) / z_const;
		}
	}
	
	drawer.alterZDepth(&tags[ currentTagID ]);
}

