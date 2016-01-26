#include <stdio.h>
#include <stdlib.h>
#include <gl/glut.h>
#include<Windows.h>
#include<time.h>
#include<math.h>
#include"sky.h"
#include "gameoverMTL.h"
#include "gameoverOBJ.h"
#include "feetMTL.h"
#include "feetOBJ.h"
#include "creamMTL.h"
#include "creamOBJ.h"
#define ROAD_WIDTH 1
#define ROAD_LENGTH 10
#define PART_ROAD_WIDTH (1.0*ROAD_WIDTH/3)
#define OBJ_SIZE 0.15
#define STEP_SIZE 0.04
#define SPEED 3
#define PARTICLES_MAX 100

typedef  unsigned char byte;
float posy = 2, posz = 0, eyex = 0, eyey = -0.5, eyez = 0.5;
float ry, rz, ra, ly, lz, la, arise;
int posx = 1, front = 1, rear = 1, grade, frm, num;
bool over, pause, up, down, turnr, turnl;
unsigned int id[6];
float up_time, angle, dist = 0.5, turna;
int flag1[ROAD_LENGTH][3] = { 0 }, flag2[ROAD_LENGTH][3] = { 0 };
int Bflag1[2 * ROAD_LENGTH][3] = { 0 }, Bflag2[ROAD_LENGTH * 2][3] = { 0 };

GLfloat ag_ambient[] = { 0.4, 0.4, 0.4, 1.000000 };
GLfloat ag_diffuse[] = { 0.5, 0.5, 0.5, 1.000000 };
GLfloat ag_specular[] = { 0.9, 0.9, 0.9, 1.000000 };
GLfloat ag_shineness[] = { 100 };

