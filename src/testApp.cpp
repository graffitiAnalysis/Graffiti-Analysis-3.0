#include "testApp.h"


//--------------------------------------------------------------
void testApp::setup(){
	
	
	// app setup
	ofSetFrameRate(30);
	ofBackground(0,0,0);
	bShowMouse = true;
	
	// sound stream setup
	ofSoundStreamSetup(0,1,this, 44100, NUM_BANDS, 4);	

	// manager and app part setups
	manager.setup();
	
	
}



//--------------------------------------------------------------
void testApp::update(){

	if( manager.mode == GA_MODE_START ) manager.update();
	else if( manager.mode == GA_MODE_RECORDER ) ;
	else if( manager.mode == GA_MODE_PLAYER ){
	 if( !grafInteractiveApp.bSetup) grafInteractiveApp.setup();
	 else grafInteractiveApp.update();
	}
	else if( manager.mode == GA_MODE_LASER ) ;
	
	ofSoundUpdate();	

}

//--------------------------------------------------------------
void testApp::draw(){

	if( manager.mode == GA_MODE_START ) manager.draw();
	else if( manager.mode == GA_MODE_RECORDER ){
		glPushMatrix();
			glTranslatef(ofGetWidth()/2,ofGetHeight()/2,0);
			ofDrawBitmapString("Recorder: Not ready yet.", 0, 0);
		glPopMatrix();
	}
	else if( manager.mode == GA_MODE_PLAYER ){
		if(grafInteractiveApp.bSetup) grafInteractiveApp.draw();
	}
	else if( manager.mode == GA_MODE_LASER ){
		glPushMatrix();
			glTranslatef(ofGetWidth()/2,ofGetHeight()/2,0);
			ofDrawBitmapString("Laser Tag: Not ready yet.", 0, 0);
		glPopMatrix();
	}
		
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
	
	// global keys always in use
	if( key == 'f' || key == 'F' )	ofToggleFullscreen();
	else if( key == 'm')
	{
		bShowMouse = !bShowMouse;
		if(!bShowMouse) ofHideCursor();
		else ofShowCursor();
		
	}	
	
	// keys to switch modes
	if( key == OF_KEY_F1 )		manager.setMode( GA_MODE_START );
	else if( key == OF_KEY_F2 ) manager.setMode( GA_MODE_RECORDER );
	else if( key == OF_KEY_F3 ) manager.setMode( GA_MODE_PLAYER );
	else if( key == OF_KEY_F4 ) manager.setMode( GA_MODE_LASER );
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){
	
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){
	
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
	
	
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

	int modifier = glutGetModifiers();
	if( modifier == GLUT_ACTIVE_SHIFT) grafInteractiveApp.bShiftOn = true;
	else grafInteractiveApp.bShiftOn = false;

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::audioReceived(float * input, int bufferSize, int nChannels){	
	// samples are "interleaved"
	if(grafInteractiveApp.audio.bSetup && grafInteractiveApp.bUseAudio)
	{
	memcpy(grafInteractiveApp.audio.audioInput, input, sizeof(float) * (NUM_BANDS));
		grafInteractiveApp.audio.bufferCounter++;
	}
}
