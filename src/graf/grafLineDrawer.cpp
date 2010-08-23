#include "grafLineDrawer.h"


grafLineDrawer::grafLineDrawer()
{
    blendPct		= .5;
	strokeVal		= 1;
	globalAlpha		= .90f;
	lineScale		= .05;
	bUseFadeInOut = true;
	outlineDist = .0001;
}

grafLineDrawer::~grafLineDrawer()
{
}


void grafLineDrawer::setup(vector<timePt> pts, float minLen, float maxLen, float blendPct, int start, int end, bool bFadeIn, bool bFadeOut)
{
	
	if( pts.size() < 1 ) return;
    if( start == -1 )   start	= 0;
    if( end   == -1 )   end		= pts.size();
	
    
	// reset me
	pts_l.clear();
	pts_r.clear();
	
	pts_lo.clear();
	pts_ro.clear();
	
	pts_lout.clear();
	pts_rout.clear();
	
	alphas.clear();
		
	// starting length of 0
	float len			= 0;
   
    
	// init prev vals for blending
	ofxVec2f	ppPrev = ofxVec2f(1,1);
	bool bSwap = false;
	
	for( int i = start; i < end; i++)
    {
        // calculate thickness of the stroke and limit mimimum
		float pct = ( pts[i].dist / maxLen );
		pct = powf(1-pct,1.5);
		
		len = (lineScale * pct) * getAlphaForLinePosition(i,(pts.size()));
		if( len < minLen ) len = minLen;
		
		float deltaA = (i==0) ? fabs( pts[i+1].angle - pts[i].angle ) : fabs( pts[i].angle - pts[i-1].angle );
        if( deltaA > (HALF_PI) && TWO_PI-deltaA > HALF_PI ) bSwap = !bSwap;
		
		//if( !bSwap ) calculatePoint(pts[i], pts[i].pos.z,len, pts[i].angle, pts_l, pts_r);
		//else calculatePoint(pts[i], pts[i].pos.z,len, pts[i].angle, pts_r, pts_l);
		//len = MAX(len,.0001);
		
		if( !bSwap )
		{
			//calculatePoint(pts[i], pts[i].pos.z,len, pts[i].angle, pts_l, pts_r);
			pts_l.push_back( ofPoint( pts[i].pos.x-( len*sin(pts[i].angle) ),pts[i].pos.y+( len*cos(pts[i].angle) ), pts[i].pos.z) );
			pts_r.push_back(ofPoint( pts[i].pos.x+( len*sin(pts[i].angle) ),pts[i].pos.y-( len*cos(pts[i].angle) ), pts[i].pos.z) );
			
			float oLen = len+outlineDist;
			pts_lout.push_back( ofPoint( pts[i].pos.x-( oLen*sin(pts[i].angle) ),pts[i].pos.y+( oLen*cos(pts[i].angle) ), pts[i].pos.z) );
			pts_rout.push_back(ofPoint( pts[i].pos.x+( oLen*sin(pts[i].angle) ),pts[i].pos.y-( oLen*cos(pts[i].angle) ), pts[i].pos.z) );
			
		}
		else 
		{
			//calculatePoint(pts[i], pts[i].pos.z,len, pts[i].angle, pts_r, pts_l);
			pts_r.push_back( ofPoint( pts[i].pos.x-( len*sin(pts[i].angle) ),pts[i].pos.y+( len*cos(pts[i].angle) ), pts[i].pos.z) );
			pts_l.push_back(ofPoint( pts[i].pos.x+( len*sin(pts[i].angle) ),pts[i].pos.y-( len*cos(pts[i].angle) ), pts[i].pos.z) );
			
			float oLen = len+outlineDist;
			pts_rout.push_back( ofPoint( pts[i].pos.x-( oLen*sin(pts[i].angle) ),pts[i].pos.y+( oLen*cos(pts[i].angle) ), pts[i].pos.z) );
			pts_lout.push_back(ofPoint( pts[i].pos.x+( oLen*sin(pts[i].angle) ),pts[i].pos.y-( oLen*cos(pts[i].angle) ), pts[i].pos.z) );
			
		}
		
		float distAlpha = MAX( globalAlpha, ( 1.2* ( 1 - (pts[i].dist/maxLen) ) ) );
		float fadeAlpha = getAlphaForLinePosition(i,(pts.size()));
		
		if(bUseFadeInOut)	alphas.push_back(distAlpha*fadeAlpha);
		else				alphas.push_back(distAlpha);
		
		pts_ro.push_back(pts_r[pts_r.size()-1]);
		pts_lo.push_back(pts_l[pts_l.size()-1]);
		
		ofxVec2f vec = ofxVec2f(pts_r[pts_r.size()-1].x-pts_l[pts_l.size()-1].x,pts_r[pts_r.size()-1].y-pts_l[pts_l.size()-1].y);
		vec = vec.normalize();
		vecs.push_back(vec);
	}
	

}

