#pragma once

#include "ofMain.h"
#include "ofxVectorMath.h"
#include "ofxOpenCv.h"

#include "guiQuad.h"
#include "laserUtils.h"
#include "coordWarping.h"
#include "hitZone.h"
#include "guiCustomImpl.h"
#include "ofxXmlSettings.h"

class colorTrackerPanel : public ofBaseDraws{
	public:
	
		colorTrackerPanel(){
			width  = 160;
			height = 120;
			
			r0Min  = 0;
			r0Max  = 0;
			g0Min  = 0;
			g0Max  = 0;
			b0Min  = 0;
			b0Max  = 0;
			r1Min  = 0; 
			r1Max  = 0;
			g1Min  = 0;
			g1Max  = 0; 
			b1Min  = 0;
			b1Max  = 0;
		}

		float getWidth(){
			return width;
		}
			
		float getHeight(){
			return height;
		}
		
		void setHsv(float hue, float hueThresh, float sat, float value){
					
			//ehh this is kind of ugly and needs to be fixed I think
			//Lets calc the rgb values for the min and max hue range			
			float rgb[3];
			float hsb[3];
			
			hsb[0] = hue-hueThresh*0.5;
			hsb[1] = sat;
			hsb[2] = value;
			
			hsbToRgb(hsb, rgb);
			
			r0Min = (int)(rgb[0]*255);
			g0Min = (int)(rgb[1]*255);
			b0Min = (int)(rgb[2]*255);
			
			hsb[0] = hue+hueThresh*0.5;
			hsb[1] = sat;
			hsb[2] = value;
					
			hsbToRgb(hsb, rgb);
								
			r0Max = (int)(rgb[0]*255);
			g0Max = (int)(rgb[1]*255);
			b0Max = (int)(rgb[2]*255);
			
			hsb[0] = hue-hueThresh*0.5;
			if(sat == 0) hsb[1] = 0.0;
			else hsb[1] = 1.0;			
			hsb[2] = 1.0;
			
			hsbToRgb(hsb, rgb);
			
			r1Min = (int)(rgb[0]*255);
			g1Min = (int)(rgb[1]*255);
			b1Min = (int)(rgb[2]*255);
			
			hsb[0] = hue+hueThresh*0.5;
			if(sat == 0) hsb[1] = 0.0;
			else hsb[1] = 1.0;	
			hsb[2] = 1.0;
					
			hsbToRgb(hsb, rgb);
								
			r1Max = (int)(rgb[0]*255);
			g1Max = (int)(rgb[1]*255);
			b1Max = (int)(rgb[2]*255);
		}


		void draw(float x, float y){
			draw(x, y, width, height);
		}
		
		//---------------------------		
		void draw(float x, float y, float w, float h){

			//this is not super accurate
			//but good for an indication
			glBegin(GL_QUADS);
				ofSetColor(r1Min, g1Min, b1Min);
				glVertex2f(x,  y);
				
				ofSetColor(r1Max, g1Max, b1Max);				
				glVertex2f(x+w, y);
				
				ofSetColor(r0Max, g0Max, b0Max);
				glVertex2f(x+w, y + h );

				ofSetColor(r0Min, g0Min, b0Min);
				glVertex2f(x, y + h);
			glEnd();

			
			glBegin(GL_LINE_LOOP);
			ofSetColor(0xFFFFFF);
				glVertex2f(x, y);
				glVertex2f(x + w, y);
				glVertex2f(x + w, y + h);
				glVertex2f(x  ,   y + h);
			glEnd();

		}	
		
		float width, height;
		int r0Min, r0Max, g0Min, g0Max, b0Min, b0Max, r1Min, r1Max, g1Min, g1Max, b1Min, b1Max;		
};

//inhereits base gui - for status message functionality
class laserTracking : public guiCustomImpl{

	public:
	
		//---------------------------
		laserTracking();

		void setClearThreshold(float clearSens);
		void setClearZone(vector <ofPoint> pts);
		void setUseClearZone(bool useClearZone);
		bool isClearZoneHit();
		
		void clearData();
		
		float getWidth(){
			return width;
		}

		float getHeight(){
			return height;
		}
		

        virtual void saveSettings(string filename){
			saveSettings();
		}
        
