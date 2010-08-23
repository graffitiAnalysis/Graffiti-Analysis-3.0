#ifndef VECTOR_FIELD_H
#define VECTOR_FIELD_H


#include "ofMain.h"

#define WIDTH_FIELD				144
#define HEIGHT_FIELD			108
#define FIELD_SIZE		   		(HEIGHT_FIELD * WIDTH_FIELD)

class vectorField{


	public :

		vectorField();
		void clearField();
		void drawField(float width, float height);
		float * getVfromPos(float xpos, float ypos);
		void fadeField (float fadeAmount); // 0 = complete, 1 = none

		void update(ofPoint pt, ofPoint vel, float scale, float w, float h);

		float getXVel(float xpos, float ypos);
		float getYVel(float xpos, float ypos);
		ofPoint getVel(float xpos, float ypos);
		inline ofPoint getVel(int pos){ return ofPoint(fx[pos],fy[pos]) ; }


		float fx[WIDTH_FIELD * HEIGHT_FIELD];
		float fy[WIDTH_FIELD * HEIGHT_FIELD];


        bool bReset;

        protected:
            void updateField(float x, float y, float vel, float w, float h);
            void addToFieldAnSpecialCircleShape(float x, float y, float fx, float fy, float radius, float strength);



};

#endif
