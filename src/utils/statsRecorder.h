#ifndef _STAT_RECORDER_
#define _STAT_RECORDER_


#include "ofMain.h"
#include <vector>

/*
A statistics recorder for float values.
Setup the object with a value range. All values are stored normalized based on the range , but
can be returned at the original value.
*/

class statsRecorder{

	public:

		statsRecorder();
		~statsRecorder();


		/* setup()
		maxValues: maximum number of values to record
		lo: low value in the range (used to normalize but values can be lower)
		hi: high value in range (used to normalize but values can be higher)
		*/
		void	setup( int maxValues, float lo, float hi );


		/* update()
		Pass in the next value. Call only when you want to add a value.
		*/
		void	update(  float val  );


		void	reset();

		/* draw()
		x,y :           position to draw
		w,h:            size to draw at (scaled base on this)
		nLastValues:    use to draw a line at the average of this number of most recent values. If 0 it will not draw.
		label:          a label for the recorder, set "" for blank
		bLabeled:       if true, draws the lowest and highest values
		nColor:         color to draw line
		*/
		void	draw( int x_=0, int y_=0, float w = 0, float h = 0, int nLastValues = 0, string label = "", bool bLabeled = false, int nColor = 0xff00ff);

		void	drawLine( int x_, int y_, float scaleH=1, float scaleW = 1, int nColor = 0xff00ff);
        void    drawAverage( int x_, int y_,int lastNVals, float scaleH = 1, float scaleW = 1 );

		/* getAverage()
		Returns the average of the most recent values up to the number passed to the function.
		*/
		float	getAverage( int lastNVals );

		/* getLastValue()
		Returns the most recent value passed in. Not scaled or normalized.
		*/
		float	getLastValue();

        /* getLastValueNormal()
        Returns the value normalized based on the range (may not be between 0 and 1 if numbers added are out of range)
        */
        float   getLastValueNormal();

		/* addMarker();
		Adds a line to draw at a given height (such as midpoint of value range)
		*/
		void    addMarker(float yPos);

		std::vector<float>stats;
		std::vector<float>marker;

	protected:


		int     maxValues;
		float   loVal, hiVal, range;




};

#endif