void grafLineDrawer::setupUniform(vector<timePt> pts, float lineDist, int start, int end)
{
	
	if( pts.size() < 1 ) return;
    if( start == -1 )   start	= 0;
    if( end   == -1 )   end		= pts.size();
	
    
	// reset me
	pts_l.clear();
	pts_r.clear();
	
	pts_lo.clear();
	pts_ro.clear();
	
	pts_lout.clear();
	pts_rout.clear();
	
	alphas.clear();
	
	// starting length of 0
	float len			= 0;
	
    
	// init prev vals for blending
	ofxVec2f	ppPrev = ofxVec2f(1,1);
	bool bSwap = false;
	
	for( int i = start; i < end; i++)
    {
        float fadeAlpha = getAlphaForLinePosition(i,(pts.size()));
		len = lineDist*fadeAlpha;
		
		float deltaA = (i==0) ? fabs( pts[i+1].angle - pts[i].angle ) : fabs( pts[i].angle - pts[i-1].angle );
        if( deltaA > (HALF_PI) && TWO_PI-deltaA > HALF_PI ) bSwap = !bSwap;
		
		 //len = MAX(len,.0001);
		 
		 if( !bSwap )
		{
			//calculatePoint(pts[i], pts[i].pos.z,len, pts[i].angle, pts_l, pts_r);
			pts_l.push_back( ofPoint( pts[i].pos.x-( len*sin(pts[i].angle) ),pts[i].pos.y+( len*cos(pts[i].angle) ), pts[i].pos.z) );
			pts_r.push_back(ofPoint( pts[i].pos.x+( len*sin(pts[i].angle) ),pts[i].pos.y-( len*cos(pts[i].angle) ), pts[i].pos.z) );
			
			float oLen = len+outlineDist;
			pts_lout.push_back( ofPoint( pts[i].pos.x-( oLen*sin(pts[i].angle) ),pts[i].pos.y+( oLen*cos(pts[i].angle) ), pts[i].pos.z) );
			pts_rout.push_back(ofPoint( pts[i].pos.x+( oLen*sin(pts[i].angle) ),pts[i].pos.y-( oLen*cos(pts[i].angle) ), pts[i].pos.z) );
			
		}
		else 
		{
			//calculatePoint(pts[i], pts[i].pos.z,len, pts[i].angle, pts_r, pts_l);
			pts_r.push_back( ofPoint( pts[i].pos.x-( len*sin(pts[i].angle) ),pts[i].pos.y+( len*cos(pts[i].angle) ), pts[i].pos.z) );
			pts_l.push_back(ofPoint( pts[i].pos.x+( len*sin(pts[i].angle) ),pts[i].pos.y-( len*cos(pts[i].angle) ), pts[i].pos.z) );
			
			float oLen = len+outlineDist;
			pts_rout.push_back( ofPoint( pts[i].pos.x-( oLen*sin(pts[i].angle) ),pts[i].pos.y+( oLen*cos(pts[i].angle) ), pts[i].pos.z) );
			pts_lout.push_back(ofPoint( pts[i].pos.x+( oLen*sin(pts[i].angle) ),pts[i].pos.y-( oLen*cos(pts[i].angle) ), pts[i].pos.z) );
			
		}
		
		float distAlpha = globalAlpha;//MAX( globalAlpha, ( 1.2* ( 1 - (pts[i].dist/maxLen) ) ) );
		
		
		if(bUseFadeInOut)	alphas.push_back(distAlpha*fadeAlpha);
		else				alphas.push_back(distAlpha);
	}
	
	//average(.25);
	
}

