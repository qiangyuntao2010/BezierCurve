#include <GL/glut.h>
#include <iostream>
#include "curve.h"
#define eps 1e-8
#define zero(x) (((x)>0?(x):-(x))<eps)

CubicBezierCurve curve;
GLsizei width = 640, height = 480;
int edit_ctrlpts_idx = -1;
bool isDrawControlMesh = true;
bool isDottedLine = false;


struct qpoint{float x,y;};


int hit_index(CubicBezierCurve *curve, int x, int y)
{
	for (int i = 0; i < 4; i++)
	{
		REAL tx = curve->control_pts[i][0] - x;
		REAL ty = curve->control_pts[i][1] - y;
		if ((tx * tx + ty * ty) < 30) return i;
	}
	return -1;
}

void init()
{
	SET_PT2(curve.control_pts[0], 50, 100);
	SET_PT2(curve.control_pts[1], 200, 300);
	SET_PT2(curve.control_pts[2], 400, 300);
	SET_PT2(curve.control_pts[3], 550, 100);

	glClearColor(1.0, 1.0, 1.0, 1.0);
	glMatrixMode(GL_PROJECTION);
	gluOrtho2D(0, width, 0, height);
}

void reshape_callback(GLint nw, GLint nh)
{
	width = nw;
	height = nh;
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, width, 0, height);
}

double xmult(qpoint p1,qpoint p2,qpoint p0){
	if((p0.x==p1.x&&p0.y==p1.y)||(p0.x==p2.x&&p0.y==p2.y))
	{
		return 1;
	}
	else
	{	
 		return (p1.x-p0.x)*(p2.y-p0.y)-(p2.x-p0.x)*(p1.y-p0.y);
	}
}
//if the point is on the line
int dot_online_in(qpoint p,qpoint l1,qpoint l2){
	return zero(xmult(p,l1,l2))&&(l1.x-p.x)*(l2.x-p.x)<eps&&(l1.y-p.y)*(l2.y-p.y)<eps;
}
//if the two poin on the same side
int same_side(qpoint p1,qpoint p2,qpoint l1,qpoint l2){
	return xmult(l1,p1,l2)*xmult(l1,p2,l2)>eps;
}
//if the line is parallel
int parallel(qpoint u1,qpoint u2,qpoint v1,qpoint v2){
	return zero((u1.x-u2.x)*(v1.y-v2.y)-(v1.x-v2.x)*(u1.y-u2.y));
}
//if th three is on the same line
int dots_inline(qpoint p1,qpoint p2,qpoint p3){
	return zero(xmult(p1,p2,p3));
}
//judge the two line intersection or not
int intersect_in(qpoint u1,qpoint u2,qpoint v1,qpoint v2){
	if (!dots_inline(u1,u2,v1)||!dots_inline(u1,u2,v2))
		return !same_side(u1,u2,v1,v2)&&!same_side(v1,v2,u1,u2);
	return dot_online_in(u1,v1,v2)||dot_online_in(u2,v1,v2)||dot_online_in(v1,u1,u2)||dot_online_in(v2,u1,u2);
}
//get the line intersection
qpoint intersection(qpoint u1,qpoint u2,qpoint v1,qpoint v2){
	qpoint ret=u1;
	double t=((u1.x-v1.x)*(v1.y-v2.y)-(u1.y-v1.y)*(v1.x-v2.x))
		/((u1.x-u2.x)*(v1.y-v2.y)-(u1.y-u2.y)*(v1.x-v2.x));
	ret.x+=(u2.x-u1.x)*t;
	ret.y+=(u2.y-u1.y)*t;
	return ret;
}

void judge(qpoint u1,qpoint u2,qpoint v1, qpoint v2)
{
	qpoint ans;
	if (parallel(u1,u2,v1,v2)||!intersect_in(u1,u2,v1,v2)){
	}
	else{
		ans=intersection(u1,u2,v1,v2);
		std::cout<<ans.x<<" "<<ans.y<<std::endl;
		glColor3ub(225, 0, 0);
		glPointSize(7.0);
		glBegin(GL_POINTS);
		glVertex2f(ans.x, ans.y);
		glEnd();
	}
}

