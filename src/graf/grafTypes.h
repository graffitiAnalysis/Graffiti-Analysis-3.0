#ifndef GRAFTYPES_H
#define GRAFTYPES_H

#include "ofMain.h"

/*class timePt : public ofPoint{
	
	public:
	
	timePt(){
		x = 0;
        y = 0;
		z = 0;
		time = 0;
		dist=0;
		angle=0;
		ptType=0;
	};
	virtual ~timePt(){};
	
	
	
	//ofPoint		pos;
	float		time;
	float       dist;
	float       angle;
	int			ptType;
};
*/
typedef struct {

	ofPoint		pos;
	float		time;
	float       dist;
	float       angle;
	int			ptType;

}timePt;

typedef struct {
	
	vector<timePt> pts;
	vector<timePt> pts_orig;
		
} strokes;


#endif // GRAFTYPES_H