void grafLineDrawer::calculatePoint(timePt pt, float time_num, float dist, float angle, vector<ofPoint>&left, vector<ofPoint>&right)
{
    dist = MAX(dist,.0001);
	
    float left_x,left_y,right_x,right_y;
	
    left_x 	= pt.pos.x-(sin(pt.angle)*dist);
    left_y 	= pt.pos.y+(cos(pt.angle)*dist);
	right_x = pt.pos.x+(sin(pt.angle)*dist);
    right_y = pt.pos.y-(cos(pt.angle)*dist);
	
    left.push_back( ofPoint(left_x,left_y,time_num) );
    right.push_back( ofPoint(right_x,right_y,time_num) );
}

void grafLineDrawer::draw( int lastPoint, float alpha, int startPoint )
{
	//cout << "lastp " << lastPoint << " " << pts_r.size() << endl;
	
	if( lastPoint == -1 ) lastPoint = pts_r.size();
	if( startPoint == -1 ) startPoint = 1;
	
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	
	//cout << "lastPoint " << lastPoint << endl;
	float linePoints[6];
	
    glLineWidth(2);
	
	float colorOut[8] = {0,0,0,1,0,0,0,1};
	
	for(int i = startPoint; i < lastPoint; i++)
    {
		
		float gVal = strokeVal*alpha;
		
        float color[4*4];
		
		float points[3*4];
		
		int		ar  = 0;
		int		cr  = 0;
		
        points[ar]   = pts_l[i-1].x;
        points[++ar] = pts_l[i-1].y;
        points[++ar] = pts_l[i-1].z;
        color[cr]	 = gVal;
		color[++cr]  = gVal;
		color[++cr]  = gVal;
		color[++cr]  = globalAlpha* alphas[i-1]*alpha;
		
        points[++ar] = pts_r[i-1].x;
        points[++ar] = pts_r[i-1].y;
        points[++ar] = pts_r[i-1].z;
        color[++cr] = gVal;
		color[++cr] = gVal;
		color[++cr] = gVal;
		color[++cr] = globalAlpha* alphas[i-1]*alpha;
		
       
        // add prev point
        points[++ar] = pts_r[i].x;
        points[++ar] = pts_r[i].y;
        points[++ar] = pts_r[i].z;
		color[++cr]  = gVal;
		color[++cr]  = gVal;
		color[++cr]  = gVal;
		color[++cr]  =globalAlpha* alphas[i]*alpha;
		
        points[++ar]  = pts_l[i].x;
        points[++ar]  = pts_l[i].y;
        points[++ar]  = pts_l[i].z;
		color[++cr]  = gVal;
		color[++cr]  = gVal;
		color[++cr]  = gVal;
		color[++cr]  = globalAlpha* alphas[i]*alpha;
		
        glVertexPointer(3, GL_FLOAT, 0, points);
		glColorPointer( 4, GL_FLOAT, 0, color);
		
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		
		linePoints[0] = pts_lout[i-1].x;
        linePoints[1] = pts_lout[i-1].y;
        linePoints[2] = pts_lout[i-1].z;
		
        linePoints[3] = pts_lout[i].x;
        linePoints[4] = pts_lout[i].y;
        linePoints[5] = pts_lout[i].z;
		
		float pct = alphas[i];
		colorOut[3] = .9*alpha*powf(pct,1.5);
		colorOut[7] = .9*alpha*powf(pct,1.5);
		
		glColorPointer( 4, GL_FLOAT, 0, colorOut);
		glVertexPointer(3, GL_FLOAT, 0, &linePoints[0]);
        glDrawArrays(GL_LINES, 0, 2);
		
		linePoints[0] = pts_rout[i-1].x;
        linePoints[1] = pts_rout[i-1].y;
        linePoints[2] = pts_lout[i-1].z;
		
        linePoints[3] = pts_rout[i].x;
        linePoints[4] = pts_rout[i].y;
        linePoints[5] = pts_rout[i].z;
		
		//pct = alphas[i];
       // glColor4f(0,0,0,.9*alpha*powf(pct,1.5));
		
        glColorPointer( 4, GL_FLOAT, 0, colorOut);
		glVertexPointer(3, GL_FLOAT, 0, &linePoints[0]);
        glDrawArrays(GL_LINES, 0, 2);

    }
	
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	
}

