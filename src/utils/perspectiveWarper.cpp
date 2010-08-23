/*
 *  perspectiveWarper.cpp
 *  GA_Interactive
 *
 *  Created by Chris on 6/11/10.
 *  Copyright 2010 csugrue. All rights reserved.
 *
 */

#include "perspectiveWarper.h"
#include "ofxVec2f.h"

static const float DISTANCE_THRESH = 100.0f;
static const float DISTANCE_THRESH_SQ = DISTANCE_THRESH*DISTANCE_THRESH;

PerspectiveWarper::PerspectiveWarper()
{
	bEditing = false;
	mouse_index = -1;
	invert = false;
	
	ofAddListener(ofEvents.mouseMoved, this, &PerspectiveWarper::mouseMoved);
	ofAddListener(ofEvents.mouseDragged, this, &PerspectiveWarper::mouseDragged);
	
	editOffset.set(0,0);
	editScale = 1;
}

PerspectiveWarper::~PerspectiveWarper()
{
	cvReleaseMat( &warp_translate );
	cvReleaseMat( &warp_translate_inv );
	
	ofRemoveListener(ofEvents.mouseMoved, this, &PerspectiveWarper::mouseMoved);
	ofRemoveListener(ofEvents.mouseDragged, this, &PerspectiveWarper::mouseDragged);
	
	delete [] u;
	delete [] v;
}

void PerspectiveWarper::initWarp( float _orig_width, float _orig_height, int nx, int ny )
{
	num_x_sections = nx;
	num_y_sections = ny;
	
	// allocate array num_y_sections+1 x num_x_sections+1
	u = new float[ (num_y_sections+1)*(num_x_sections+1)];
	v = new float[ (num_y_sections+1)*(num_x_sections+1)];
	/*u.resize( num_y_sections + 1 );
	v.resize( num_y_sections + 1 );
	for ( int i=0; i<num_y_sections + 1; i++ )
	{
		u[i].resize( num_x_sections+1 );
		v[i].resize( num_x_sections+1 );
	}*/
	
	initWarp(_orig_width, _orig_height);
	
}

void PerspectiveWarper::initWarp( float _orig_width, float _orig_height )
{
	orig_width = _orig_width;
	orig_height = _orig_height;
	warp_translate = cvCreateMat( 3, 3, CV_32FC1 );
	warp_translate_inv = cvCreateMat( 3, 3, CV_32FC1 );
	dest_points[0].set( 0.0f, 0.0f );
	dest_points[1].set( orig_width, 0.0f );
	dest_points[2].set( orig_width, orig_height );
	dest_points[3].set( 0.0f, orig_height );
	
	for( int i = 0; i < 4; i++)
		points[i] = dest_points[i];
	
	warp_transform_storage_in = cvCreateMat( 1,1, CV_32FC2 );
	warp_transform_storage_out = cvCreateMat( 1,1, CV_32FC2 );
	
	// setup points array
	//resetPoints();
	
}

void PerspectiveWarper::shutdownWarp()
{
	cvReleaseMat( &warp_translate );
	cvReleaseMat( &warp_translate_inv );
	cvReleaseMat( &warp_transform_storage_in );
	cvReleaseMat( &warp_transform_storage_out );
}

void PerspectiveWarper::recalculateWarp( ofPoint src[4], ofPoint dst[4] )
{
	for ( int i=0; i<4; i++ )
	{
		points[i] = src[i];
		dest_points[i] = dst[i];
	}
	recalculateWarp();
}

void PerspectiveWarper::recalculateWarp( float new_points[] )
{
	for ( int i=0; i<4; i++ )
	{
		points[i].x = new_points[2*i];
		points[i].y = new_points[2*i+1];
	}
	recalculateWarp();
}

