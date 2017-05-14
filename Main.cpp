// 在此处引用程序要求的附加头文件:
#include "My_Window.h"
#include "Texture.h"
#include <time.h>
#include<stdio.h>
#include "math.h"
#include "perlin_noise.h"
#define	MAX_PARTICLES	7000
#define MAX_TAIL 3
#define HORN  5   //障碍物的角数
#define radius 3 //障碍物的半径
// 在此处加入程序要求的库到链接器中:
#pragma comment(lib, "opengl32.lib")   
#pragma comment(lib, "glu32.lib")
#pragma comment(lib, "glaux.lib")

// 在此处定义全局变量:
GL_Window*	OGL_window;   
Keys*		OGL_keys;

// 在此处定义用户变量:
#define	MAXTEXTURE	2													// 定义纹理贴图数量
GLuint texture[MAXTEXTURE];												// 纹理数组，保存纹理名字(name)
GLint loop,TailLen;

struct Point_1
{
	float x;
	float y;
};
typedef struct 
{
	float	life;														// 粒子生命
	float	fade;														// 粒子衰减速度
	float	r,g,b;														// 粒子的颜色
	float	x,y,z;														// 粒子位置
	float	xSpeed,ySpeed,zSpeed;										// 粒子的速度
	float	xg,yg,zg;													// 粒子的运动加速度	
} PARTICLES;
PARTICLES particle[MAX_PARTICLES][MAX_TAIL];
PARTICLES bubble[MAX_PARTICLES];
GLfloat	speedFator = 0.033f;	
void BubbleInit();
void ParticleInit();
BOOL Initialize(GL_Window* window, Keys* keys)						// 初始化场景
{
	// 设置全局变量
	OGL_window	= window;
	OGL_keys	= keys;

	// 在此处初始化绘制场景
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);							// 清屏为黑色
	glClearDepth(1.0f);												// 设置深度缓存
//	glDepthFunc(GL_LEQUAL);
	glEnable(GL_DEPTH_TEST);										// 关闭深度测试
	glShadeModel(GL_SMOOTH);										// 阴暗处理采用平滑方式
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);				// 最精细的透视计算	
	srand( (unsigned)time(NULL) );		 	
    BuildTexture("data/waterfall.jpg", texture[1]);
	BuildTexture("data/star.bmp", texture[0]);
	ParticleInit();	
	BubbleInit();
	return TRUE;													// 初始化成功返回TRUE
}

void DrawParticle();

void DrawMountain();
void BubbleInit();
void DrawBubble();
void DrawSceneGL(void)												// 绘制场景
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);				// 清除颜色和深度缓存
	glLoadIdentity();												// 重置当前矩阵
	// 在此处添加代码进行绘制:	
	DrawMountain();
	//DrawParticle();														// 调用粒子绘制函数	
	DrawBubble();
	glFlush();														// 刷新GL命令队列
}

void Deinitialize(void)												// 在此处做退出前扫尾工作
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


Point_1 Collision_point(Point_1 p0, Point_1 p1, Point_1 Cen_circle)//得到碰撞点 p0是圆外一点，p1是圆内一点，Cen_circle是圆心
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

Point_1 Collision_reaction(Point_1 incident_point, Point_1 collision_point, Point_1 Cen_cir_point)//得到反射向量
{
	Point_1 N, L, Unitized_N, Unitized_L, K;//N,L,K分别是向量,Unitized_N,Unitized_L是单位化的向量
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
	return K;//反射向量
}