void grafLineDrawer::drawOutline( int lastPoint, float alpha, float lineWidth, int startPoint  )
{
	if( startPoint == -1 ) startPoint = 1;
	if( lastPoint == -1 ) lastPoint = pts_r.size();
	
	float zAdd = .75;//.25f;
	
	float linePoints[6];
	
    glLineWidth(lineWidth);
    glColor4f(1,0,0,1);
	
	glEnableClientState(GL_VERTEX_ARRAY);
	
	for( int i=startPoint; i<lastPoint-1; i++)
    {
        linePoints[0] = pts_l[i-1].x;
        linePoints[1] = pts_l[i-1].y;
        linePoints[2] = pts_l[i-1].z+zAdd;
		
        linePoints[3] = pts_l[i].x;
        linePoints[4] = pts_l[i].y;
        linePoints[5] = pts_l[i].z+zAdd;
		
        float pct = alphas[i];
		//getAlphaForLinePosition(i,PR->pts.size());
        glColor4f(0,0,0,.9*alpha*powf(pct,1.5));
		
        
        glVertexPointer(3, GL_FLOAT, 0, &linePoints[0]);
        glDrawArrays(GL_LINES, 0, 2);
		
    }
	
    for( int i=1; i<lastPoint-1; i++)
    {
        linePoints[0] = pts_r[i-1].x;
        linePoints[1] = pts_r[i-1].y;
        linePoints[2] = pts_r[i-1].z+zAdd;
		
        linePoints[3] = pts_r[i].x;
        linePoints[4] = pts_r[i].y;
        linePoints[5] = pts_r[i].z+zAdd;
		
        float pct = alphas[i];//getAlphaForLinePosition(i,PR->pts.size());
        glColor4f(0,0,0,.9*alpha*powf(pct,1.5));
        glVertexPointer(3, GL_FLOAT, 0, &linePoints[0]);
        glDrawArrays(GL_LINES, 0, 2);
		
    }
	
	glDisableClientState(GL_VERTEX_ARRAY);
	
    glLineWidth(1.f);
	
}

