#ifndef GRAFCURVESMOOTHER_H
#define GRAFCURVESMOOTHER_H

#include "grafTagMulti.h"

class grafCurveSmoother
{
    public:
        grafCurveSmoother();
        virtual ~grafCurveSmoother();

        void smoothTag( int resolution, grafTagMulti * src);
	


    protected:
		
		void smoothStroke(int resolution, strokes * stroke);
		
		grafTagMulti tempTag;
	
	private:
};

#endif // GRAFCURVESMOOTHER_H
