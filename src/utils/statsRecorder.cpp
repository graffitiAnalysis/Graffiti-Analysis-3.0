#include "statsRecorder.h"
statsRecorder::statsRecorder()
{
    maxValues = 100;
	loVal = 0;
	hiVal = 1;
	range = 1;
}

void statsRecorder::setup( int max, float lo, float hi )
{

	maxValues = max;
	loVal = lo;
	hiVal = hi;
	range = hi - lo;


}
//----------------------------------------------------------------------------------------------------------------
statsRecorder::~statsRecorder()
{
	stats.clear();
	marker.clear();
}
//----------------------------------------------------------------------------------------------------------------
void statsRecorder::reset()
{
	stats.clear();
	marker.clear();
}
//----------------------------------------------------------------------------------------------------------------
void statsRecorder::update( float val )
{

	stats.push_back( (val-loVal) / range);
	if( stats.size() > (unsigned int)maxValues ) stats.erase( stats.begin() );


}
//----------------------------------------------------------------------------------------------------------------
float statsRecorder::getAverage(int lastNVals)
{
    int total = lastNVals;

    if(lastNVals <= 0 ) total = stats.size();
    else total = MIN((int)stats.size(),lastNVals);

	float avg = 0.f;

	for(int i = 0; i < total; i++)
	{
		avg += stats[i];
	}

	if(total > 0)
	{
	     return avg / (float)total;
	}else
		return 0;

}
//----------------------------------------------------------------------------------------------------------------
float statsRecorder::getLastValue()
{
	if(stats.size()>1)
	return loVal + ( stats[ stats.size()-1 ] * range );
	else return 0;
}
//----------------------------------------------------------------------------------------------------------------
float statsRecorder::getLastValueNormal()
{
	if(stats.size()>1)
	return stats[ stats.size()-1 ];
	else return 0;
}
//----------------------------------------------------------------------------------------------------------------
void statsRecorder::addMarker(float yPos)
{
    marker.push_back( ( (yPos-loVal) / range) );
}
//----------------------------------------------------------------------------------------------------------------
void statsRecorder::draw( int x_, int y_, float w, float h, int nLastValues,string label, bool bLabeled, int nColor )
{



    float scaleH = h;
    float scaleW = w / maxValues;

    if(w == 0 ) scaleW = 1;
    if(h == 0 ) scaleH = maxValues*scaleW;


    ofSetColor(0x333333);
	ofRect(x_,y_,maxValues*scaleW,scaleH);


	glPushMatrix();

        glTranslatef(x_,y_,0);


        ofSetColor(0x888888);
        for(unsigned int i = 0; i < marker.size(); i++)
        {
            float y1 = scaleH-(marker[i]*scaleH);
            ofLine(0,y1,maxValues*scaleW,y1);
        }

        if(nLastValues > 0) drawAverage( 0,  scaleH,  nLastValues, scaleH, scaleW );

        drawLine(0,scaleH,scaleH,scaleW,nColor);

        ofSetColor(0xffffff);
        ofDrawBitmapString(label,5,12);


        if(bLabeled)
        {
        ofDrawBitmapString( ofToString(loVal,2),maxValues*scaleW + 10, scaleH);
        ofDrawBitmapString( ofToString(hiVal,2),maxValues*scaleW + 10, 12);
        }

    glPopMatrix();

}
//----------------------------------------------------------------------------------------------------------------
void statsRecorder::drawLine( int x_, int y_, float scaleH, float scaleW, int nColor )
{

	glBegin( GL_LINE_STRIP );


    ofSetColor(nColor);

    for(unsigned int i = 0; i < stats.size(); i++)
    {
        glVertex2f( x_+ i*scaleW, y_ - (stats[i]*scaleH));
    }

    glEnd();





}

void statsRecorder::drawAverage( int x_, int y_, int lastNVals, float scaleH, float scaleW )
{

    float average = getAverage(lastNVals);
    float x1 = x_;
    float y1 = y_-average*scaleH;
    float x2 = x_+maxValues*scaleW+10;
    float y2 = y_-average*scaleH;

    ofLine(x1,y1,x2,y2);
}

