#include "curve.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>

na=1;
num=1;
enum XY { X = 0, Y };
#define PI 3.141592
#define SF 1
#define NN 10
#define SIN na*(sin(num*PI*t))
#define SET_VECTOR2(V, V1, V2)          do { (V)[X] = (V1); (V)[Y] = (V2); } while (0)
#define COPY_PT(DST, SRC)               do { (DST)[X] = SRC[X]; (DST)[Y] = SRC[Y]; } while (0)
#define VECTOR2_X_SCALA_ADD(O, V, S)    do { O[X] += (S) * (V)[X]; O[Y] += (S) * (V)[Y]; } while (0)
#define DERIVATIVE_CHANG_ADD(Q,P0,P1,S)        do { Q[X] += (S) * ((P1)[X]-(P0)[X]);Q[Y] += (S) * ((P1[Y]-(P0)[Y]));} while (0)



#ifdef DEBUG
void PRINT_CTRLPTS(CubicBezierCurve* crv){
	int i;
	printf("curve %p\n[\n", crv);
	for (i=0; i<4; ++i)
		printf("[%f, %f]\n", crv->control_pts[i][X], crv->control_pts[i][Y]);
	printf("]\n");
}
#endif

void evaluate(const CubicBezierCurve *curve, const REAL t, Point value)
{
    Point pttmp;
    
    const REAL s1=3*(1-t*t);
    const REAL s2=6*(1-t)*t;
    const REAL s3=3*t*t;
    
    DERIVATIVE_CHANG_ADD(pttmp,curve->control_pts[0],curve->control_pts[1],s1);
    DERIVATIVE_CHANG_ADD(pttmp,curve->control_pts[2],curve->control_pts[1],s2);
    DERIVATIVE_CHANG_ADD(pttmp,curve->control_pts[3],curve->control_pts[2],s3);
    
    float tmp1=pttmp[0];
    float tmp2=pttmp[1];
    
    pttmp[0]=10*SIN;
    pttmp[1]=(0-((10*tmp1)/tmp2))*SIN;
    
    const REAL t_inv = 1.0f - t;
	const REAL t_inv_sq = t_inv * t_inv;
	const REAL t_sq = t * t;

	const REAL b0 = t_inv_sq * t_inv;
	const REAL b1 = 3 * t_inv_sq * t;
	const REAL b2 = 3 * t_inv * t_sq;
	const REAL b3 = t_sq * t;
	
    SET_VECTOR2(value, 0, 0);
	VECTOR2_X_SCALA_ADD(value, curve->control_pts[0], b0);
	VECTOR2_X_SCALA_ADD(value, curve->control_pts[1], b1);
	VECTOR2_X_SCALA_ADD(value, curve->control_pts[2], b2);
	VECTOR2_X_SCALA_ADD(value, curve->control_pts[3], b3);

    value[0] += pttmp[0];
    value[1] += pttmp[1];
}
