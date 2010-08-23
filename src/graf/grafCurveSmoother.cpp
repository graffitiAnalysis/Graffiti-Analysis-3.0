#include "grafCurveSmoother.h"

bool sort_zpos_compare( timePt a, timePt b ) {
	
	return (a.pos.z < b.pos.z);
}

grafCurveSmoother::grafCurveSmoother()
{
    //ctor
}

grafCurveSmoother::~grafCurveSmoother()
{
    //dtor
}


void grafCurveSmoother::smoothTag( int resolution, grafTagMulti * src )
{
    
	tempTag.clear();
	tempTag.z_const = src->z_const;
	tempTag.minPtDist = src->minPtDist;
	
	for( int i = 0; i < src->myStrokes.size(); i++)
	{
		smoothStroke( resolution, &src->myStrokes[i] );
	}
	
	//cout << "temp strokes " << tempTag.myStrokes.size() << endl;
	
	tempTag.cleanUpLastStroke();
	src->myStrokes.clear();//clear(false);
	
	for( int i = 0; i < tempTag.myStrokes.size(); i++)
	{
		//cout << "temp strokes pts " << tempTag.myStrokes[i].pts.size() << endl;
		src->myStrokes.push_back( tempTag.myStrokes[i]);
		//smoothStroke( resolution, &src->myStrokes[i] );
	}
	
	
}


void grafCurveSmoother::smoothStroke(int resolution, strokes * src)
{
	if( src->pts.size() < 4 ){
		for( int j = 0; j < 4; j++)
			tempTag.addNewPoint(src->pts[j].pos,src->pts[j].dist,src->pts[j].angle,src->pts[j].time,false);
		return;
	}
	
	
	
	//for( int j = 0; j < 4; j++)
	//	tempTag.addNewPoint(src->pts[j].pos,src->pts[j].dist,src->pts[j].angle,src->pts[j].time,false);
	
	
	for( int j = 4;j < src->pts.size(); j+=1)
    {
        int startPos = (int)j- 3;
		
		float x0 = src->pts[startPos + 0].pos.x;
		float y0 = src->pts[startPos + 0].pos.y;
		float z0 = src->pts[startPos + 0].time;
		float a0 = src->pts[startPos + 0].dist;
		
		float x1 = src->pts[startPos + 1].pos.x;
		float y1 = src->pts[startPos + 1].pos.y;
		float z1 = src->pts[startPos + 1].time;
		float a1 = src->pts[startPos + 1].dist;
		
		float x2 = src->pts[startPos + 2].pos.x;
		float y2 = src->pts[startPos + 2].pos.y;
		float z2 = src->pts[startPos + 2].time;
		float a2 = src->pts[startPos + 2].dist;
		
		float x3 = src->pts[startPos + 3].pos.x;
		float y3 = src->pts[startPos + 3].pos.y;
		float z3 = src->pts[startPos + 3].time;
		float a3 = src->pts[startPos + 3].dist;
		
		float t,t2,t3;
		float x,y,z,a;
		
		for (int i = 1; i < resolution; i++){
			
			t 	=  (float)i / (float)(resolution-1);
			t2 	= t * t;
			t3 	= t2 * t;
			
			x = 0.5f * ( ( 2.0f * x1 ) +
						( -x0 + x2 ) * t +
						( 2.0f * x0 - 5.0f * x1 + 4 * x2 - x3 ) * t2 +
						( -x0 + 3.0f * x1 - 3.0f * x2 + x3 ) * t3 );
			
			y = 0.5f * ( ( 2.0f * y1 ) +
						( -y0 + y2 ) * t +
						( 2.0f * y0 - 5.0f * y1 + 4 * y2 - y3 ) * t2 +
						( -y0 + 3.0f * y1 - 3.0f * y2 + y3 ) * t3 );
			
			z = 0.5f * ( ( 2.0f * z1 ) +
						( -z0 + z2 ) * t +
						( 2.0f * z0 - 5.0f * z1 + 4 * z2 - z3 ) * t2 +
						( -z0 + 3.0f * z1 - 3.0f * z2 + z3 ) * t3 );
			
			a = 0.5f * ( ( 2.0f * a1 ) +
						( -a0 + a2 ) * t +
						( 2.0f * a0 - 5.0f * a1 + 4 * a2 - a3 ) * t2 +
						( -a0 + 3.0f * a1 - 3.0f * a2 + a3 ) * t3 );
			
			timePt newPoint;
			newPoint.pos.x = x;//*src->width;
			newPoint.pos.y = y;//*src->width;
			newPoint.pos.z = 0;
			newPoint.time = z;
			
			tempTag.addNewPoint(newPoint.pos,a,-1,z,false);
		}
		
    }
	
    /*
	 timePt newPoint;
    newPoint.x = src->pts[src->pts.size()-1].x;//*src->width;
    newPoint.y = src->pts[src->pts.size()-1].y;//*src->width;
    newPoint.z = 0;
		
    dst->rotation = src->rotation;
    dst->position = src->position;
    dst->z_const = src->z_const;
	*/
	int stk = tempTag.myStrokes.size()-1;
	sort( tempTag.myStrokes[stk].pts.begin(),tempTag.myStrokes[stk].pts.end(), sort_zpos_compare );
	tempTag.nextStroke();
	
	
}


