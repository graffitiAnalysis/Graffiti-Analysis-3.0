#pragma once
#include "ofMain.h"
#include "Box2D.h"

class ofxBox2dRender : public b2DebugDraw {
	
public:
	
	float scaleFactor;
	
	void setScale(float f) {
		scaleFactor = f;
	}
	
	void DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) {
		glColor3f(color.r, color.g, color.b);
		glBegin(GL_LINE_LOOP);
		for (int32 i = 0; i < vertexCount; ++i)
		{
			glVertex2f(vertices[i].x*OFX_BOX2D_SCALE, vertices[i].y*OFX_BOX2D_SCALE);
		}
		glEnd();
	}
	void DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) {
		
		ofSetColor(0xffffff);
		ofBeginShape();
		for(int i=0; i<vertexCount; ++i) {
			ofVertex(vertices[i].x*OFX_BOX2D_SCALE, vertices[i].y*OFX_BOX2D_SCALE);
		}
		ofEndShape();
		
		/*glEnable(GL_BLEND);
		glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glColor4f(0.5f * color.r, 0.5f * color.g, 0.5f * color.b, 0.5f);
		glBegin(GL_TRIANGLE_FAN);
		for(int32 i = 0; i < vertexCount; ++i) {
			glVertex2f(vertices[i].x*scaleFactor, vertices[i].y*scaleFactor);
		}
		glEnd();
		glDisable(GL_BLEND);
		
		glColor4f(color.r, color.g, color.b, 1.0f);
		glBegin(GL_LINE_LOOP);
		for (int32 i = 0; i < vertexCount; ++i) {
			glVertex2f(vertices[i].x*scaleFactor, vertices[i].y*scaleFactor);
		}
		glEnd();*/
	
	}
	void DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color) {
		const float32 k_segments = 16.0f;
		const float32 k_increment = 2.0f * b2_pi / k_segments;
		float32 theta = 0.0f;
		glColor3f(color.r, color.g, color.b);
		glBegin(GL_LINE_LOOP);
		for (int32 i = 0; i < k_segments; ++i)
		{
			b2Vec2 v = center + radius * b2Vec2(cosf(theta), sinf(theta));
			glVertex2f(v.x, v.y);
			theta += k_increment;
		}
		glEnd();
	}
	void DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color) {
		const float32 k_segments    = 16.0f;
		const float32 k_increment   = 2.0f * b2_pi / k_segments;
		float32 theta			    = 0.0f;
		float rad = (radius*scaleFactor);
		glEnable(GL_BLEND);
		glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		ofSetColor(255, 255, 255, 200);
		ofFill();
		ofCircle(center.x*scaleFactor, center.y*scaleFactor, rad);
		glEnd();
		
	}
	void DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color) {
		ofSetColor(255, 255, 255, 200);
		glBegin(GL_LINES);
		glVertex2f(p1.x*OFX_BOX2D_SCALE, p1.y*OFX_BOX2D_SCALE);
		glVertex2f(p2.x*OFX_BOX2D_SCALE, p2.y*OFX_BOX2D_SCALE);
		glEnd();
	}
	void DrawXForm(const b2XForm& xf) {
	}
	void DrawPoint(const b2Vec2& p, float32 size, const b2Color& color) {
	}
	void DrawString(int x, int y, const char* string, ...) {
	}
	void DrawAABB(b2AABB* aabb, const b2Color& color) {
		glColor3f(color.r, color.g, color.b);
		glBegin(GL_LINE_LOOP);
		glVertex2f(aabb->lowerBound.x, aabb->lowerBound.y);
		glVertex2f(aabb->upperBound.x, aabb->lowerBound.y);
		glVertex2f(aabb->upperBound.x, aabb->upperBound.y);
		glVertex2f(aabb->lowerBound.x, aabb->upperBound.y);
		glEnd();
	}
	
};