// �ڴ˴����ó���Ҫ��ĸ���ͷ�ļ�:
#include "My_Window.h"
#include "Texture.h"
#include <time.h>
#include<stdio.h>
#include "math.h"
#include "perlin_noise.h"
#define	MAX_PARTICLES	7000
#define MAX_TAIL 3
#define HORN  5   //�ϰ���Ľ���
#define radius 3 //�ϰ���İ뾶
// �ڴ˴��������Ҫ��Ŀ⵽��������:
#pragma comment(lib, "opengl32.lib")   
#pragma comment(lib, "glu32.lib")
#pragma comment(lib, "glaux.lib")

// �ڴ˴�����ȫ�ֱ���:
GL_Window*	OGL_window;   
Keys*		OGL_keys;

// �ڴ˴������û�����:
#define	MAXTEXTURE	2													// ����������ͼ����
GLuint texture[MAXTEXTURE];												// �������飬������������(name)
GLint loop,TailLen;

struct Point_1
{
	float x;
	float y;
};
typedef struct 
{
	float	life;														// ��������
	float	fade;														// ����˥���ٶ�
	float	r,g,b;														// ���ӵ���ɫ
	float	x,y,z;														// ����λ��
	float	xSpeed,ySpeed,zSpeed;										// ���ӵ��ٶ�
	float	xg,yg,zg;													// ���ӵ��˶����ٶ�	
} PARTICLES;
PARTICLES particle[MAX_PARTICLES][MAX_TAIL];
PARTICLES bubble[MAX_PARTICLES];
GLfloat	speedFator = 0.033f;	
void BubbleInit();
void ParticleInit();
BOOL Initialize(GL_Window* window, Keys* keys)						// ��ʼ������
{
	// ����ȫ�ֱ���
	OGL_window	= window;
	OGL_keys	= keys;

	// �ڴ˴���ʼ�����Ƴ���
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);							// ����Ϊ��ɫ
	glClearDepth(1.0f);												// ������Ȼ���
//	glDepthFunc(GL_LEQUAL);
	glEnable(GL_DEPTH_TEST);										// �ر���Ȳ���
	glShadeModel(GL_SMOOTH);										// �����������ƽ����ʽ
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);				// �ϸ��͸�Ӽ���	
	srand( (unsigned)time(NULL) );		 	
    BuildTexture("data/waterfall.jpg", texture[1]);
	BuildTexture("data/star.bmp", texture[0]);
	ParticleInit();	
	BubbleInit();
	return TRUE;													// ��ʼ���ɹ�����TRUE
}

void DrawParticle();

void DrawMountain();
void BubbleInit();
void DrawBubble();
void DrawSceneGL(void)												// ���Ƴ���
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);				// �����ɫ����Ȼ���
	glLoadIdentity();												// ���õ�ǰ����
	// �ڴ˴���Ӵ�����л���:	
	DrawMountain();
	//DrawParticle();														// �������ӻ��ƺ���	
	DrawBubble();
	glFlush();														// ˢ��GL�������
}

void Deinitialize(void)												// �ڴ˴����˳�ǰɨβ����
{
}
//***********************************SOME FUNCTIONS************************

void ParticleInit(void)
{
	for (loop=0; loop<MAX_PARTICLES; loop++)
	{
		particle[loop][0].life=1.0f;
		particle[loop][0].fade=float(rand()%100)/20000.0f+0.0013f;////////////////////////////

		particle[loop][0].x = float(rand() % 28) - 43.0f;
		particle[loop][0].y = float(rand()%2)+20;
		particle[loop][0].z = 0;
		particle[loop][0].xSpeed =8.0f+float(rand()%2);;
		particle[loop][0].ySpeed =0.0f;
		particle[loop][0].zSpeed =0.0f;
		
		particle[loop][0].xg =0.0f;
		particle[loop][0].yg =-9.8f;
		particle[loop][0].zg =0.0f;			
	}
}

void Collision_detection(float x,float y)
{
	
}


