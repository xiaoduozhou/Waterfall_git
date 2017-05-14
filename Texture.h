#ifndef GL_TEXTURE_LOADER
#define GL_TEXTURE_LOADER

typedef struct													// ����һ���ṹ��
{
	GLubyte	*imageData;											// ͼ������ (���32bit)
	GLuint	bpp;												// ÿһ���ص�ͼ����ɫ���
	GLuint	width;												// ͼ����
	GLuint	height;												// ͼ��߶�
	GLuint	texID;												// ����ID
} TextureTga;

// ����BMP,JPG,GIF���ļ�
BOOL BuildTexture(char *szPathName, GLuint &texid);

// ����TGA�ļ�
BOOL BuildTexture(char *filename, TextureTga *texture);

#endif