		virtual void reloadSettings(){
			QUAD.readFromFile("settings/cameraQuad.xml");
			
			ofxXmlSettings xml;
			xml.loadFile("settings/clearZone.xml");
			
			clearZone.points[0].x = xml.getValue("quad:pt0:x", clearZone.points[0].x);
			clearZone.points[0].y = xml.getValue("quad:pt0:y", clearZone.points[0].y);

			clearZone.points[1].x = xml.getValue("quad:pt1:x", clearZone.points[1].x);
			clearZone.points[1].y = xml.getValue("quad:pt1:y", clearZone.points[1].y);

			clearZone.points[2].x = xml.getValue("quad:pt2:x", clearZone.points[2].x);
			clearZone.points[2].y = xml.getValue("quad:pt2:y", clearZone.points[2].y);

			clearZone.points[3].x = xml.getValue("quad:pt3:x", clearZone.points[3].x);
			clearZone.points[3].y = xml.getValue("quad:pt3:y", clearZone.points[3].y);
			
			printf("clearZone.points[0].x  is %f\n", clearZone.points[0].x );
			
			clearZone.updatePoints();
		}
		
        virtual void saveSettings(){
			QUAD.saveToFile("settings/cameraQuad.xml");
			ofxXmlSettings xml;
			
			xml.setValue("quad:pt0:x", clearZone.points[0].x);
			xml.setValue("quad:pt0:y", clearZone.points[0].y);

			xml.setValue("quad:pt1:x", clearZone.points[1].x);
			xml.setValue("quad:pt1:y", clearZone.points[1].y);

			xml.setValue("quad:pt2:x", clearZone.points[2].x);
			xml.setValue("quad:pt2:y", clearZone.points[2].y);

			xml.setValue("quad:pt3:x", clearZone.points[3].x);
			xml.setValue("quad:pt3:y", clearZone.points[3].y);		
				
			xml.saveFile("settings/clearZone.xml");			
		}
        
		virtual void loadSettings(string filename){
			reloadSettings();
		}		
				
		//changing cameras or switching from/to the camera mode
		//requires the app to be restarted - mabe we can change this?
		//---------------------------		
		void setupCamera(int deviceNumber, int width, int height);
		
		//same as above - from/to using test movies
		//requires a restart
		//---------------------------		
		void setupVideo(string videoPath);
		
		//good for adjusting the color balance, brightness etc
		//---------------------------		
		void openCameraSettings();
		
		//do all our openCV allocation
		//and loads our quad settings from xml
		//---------------------------		
		void setupCV(string filePath);
		
		//the heart of the beast - where we process 
		//the incoming frame and look for a laser
		//---------------------------		
		void processFrame(float hue, float hueThresh, float sat, float value, int minSize, int deadCount, float jumpDist, bool slowButAccurateQuad);		
	
		//if you want to warp the coords to another quad - give the four points 
		//-----------------------------------------------------------------------
		void getWarpedCoordinates(ofxPoint2f * dst, float *warpedX, float *warpedY);
		
		//tells you if new points have arrived
		bool newData();
		
		//tells you if a new stroke has been detected
		bool isStrokeNew();	
		
		//clear the new stroke flag
		void clearNewStroke();
		
		//this formats the x and y coordinates
		//to a string that can be sent to other apps
		//over tcp and udp we will see both regular
		//and transformed coordinates
		//---------------------------		
		string getLaserPointString();

		void calcColorRange(float hue, float hueThresh, float sat, float value);
		void drawPanels(float x, float y);
		void drawPanels(float x, float y, float w, float h);

		void draw(float x, float y, float w, float h);

		void drawColorRange(float x, float y, float w, float h);
		void drawClearZone(float x, float y, float w, float h);
		void drawQuadSetupImage(float x, float y, float w, float h);
		
		//---------------------------		
		//---------------------------		
		
		ofVideoPlayer  	VP;
		ofVideoGrabber 	VG;
		guiQuad			QUAD;
		coordWarping	CW;
		
		ofxPoint2f 			warpSrc[4];
		ofxPoint2f 			warpDst[4];
		
		ofxPoint2f			smoothPos;
		ofxPoint2f			smoothVel;
		ofxPoint2f 			outPoints;
		
		ofxCvColorImage  	VideoFrame;
		ofxCvColorImage 	WarpedFrame;
		ofxCvColorImage 	hsvFrame;
		ofxCvGrayscaleImage PresenceFrame;
		ofxCvContourFinder	Contour;
		
		ofImage				resizeMe;
		
		float width, height;
		
		unsigned char * pre;
		
		ofxXmlSettings xmlForHitZone;
		string sendStr;
		
		bool bCameraSetup, bVideoSetup, newPos, newStroke, shouldClear, accurateQuad;
		
		int W;
		int H;
		int noLaserCounter;
		int clearThresh;
		
		bool bStartedInClearZone;
		bool bSetup;

		float laserX;
		float laserY;
		float oldX;
		float oldY;
		float distX;
		float distY;
		float distDifference;
			
		hitZone clearZone;
		hitZone brushZone;
		
		colorTrackerPanel trackerPanel;
		
		int r0Min, r0Max, g0Min, g0Max, b0Min, b0Max, r1Min, r1Max, g1Min, g1Max, b1Min, b1Max;
};