Point_1 Collision_point(Point_1 p0, Point_1 p1, Point_1 Cen_circle)//�õ���ײ�� p0��Բ��һ�㣬p1��Բ��һ�㣬Cen_circle��Բ��
{
	Point_1 M;
	float a, b, c, t1, t2, t, m = Cen_circle.x, n = Cen_circle.y;
	a = (p1.x - p0.x)*(p1.x - p0.x) + (p1.y - p0.y)*(p1.y - p0.y);
	b =  2 * ((p0.x - m)*(p1.x - p0.x) + (p0.y - n)*(p1.y - p0.y));
	c = p0.x*p0.x + p0.y*p0.y - 2 * m*p0.x - 2 * n*p0.y + m*m + n*n - radius;
	t1 = (-b + sqrt(b*b - radius * a*c)) / (2 * a);
	t2 = (-b - sqrt(b*b - radius * a*c)) / (2 * a);
	t = (t1 >= 0 && t1 <= 1) ? t1 : t2;
	M.x = (p1.x - p0.x)*t + p0.x;
	M.y = (p1.y - p0.y)*t + p0.y;
	return M;
}

Point_1 Collision_reaction(Point_1 incident_point, Point_1 collision_point, Point_1 Cen_cir_point)//�õ���������
{
	Point_1 N, L, Unitized_N, Unitized_L, K;//N,L,K�ֱ�������,Unitized_N,Unitized_L�ǵ�λ��������
	L.x = (incident_point.x - collision_point.x);
	L.y = (incident_point.y - collision_point.y);
	Unitized_L.x = L.x / sqrt(L.x*L.x + L.y*L.y);
	Unitized_L.y = L.y / sqrt(L.x*L.x + L.y*L.y);
	N.x = (collision_point.x - Cen_cir_point.x);
	N.y = (collision_point.y - Cen_cir_point.y);
	Unitized_N.x = N.x / sqrt(N.x*N.x + N.y*N.y);
	Unitized_N.y = N.y / sqrt(N.x*N.x + N.y*N.y);
	K.x = 2 * Unitized_N.x*(Unitized_N.x*Unitized_L.x + Unitized_N.y*Unitized_L.y) - Unitized_L.x;
	K.y = 2 * Unitized_N.y*(Unitized_N.x*Unitized_L.x + Unitized_N.y*Unitized_L.y) - Unitized_L.y;
	return K;//��������
}




