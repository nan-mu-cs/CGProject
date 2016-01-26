#pragma once
//
//  sky.h
//  CGprj2
//
//  Created by Green on 12/25/15.
//  Copyright ? 2015 Green. All rights reserved.
//

#ifndef sky_h
#define sky_h
#include<GL/glut.h>
class Sky
{
public:
	Sky(float skyX, float skyY, float skyZ, float skyR, unsigned int texture)
	{
		X = skyX;
		Y = skyY;
		Z = skyZ;
		R = skyR;
		T = texture;
		//
		qobj = gluNewQuadric();
		gluQuadricNormals(qobj, GLU_SMOOTH);
		gluQuadricTexture(qobj, GL_TRUE);
	}

	void Sky::show()
	{
		static float r;
		r += 0.08;
		glEnable(GL_TEXTURE_2D);
		glPushMatrix();
		glFrontFace(GL_CCW);
		glCullFace(GL_BACK);
		glBindTexture(GL_TEXTURE_2D, T);
		glTranslatef(X, Y, Z);
		glRotatef(r, 0.0f, 0.0f, 1.0f);
		glColor4f(1, 1, 1, 1);
		gluSphere(qobj, R, 100, 100);
		glPopMatrix();
		glDisable(GL_TEXTURE_2D);
	}
public:
	float X;
	float Y;
	float Z;
	float R;
	unsigned int T;
	GLUquadricObj *qobj; 
	//static float r;
};

#endif /* sky_h */