void DrawParticle(void)
{									
	glPushMatrix();	
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glEnable(GL_BLEND);	
	glEnable(GL_TEXTURE_2D);
	Point_1 Cen_circle;
	Cen_circle.x = -34, Cen_circle.y = -14;  //-32 -16
	for (loop=0; loop<MAX_PARTICLES; loop++)						// 循环绘制
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
			//保留上一刻粒子运动的位置
			// 粒子位置更新
			particle[loop][0].x      += particle[loop][0].xSpeed / (speedFator*1000.0f);
			particle[loop][0].y      += particle[loop][0].ySpeed / (speedFator*1000.0f);
			particle[loop][0].z      += particle[loop][0].zSpeed / (speedFator*1000.0f);
			// 粒子速度更新
			particle[loop][0].xSpeed += particle[loop][0].xg / (speedFator*1000.0f);
			particle[loop][0].ySpeed += particle[loop][0].yg / (speedFator*1000.0f);
			particle[loop][0].zSpeed += particle[loop][0].zg / (speedFator*1000.0f);	

			
		}
		else
		{
			
			Point_1 p0, p1, collision_point, reflection_vector;//collision_point是碰撞点 reflection_vector是反射向量
			p0.x = particle[loop][1].x;
			p0.y = particle[loop][1].y;
			p1.x = particle[loop][0].x;	   
			p1.y = particle[loop][0].y;
			
			collision_point = Collision_point(p0, p1, Cen_circle);
			reflection_vector = Collision_reaction(p0, collision_point, Cen_circle);
			particle[loop][1].x = collision_point.x;
			particle[loop][1].y = collision_point.y;//保留上一个状态

			particle[loop][0].x = reflection_vector.x + collision_point.x;
			particle[loop][0].y = reflection_vector.y + collision_point.y;//反射向量加上碰撞点得到反射点
			
			
			float v = sqrt(particle[loop][1].xSpeed*particle[loop][1].xSpeed + particle[loop][1].ySpeed*particle[loop][1].ySpeed);
			float angle = atan(reflection_vector.y / reflection_vector.x);
			particle[loop][0].xSpeed = 0.3*v*sin(angle);
			particle[loop][0].ySpeed = 0.4*v*cos(angle);
			//particle[loop][0].life *= 2.5;
			//particle[loop][0].life += 2 * particle[loop][0].ySpeed / particle[loop][0].yg *particle[loop][0].fade;
		//	particle[loop][0].life += 200 * particle[loop][0].fade;

		}
		particle[loop][0].life -= particle[loop][0].fade;	// 粒子生命的衰减 
	
		if (particle[loop][0].y < -19.5)
		{	   
			particle[loop][0].xSpeed += rand() % 3;
			particle[loop][0].ySpeed = rand() % 2 + 6;
		}
		if (particle[loop][0].life <= 0.0f)			// 如果粒子衰减到0
		{
	        particle[loop][0].life = 1.3f;				// 赋予新生命
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
			particle[loop][0].zSpeed = 0.0f;				// z轴速度与方向

			particle[loop][0].xg = 0.0f;										// 沿x轴的加速度
			particle[loop][0].yg = -9.8f;										// 沿y轴的加速度
			particle[loop][0].zg = 0.0f;										// 沿z轴的加速度													
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
	for (loop = 0; loop<MAX_PARTICLES; loop++)						// 循环绘制
	{
		x = bubble[loop].x;											// 定义粒子坐标
		y = bubble[loop].y;
		z = bubble[loop].z;
		glColor4f(0.5, 0.5, 0.5, bubble[loop].life);
		glBegin(GL_TRIANGLE_STRIP);										// 绘制粒子
		glTexCoord2f(1.0f, 1.0f); glVertex3f(x + 0.5f, y + 0.5f, z);
		glTexCoord2d(0.0f, 1.0f); glVertex3f(x - 0.5f, y + 0.5f, z);
		glTexCoord2d(1.0f, 0.0f); glVertex3f(x + 0.5f, y - 0.5f, z);
		glTexCoord2d(0.0f, 0.0f); glVertex3f(x - 0.5f, y - 0.5f, z);
		glEnd();
		// 粒子位置更新
		bubble[loop].x += bubble[loop].xSpeed / (speedFator*1000.0f);
		bubble[loop].y += bubble[loop].ySpeed / (speedFator*1000.0f);
		bubble[loop].z += bubble[loop].zSpeed / (speedFator*1000.0f);
		// 粒子速度更新
		bubble[loop].xSpeed += bubble[loop].xg / (speedFator*1000.0f);
		bubble[loop].ySpeed += bubble[loop].yg / (speedFator*1000.0f);
		bubble[loop].zSpeed += bubble[loop].zg / (speedFator*1000.0f);

		bubble[loop].life -= bubble[loop].fade;	// 粒子生命的衰减 		 	
		if (bubble[loop].life <= 0.0f)			// 如果粒子衰减到0
		{
			bubble[loop].life = 1.0f;				// 赋予新生命
			bubble[loop].fade = ((-1)*float(rand() % 3) + 96) / 10000;

			bubble[loop].x = float(rand() % 35) - 8.0f;
			bubble[loop].y = float(rand() % 1) - 28;
			
			bubble[loop].xSpeed = 0.0f + float(rand() % 2);
			bubble[loop].ySpeed = 1.0f;

			bubble[loop].xg = 0.0f;										// 沿x轴的加速度
			bubble[loop].yg = 1.0f;										// 沿y轴的加速度 														
		}
	}
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
	glPopMatrix();
}
