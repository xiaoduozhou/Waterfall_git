#ifndef _My_WINDOW_H
#define _My_WINDOW_H

#include <windows.h>													// Windows��ͷ�ļ�
#include <gl\gl.h>														// OpenGL32���ͷ�ļ�
#include <gl\glu.h>														// Glu32���ͷ�ļ�
#include <gl\glaux.h>
#include <time.h>

#define WM_TOGGLEFULLSCREEN (WM_USER + 1)								// ����ȫ��/����ģʽ�л���Ϣ
#define TITLE		"WaterFall-By Shoarh"									// ���崰�ڱ���
#define CLASSNAME	"My_OPENGL"										// ���崰������
#define	WIDTH		640													// ���崰�ڿ��
#define HEIGHT		480													// ���崰�ڸ߶�
#define BPP			16													// ����ÿ���ص�λ��

// �ڴ˴������봰����صĽṹ��:
typedef struct {														// ���崦����̵Ľṹ��
	BOOL keyDown [256];													// �洢���̰���״̬������
} Keys;

typedef struct {														// ����洢Ӧ�ó���ʵ���Ľṹ��
	HINSTANCE		hInstance;											// Ӧ�ó���ʵ��
	const char*		className;											// Ӧ�ó�������
} Application;

typedef struct {														// �����ʼ����������Ϣ
	Application*	application;										// ������Ӧ�ó���
	char*			title;												// ���ڱ���
	int				width;												// ���ڿ��
	int				height;												// ���ڸ߶�
	int				bitsPerPixel;										// ÿ���ص�λ��
	BOOL			isFullScreen;										// �Ƿ�ȫ��
} GL_WindowInit;

typedef struct {														// ���崰�ڽṹ��
	Keys*			keys;												// ���ڵļ���
	HWND			hWnd;												// ���ھ��
	HDC				hDC;												// �豸������
	HGLRC			hRC;												// ����������
	GL_WindowInit	init;												// Window��ʼ����Ϣ�Ľṹ��
	BOOL			isVisible;											// �����Ƿ��ڻ̬
} GL_Window;

static BOOL g_isProgramLooping;
static BOOL g_createFullScreen;

// �˴���ģ���а����ĺ�����ǰ������:
void TerminateApplication (GL_Window* window);							// ��������
BOOL Initialize(GL_Window* window, Keys* keys);							// ���������ǰ�ĳ�ʼ��ֵ
void DrawSceneGL(void);													// ��������ɳ����Ļ���
void Update(void);														// �ڴ˴����¶���Ϣ�Ķ���
void Deinitialize(void);												// �ڴ˴����˳�ǰɨβ����

#endif																	// _My_WINDOW_H