void grafLineDrawer::average(float pct)
{
    float pctSis = (1-pct) * .5f;
	
    for( int i = 1; i < pts_l.size(); i++)
    {
        if( i >= 2  )
		{
			pts_l[i-1].x = (pctSis * pts_l[i-2].x) + (pct * pts_l[i-1].x) + (pctSis * pts_l[i].x);
			pts_l[i-1].y = (pctSis * pts_l[i-2].y) + (pct * pts_l[i-1].y) + (pctSis * pts_l[i].y);
			pts_lout[i-1].x = (pctSis * pts_lout[i-2].x) + (pct * pts_lout[i-1].x) + (pctSis * pts_lout[i].x);
			pts_lout[i-1].y = (pctSis * pts_lout[i-2].y) + (pct * pts_lout[i-1].y) + (pctSis * pts_lout[i].y);
		}else if( i== 1 )
		{
			pts_l[i-1].x = (pctSis * pts_l[i+1].x) + (pct * pts_l[i-1].x) + (pctSis * pts_l[i].x);
			pts_l[i-1].y = (pctSis * pts_l[i+1].y) + (pct * pts_l[i-1].y) + (pctSis * pts_l[i].y);
			pts_lout[i-1].x = (pctSis * pts_lout[i+1].x) + (pct * pts_lout[i-1].x) + (pctSis * pts_lout[i].x);
			pts_lout[i-1].y = (pctSis * pts_lout[i+1].y) + (pct * pts_lout[i-1].y) + (pctSis * pts_lout[i].y);
		}
		
		
		if( i == pts_l.size()-1 )
		{
			pts_l[i].x = (pctSis * pts_l[i-2].x) + (pctSis * pts_l[i-1].x) + (pct * pts_l[i].x);
			pts_l[i].y = (pctSis * pts_l[i-2].y) + (pctSis * pts_l[i-1].y) + (pct * pts_l[i].y);
			pts_lout[i].x = (pctSis * pts_lout[i-2].x) + (pctSis * pts_lout[i-1].x) + (pct * pts_lout[i].x);
			pts_lout[i].y = (pctSis * pts_lout[i-2].y) + (pctSis * pts_lout[i-1].y) + (pct * pts_lout[i].y);
		}
    }
	
	
    for( int i = 1; i < pts_r.size(); i++)
    {
        if( i >= 2 )
		{
			pts_r[i-1].x = (pctSis * pts_r[i-2].x) + (pct * pts_r[i-1].x) + (pctSis * pts_r[i].x);
			pts_r[i-1].y = (pctSis * pts_r[i-2].y) + (pct * pts_r[i-1].y) + (pctSis * pts_r[i].y);
			
			pts_rout[i-1].x = (pctSis * pts_rout[i-2].x) + (pct * pts_rout[i-1].x) + (pctSis * pts_rout[i].x);
			pts_rout[i-1].y = (pctSis * pts_rout[i-2].y) + (pct * pts_rout[i-1].y) + (pctSis * pts_rout[i].y);
		}else if( i == 1 )
		{
			pts_r[i-1].x = (pctSis * pts_r[i+1].x) + (pct * pts_r[i-1].x) + (pctSis * pts_r[i].x);
			pts_r[i-1].y = (pctSis * pts_r[i+1].y) + (pct * pts_r[i-1].y) + (pctSis * pts_r[i].y);
			
			pts_rout[i-1].x = (pctSis * pts_rout[i+1].x) + (pct * pts_rout[i-1].x) + (pctSis * pts_rout[i].x);
			pts_rout[i-1].y = (pctSis * pts_rout[i+1].y) + (pct * pts_rout[i-1].y) + (pctSis * pts_rout[i].y);
		}
		
		if( i == pts_r.size()-1 )
		{
			pts_r[i].x = (pctSis * pts_r[i-2].x) + (pctSis * pts_r[i-1].x) + (pct * pts_r[i].x);
			pts_r[i].y = (pctSis * pts_r[i-2].y) + (pctSis * pts_r[i-1].y) + (pct * pts_r[i].y);
			
			pts_rout[i].x = (pctSis * pts_rout[i-2].x) + (pctSis * pts_rout[i-1].x) + (pct * pts_rout[i].x);
			pts_rout[i].y = (pctSis * pts_rout[i-2].y) + (pctSis * pts_rout[i-1].y) + (pct * pts_rout[i].y);
		}
    }
	
	for( int i = 0; i < pts_r.size(); i++)
	{
		pts_r[i].x = (.99 * pts_r[i].x) + (.01 * pts_l[i].x);
		pts_r[i].y = (.99 * pts_r[i].y) + (.01 * pts_l[i].y);
		
		pts_l[i].x = (.99 * pts_l[i].x) + (.01 * pts_r[i].x);
		pts_l[i].y = (.99 * pts_l[i].y) + (.01 * pts_r[i].y);
		
		pts_rout[i].x = (.99 * pts_rout[i].x) + (.01 * pts_lout[i].x);
		pts_rout[i].y = (.99 * pts_rout[i].y) + (.01 * pts_lout[i].y);
		
		pts_lout[i].x = (.99 * pts_lout[i].x) + (.01 * pts_rout[i].x);
		pts_lout[i].y = (.99 * pts_lout[i].y) + (.01 * pts_rout[i].y);
	}
	
}


float grafLineDrawer::getAlphaForLinePosition( int pos, int total )
{
    float pct = 1;
	
    int min = 10;
    int max = total - min;
	
    if (pos < min ) pct = pos / (float) (min-1);
    if (pos > max ) pct = 1 - ( ( pos-max )  / (float) (min-1) );
	
    return pct;
}