void PerspectiveWarper::recalculateWarp()
{
	//printf("%f %f\n", orig_width, orig_height );
	// compute matrix for perspectival warping (homography)
	CvPoint2D32f cvsrc[4];
	CvPoint2D32f cvdst[4];
	cvSetZero( warp_translate );
	for (int i = 0; i < 4; i++ ) {
		cvsrc[i].x = points[i].x;
		cvsrc[i].y = points[i].y;
		cvdst[i].x = dest_points[i].x;
		cvdst[i].y = dest_points[i].y;
	}
	if ( invert )
	{
		cvWarpPerspectiveQMatrix( cvdst, cvsrc, warp_translate );  // calculate homography
		cvWarpPerspectiveQMatrix( cvsrc, cvdst, warp_translate_inv );
	}
	else
	{
		cvWarpPerspectiveQMatrix( cvsrc, cvdst, warp_translate );  // calculate homography
		cvWarpPerspectiveQMatrix( cvdst, cvsrc, warp_translate_inv );
	}
	
	
	if( num_x_sections > 0 && num_y_sections > 0 ) reCalculateUV();
	
}

void PerspectiveWarper::dumpWarpMatrix()
{
	for ( int i=0; i<3; i++)
		printf("%f %f %f\n",
			   cvmGet( warp_translate, i, 0 ),
			   cvmGet( warp_translate, i, 1 ),
			   cvmGet( warp_translate, i, 2 ) );
}

ofPoint PerspectiveWarper::warpPoint( const ofPoint& p )
{
	// warp p by the warp_translate matrix
	
	cvSet2D(warp_transform_storage_in, 0, 0, cvScalar( p.x, p.y ) );
	cvPerspectiveTransform( warp_transform_storage_in, warp_transform_storage_out, warp_translate );
	
	CvScalar result = cvGet2D( warp_transform_storage_out, 0, 0 );
	
	return ofPoint( result.val[0], result.val[1] );
}

ofPoint PerspectiveWarper::warpPointInv( const ofPoint& p )
{
	// warp p by the warp_translate matrix
	
	cvSet2D(warp_transform_storage_in, 0, 0, cvScalar( p.x, p.y ) );
	cvPerspectiveTransform( warp_transform_storage_in, warp_transform_storage_out, warp_translate_inv );
	
	CvScalar result = cvGet2D( warp_transform_storage_out, 0, 0 );
	
	return ofPoint( result.val[0], result.val[1] );
}

void PerspectiveWarper::drawEditInterface( float x, float y, float scale )
{
	editOffset.set(x,y);
	editScale = scale;
	
	ofNoFill();
	
	ofRect(x,y,orig_width*scale,orig_height*scale);
	
	for ( int i=0; i<4; i++ )
	{
		ofSetColor( 0x77FF77 );
		ofLine( 
			   x+(points[i].x*scale), y+(points[i].y*scale),
			   x+(points[(i+1)%4].x*scale), y+(points[(i+1)%4].y*scale) );
		
		if ( mouse_index == i ) ofSetColor( 0x7777ff );
		ofCircle( x+(points[i].x*scale), y+(points[i].y*scale), 10*scale );
	}
	
	ofSetColor(0xffffff);
}


void PerspectiveWarper::mouseMoved(ofMouseEventArgs& event)
{
	if ( !bEditing ) return;
	
	float x = event.x;
	float y = event.y;
	
	ofxVec2f mouse_pos( (x/editScale)-editOffset.x, (y/editScale)-editOffset.y );
	
	int best_distance = (mouse_pos - points[0]).lengthSquared();
	int best_index = 0;
	for ( int i=1; i<4; i++ )
	{
		int distance = (mouse_pos - points[i]).lengthSquared();
		if ( distance < best_distance )
		{
			best_distance = distance;
			best_index = i;
		}
	}
	
	if ( best_distance < DISTANCE_THRESH_SQ )
		mouse_index = best_index;
	else
		mouse_index = -1;
	
	
}