struct star
{
	float x;
	float y;
	float z;
	float vx;
	float vy;
	float vz;
	float ax;
	float ay;
	float az;
	bool life;
	float fade;
}stars[PARTICLES_MAX];
void LoadBmp(char *file, GLuint ID)
{
	
	int width, height, i;
	byte *image, t;          //接受图像数据
	FILE *fp;            //文件指针
	BITMAPFILEHEADER FileHeader;    //接受位图文件头
	BITMAPINFOHEADER InfoHeader;    //接受位图信息头
	fp = fopen(file, "rb");
	if (fp == NULL)
	{
		perror("LoadBitmap");        //打开文件失败
		Sleep(5000);
		exit(0);
	}
	fread(&FileHeader, sizeof(BITMAPFILEHEADER), 1, fp);

	if (FileHeader.bfType != 0x4D42)  //确保文件是一个位图文件，效验文件类型
	{
		printf("Error: This file is not a bmp file!");
		fclose(fp);
		Sleep(5000);
		exit(0);
	}

	fread(&InfoHeader, sizeof(BITMAPINFOHEADER), 1, fp);
	width = InfoHeader.biWidth;
	height = InfoHeader.biHeight;
	if (InfoHeader.biSizeImage == 0)          //确保图像数据的大小
	{
		InfoHeader.biSizeImage = width*height * 3;
	}
	fseek(fp, FileHeader.bfOffBits, SEEK_SET);  //将文件指针移动到实际图像数据处
	image = (byte *)malloc(sizeof(byte)*InfoHeader.biSizeImage); //申请空间
	if (image== NULL)
	{
		free(image);
		printf("Error: No enough space!");
		Sleep(5000);
		exit(0);
	}
	fread(image, 1, InfoHeader.biSizeImage, fp);
	for (i = 0; i<InfoHeader.biSizeImage; i += 3)
	{
		t = image[i];
		image[i] = image[i + 2];
		image[i + 2] = t;
	}
	fclose(fp);
	glBindTexture(GL_TEXTURE_2D, ID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_LINEAR);
	//glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	//glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	//glEnable(GL_TEXTURE_GEN_S);
	//glEnable(GL_TEXTURE_GEN_T);

	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, width,
		height, GL_RGB, GL_UNSIGNED_BYTE, image);
}
void InitTexture()
{
	glGenTextures(6, id);
	LoadBmp("ice1.bmp", id[0]);
	LoadBmp("sea2.bmp", id[1]);
	LoadBmp("ice1.bmp", id[2]);
	LoadBmp("ice2.bmp", id[3]);
	LoadBmp("kirby.bmp", id[4]);
}
void DrawStars()
{
	for (int i = 0; i < PARTICLES_MAX; i++)
	{
		if (stars[i].life <= 0)
		{
			stars[i].x = 1.0*ROAD_WIDTH*rand() / RAND_MAX - 0.5*ROAD_WIDTH;
			stars[i].y = 1.0*ROAD_LENGTH*rand() / RAND_MAX;
			stars[i].z = 1.0*rand() / RAND_MAX / 100;
			stars[i].life = 1.0*rand() / RAND_MAX;
			stars[i].fade = 1.0*rand() / RAND_MAX / 10;
		}
		else
			stars[i].life -= stars[i].fade;
	}

	glPushMatrix();
	glTranslatef(0, -posy, 0);
	for (int i = 0; i < PARTICLES_MAX; i++)
	{
		glColor4f(1.0, 1.0, 1.0, stars[i].life);
		glPushMatrix();
		glTranslated(stars[i].x, stars[i].y, stars[i].z);
		glutSolidSphere(0.01, 100, 100);
		glPopMatrix();
	}
	glPopMatrix();
}
void InitBonus()
{
	srand((int)time(0));
	srand((int)time(0));
	for (int i = 0; i <2 * ROAD_LENGTH; i++)
		for (int j = 0; j < 3; j++)
			Bflag1[i][j] = Bflag2[i][j];
	for (int i = 0; i <2* ROAD_LENGTH; i++)
		for (int j = 0; j < 3; j++)
			Bflag2[i][j] = 0;
	for (int i = 2; i <2* ROAD_LENGTH-2; i++) {
		int type = 3.0*rand() / RAND_MAX;
		for (int j = 0; j < 3; j++) {
			Bflag2[i][j] = 2 * rand() / RAND_MAX*type;
			if (!(i % 2) | (flag2[i/2][j] == 0))
				Bflag2[i][j] = 0;
		}
	}
}
void DrawOneBonus(int y, int x, int type)
{
	switch (type) {
	case 1:
		glTranslatef((x - 1)*PART_ROAD_WIDTH, 1.0*y/2, 0.15);
		glRotatef(90, 1, 0, 0);
		glBegin(GL_TRIANGLES);
		for (int i = 0; i < creamMTLNumMaterials; i++) {
			int first = creamMTLFirst[i];
			glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, creamMTLAmbient[i]);
			glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, creamMTLDiffuse[i]);
			glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, creamMTLSpecular[i]);
			for (int j = first; j < first + creamMTLCount[i]; j++) {
				glNormal3f(creamOBJNormals[3 * j], creamOBJNormals[3 * j + 1], creamOBJNormals[3 * j + 2]);
				glVertex3f(creamOBJVerts[3 * j], creamOBJVerts[3 * j + 1], creamOBJVerts[3 * j + 2]);
			}
		}
		glEnd();
		break;
		/*
	case 2:
		glTranslatef((x - 1)*PART_ROAD_WIDTH, y + 0.5, 0.3);
		glBegin(GL_TRIANGLES);
		for (int i = 0; i < creamMTLNumMaterials; i++) {
			int first = creamMTLFirst[i];
			glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, creamMTLAmbient[i]);
			glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, creamMTLDiffuse[i]);
			glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, creamMTLSpecular[i]);
			for (int j = first; j < first + creamMTLCount[i]; j++) {
				glNormal3f(creamOBJNormals[3 * j], creamOBJNormals[3 * j + 1], creamOBJNormals[3 * j + 2]);
				glVertex3f(creamOBJVerts[3 * j], creamOBJVerts[3 * j + 1], creamOBJVerts[3 * j + 2]);
			}
		}
		glEnd();
		break;
		*/
	default:break;
	}
}
void DrawBonus()
{
	glPushMatrix();
	glTranslatef(0, -posy, 0);
	for (int i = 0; i < 2*ROAD_LENGTH; i++)
		for (int j = 0; j < 3; j++)
			if (Bflag1[i][j]>0) {
				if ((i == 0 || i == 1) && rear != 1) {
					Bflag1[i][j] = 0;
					continue;
				}
				if ((i == 2*ROAD_LENGTH - 1 || i == 2*ROAD_LENGTH - 2) && front != 1) {
					Bflag1[i][j] = 0;
					continue;
				}
				glPushMatrix();
					DrawOneBonus(i, j, Bflag1[i][j]);
				glPopMatrix();
			}
	if (front == 1)
	{
		for (int i = 0; i < 2*ROAD_LENGTH; i++)
			for (int j = 0; j < 3; j++)
				if (Bflag2[i][j]>0) {
					if (Bflag2[i][j] == 2 || Bflag2[i][j] == 5)
						if (j>0) break;
					glPushMatrix();
					DrawOneBonus(i + 2*ROAD_LENGTH, j, Bflag2[i][j]);
					glPopMatrix();
				}
	}
	//glPopMatrix();
	//glDisable(GL_LIGHTING);
	glPopMatrix();
}
void InitObs()
{
	srand((int)time(0));
	for (int i = 0; i < ROAD_LENGTH; i++)
		for (int j = 0; j < 3; j++)
			flag1[i][j] = flag2[i][j];
	for (int i = 0; i < ROAD_LENGTH; i++)
		for (int j = 0; j < 3; j++)
			flag2[i][j] = 0;
	for (int i = 0; i < ROAD_LENGTH; i++) {
		int type = 6.0*rand() / RAND_MAX;
		for (int j = 0; j < 3; j++) {
			flag2[i][j] = 2 * rand() / RAND_MAX*type;
			if (flag2[i][j] == 2) {
				flag2[i][0] = 2;
				flag2[i][1] = 2;
				flag2[i][2] = 2;
				break;
			}
			if (flag2[i][j] == 5) {
				flag2[i][0] = 5;
				flag2[i][1] = 5;
				flag2[i][2] = 5;
				break;
			}
		}
	}
}
void DrawRoad()
{
	glClearColor(0, 0, 0, 0);
	glPushMatrix();
	if (turnl) glRotatef(-turna, 0, 0, 1);
	else if (turnr) glRotatef(turna, 0, 0, 1);
	glTranslatef(0, -posy, 0);
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, id[0]);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);glVertex3f(-0.5*ROAD_WIDTH, 0, 0.0);
	glTexCoord2f(1.0f, 0.0f);glVertex3f(-0.5*ROAD_WIDTH, ROAD_LENGTH, 0);
	glTexCoord2f(1.0f, 1.0f);glVertex3f(0.5*ROAD_WIDTH, ROAD_LENGTH, 0);
	glTexCoord2f(0.0f, 1.0f);glVertex3f(0.5*ROAD_WIDTH,0 , 0.0);
	glEnd();
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5*ROAD_WIDTH, 0, -0.2);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(-0.5*ROAD_WIDTH, ROAD_LENGTH, -0.2);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(0.5*ROAD_WIDTH, ROAD_LENGTH, -0.2);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(0.5*ROAD_WIDTH, 0, -0.2);
	glEnd();
	glBindTexture(GL_TEXTURE_2D, id[3]);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5*ROAD_WIDTH, 0, 0.0);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(-0.5*ROAD_WIDTH, ROAD_LENGTH, 0);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(-0.5*ROAD_WIDTH, ROAD_LENGTH, -0.2);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.5*ROAD_WIDTH, 0, -0.2);
	glEnd();
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(0.5*ROAD_WIDTH, 0, 0.0);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(0.5*ROAD_WIDTH, ROAD_LENGTH, 0);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(0.5*ROAD_WIDTH, ROAD_LENGTH, -0.2);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(0.5*ROAD_WIDTH, 0, -0.2);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, id[0]);

	switch (front)
	{
	case 0:
		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5*ROAD_WIDTH, ROAD_LENGTH, 0.0);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(-ROAD_LENGTH + 0.5*ROAD_WIDTH, ROAD_LENGTH, 0.0);
		glTexCoord2f(1.0f, 1.0f);	glVertex3f(-ROAD_LENGTH + 0.5*ROAD_WIDTH, ROAD_LENGTH - ROAD_WIDTH, 0.0);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.5*ROAD_WIDTH, ROAD_LENGTH - ROAD_WIDTH, 0.0);
		glEnd();
		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5*ROAD_WIDTH, ROAD_LENGTH, -0.2);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(-ROAD_LENGTH + 0.5*ROAD_WIDTH, ROAD_LENGTH, -0.2);
		glTexCoord2f(1.0f, 1.0f);	glVertex3f(-ROAD_LENGTH + 0.5*ROAD_WIDTH, ROAD_LENGTH - ROAD_WIDTH, -0.2);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.5*ROAD_WIDTH, ROAD_LENGTH - ROAD_WIDTH, -0.2);
		glEnd();
		glBindTexture(GL_TEXTURE_2D, id[3]);
		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5*ROAD_WIDTH, ROAD_LENGTH, 0.0);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(-ROAD_LENGTH + 0.5*ROAD_WIDTH, ROAD_LENGTH, 0.0);
		glTexCoord2f(1.0f, 1.0f);	glVertex3f(-ROAD_LENGTH + 0.5*ROAD_WIDTH, ROAD_LENGTH, -0.2);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.5*ROAD_WIDTH, ROAD_LENGTH, -0.2);
		glEnd();
		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5*ROAD_WIDTH, ROAD_LENGTH - ROAD_WIDTH, 0.0);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(-ROAD_LENGTH + 0.5*ROAD_WIDTH, ROAD_LENGTH - ROAD_WIDTH, 0.0);
		glTexCoord2f(1.0f, 1.0f);	glVertex3f(-ROAD_LENGTH + 0.5*ROAD_WIDTH, ROAD_LENGTH - ROAD_WIDTH, -0.2);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.5*ROAD_WIDTH, ROAD_LENGTH - ROAD_WIDTH, -0.2);
		glEnd();
		break;
	case 1:
		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5*ROAD_WIDTH, 2 * ROAD_LENGTH, 0.0);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(-0.5*ROAD_WIDTH, ROAD_LENGTH, 0.0);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(0.5*ROAD_WIDTH, ROAD_LENGTH, 0.0);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(0.5*ROAD_WIDTH, 2 * ROAD_LENGTH, 0.0);
		glEnd();
		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5*ROAD_WIDTH, 2 * ROAD_LENGTH, -0.2);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(-0.5*ROAD_WIDTH, ROAD_LENGTH, -0.2);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(0.5*ROAD_WIDTH, ROAD_LENGTH, -0.2);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(0.5*ROAD_WIDTH, 2 * ROAD_LENGTH, -0.2);
		glEnd();
		glBindTexture(GL_TEXTURE_2D, id[3]);
		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5*ROAD_WIDTH, 2 * ROAD_LENGTH, 0.0);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(-0.5*ROAD_WIDTH, ROAD_LENGTH, 0.0);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(-0.5*ROAD_WIDTH, ROAD_LENGTH, -0.2);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.5*ROAD_WIDTH, 2 * ROAD_LENGTH, -0.2);
		glEnd();
		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(0.5*ROAD_WIDTH, 2 * ROAD_LENGTH, 0.0);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(0.5*ROAD_WIDTH, ROAD_LENGTH, 0.0);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(0.5*ROAD_WIDTH, ROAD_LENGTH, -0.2);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(0.5*ROAD_WIDTH, 2 * ROAD_LENGTH, -0.2);
		glEnd();
		break;
	case 2:
		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(0.5*ROAD_WIDTH, ROAD_LENGTH, 0.0);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(ROAD_LENGTH - 0.5*ROAD_WIDTH, ROAD_LENGTH - ROAD_WIDTH, 0.0);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(ROAD_LENGTH - 0.5*ROAD_WIDTH, ROAD_LENGTH, 0.0);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(0.5*ROAD_WIDTH, ROAD_LENGTH - ROAD_WIDTH, 0.0);
		glEnd();
		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(0.5*ROAD_WIDTH, ROAD_LENGTH, -0.2);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(ROAD_LENGTH - 0.5*ROAD_WIDTH, ROAD_LENGTH - ROAD_WIDTH, -0.2);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(ROAD_LENGTH - 0.5*ROAD_WIDTH, ROAD_LENGTH, -0.2);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(0.5*ROAD_WIDTH, ROAD_LENGTH - ROAD_WIDTH, -0.2);
		glEnd();
		glBindTexture(GL_TEXTURE_2D, id[3]);
		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(0.5*ROAD_WIDTH, ROAD_LENGTH, 0.0);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(ROAD_LENGTH - 0.5*ROAD_WIDTH, ROAD_LENGTH, 0.0);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(ROAD_LENGTH - 0.5*ROAD_WIDTH, ROAD_LENGTH, -0.2);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(0.5*ROAD_WIDTH, ROAD_LENGTH, -0.2);
		glEnd();
		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(0.5*ROAD_WIDTH, ROAD_LENGTH - ROAD_WIDTH, 0.0);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(ROAD_LENGTH - 0.5*ROAD_WIDTH, ROAD_LENGTH - ROAD_WIDTH, 0.0);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(ROAD_LENGTH - 0.5*ROAD_WIDTH, ROAD_LENGTH - ROAD_WIDTH, -0.2);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(0.5*ROAD_WIDTH, ROAD_LENGTH - ROAD_WIDTH, -0.2);
		glEnd();
		break;
	default:break;
	}

	glBindTexture(GL_TEXTURE_2D, id[0]);
	switch (rear)
	{
	case 0:
		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-ROAD_LENGTH + 0.5*ROAD_WIDTH, 0, 0.0);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(-0.5*ROAD_WIDTH, 0, 0.0);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(-0.5*ROAD_WIDTH, ROAD_WIDTH, 0.0);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-ROAD_LENGTH + 0.5*ROAD_WIDTH, ROAD_WIDTH, 0.0);
		glEnd();
		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-ROAD_LENGTH + 0.5*ROAD_WIDTH, 0, -0.2);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(-0.5*ROAD_WIDTH, 0, -0.2);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(-0.5*ROAD_WIDTH, ROAD_WIDTH, -0.2);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-ROAD_LENGTH + 0.5*ROAD_WIDTH, ROAD_WIDTH, -0.2);
		glEnd();
		glBindTexture(GL_TEXTURE_2D, id[3]);
		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-ROAD_LENGTH + 0.5*ROAD_WIDTH, 0, 0.0);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(-0.5*ROAD_WIDTH, 0, 0.0);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(-0.5*ROAD_WIDTH, 0.0, -0.2);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-ROAD_LENGTH + 0.5*ROAD_WIDTH, 0.0, -0.2);
		glEnd();
		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-ROAD_LENGTH + 0.5*ROAD_WIDTH, ROAD_WIDTH, 0.0);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(-0.5*ROAD_WIDTH, ROAD_WIDTH, 0.0);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(-0.5*ROAD_WIDTH, ROAD_WIDTH, -0.2);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-ROAD_LENGTH + 0.5*ROAD_WIDTH, ROAD_WIDTH, -0.2);
		glEnd();
		break;
	case 1:
		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5*ROAD_WIDTH, - ROAD_LENGTH, 0.0);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(-0.5*ROAD_WIDTH,0 , 0.0);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(0.5*ROAD_WIDTH, 0, 0.0);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(0.5*ROAD_WIDTH, -ROAD_LENGTH, 0.0);
		glEnd();
		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5*ROAD_WIDTH, -ROAD_LENGTH, -0.2);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(-0.5*ROAD_WIDTH, 0, -0.2);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(0.5*ROAD_WIDTH, 0, -0.2);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(0.5*ROAD_WIDTH, -ROAD_LENGTH, -0.2);
		glEnd();
		glBindTexture(GL_TEXTURE_2D, id[3]);
		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5*ROAD_WIDTH, -ROAD_LENGTH, 0.0);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(-0.5*ROAD_WIDTH, 0, 0.0);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(-0.5*ROAD_WIDTH, 0, -0.2);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.5*ROAD_WIDTH, -ROAD_LENGTH, -0.2);
		glEnd();
		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(0.5*ROAD_WIDTH, -ROAD_LENGTH, 0.0);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(0.5*ROAD_WIDTH, 0, 0.0);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(0.5*ROAD_WIDTH, 0, -0.2);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(0.5*ROAD_WIDTH, -ROAD_LENGTH, -0.2);
		glEnd();
		break;
	case 2:
		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(0.5*ROAD_WIDTH,0 , 0.0);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(ROAD_LENGTH - 0.5*ROAD_WIDTH,0 , 0.0);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(ROAD_LENGTH - 0.5*ROAD_WIDTH, ROAD_WIDTH, 0.0);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(0.5*ROAD_WIDTH, ROAD_WIDTH, 0.0);
		glEnd();
		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(0.5*ROAD_WIDTH, 0, -0.2);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(ROAD_LENGTH - 0.5*ROAD_WIDTH, 0, -0.2);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(ROAD_LENGTH - 0.5*ROAD_WIDTH, ROAD_WIDTH, -0.2);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(0.5*ROAD_WIDTH, ROAD_WIDTH, -0.2);
		glEnd();
		glBindTexture(GL_TEXTURE_2D, id[3]);
		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(0.5*ROAD_WIDTH, 0, 0.0);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(ROAD_LENGTH - 0.5*ROAD_WIDTH, 0, 0.0);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(ROAD_LENGTH - 0.5*ROAD_WIDTH, 0, -0.2);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(0.5*ROAD_WIDTH, 0,-0.2);
		glEnd();
		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(0.5*ROAD_WIDTH, ROAD_WIDTH, 0.0);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(ROAD_LENGTH - 0.5*ROAD_WIDTH, ROAD_WIDTH, 0.0);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(ROAD_LENGTH - 0.5*ROAD_WIDTH, ROAD_WIDTH, -0.2);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(0.5*ROAD_WIDTH, ROAD_WIDTH, -0.2);
		glEnd();
		break;
	default:break;
	}
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
}
void RestartGame()
{
	eyex = 0; eyey = -0.5; eyez = 0.5;
	over = 0; pause = 0;
	up = 0; down = 0;
	up_time = 0;
	angle = 0;
	posy = 2;posz = 0;posx = 1;
	front = 1;rear = 1;
	grade = 0;
	frm = 0;
	ry=0, rz=0, ra=0, ly=0, lz=0, la=0, arise=0;
	for (int i = 0; i < ROAD_LENGTH; i++)
		for (int j = 0; j < 3; j++)
		{
			flag1[i][j] = 0;
			flag2[i][j] = 0;
		}
}
void processKeyUp(unsigned char key, int x, int y)
{
	if (!pause) {
		switch (key)
		{
		case 's':
			down = false;
			posz = 0;
			break;
		}
	}
}
void processKeyboard(unsigned char key, int x, int y)
{
	if (!pause) {
		switch (key)
		{
		case 'a':
			posx--;
			if (posx < 0) {
				printf("Go left out\n");
				over = true;
			}
			break;
		case 'd':
			posx++;
			if (posx > 2) {
				printf("Go right out\n");
				over = true;
			}
			break;
		case 'w':
			if (!up) {
				up = true;
				up_time = 0;
			}
			break;
		case 's':
			down = true;
			break;
		default:break;
		}
	}
	if (!over) {
		switch (key) {
		case 'p':
			pause = !pause;
			printf("pause=%d\n", pause);
			break;
		case 'q':
			angle -= 0.1;
			break;
		case 'e':
			angle += 0.1;
			break;
		default:break;
		}
		if (pause)
		{
			switch (key)
			{
			case 'w':
				eyez += 0.1;
				break;
			case 's':
				eyez -= 0.1;
				break;
			case 'a':
				dist -= 0.1;
				break;
			case 'd':
				dist += 0.1;
				break;
			default:break;
			}
		}
	}
}
void processSpecialUp(int key, int x, int y)
{
	if (!pause) {
		switch (key)
		{
		case GLUT_KEY_DOWN:
			down = false;
			posz = 0;
			break;
		}
	}
}
void processSpecial(int key, int x, int y)
{
	if (!pause) {
		switch (key) {
		case GLUT_KEY_UP:
			if (!up) {
				up = true;
				up_time = 0;
			}
			break;
		case GLUT_KEY_DOWN:
			down = true;
			break;
		case GLUT_KEY_LEFT:
			if (posy - OBJ_SIZE > ROAD_LENGTH - 2 * ROAD_WIDTH&&front == 0)
			{
				turnl = true;
			}
			else {
				posx = -1;
				printf("Turn left wrong\n");
				over = true;
			}
			break;
		case GLUT_KEY_RIGHT:
			if (posy - OBJ_SIZE > ROAD_LENGTH - 2 * ROAD_LENGTH&&front == 2)
			{
				turnr = true;
				/*
				*/
			}
			else {
				posx = 3;
				printf("Turn right wrong\n");
				over = true;
			}
			break;
		default:break;
		}
	}
	switch (key)
	{
	case GLUT_KEY_F1:
		angle = 0;
		eyez = 0.5;
		dist = 1.5;
		break;
	case GLUT_KEY_F2:
		eyez = 5;
		break;
	case GLUT_KEY_HOME:
		if (over) RestartGame();
		break;
	case GLUT_KEY_END:
		if (over) exit(0);
		break;
	default:break;
	}
}
bool isCrush(float y, int type)
{
	switch (type)
	{
	case 1:
	case 2:
	case 3:
		if(y<PART_ROAD_WIDTH||y>2*PART_ROAD_WIDTH) return false;
		else if (up) return false;
		break;
	case 4:
	case 5:
		if (y<PART_ROAD_WIDTH || y>2 * PART_ROAD_WIDTH) return false;
		else if (down) return false;
		break;
	default: 
		return false; 
	}
	return true;
}
void DrawOneObs(int y, int x, int type)
{
	GLUquadricObj *quadratic;
	quadratic = gluNewQuadric();
	switch (type) {
	case 1:
		glTranslatef((x - 1)*PART_ROAD_WIDTH, y + 0.5, 0);
		glutSolidSphere(PART_ROAD_WIDTH / 2, 100, 100);
		break;
	case 2:
		if (x == 0) {
			for (int i = 0; i < 6; i++) {
				glPushMatrix();
				glTranslatef(-5.0 / 4 * PART_ROAD_WIDTH + i*PART_ROAD_WIDTH / 2, y + 0.5, 0);
				glutSolidCone(PART_ROAD_WIDTH / 4, PART_ROAD_WIDTH / 2, 100, 100);
				glPopMatrix();
			}
		}
		break;
	case 3:
		glTranslatef((x - 1)*PART_ROAD_WIDTH, y + 0.5, 0);
		glutSolidCube(PART_ROAD_WIDTH);
		break;
	case 4:
		glTranslatef((x - 1.5)*PART_ROAD_WIDTH, y + 0.5, PART_ROAD_WIDTH / 2);
		glRotatef(90, 0, 1, 0);
		gluCylinder(quadratic, 0.05, 0.05, PART_ROAD_WIDTH, 100, 100);
		break;
	case 5:
		if (x == 0) {
			glTranslatef(-1.5*PART_ROAD_WIDTH, +y + 0.5, PART_ROAD_WIDTH / 2);
			glRotatef(90, 0, 1, 0);
			gluCylinder(quadratic, 0.05, 0.05, ROAD_WIDTH, 100, 100);
		}
	default:break;
	}
}
void DrawObs()
{
	//glColor3f(1.0, 0.0, 0.0);
	///glEnable(GL_LIGHTING);
	//glEnable(GL_LIGHT1);
	//glPushMatrix();

	glMaterialfv(GL_FRONT, GL_AMBIENT, ag_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, ag_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, ag_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, ag_shineness);
	glPushMatrix();
	glTranslatef(0, -posy, 0);
	for (int i = 0; i < ROAD_LENGTH; i++)
		for (int j = 0; j < 3; j++)
			if (flag1[i][j]>0) {
				if ((i == 0 || i == 1) && rear != 1) {
					flag1[i][j] = 0;
					continue;
				}
				if ((i == ROAD_LENGTH - 1 || i == ROAD_LENGTH - 2) && front != 1) {
					flag1[i][j] = 0;
					continue;
				}
				glPushMatrix();
				DrawOneObs(i, j, flag1[i][j]);
				glPopMatrix();
			}
	if (front == 1)
	{
		for (int i = 0; i < ROAD_LENGTH; i++)
			for (int j = 0; j < 3; j++)
				if (flag2[i][j]>0) {
					if (flag2[i][j] == 2 || flag2[i][j] == 5)
						if (j>0) break;
					glPushMatrix();
					DrawOneObs(i + ROAD_LENGTH, j, flag2[i][j]);
					glPopMatrix();
				}
	}
	//glPopMatrix();
	//glDisable(GL_LIGHTING);
	glPopMatrix();
}
void reshape(int width, int height)
{
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(90, 1.0*width / height, 0.1, 1000);
	int err = glGetError();
	if (err != 0) {
		fprintf(stderr, "Error at reshape:%d\n", err);
	}
}
void DrawRole()
{
	glPushMatrix();
	glTranslatef(-0.07, 0, 0);
	glTranslatef(0, ly, lz);
	glRotatef(180, 0, 0, 1);
	glRotatef(90, 1, 0, 0);
	glRotatef(la, -1, 0, 0);
	glBegin(GL_TRIANGLES);

	for (int i = 0; i < feetMTLNumMaterials; i++) {
		int first = feetMTLFirst[i];
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, feetMTLAmbient[i]);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, feetMTLDiffuse[i]);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, feetMTLSpecular[i]);
		for (int j = first; j < first + feetMTLCount[i]; j++) {
			glNormal3f(feetOBJNormals[3 * j], feetOBJNormals[3 * j + 1], feetOBJNormals[3 * j + 2]);
			glVertex3f(feetOBJVerts[3 * j], feetOBJVerts[3 * j + 1], feetOBJVerts[3 * j + 2]);
		}
	}
	glEnd();
	glPopMatrix();
	
	glPushMatrix();
	glTranslatef(0.07, 0, 0);
	glTranslatef(0, ry, rz);
	//glTranslatef(0, -0.2, 0);
	glRotatef(180, 0, 0,1);
	glRotatef(90, 1, 0, 0);
	glRotatef(ra, -1, 0, 0);
	glBegin(GL_TRIANGLES);
	for (int i = 0; i < feetMTLNumMaterials; i++) {
		int first = feetMTLFirst[i];
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, feetMTLAmbient[i]);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, feetMTLDiffuse[i]);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, feetMTLSpecular[i]);
		for (int j = first; j < first + feetMTLCount[i]; j++) {
			glNormal3f(feetOBJNormals[3 * j], feetOBJNormals[3 * j + 1], feetOBJNormals[3 * j + 2]);
			glVertex3f(feetOBJVerts[3 * j], feetOBJVerts[3 * j + 1], feetOBJVerts[3 * j + 2]);
		}
	}
	glEnd();
	glPopMatrix();
	
	GLUquadric *q = gluNewQuadric();
	gluQuadricNormals(q, GLU_SMOOTH);
	gluQuadricTexture(q, GL_TRUE);

	glEnable(GL_TEXTURE_2D);
	glPushMatrix();
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);
	glBindTexture(GL_TEXTURE_2D, id[4]);
	glRotatef(180, 0, 0, 1);
	glTranslatef(0, 0, 0.18);
	glColor4f(1, 1, 1, 1);
	gluSphere(q, 0.15, 100, 100);
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);

}
void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//gluLookAt(eyex, eyey, eyez, centx, centy, centz, upx, upy, upz);
	gluLookAt(dist*sin(angle), -dist*cos(angle), eyez,
		0, 0, 0.3,
		0, 0, 1);
	//drawObj(curx,cury,curz);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	Sky sky(0.0, 0.0, 0.0, 20.0, id[1]);
	sky.show();
	DrawRoad();
	DrawObs();
	DrawBonus();
	glPushMatrix();
	glRotatef(90,1,0,0);
	glTranslatef(0, 0.5, 0);
	if (over) {
		glBegin(GL_TRIANGLES);
		for (int i = 0; i < gameoverMTLNumMaterials; i++) {
			int first = gameoverMTLFirst[i];
			glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, gameoverMTLAmbient[i]);
			glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, gameoverMTLDiffuse[i]);
			glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, gameoverMTLSpecular[i]);
			for (int j = first; j < first + gameoverMTLCount[i]; j++) {
				glNormal3f(gameoverOBJNormals[3 * j], gameoverOBJNormals[3 * j + 1], gameoverOBJNormals[3 * j + 2]);
				glVertex3f(gameoverOBJVerts[3 * j], gameoverOBJVerts[3 * j + 1], gameoverOBJVerts[3 * j + 2]);
			}
		}
		glEnd();
	}
	glPopMatrix();

	//Draw role
	glPushMatrix();
	glTranslatef(1.0*(posx - 1)*ROAD_WIDTH / 3, 0, posz+arise);

	//glutSolidSphere(0.15, 100, 100);
	//glutSolidCone(0.1,0.3,100,100);
	//glutSolidTorus(0.1,0.1,100,100);
	//glutSolidTeapot(0.1);
	DrawRole();
	glPopMatrix();
	glutSwapBuffers();
	int err = glGetError();
	if (err != 0) {
		fprintf(stderr, "Error at display:%d\n", err);
	}
}
void runtimer(int id)
{
	if (!pause) {
		if (turna >= 90) {
			if (turnl)
			{
				int next = rand()*1.0 / RAND_MAX * 3;
				rear = front;
				front = next;
				//posx = 0;
				posy = 0;
				InitObs();
				InitBonus();
				turnl = 0;
			}
			else if(turnr)
			{
				int next = rand()*1.0 / RAND_MAX * 3;
				rear = front;
				front = next;
				//posx = 0;
				posy = 0;
				InitObs();
				InitBonus();
				turnr = 0;
			}
			turna = 0;
		}
		if (turnl | turnr) {
			turna += 30;
			posy += (ROAD_LENGTH - 2.5*PART_ROAD_WIDTH + posx*PART_ROAD_WIDTH - posy) / 3;
		}
		else {
			posy += 1.0*SPEED / 20;
			if (posy >= ROAD_LENGTH) {
				if (front != 1)
				{
					printf("No turn\n");
					over = true;
				}
				else {
					int RandDir = rand()*1.0 / RAND_MAX * 3;
					rear = 1;
					front = RandDir;
					posy -= ROAD_LENGTH;
					InitObs();
					InitBonus();
				}
			}
		}
		if (up)
		{
			up_time += 1.0 / 20;
			posz = up_time * 2 - 6 * up_time*up_time;
			if (posz <= 0)
			{
				posz = 0;
				up = false;
			}
		}
		else if (down)
		{
			posz = -1.5*OBJ_SIZE;
		}
		else
		{
			frm++;
			if (frm == 10) frm = 0;
			switch (frm) {
			case 0: ra = 0; ry = 0; rz = 0; la = 0; ly = 0; lz = 0; arise = 0; break;
			case 1: ra = 20; ry = 0.06; rz = 0.01; la = -45; ly = -0.07; lz = 0.02; arise = 0.03; break;
			case 2: ra = 40; ry = 0.12; rz = 0.06; la = -90; ly = -0.14; lz = 0.1; ; arise = 0.06; break;
			case 4:ra = 20; ry = 0.06; rz = 0.01; la = -45; ly = -0.07; lz = 0.02; arise = 0.03; break;
			case 5: ra = 0; ry = 0; rz = 0; la = 0; ly = 0; lz = 0; arise = 0; break;
			case 6:ra = -45; ry = -0.07; rz = 0.02; la = 20; ly = 0.06; lz = 0.01; arise = 0.03; break;
			case 7:ra = -90; ry = -0.14; rz = 0.1; la = 40; ly = 0.12; lz = 0.06; arise = 0.06; break;
			case 9:ra = -45; ry = -0.07; rz = 0.02; la = 20; ly = 0.06; lz = 0.01; arise = 0.03; break;
			default:break;
			}
		}
		if (flag1[(int)posy][posx] > 0)
		{
			if (isCrush(posy - (int)posy, flag1[(int)posy][posx]))
			{
				printf("crush %d in %d, %d\n", flag1[(int)posy][posx], (int)posy, posx);

				over = true;
			}
		}
		if (Bflag1[(int)(posy*2)][posx] > 0)
		{
			Bflag1[(int)(posy * 2)][posx] = 0;
		}
		grade += 1;
	}
	display();
	if (over) { pause = 1;}
	glutTimerFunc(50, runtimer, 1);
}
void init() 
{
	glClearColor(0, 0, 0, 0);
	GLfloat mat_specular[] = { 1,1,1,1 };
	GLfloat shineness[] = { 100 };
	GLfloat light0_pos[] = { 0,10,0,1 };
	GLfloat light1_pos[] = { 0,-1,-2 };
	GLfloat white[] = {1.0,1.0,1.0,1 };
	GLfloat ambient[] = { 0.1,0.1,0.1,1.0 };

	glShadeModel(GL_SMOOTH);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, shineness);
	glColorMaterial(GL_FRONT, GL_AMBIENT);
	glLightfv(GL_LIGHT0, GL_POSITION, light0_pos);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, white);
	glLightfv(GL_LIGHT0, GL_SPECULAR, white);
	glLightfv(GL_LIGHT1, GL_POSITION, light1_pos);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, white);
	glLightfv(GL_LIGHT1, GL_SPECULAR, white);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	int err = glGetError();
	if (err != 0) {
		fprintf(stderr, "Error at init:%d\n", err);
	}
}
int main(int argc, char **argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowPosition(400, 300);
	glutInitWindowSize(400, 300);
	glutCreateWindow("Running");
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	init();
	glutDisplayFunc(display);
	glutTimerFunc(50, runtimer, 1);
	glutReshapeFunc(reshape);
	glutSpecialFunc(processSpecial);
	glutSpecialUpFunc(processSpecialUp);
	glutKeyboardFunc(processKeyboard);
	glutKeyboardUpFunc(processKeyUp);
	InitTexture();
	glutMainLoop();
	return 0;
}