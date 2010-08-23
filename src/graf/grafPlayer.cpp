#include "grafPlayer.h"

grafPlayer::grafPlayer()
{

    bPaused         = false;
    bDonePlaying    = false;
	bReset			= true;

    timeCounter     = 0.f;
    timeOfLastFrame = 0.f;
    timeInRange     = 0.f;

	nowIndex        = 0;
    nowDist         = 0.f;
    nowAngle        = 0.f;
    nowPointForTime.set(0,0,0);

}

grafPlayer::~grafPlayer()
{
    //dtor
}

void grafPlayer::update(grafTagMulti * myTag )
{

    //----- don't do it if there are no points
    if( myTag->myStrokes.size() == 0 )			return;
	if( myTag->myStrokes[0].pts.size() == 0 )	return;

    //----- check time change to update animation
    float diffTime		        = ofGetElapsedTimef() - timeOfLastFrame;
	timeOfLastFrame		        = ofGetElapsedTimef();
	if(!bPaused) timeCounter	+= diffTime;


    //------ set current point and time
   calcPointForTime( timeCounter, myTag);

   

}

void grafPlayer::calcPointForTime(float time, grafTagMulti * myTag)
{
	
	//----- don't do it if there are no points
    if( myTag->myStrokes.size() == 0 )			return;
	if( myTag->myStrokes[0].pts.size() == 0 )	return;
	
	
	//----- get the total time of the tag
	float totalTime     = getDuration(myTag);
	
	
	//----- set the current time
	timeInRange = time;
	if(  timeInRange > totalTime ){
		
		bDonePlaying = true;
		//timeCounter = 0;
		//timeInRange = totalTime;
		int lastStroke = myTag->myStrokes.size();
		int lastPoint = myTag->myStrokes[lastStroke].pts.size()-1;
		nowStroke = lastStroke;
		nowIndex = lastPoint;
		return;
	}
	
	
	//---- find the closest point to the current time
	int whatStrokeAmIn		= 0;
	int whatPointAmINear	= 0;
	
	bool bFound = false;
	
	for (int i = 0; i < myTag->myStrokes.size(); i++)
	{
		for (int j = 0; j < myTag->myStrokes[i].pts.size(); j++)
		{
			if( myTag->myStrokes[i].pts[j].time > timeInRange)
			{
				whatStrokeAmIn		= i;
				whatPointAmINear	= j;
				bFound = true;
				break;
			}
		}
		if(bFound) break;
	}
	
	
	nowIndex		= whatPointAmINear;
	nowStroke		= whatStrokeAmIn;
	
	//---- make calculations
	if (whatPointAmINear > 0)
	{
		
		float timea = myTag->myStrokes[whatStrokeAmIn].pts[whatPointAmINear - 1].time;
		ofPoint pta = myTag->myStrokes[whatStrokeAmIn].pts[whatPointAmINear - 1].pos;
		float timeb = myTag->myStrokes[whatStrokeAmIn].pts[whatPointAmINear    ].time;
		ofPoint ptb = myTag->myStrokes[whatStrokeAmIn].pts[whatPointAmINear    ].pos;
		
		float totalDurationBetweenThesePts	= timeb - timea;
		float myPositionBetweenThesePts		= timeInRange - timea;
		float pct = myPositionBetweenThesePts / totalDurationBetweenThesePts;
		
		ofPoint mix(0,0,0);
		mix.x = (1-pct) * pta.x + (pct) * ptb.x;
		mix.y = (1-pct) * pta.y + (pct) * ptb.y;
		mix.z = (1-pct) * pta.z + (pct) * ptb.z;
		
		nowDist  = (1-pct) * myTag->myStrokes[whatStrokeAmIn].pts[whatPointAmINear - 1].dist + (pct) * myTag->myStrokes[whatStrokeAmIn].pts[whatPointAmINear].dist;
		nowAngle = (1-pct) * myTag->myStrokes[whatStrokeAmIn].pts[whatPointAmINear - 1].angle + (pct) * myTag->myStrokes[whatStrokeAmIn].pts[whatPointAmINear].angle;
		
		nowPointForTime = mix;
		
	} else {
		
		nowDist			=  myTag->myStrokes[whatStrokeAmIn].pts[whatPointAmINear].dist;
		nowAngle		=  myTag->myStrokes[whatStrokeAmIn].pts[whatPointAmINear].angle;
		nowPointForTime =  myTag->myStrokes[whatStrokeAmIn].pts[whatPointAmINear].pos;
	}
	
}


void grafPlayer::drawNowPoint( float scale )
{
	
	ofRect( nowPointForTime.x*scale, nowPointForTime.y*scale, 2, 2);
	
}

