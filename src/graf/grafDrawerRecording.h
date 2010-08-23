#ifndef GRAF_DRAWER_RECORDING_H
#define GRAF_DRAWER_RECORDING_H

#include "ofMain.h"
#include "grafLineDrawer.h"
#include "grafTagMulti.h"

class grafDrawerRecording
{
    public:
        grafDrawerRecording();
        virtual ~grafDrawerRecording();
		
		void setup(grafTagMulti * myTag, int startStroke, int startPt, int endStroke, int endPt);
		
		void draw( int startStroke, int startPoint, int lastStroke, int lastPoint);
		void drawTimeLine( ofPoint center, float currentTime, float startTime, float z_const, ofTrueTypeFont * font, float scale );
		void drawBoundingBox( ofPoint pmin, ofPoint pmax, ofPoint pcenter );
		void drawWireFrame(grafTagMulti * myTag,int startStroke, int startPoint, int lastStroke, int lastPoint);
	
		void transition( float dt, float pct );
		void average( float pct );
		void setAlpha(float a);
		void setLineScale( float val );
	
	
		vector<grafLineDrawer *> lines;
		
		float alpha;
		float minLen;
		float maxLen;
		float lineWidth;
		float lineAlpha;
		float lineScale;
	
		bool bSetupDrawer;
	
	
	
    protected:
		
		
	
    private:
};

#endif // GRAF_DRAWER_H