void DrawParticle(void)
{									
	glPushMatrix();	
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glEnable(GL_BLEND);	
	glEnable(GL_TEXTURE_2D);
	Point_1 Cen_circle;
	Cen_circle.x = -34, Cen_circle.y = -14;  //-32 -16
	for (loop=0; loop<MAX_PARTICLES; loop++)						// ѭ������
	{								
		glDisable(GL_DEPTH_TEST);	
		glBindTexture(GL_TEXTURE_2D, texture[0]);	
		if ((particle[loop][0].x - Cen_circle.x)*(particle[loop][0].x - Cen_circle.x) + (particle[loop][0].y - Cen_circle.y)*(particle[loop][0].y - Cen_circle.y) > radius*radius)
		{
		
			glColor4f(0.7, 0.7, 0.9, 0.9);
			glBegin(GL_TRIANGLE_STRIP);		//DRAW TAIL
			glTexCoord2f(1.0f, 1.0f); glVertex3f(particle[loop][TailLen].x + 0.4f, particle[loop][TailLen].y + 0.4f, particle[loop][TailLen].z);
			glTexCoord2f(0.0f, 1.0f); glVertex3f(particle[loop][TailLen].x - 0.4f, particle[loop][TailLen].y + 0.4f, particle[loop][TailLen].z);
			glTexCoord2f(1.0f, 0.0f); glVertex3f(particle[loop][TailLen].x + 0.4f, particle[loop][TailLen].y - 0.4f, particle[loop][TailLen].z);
			glTexCoord2f(0.0f, 0.0f); glVertex3f(particle[loop][TailLen].x - 0.4f, particle[loop][TailLen].y - 0.4f, particle[loop][TailLen].z);
			glEnd();
			glEnable(GL_DEPTH_TEST);
			particle[loop][1].x = particle[loop][0].x;
			particle[loop][1].y = particle[loop][0].y;
			particle[loop][1].z = particle[loop][0].z;
			particle[loop][1].xSpeed = particle[loop][0].xSpeed;
			particle[loop][1].ySpeed = particle[loop][0].ySpeed;
			particle[loop][1].zSpeed = particle[loop][0].zSpeed;
			//������һ�������˶���λ��
			// ����λ�ø���
			particle[loop][0].x      += particle[loop][0].xSpeed / (speedFator*1000.0f);
			particle[loop][0].y      += particle[loop][0].ySpeed / (speedFator*1000.0f);
			particle[loop][0].z      += particle[loop][0].zSpeed / (speedFator*1000.0f);
			// �����ٶȸ���
			particle[loop][0].xSpeed += particle[loop][0].xg / (speedFator*1000.0f);
			particle[loop][0].ySpeed += particle[loop][0].yg / (speedFator*1000.0f);
			particle[loop][0].zSpeed += particle[loop][0].zg / (speedFator*1000.0f);	

			
		}
		else
		{
			
			Point_1 p0, p1, collision_point, reflection_vector;//collision_point����ײ�� reflection_vector�Ƿ�������
			p0.x = particle[loop][1].x;
			p0.y = particle[loop][1].y;
			p1.x = particle[loop][0].x;	   
			p1.y = particle[loop][0].y;
			
			collision_point = Collision_point(p0, p1, Cen_circle);
			reflection_vector = Collision_reaction(p0, collision_point, Cen_circle);
			particle[loop][1].x = collision_point.x;
			particle[loop][1].y = collision_point.y;//������һ��״̬

			particle[loop][0].x = reflection_vector.x + collision_point.x;
			particle[loop][0].y = reflection_vector.y + collision_point.y;//��������������ײ��õ������
			
			
			float v = sqrt(particle[loop][1].xSpeed*particle[loop][1].xSpeed + particle[loop][1].ySpeed*particle[loop][1].ySpeed);
			float angle = atan(reflection_vector.y / reflection_vector.x);
			particle[loop][0].xSpeed = 0.3*v*sin(angle);
			particle[loop][0].ySpeed = 0.4*v*cos(angle);
			//particle[loop][0].life *= 2.5;
			//particle[loop][0].life += 2 * particle[loop][0].ySpeed / particle[loop][0].yg *particle[loop][0].fade;
		//	particle[loop][0].life += 200 * particle[loop][0].fade;

		}
		particle[loop][0].life -= particle[loop][0].fade;	// ����������˥�� 
	
		if (particle[loop][0].y < -19.5)
		{	   
			particle[loop][0].xSpeed += rand() % 3;
			particle[loop][0].ySpeed = rand() % 2 + 6;
		}
		if (particle[loop][0].life <= 0.0f)			// �������˥����0
		{
	        particle[loop][0].life = 1.3f;				// ����������
			particle[loop][0].fade = ((-1)*float(rand() % 3) + 96) / 10000;

			//particle[loop][0].x = float(rand() % 24) - 40.0f;
			particle[loop][0].x =  float(rand() % 28) - 43.0f;
			particle[loop][0].y = float(rand() % 3) + 22;
			particle[loop][0].z = float(-1 * (rand() % 10) - 85.0f);

			particle[loop][1].x = 0;
			particle[loop][1].y = 0;
			particle[loop][1].z = 0;
			///////////////////////////////////////////////////////////////////////////////////////////////////////
			int rand = PerlinNoise_2D(particle[loop][0].x, particle[loop][0].y);
			particle[loop][0].xSpeed = 2.5 + fmod(rand * 10, 5.0);////
			particle[loop][0].ySpeed = 0.0f;
			particle[loop][0].zSpeed = 0.0f;				// z���ٶ��뷽��

			particle[loop][0].xg = 0.0f;										// ��x��ļ��ٶ�
			particle[loop][0].yg = -9.8f;										// ��y��ļ��ٶ�
			particle[loop][0].zg = 0.0f;										// ��z��ļ��ٶ�													
		}
	
	}

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
	glPopMatrix();
}
void DrawMountain(void)
{
	glPushMatrix();
//	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glBindTexture(GL_TEXTURE_2D, texture[1]);
	glEnable(GL_TEXTURE_2D);
							
	glBegin(GL_QUADS);	
	glColor4f(0.35f,0.65f,1.0f,0.55);
	glTexCoord2f(0.0f, 0.0f); 	glVertex3f(-50, -35, -75);	
	glTexCoord2f(1.0f, 0.0f);	glVertex3f( 50, -35, -75);
	
    glColor4f(1.0f,1.0f,1.0f,0.75);
	glTexCoord2f(1.0f, 1.0f); 	glVertex3f( 50, 30, -75);	
	glTexCoord2f(0.0f, 1.0f); 	glVertex3f(-50, 30, -75);
	glEnd(); 
		
	glDisable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);
	
	glPopMatrix();
}
void BubbleInit(void)
{

	for (loop = 0; loop<MAX_PARTICLES; loop++)
	{
		bubble[loop].life = 1.0f;
		bubble[loop].fade = float(rand() % 100) / 20000.0f + 0.0003f;
		bubble[loop].x = float(rand() % 35) - 35.0f;
		bubble[loop].y = float(rand() % 2) + 20;
		bubble[loop].z = float(-1 * (rand() % 10) - 85.0f) - 100;

		bubble[loop].xSpeed = 8.0f + float(rand() % 2);;
		bubble[loop].ySpeed = 0.0f;
		bubble[loop].zSpeed = 0.0f;

		bubble[loop].xg = 0.0f;
		bubble[loop].yg = -9.8f;
		bubble[loop].zg = 0.0f;

	}
}
void DrawBubble(void)
{
	glPushMatrix();
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glEnable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);
	float x, y, z;
	for (loop = 0; loop<MAX_PARTICLES; loop++)						// ѭ������
	{
		x = bubble[loop].x;											// ������������
		y = bubble[loop].y;
		z = bubble[loop].z;
		glColor4f(0.5, 0.5, 0.5, bubble[loop].life);
		glBegin(GL_TRIANGLE_STRIP);										// ��������
		glTexCoord2f(1.0f, 1.0f); glVertex3f(x + 0.5f, y + 0.5f, z);
		glTexCoord2d(0.0f, 1.0f); glVertex3f(x - 0.5f, y + 0.5f, z);
		glTexCoord2d(1.0f, 0.0f); glVertex3f(x + 0.5f, y - 0.5f, z);
		glTexCoord2d(0.0f, 0.0f); glVertex3f(x - 0.5f, y - 0.5f, z);
		glEnd();
		// ����λ�ø���
		bubble[loop].x += bubble[loop].xSpeed / (speedFator*1000.0f);
		bubble[loop].y += bubble[loop].ySpeed / (speedFator*1000.0f);
		bubble[loop].z += bubble[loop].zSpeed / (speedFator*1000.0f);
		// �����ٶȸ���
		bubble[loop].xSpeed += bubble[loop].xg / (speedFator*1000.0f);
		bubble[loop].ySpeed += bubble[loop].yg / (speedFator*1000.0f);
		bubble[loop].zSpeed += bubble[loop].zg / (speedFator*1000.0f);

		bubble[loop].life -= bubble[loop].fade;	// ����������˥�� 		 	
		if (bubble[loop].life <= 0.0f)			// �������˥����0
		{
			bubble[loop].life = 1.0f;				// ����������
			bubble[loop].fade = ((-1)*float(rand() % 3) + 96) / 10000;

			bubble[loop].x = float(rand() % 35) - 8.0f;
			bubble[loop].y = float(rand() % 1) - 28;
			
			bubble[loop].xSpeed = 0.0f + float(rand() % 2);
			bubble[loop].ySpeed = 1.0f;

			bubble[loop].xg = 0.0f;										// ��x��ļ��ٶ�
			bubble[loop].yg = 1.0f;										// ��y��ļ��ٶ� 														
		}
	}
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
	glPopMatrix();
}
