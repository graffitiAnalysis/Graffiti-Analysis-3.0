#ifndef _TEST_APP
#define _TEST_APP


#include "ofMain.h"
#include "grafPlayerApp.h"
#include "gaManagerApp.h"

/*
Graffiti Analysis 2.0: Audio Interactive
2010

Analyzes audio input and plays <gml> tags that respond to fft data.

*/

#include "ofxQtVideoSaver.h"


class testApp : public ofBaseApp{

	public:

		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void resized(int w, int h);
		void audioReceived(float * input, int bufferSize, int nChannels);
		
		GaManagerApp		manager;
		GrafPlayerApp	grafInteractiveApp;
		bool bShowMouse;
		
		//----- quicktime recording
		ofxQtVideoSaver	movieSaver;
		bool bRecordingMovie;
		bool bUseRecorder;

};

#endif
