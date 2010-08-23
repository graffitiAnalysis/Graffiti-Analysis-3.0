#ifndef GRAF_LINE_DRAWER_H
#define GRAF_LINE_DRAWER_H

#include "ofMain.h"
#include "ofxVectorMath.h"
#include "grafTypes.h"

class grafLineDrawer
{
    public:
        grafLineDrawer();
        virtual ~grafLineDrawer();
		
		void setup(vector<timePt> pts, float minDist, float maxDist, float blendPct, int start=-1, int end=-1, bool bFadeIn = false, bool bFadeOut = false);
		void setupUniform(vector<timePt> pts, float lineDist, int start=-1, int end=-1);
		void average( float pct );
	
		void draw( int lastPt, float alpha = .75f, int startPt = -1);
		void drawOutline( int lastPt, float alpha = .75f, float lineW = 1,int startPt = -1);
	
		float blendPct;
		float strokeVal;
	
		vector<ofPoint> pts_l;	// end points of line
		vector<ofPoint> pts_r;
	
		vector<ofPoint> pts_lo;	// origins so can deform after
		vector<ofPoint> pts_ro;
		
		vector<ofxVec2f> vecs;	// vectors of one end point to other
		
		vector<ofPoint> pts_lout;	// endpoints outline
		vector<ofPoint> pts_rout;
	
		vector<float>	alphas;	// alpha value at each point
	
	
		ofPoint		end_pt_l;
		ofPoint		end_pt_r;
	
		float		globalAlpha;
		float		lineScale;
		bool		bUseFadeInOut;
		
		float		outlineDist;	// distance outline is drawn from line (should be very low )
	
    protected:
	
		//void calculateStroke();
		void calculatePoint(timePt pt, float time_num, float dist, float angle, vector<ofPoint>&left, vector<ofPoint>&right);
		float getAlphaForLinePosition( int pos, int total );

		void drawSegment(ofPoint * ptR, ofPoint * ptL);
	
    private:
};

#endif // GRAF_LINE_DRAWER_H