ofPoint grafPlayer::getVelocityForTime( grafTagMulti * myTag)
{
    // ok the trick is to do the same as above (find the time in the gesture)
	// and to find a time slightly less... (prev position and current position).
	
	//----- don't do it if there are no points
    if( myTag->myStrokes.size() == 0 )			return ofPoint(0,0,0);
	if( myTag->myStrokes[0].pts.size() == 0 )	return ofPoint(0,0,0);
	
	//----- get the total time of the tag
	float totalTime     = getDuration(myTag);
	
	//----- set the current time
	timeInRange = timeCounter;
	
	float prevTime = MAX(0, timeInRange - 0.016666f); // time minus 1/60 of a second....
	
	calcPointForTime(prevTime, myTag);
	ofPoint prevPoint = nowPointForTime;
	
	calcPointForTime(timeCounter, myTag);
	ofPoint curPoint = nowPointForTime;

	ofPoint velocity;
	
	velocity.x			= curPoint.x - prevPoint.x;
	velocity.y			= curPoint.y - prevPoint.y;
	
	return velocity;
}
/*
ofPoint grafPlayer::getPointForTime(    float time, int & whatPointAmINear, float & dist, float & angle)
{

	if (pts.size() <= 1) return ofPoint(0,0,0);

	float totalTime     = getDuration();
    timeInRange         = time;

	while (timeInRange > totalTime){
		timeInRange -= totalTime;
	}

	whatPointAmINear = 0;
	for (int i = 0; i < pts.size(); i++){

		if (pts[i].time > timeInRange){
			whatPointAmINear = i;
			break;
		}
	}

	if (whatPointAmINear > 0){

		float timea = pts[whatPointAmINear - 1].time;
		ofPoint pta = pts[whatPointAmINear - 1].pos;
		float timeb = pts[whatPointAmINear    ].time;
		ofPoint ptb = pts[whatPointAmINear    ].pos;

		float totalDurationBetweenThesePts	= timeb - timea;
		float myPositionBetweenThesePts		= timeInRange - timea;
		float pct = myPositionBetweenThesePts / totalDurationBetweenThesePts;

		ofPoint mix(0,0,0);
		mix.x = (1-pct) * pta.x + (pct) * ptb.x;
		mix.y = (1-pct) * pta.y + (pct) * ptb.y;
		mix.z = (1-pct) * pta.z + (pct) * ptb.z;

		dist  = (1-pct) * pts[whatPointAmINear - 1].dist + (pct) * pts[whatPointAmINear].dist;
		angle = (1-pct) * pts[whatPointAmINear - 1].angle + (pct) * pts[whatPointAmINear].angle;
		//angle = atan2(pts[whatPointAmINear].y - pts[whatPointAmINear - 1].y, pts[whatPointAmINear].x - pts[whatPointAmINear-1].x);

		return mix;

	} else {

		dist    = pts[whatPointAmINear].dist;
		angle   = pts[whatPointAmINear].angle;
		return pts[whatPointAmINear].pos;
	}
}

ofPoint grafPlayer::getPointForTime( float time)
{

	if (pts.size() <= 1) return ofPoint(0,0,0);

	float totalTime       = getDuration();
    float myTimeInRange   = time;

	while (myTimeInRange > totalTime){
		myTimeInRange -= totalTime;
	}

	float whatAmINear = 0;
	for (int i = 0; i < pts.size(); i++){

		if (pts[i].time > myTimeInRange){
			whatAmINear = i;
			break;
		}
	}

	if (whatAmINear > 0){

		float timea = pts[whatAmINear - 1].time;
		ofPoint pta = pts[whatAmINear - 1].pos;
		float timeb = pts[whatAmINear    ].time;
		ofPoint ptb = pts[whatAmINear    ].pos;

		float totalDurationBetweenThesePts	= timeb - timea;
		float myPositionBetweenThesePts		= myTimeInRange - timea;
		float pct = myPositionBetweenThesePts / totalDurationBetweenThesePts;

		ofPoint mix(0,0,0);
		mix.x = (1-pct) * pta.x + (pct) * ptb.x;
		mix.y = (1-pct) * pta.y + (pct) * ptb.y;
		mix.z = (1-pct) * pta.z + (pct) * ptb.z;

		return mix;

	} else {
		return pts[whatAmINear].pos;
	}
}


*/
ofPoint grafPlayer::getCurrentPoint()
{
    return nowPointForTime;
}

float grafPlayer::getDuration(grafTagMulti * myTag)
{
    float totalDuration = 0;
	if ( myTag->myStrokes.size() > 0 && myTag->myStrokes[0].pts.size() > 0)
	{
		int lastStroke = myTag->myStrokes.size()-1;
		int lastPt = myTag->myStrokes[lastStroke].pts.size()-1;
		totalDuration = myTag->myStrokes[lastStroke].pts[lastPt].time;
		//cout << "last stroke " << lastStroke << " last pt " << lastPt << " duration " << totalDuration << endl;
		
	}
	return totalDuration;
}

float   grafPlayer::getCurrentZDepth( float z_const)
{
    return (timeInRange*1000) / z_const;
}

void grafPlayer::reset()
{
    bPaused         = false;
    bDonePlaying    = false;
	bReset			= true;
	
    timeCounter     = 0.f;
    timeInRange     = 0.f;
	timeOfLastFrame     = ofGetElapsedTimef();

	nowIndex        = 0;
	nowStroke		= 0;
    nowDist         = 0.f;
    nowAngle        = 0.f;
    nowPointForTime.set(0,0,0);
}


