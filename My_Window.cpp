#include "My_Window.h"

void TerminateApplication(GL_Window* window)							// ��������
{
	PostMessage(window->hWnd, WM_QUIT, 0, 0);							// ����WM_QUIT��Ϣ
	g_isProgramLooping = FALSE;											// ֹͣ����
} 

void ResizeWindowGL(int width, int height)								// �������ô��ڴ�С
{
	glViewport(0, 0, (GLsizei)(width), (GLsizei)(height));				// ���õ�ǰ�ӿڴ�С
	glMatrixMode(GL_PROJECTION);										// �л���ͶӰ����ģʽ
	glLoadIdentity();													// ����ͶӰ����
	gluPerspective(45, (float)width/(float)height, 0.1, 100);			// ����͸��ͶӰ
	glMatrixMode(GL_MODELVIEW);											// �л���ģ����ͼ����
	glLoadIdentity();													// ����ģ����ͼ����
}

BOOL ChangeScreenResolution(int width, int height, int bitsPerPixel)	// �޸���Ļ�ֱ���
{
	DEVMODE dmScreenSettings;											// �豸����ģʽ 
	ZeroMemory(&dmScreenSettings, sizeof(DEVMODE));						// ���
	dmScreenSettings.dmSize			= sizeof(DEVMODE);					// Devmode�ṹ�Ĵ�С
	dmScreenSettings.dmPelsWidth	= width;							// ����Ϊ��Ļ���
	dmScreenSettings.dmPelsHeight	= height;							// ����Ϊ��Ļ�߶�
	dmScreenSettings.dmBitsPerPel	= bitsPerPixel;						// ��Ϊָ��λ��
	dmScreenSettings.dmFields		= DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
	// ����������ʾģʽ�����ؽ����ע: CDS_FULLSCREEN ��ȥ��״̬��
	if (ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
	{
		return FALSE;													// �ֱ����޸�ʧ��,���� FALSE
	}
	return TRUE;														// �ֱ����޸ĳɹ�,���� TRUE
}

BOOL CreateWindowGL(GL_Window* window)									// ����OpenGL����
{
	DWORD windowStyle = WS_OVERLAPPEDWINDOW;							// ���ô�����ʽ
	DWORD windowExtendedStyle = WS_EX_APPWINDOW;						// ���ô�����չ��ʽ

	PIXELFORMATDESCRIPTOR pfd =											// ���ظ�ʽ������(pfd)������
	{
		sizeof(PIXELFORMATDESCRIPTOR),									// ���صĳߴ�
		1,																// �汾��
		PFD_DRAW_TO_WINDOW |											// pfd����֧�ִ��ڻ���
		PFD_SUPPORT_OPENGL |											// pfd����֧��OpenGL
		PFD_DOUBLEBUFFER,												// pfd����֧��˫����
		PFD_TYPE_RGBA,													// ���ظ�ʽΪRGBA
		window->init.bitsPerPixel,										// ������ɫ���
		0, 0, 0, 0, 0, 0,												// ������ɫλ��
		0,																// ��Alpha����
		0,																// ����ƫ��λ
		0,																// ���ۻ�����
		0, 0, 0, 0,														// �����ۻ�����λ 
		16,																// ��Ȼ���Ϊ16λ 
		0,																// ��ģ�建��
		0,																// �޸�������
		PFD_MAIN_PLANE,													// ��Ҫ���Ʋ�
		0,																// ����λ
		0, 0, 0															// ���Բ�����
	};

	RECT windowRect = {0, 0, window->init.width, window->init.height};	// ���崰�ڴ�С

	GLuint PixelFormat;													// �������ظ�ʽ

	if (window->init.isFullScreen == TRUE)								// �л�ȫ��
	{
		if (ChangeScreenResolution(window->init.width, window->init.height, window->init.bitsPerPixel) == FALSE)
		{
			// ȫ���л�ʧ��
			MessageBox(HWND_DESKTOP, "�޷��л���ȫ��ģʽ,�ڴ���ģʽ������.\nMode Switch Failed,Running In Windowed Mode.", "Error", MB_OK | MB_ICONEXCLAMATION);
			window->init.isFullScreen = FALSE;							// ���� isFullscreen ΪFALSE
		}
		else															// ȫ���л��ɹ�
		{
			ShowCursor (FALSE);											// �������
			windowStyle = WS_POPUP;										// ���ô�����ʽ
			windowExtendedStyle |= WS_EX_TOPMOST;						// ���ô�����չ��ʽ
		}
	}
	else
	{
		// �������ڴ�С,�������ڱ߽�
		AdjustWindowRectEx(&windowRect, windowStyle, 0, windowExtendedStyle);
	}

	// ��ʼ���� OpenGL ����
	window->hWnd = CreateWindowEx(windowExtendedStyle,					// ������չ��ʽ
									window->init.application->className,// Ӧ�ó�������
									window->init.title,					// ���ڱ���
									windowStyle,						// ������ʽ
									0, 0,								// ���ڵ� X,Y ����λ��
									windowRect.right - windowRect.left,	// ���ڿ��
									windowRect.bottom - windowRect.top,	// ���ڸ߶�
									HWND_DESKTOP,						// ������Ϊ����
									0,									// �޲˵�
									window->init.application->hInstance,// ����Ӧ�ó���ʵ�� 
									window);

	if (window->hWnd == 0)												// �����Ƿ�ɹ�����
	{
		return FALSE;													// ��ʧ��,�򷵻�FALSE
	}

	window->hDC = GetDC(window->hWnd);									// ȡ�õ�ǰ���ڵ��豸������
	if (window->hDC == 0)												// ��δ�õ��豸������
	{
		DestroyWindow(window->hWnd);									// ���ٸô���
		window->hWnd = 0;												// ���ھ������
		return FALSE;													// ����FALSE
	}

	PixelFormat = ChoosePixelFormat(window->hDC, &pfd);					// ѡ����ݵ����ظ�ʽ
	if (PixelFormat == 0)												// ��ѡ��ʧ��
	{
		ReleaseDC(window->hWnd, window->hDC);							// �ͷ��豸������
		window->hDC = 0;												// ���豸����������
		DestroyWindow(window->hWnd);									// ���ٴ���
		window->hWnd = 0;												// ���ھ������
		return FALSE;													// ����FALSE
	}

	if (SetPixelFormat(window->hDC, PixelFormat, &pfd) == FALSE)		// �������ظ�ʽ���ж��Ƿ�ʧ��
	{
		ReleaseDC(window->hWnd, window->hDC);							// �ͷ��豸������
		window->hDC = 0;												// ���豸����������
		DestroyWindow(window->hWnd);									// ���ٴ���
		window->hWnd = 0;												// ���ھ������
		return FALSE;													// ����FALSE
	}

	window->hRC = wglCreateContext(window->hDC);						// ȡ�û���������
	if (window->hRC == 0)												// ��δ�õ�����������
	{
		ReleaseDC(window->hWnd, window->hDC);							// �ͷ��豸������ 
		window->hDC = 0;												// ���豸����������
		DestroyWindow(window->hWnd);									// ���ٴ���
		window->hWnd = 0;												// ���ھ������
		return FALSE;													// ����FALSE
	}

	if (wglMakeCurrent(window->hDC, window->hRC) == FALSE)				// ���û����������ж��Ƿ�ʧ��
	{
		wglDeleteContext(window->hRC);									// ɾ������������
		window->hRC = 0;												// ����������������
		ReleaseDC(window->hWnd, window->hDC);							// �ͷ��豸������
		window->hDC = 0;												// ���豸����������
		DestroyWindow(window->hWnd);									// ���ٴ���
		window->hWnd = 0;												// ���ھ������
		return FALSE;													// ����FALSE
	}

	ShowWindow(window->hWnd, SW_NORMAL);								// ��ʾ����
	ResizeWindowGL(window->init.width, window->init.height);			// ���贰��
	ZeroMemory(window->keys, sizeof(Keys));								// ��ռ��̻�����

	return TRUE;														// ���ڴ����ɹ�
}

BOOL DestroyWindowGL(GL_Window* window)									// ���ٴ��ڲ��ͷų������õ���Դ
{
	if (window->hWnd != 0)												// �����о��?
	{	
		if (window->hDC != 0)											// �����Ƿ��еõ�����������?
		{
			wglMakeCurrent(window->hDC, 0);								// ����ǰ������ָ����Ϊ0
			if (window->hRC != 0)										// �ô����Ƿ��л���������
			{
				wglDeleteContext(window->hRC);							// �ͷŻ���������
				window->hRC = 0;										// ����������������
			}
			ReleaseDC(window->hWnd, window->hDC);						// �ͷ��豸������
			window->hDC = 0;											// ���豸����������
		}
		DestroyWindow(window->hWnd);									// ���ٴ���
		window->hWnd = 0;												// �����ھ������
	}

	if (window->init.isFullScreen)										// ��������ȫ��ģʽ��
	{
		ChangeDisplaySettings(NULL, 0);									// �л�Ϊ����ֱ���
		ShowCursor(TRUE);												// ��ʾ���
	}	
	return TRUE;														// ����TRUE
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{

	GL_Window* window = (GL_Window*)(GetWindowLong(hWnd, GWL_USERDATA));

	switch (uMsg)														// ȡ��Window����Ϣֵ
	{
	case WM_SYSCOMMAND:													// ��ȡϵͳ����
		{
			switch (wParam)												// ����ϵͳ����
			{
			case SC_SCREENSAVE:											// Ҫ��������(Screensaver)?
			case SC_MONITORPOWER:										// ��ʾ������ڵ�ģʽ(Powersave)?
				return 0;												// ��ǰ����0,��ֹϵͳ����ִ��
			}
			break;														// �˳�
		}

	case WM_CREATE:														// ����Window
		{
			CREATESTRUCT* creation = (CREATESTRUCT*)(lParam);			// ���洰�ڽṹָ��
			window = (GL_Window*)(creation->lpCreateParams);
			SetWindowLong(hWnd, GWL_USERDATA, (LONG)(window));			// �ı䴰������
			return 0;													// ����
		}
	
	case WM_CLOSE:														// �رմ���
		{
			PostMessage(window->hWnd, WM_QUIT, 0, 0);					// ��������
			g_isProgramLooping = FALSE;
			return 0;
		}

	case WM_SIZE:														// WM_SIZE��Ϣ����
		switch (wParam)													// �ж���Ϣ���
		{
		case SIZE_MINIMIZED:											// ������С��?
			window->isVisible = FALSE;									// ����isVisible Ϊ FALSE
			return 0;

		case SIZE_MAXIMIZED:											// �������?
			window->isVisible = TRUE;									// ����isVisible Ϊ TRUE
			ResizeWindowGL (LOWORD (lParam), HIWORD (lParam));			// �޸Ĵ��ڴ�СΪ Width=LoWord, Height=HiWord
			return 0;													// ����

		case SIZE_RESTORED:												// ���ڻ�ԭ?
			window->isVisible = TRUE;									// ����isVisible Ϊ TRUE
			ResizeWindowGL (LOWORD (lParam), HIWORD (lParam));			// �޸Ĵ��ڴ�СΪ Width=LoWord, Height=HiWord
			return 0;													// ����
		}
		break;															// �˳�

	case WM_KEYDOWN:													// ���¼��̻�����
		if ((wParam >= 0) && (wParam <= 255))							// �����Ƿ�Ϸ�?
		{
			window->keys->keyDown[wParam] = TRUE;						// ����Ӧ��Ϊ TRUE (�����µļ�)
			return 0;													// ����
		}
		break;															// �˳�

	case WM_KEYUP:														// �ɿ�������ʱ����¼��̻���
		if ((wParam >= 0) && (wParam <= 255))							// �����Ƿ�Ϸ�?
		{
			window->keys->keyDown[wParam] = FALSE;						// ����Ӧ��Ϊ FALSE (���ͷŵļ�)
			return 0;													// ����
		}
		break;															// �˳�

	case WM_TOGGLEFULLSCREEN:											// �л�ȫ��/����ģʽ
		{
			g_createFullScreen = !g_createFullScreen;
			PostMessage(hWnd, WM_QUIT, 0, 0);
			break;														// �˳�
		}

	default:
		break;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);					// �������򲻴������Ϣ���� DefWindowProc 
}

BOOL RegisterWindowClass(Application* application)						// Ϊ��Ӧ�ó���ע��һ����.
{	
	WNDCLASSEX windowClass;												// ������
	ZeroMemory(&windowClass, sizeof(WNDCLASSEX));						// ����ڴ�
	windowClass.cbSize		= sizeof(WNDCLASSEX);						// ������Ĵ�С
	windowClass.style		= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;		// ���ƶ�,�ı��С��ʱ���ػ� 
	windowClass.lpfnWndProc		= (WNDPROC)(WindowProc);				// ��WindowProc����������Ϣ
	windowClass.hInstance		= application->hInstance;				// ����ʵ��
	windowClass.hbrBackground	= (HBRUSH)(COLOR_APPWORKSPACE);			// �౳���Ļ�ˢ��ɫ
	windowClass.hCursor			= LoadCursor(NULL, IDC_ARROW);			// �����ͷ���
	windowClass.lpszClassName	= application->className;				// ����Ӧ�ó��������
	if (RegisterClassEx(&windowClass) == 0)								// ע����ʧ��?
	{
		MessageBox(HWND_DESKTOP, "Ӧ�ó�����ע��ʧ��!\nRegisterClassEx Failed!", "Error", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;													// ����FALSE (ע��ʧ��)
	}
	return TRUE;														// ����TRUE	(ע��ɹ�)
}

// Ӧ�ó������� (WinMain)
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	Application			application;									// Ӧ�ó���
	GL_Window			window;											// ����
	Keys				keys;											// ���̰���
	MSG					msg;											// Window��Ϣ
	BOOL				isMessagePumpActive;

	// ��Ӧ�ó���ֵ
	application.className = CLASSNAME;									// ����������
	application.hInstance = hInstance;									// �������

	// ���������Ϣ����
	ZeroMemory(&window, sizeof(GL_Window));								// ���㴰�ڱ������ڴ�ռ�
	window.keys					= &keys;								// ���ð���
	window.init.application		= &application;							// ���ô��ڳ���
	window.init.title			= TITLE;								// ���ñ���
	window.init.width			= WIDTH;								// ���ô��ڿ��
	window.init.height			= HEIGHT;								// ���ô��ڸ߶�
	window.init.bitsPerPixel	= BPP;									// ����ÿ���ص�λ��
	window.init.isFullScreen	= FALSE;								// ���ó�ʼ�����Ƿ�ȫ����(FALSE)
	ZeroMemory(&keys, sizeof(Keys));									// ���̻�������

	if (RegisterWindowClass(&application) == FALSE)						// ע�����Ƿ�ʧ��
	{
		MessageBox(HWND_DESKTOP, "������ע��ʧ��!\nError Registering Window Class!", "Error", MB_OK | MB_ICONEXCLAMATION);
		return -1;														// ��������
	}

	g_isProgramLooping = TRUE;											// ��g_isProgramLooping��TRUE
	g_createFullScreen = window.init.isFullScreen;						// g_createFullScreen ��ΪĬ��ֵ
	while (g_isProgramLooping)											// ѭ��ֱ������ֹͣ
	{
		// ��������
		window.init.isFullScreen = g_createFullScreen;					// �����Ƿ�ȫ����Ϣ
		if (CreateWindowGL(&window) == TRUE)							// ����GL���ڳɹ���?
		{
			if (Initialize(&window, &keys) == FALSE)					// ��ʼ��
			{
				// ʧ��
				PostMessage(window.hWnd, WM_QUIT, 0, 0);				// �׳���ϢWM_QUIT
				g_isProgramLooping = FALSE;
			}
			else
			{	// ��ʼ���ɹ�
				isMessagePumpActive = TRUE;								// ��isMessagePumpActiveΪTRUE
				while (isMessagePumpActive == TRUE)						// ��isMessagePumpActiveΪTRUEʱ
				{
					// �ɹ��������ڣ����Window��Ϣ
					if (PeekMessage(&msg, window.hWnd, 0, 0, PM_REMOVE) != 0)
					{
						// ���WM_QUIT��Ϣ
						if (msg.message != WM_QUIT)						// WM_QUIT��Ϣ?
						{
							DispatchMessage(&msg);						// �������,������Ϣ
						}
						else											// ����(��Ϣ��WM_QUIT)
						{
							isMessagePumpActive = FALSE;				// ���� Message Pump
						}
					}
					else												// ���û����Ϣ
					{
						if (window.isVisible == FALSE)					// ������ڲ��ɼ�
						{
							WaitMessage();								// �ȴ���Ϣ
						}
						else											// ������ڿɼ�
						{
							DrawSceneGL();								// ���Ƴ���
							SwapBuffers(window.hDC);					// ��������
						}
					}
				}														// ��isMessagePumpActive ΪTRUEʱѭ��ִ��
			}

			// �������
			Deinitialize();												// ��ɨβ����
			DestroyWindowGL(&window);									// ���ٴ���
		}
		else															// ������ڴ���ʧ��
		{
			MessageBox(HWND_DESKTOP, "����OpenGL����ʧ��\nError Creating OpenGL Window", "Error", MB_OK | MB_ICONEXCLAMATION);
			g_isProgramLooping = FALSE;									// ����ѭ��
		}
	}																	
	UnregisterClass(application.className, application.hInstance);		// ע��������
	return 0;
}