void intersect(qpoint* vector,int size)
{
	for(int out=0;out<(size-1);out++){
		for(int in=out+1;in<(size-1);in++)
		{
			judge(vector[out],vector[out+1],vector[in],vector[in+1]);	
		}		
	}
}
	
void display_callback()
{
#define RES 100
	struct qpoint vector[RES];
	/* curve */
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3ub(0, 0, 0);
	if (isDottedLine)
		glBegin(GL_LINES);
	else
		glBegin(GL_LINE_STRIP);
	for (int i = 0; i < RES; i++)
	{
		Point pt;
		const REAL t = (REAL)i / (REAL)RES;
		evaluate(&curve, t, pt);
		glVertex2f(pt[0], pt[1]);
		vector[i].x=pt[0];
		vector[i].y=pt[1];
	}
	glEnd();
	intersect(vector,(int)RES);

	/* control mesh */
	if (isDrawControlMesh)
	{
		glColor3ub(255, 0, 0);
		glBegin(GL_POINTS);
		for (int i = 0; i < 4; i++)
		{
			REAL *pt = curve.control_pts[i];
			glVertex2f(pt[0], pt[1]);
		}
		glEnd();
	}

	/* control pts */
	glColor3ub(0, 0, 255);
	glPointSize(10.0);
	glBegin(GL_POINTS);
	for (int i = 0; i < 4; i++)
	{
		REAL *pt = curve.control_pts[i];
		glVertex2f(pt[0], pt[1]);
	}
	glEnd();
	glutSwapBuffers();
}

// void glutMouseFunc(void (*func)(int button, int state, int x, int y));
void mouse_callback(GLint button, GLint action, GLint x, GLint y)
{
	if (GLUT_LEFT_BUTTON == button)
	{
		switch (action)
		{
			case GLUT_DOWN:
				edit_ctrlpts_idx = hit_index(&curve, x, height - y);
				break;
			case GLUT_UP:
				edit_ctrlpts_idx = -1;
				break;
			default: break;
		}
	}
	glutPostRedisplay();
}

// void glutMotionFunc(void (*func)(int x, int y));
void mouse_move_callback(GLint x, GLint y)
{
	if (edit_ctrlpts_idx != -1)
	{
		curve.control_pts[edit_ctrlpts_idx][0] = x;
		curve.control_pts[edit_ctrlpts_idx][1] = height - y;
	}
	glutPostRedisplay();
}

// void glutKeyboardFunc(void (*func)(unsigned char key, int x, int y));
void keyboard_callback(unsigned char key, int x, int y)
{
	switch(key)
	{
    case 'w': case 'W':
        break;
    case 's': case 'S':
        break;
    case 'a': case 'A':
        break;
    case 'd': case 'D':
        break;
	case 'i': case 'I':
		SET_PT2(curve.control_pts[0], 50, 100);
		SET_PT2(curve.control_pts[1], 200, 300);
		SET_PT2(curve.control_pts[2], 400, 300);
		SET_PT2(curve.control_pts[3], 550, 100);
		break;
	case 'l': case 'L':
		isDottedLine ^= true;
		break;
	case 'c': case 'C':
		isDrawControlMesh ^= true;
		break;
	case (27): exit(0); break;
	default: break;
	}
	glutPostRedisplay();
}

int main(int argc, char *argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(width, height);
	glutCreateWindow("Bezier Editor");

	init();
	glutReshapeFunc(reshape_callback);
	glutMouseFunc(mouse_callback);
	glutMotionFunc(mouse_move_callback);
	glutDisplayFunc(display_callback);
	glutKeyboardFunc(keyboard_callback);
	glutMainLoop();

	return 0;
}
