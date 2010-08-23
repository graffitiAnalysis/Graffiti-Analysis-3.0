#include "vectorField.h"
//===============================================================
vectorField::vectorField(){

	clearField();
	bReset = false;


}

//===============================================================
void vectorField::clearField(){

	//set vector field = to 0
	for (int i = 0; i < FIELD_SIZE; i++){
		fx[i] = 0.0f;
		fy[i] = 0.0f;
	}
}

static float lastx = 0;
static float lasty = 0;

void  vectorField::update(ofPoint pt, ofPoint vel, float scale,  float w, float h)
{
    if( vel.x == 0 && vel.y == 0 )
    {
         lastx = pt.x*scale;
         lasty = pt.y*scale;
    }

    float tempVel =  5 * sqrt(vel.x * vel.x + vel.y * vel.y);
    updateField(pt.x*scale, pt.y*scale, tempVel,  w,  h);
}


//===============================================================
void vectorField::updateField(float x, float y, float vel, float w, float h)
{

    float pctx = x/w;
    float pcty = y/h;

    if (bReset)
    {
        lastx = x;
        lasty = y;
        bReset = false;
    }

    float vx = x - lastx;
    float vy = y - lasty;

    lastx = x;
    lasty = y;

    addToFieldAnSpecialCircleShape(pctx, pcty,vx, vy, 4.0f, 0.3f);

    //if(vel > 6) vel = 6;

	//addToFieldAnSpecialCircleShape(pctx, pcty,vx, vy, vel, 0.3f);

}
//===============================================================
void vectorField::fadeField (float fadeAmount){

	//apply fade to the vector field
	for (int i = 0; i < FIELD_SIZE; i++){
		fx[i] *= fadeAmount;
		fy[i] *= fadeAmount;
	}
}

//===============================================================
void vectorField::addToFieldAnSpecialCircleShape(float x, float y, float vx, float vy, float radius, float strength){

	//add a circle vector at mouse point

	//turn num from 1-0 into num between w-h
	int fieldPosX = (int)(x * (float)WIDTH_FIELD);
	int fieldPosY = (int)(y * (float)HEIGHT_FIELD);

	float r2 = radius * radius;
    int pos;
    float distance;
    float pct;
    float strongness;
    float unit_px;
    float unit_py;


	for (int i = 0; i < WIDTH_FIELD; i++){
		for (int j = 0; j < HEIGHT_FIELD; j++){

			float distanceSq = (fieldPosX-i)*(fieldPosX-i) + (fieldPosY-j)*(fieldPosY-j);

			if (distanceSq < r2 && distanceSq > 0){

				pos = j * WIDTH_FIELD + i;
                distance = sqrt(distanceSq);
				pct = 1.0f - (distance / radius);
				strongness = strength * pct;
				unit_px = (i - fieldPosX) / distance;
				unit_py = (j - fieldPosY) / distance;

				fx[pos] += vx * strongness;
				fy[pos] += vy * strongness;

			}

		}
	}

}

//===============================================================
void vectorField::drawField(float width, float height){

	float scalex = width / (float)WIDTH_FIELD;
	float scaley = height / (float)HEIGHT_FIELD;


	glColor3f(0.6,0.6,0.6);


	for (int i = 0; i < WIDTH_FIELD; i+=2){
		for (int j = 0; j < HEIGHT_FIELD; j+=2){

			// pos in array
			int pos = j * WIDTH_FIELD + i;
			// pos on screen
			float px = 	i * scalex;
			float py = 	j * scaley;
			float px2 = 	px + fx[pos];
			float py2 = 	py + fy[pos];

			glBegin(GL_POINTS);
				glVertex2f(px, py);
			glEnd();

			glBegin(GL_LINES);
			//glEnable(GL_BLEND);
			//glBlendFunc(.5, .5);
				glVertex2f(px, py);
				glVertex2f(px2, py2);
			//glDisable(GL_BLEND);
			glEnd();
		}
	}
}

//===============================================================
float * vectorField::getVfromPos(float xpos, float ypos){
	// this function takes in 2 nums between 1-0 and
	// finds its position in the VF and
	// returns values in the Vector field


	// xpos, ypos = 0-1
	// so coordinate stuff is ok...
	float vFromPos[2]; // pack x,y and send back


	//turn number between 1-0 into something within w - h
	int fieldPosX = (int)(xpos * (float)WIDTH_FIELD);
	int fieldPosY = (int)(ypos * (float)HEIGHT_FIELD);


	//cap values
	fieldPosX = max(0, min(fieldPosX, WIDTH_FIELD-1));
	fieldPosY = max(0, min(fieldPosY, HEIGHT_FIELD-1));


	// pos in array
	int pos = fieldPosY * WIDTH_FIELD + fieldPosX;



	// OK THIS COULD BE LINEAR, BILINEAR
	// OR NEAREST NEIGHBOR
	// FOR NOW, NEAREST NEIGHBOR
	// BUT THIS IS IMPORTANT STUFF

	vFromPos[0] = fx[pos];
	vFromPos[1] = fy[pos];

	return vFromPos;

}
//===============================================================
float vectorField::getXVel(float xpos, float ypos)
{
    int fieldPosX = (int)(xpos * (float)WIDTH_FIELD);
	int fieldPosY = (int)(ypos * (float)HEIGHT_FIELD);

	//cap values
	fieldPosX = max(0, min(fieldPosX, WIDTH_FIELD-1));
	fieldPosY = max(0, min(fieldPosY, HEIGHT_FIELD-1));

	// pos in array
	int pos = fieldPosY * WIDTH_FIELD + fieldPosX;

	return fx[pos];
}

float vectorField::getYVel(float xpos, float ypos)
{
    int fieldPosX = (int)(xpos * (float)WIDTH_FIELD);
	int fieldPosY = (int)(ypos * (float)HEIGHT_FIELD);

	//cap values
	fieldPosX = max(0, min(fieldPosX, WIDTH_FIELD-1));
	fieldPosY = max(0, min(fieldPosY, HEIGHT_FIELD-1));

	// pos in array
	int pos = fieldPosY * WIDTH_FIELD + fieldPosX;

	return fy[pos];
}
ofPoint vectorField::getVel(float xpos, float ypos)
{
    int fieldPosX = (int)(xpos * (float)WIDTH_FIELD);
	int fieldPosY = (int)(ypos * (float)HEIGHT_FIELD);

	if( fieldPosX < 0 || fieldPosX > WIDTH_FIELD-1 ||
        fieldPosY < 0 || fieldPosY > HEIGHT_FIELD-1
	)
	return ofPoint(0,0);

	//cap values
	fieldPosX = max(0, min(fieldPosX, WIDTH_FIELD-1));
	fieldPosY = max(0, min(fieldPosY, HEIGHT_FIELD-1));


	// pos in array
	int pos = fieldPosY * WIDTH_FIELD + fieldPosX;

	return ofPoint(fx[pos],fy[pos]);
}

//===============================================================