void PerspectiveWarper::mouseDragged(ofMouseEventArgs& event)
{
	if ( !bEditing ) return;
	
	float x = event.x;
	float y = event.y;
	
	int dx = x-editOffset.x;//px;
	int dy = y-editOffset.y;//py;
	//px = x;
	//py = y;
	
	if ( mouse_index != -1 )
	{
		points[mouse_index].x = dx*(1/editScale);
		points[mouse_index].y = dy*(1/editScale);
		recalculateWarp();
	}
	
	
}


void PerspectiveWarper::saveToXml( string filename )
{
	ofxXmlSettings xml;
	
	xml.addTag("warper");
	xml.pushTag("warper");
	
	for ( int i=0; i<4; i++ )
	{
		int index = xml.addTag( "point" );
		xml.pushTag( "point", index );
		xml.addValue( "x", points[i].x );
		xml.addValue( "y", points[i].y );
		xml.popTag();
	}
	
	xml.saveFile(filename);
}

void PerspectiveWarper::loadFromXml(string filename )
{
	ofxXmlSettings xml;
	xml.loadFile(filename);
	
	xml.pushTag("warper");
	
	int count = xml.getNumTags( "point" );
	assert( count == 0 || count == 4 );
	if( count == 0 )
	{
		resetPoints();
	}
	else
		for ( int i=0; i<4; i++ )
		{
			xml.pushTag( "point", i );
			points[i].x = xml.getValue( "x", 0.0f );
			points[i].y = xml.getValue( "y", 0.0f );
			xml.popTag();
		}
	
	xml.popTag();
	
	recalculateWarp();
}


void PerspectiveWarper::resetPoints()
{
	points[0].set( 0.5f, 0.5f );
	points[1].set( 100.0f, 0.5f );
	points[2].set( 100.0f, 100.0f );
	points[3].set( 0.5f, 100.0f );
}


void PerspectiveWarper::reCalculateUV()
{
	GLint px0 = 0;		// up to you to get the aspect ratio right
	GLint py0 = 0;
	GLint px1 = (GLint)orig_width;
	GLint py1 = (GLint)orig_height;
	
	// for rect mode center, let's do this:
	if (ofGetRectMode() == OF_RECTMODE_CENTER){
		px0 = (GLint)-orig_width/2;
		py0 = (GLint)-orig_height/2;
		px1 = (GLint)+orig_width/2;
		py1 = (GLint)+orig_height/2;
	}
	
	
	GLfloat x_step = (px1-px0)/(float)num_x_sections;
	GLfloat y_step = (py1-py0)/(float)num_y_sections;
	
	GLfloat curr_y = py0;
	
	for ( int i=0; i<num_y_sections+1; i++ )
	{
		GLfloat curr_x = px0;
		
		for ( int j=0; j<num_x_sections+1; j++ )
		{
			// warp
			ofPoint warped = warpPointInv( ofPoint( curr_x, curr_y ) );
			
			int p = i * (num_x_sections+1) + j;
			
			// store
			u[p] = warped.x;
			v[p] = warped.y;
			
			// step
			curr_x += x_step;
		}
		
		// and step
		curr_y += y_step;
	}
}


void PerspectiveWarper::drawUV( float x, float y, float scale )
{
	glLineWidth(1);
	
	glPushMatrix();
	glTranslatef(x,y,0);
	for ( int i=0; i<num_y_sections+1; i++ )
	{
		for ( int j=0; j<num_x_sections+1; j++ )
		{
			int p = i * (num_x_sections+1) + j;
			
			//ofRect(u[p]*scale,v[p]*scale,1*scale,1*scale);
			if(j < num_x_sections)
			{
				int p1 = i * (num_x_sections+1) + (j+1);
				ofLine(u[p]*scale,v[p]*scale,u[p1]*scale,v[p1]*scale);
			}
			
			if(i < num_y_sections)
			{
				int p1 = (i+1) * (num_x_sections+1) + j;
				ofLine(u[p]*scale,v[p]*scale,u[p1]*scale,v[p1]*scale);
			}
		}
	}
	glPopMatrix();
}
