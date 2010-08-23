#ifndef GRAFTAGMULTI_H
#define GRAFTAGMULTI_H

#include "ofMain.h"
#include "ofxVectorMath.h"

#include "grafTypes.h"

class grafTagMulti
{
    public:
        grafTagMulti();
        virtual ~grafTagMulti();

        grafTagMulti(  const grafTagMulti &  mom );


		void mouseDragged(ofMouseEventArgs& event);
		void mousePressed(ofMouseEventArgs& event);
		void mouseReleased(ofMouseEventArgs& event);
		//void mouseMoved(int x, int y);
	
		// clears all point arrays
        void clear(bool bAdddInitStroke = true );
		void draw();
		void drawSkeleton(bool bCentered = true);
		void drawBoundingBox();
		void drawBoundingBox( ofPoint pmin, ofPoint pmax, ofPoint pcenter );

		void setDrawScale( float val ){ drawScale = val; }
		void applyDrawScale();

        // create a new point and add to end of pts vector -- use this for loading tags (following for recording)
        // pt     -normalized position of new point
        // dist   -distance of this point to last (used for line width calculations). If valuse = -1 it will calculate distance from previous point
        // angle  -angle of this point to last. If value = -1 it will calculate to previous
        // time   -time stamp associated with this new point. If value = -1, time is based on elapsedTime - startTime
        bool addNewPoint(ofPoint pt, float dist = -1, float angle = -1, float time = -1, bool bAverage = true);
		
        // advance to next stroke
		void nextStroke();
		void cleanUpLastStroke();
	
		// calculates the min and max distance between points for this tag
        void calcMinMax();

        // averages to smooth out
		void average();
		void averagePts( strokes * stroke );

        // get data
        int     getStartPt(){		return ptStart;		 }
        int     getEndPt(){			return ptEnd;		 }


		vector <strokes>		myStrokes;            // al points in tag

		// global tag draw params
		ofPoint             rotation;
		ofPoint             position;
		float               z_const;
		float				drawScale;

		// tag structural data
		ofPoint             min,max,center;
		float               distMin, distMax;
		float				minPtDist;
		
		// tag info
		string				tagname;
		bool                bPlayed;
		float				startTime;
		int					maxNumPts;
		int                 ptStart, ptEnd;             // start and end points. if not edited ptStart = 0 and ptEnd = lastPoint
	
		bool				bMouseActive;
		float				lastX,lastY;
		bool				bShift;
		
		int getNPts(){ return nPts; }
	
		
    protected:

		int nPts;


    private:
};

#endif // GRAFTAGMULTI_H
