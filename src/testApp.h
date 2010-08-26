#ifndef _TEST_APP
#define _TEST_APP


#include "ofMain.h"
#include "grafPlayerApp.h"
#include "gaManagerApp.h"
#include "grafLaserApp.h"


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
		GrafPlayerApp		grafInteractiveApp;
		GrafLaserApp		grafLaserApp;
		
		bool bShowMouse;
		
		

};

#endif
