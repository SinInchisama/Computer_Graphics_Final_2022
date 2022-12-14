#include <iostream>
#include <gl/glew.h> // �ʿ��� ������� include
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <gl/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <random>
#include <FreeImage.h>
#include <time.h>

using namespace std;

typedef struct Box {
	glm::vec3 Bscale;
	glm::vec3 Blocate;
	glm::vec3 Bcolor;

	int offset;
	float Color;

	glm::vec4 Bounding_box[2];
}Box;

typedef struct Player {
	glm::vec3 Pscale;
	glm::vec3 Plocate;
	glm::vec3 PColor;
	glm::vec3 Move;

	glm::vec3 cameraPos;
	glm::vec3 cameraDirection;

	int x,y, z;
	float gravity;
	int prev_locate[2];
	float lotate;
	bool view;

	float speed;

	bool left_rotate;
	bool right_rotate;
}Player;

typedef struct Texture {
	glm::vec3 Tlocate;
	int Trotate;
	glm::vec3 Tscale;
	glm::mat4 Ttr;
}Texture;

void make_vertexShader();
void make_fragmentShader();
void InitBuffer();
void InitShader();
void TimerFunction(int value);
void SKeyDownboard(int key, int x, int y);
void SKeyUpboard(int key, int x, int y);
GLvoid Reshape(int w, int h);
GLvoid drawScene();
GLvoid KeyDownboard(unsigned char key, int x, int y);
GLvoid KeyUpboard(unsigned char key, int x, int y);
char* filetobuf(const char* file);
void Boxinit(int x, int y);
void Draw_filed(BOOL View_draw_background);
void playerinit();
void Box_crash(Player* p);
void Player_camera(Player* p);
void Play_state();

void Crash(Player* p, int inspection);
int collide(Player* p, Box b, glm::mat4 TR);

void Drawtime();

void defineVertexArrayObject();
void Texture_init();
GLuint CreateTexture(char const* filename);



GLchar* vertexsource, * fragmentsource; //--- �ҽ��ڵ� ���� ����
GLuint vertexshader, fragmentshader; //--- ���̴� ��ü
GLuint s_program;
GLint result;
GLuint vao, vbo[2], EBO[2], linevbo[2], linevao;

GLuint Text_vertexShader;
GLuint Text_fragmentShader;

float Cameraz = 15.0f;
float Camerax = 0.0f;
float ySelfRot = 0.0f;
float yZeroRot = 0.0f;
float Viewz = -1.0f;
float Viewy = 15.0f;
float xScale;
float yScale;
float zScale;
float BlockSpeed = 0.0f;

bool ra, Drop = true,Game_over;
int xcount = 20, Ycount = 20;
time_t Now_time,start_time;
float yz = 0.0f;
Box All_Box[20][20];
Player player[2];
Texture Timecount[2];
Texture Worldbox[4];

GLuint trianglePositionVertexBufferObjectID, triangleColorVertexBufferObjectID;
GLuint triangleTextureCoordinateBufferObjectID;
GLuint triangleVertexArrayObject;
GLuint triangleShaderProgramID;

GLUquadricObj* qobj = gluNewQuadric();

glm::vec3 cameraPos;
glm::vec3 cameraPos_1;
glm::vec3 cameraDirection; //--- ī�޶� �ٶ󺸴� ����
glm::vec3 cameraDirection_1;
glm::vec3 cameraUp; //--- ī�޶� ���� ����

glm::vec3 originPos;

glm::mat4 view;
glm::mat4 projection;

GLuint texureId[11];

float vPositionList[] = {
0.5f, 1.0f, 0.5f, // �� ���� ���
0.5f, 1.0f, -0.5f, // �� ���� ���
-0.5f, 1.0f, -0.5f, // �� ���� ���
-0.5f, 1.0f, 0.5f, // �� ���� ���
-0.5f,0.0f,0.5f, // �� ���� �ϴ�
-0.5f,0.0f,-0.5f,	// �� ���� �ϴ�
0.5f,0.0f,-0.5f,	// �� ���� �ϴ�
0.5f,0.0f,0.5f,		// �� ���� �ϴ�

0.5f,0.0f,0.5f,
0.5f,0.0f,-0.5f,
-0.5f,0.0f,0.5f, // �� ���� �ϴ�
-0.5f,0.0f,-0.5f,	// �� ���� �ϴ�
0.0f,0.5f,0.0f
};

unsigned int index[] = {
1, 3, 0, // ù ��° �ﰢ��			// �� �簢��
1, 2, 3, // �� ��° �ﰢ��
5, 4, 2,									// ���� �簢��
3, 2, 4,
0, 7, 6,							// ������ �簢��
6, 1, 0,
3, 4, 7,							// �� �簢��
7, 0, 3,
6, 5, 2,								// �� �簢��
6, 2, 1,
6, 7, 4,	// �� ��° �ﰢ��			// �Ʒ� �簢��
6, 4, 5,	// �� ��° �ﰢ��

8,9,10,
10,9,11,
10,8,12,		// �� �ﰢ��
8,9,12,			// ������
9,11,12,		// ����
11,10,12		// ����
};

float line[]{
5.0f,0.0f,0.0f,
-5.0f,0.0f,0.0f,
0.0f,5.0f,0.0f,
0.0f,-5.0f,0.0f,
0.0f,0.0f,5.0f,
0.0f,0.0f,-5.0f
};


void main(int argc, char** argv) //--- ������ ����ϰ� �ݹ��Լ� ����
{
	//--- ������ �����ϱ�
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(1260, 700);
	glutCreateWindow("Example1");
	//--- GLEW �ʱ�ȭ�ϱ�
	glewExperimental = GL_TRUE;

	start_time = time(NULL);

	Texture_init();

	Boxinit(xcount, Ycount);
	glewInit();
	InitShader();
	InitBuffer();
	glutDisplayFunc(drawScene);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(KeyDownboard); // Ű���� �Է� �ݹ��Լ� ����
	glutKeyboardUpFunc(KeyUpboard);
	glutSpecialFunc(SKeyDownboard);
	glutSpecialUpFunc(SKeyUpboard);
	glutTimerFunc(50, TimerFunction, 1);
	glutMainLoop();
}

void make_vertexShader() {
	vertexsource = filetobuf("vertex.glsl");//--- ���ؽ� ���̴� ��ü �����
	vertexshader = glCreateShader(GL_VERTEX_SHADER);
	//--- ���̴� �ڵ带 ���̴� ��ü�� �ֱ�
	glShaderSource(vertexshader, 1, (const GLchar**)&vertexsource, 0);
	//--- ���ؽ� ���̴� �������ϱ�
	glCompileShader(vertexshader);
	//--- �������� ����� ���� ���� ���: ���� üũ
	GLchar errorLog[512];
	glGetShaderiv(vertexshader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(vertexshader, 512, NULL, errorLog);
		std::cerr << "ERROR: vertex shader ������ ����\n" << errorLog << std::endl;
		return;
	}

	const GLchar* vertexShaderSource =
		"#version 330 core\n"
		"in vec3 positionAttribute;"
		"uniform mat4 modelTransform;"
		"uniform mat4 viewTransform;"
		"uniform mat4 projectionTransform;"
		//"in vec3 colorAttribute;"
		"in vec2 textureCoordinateAttribute;"
		//"out vec3 passColorAttribute;"
		"out vec2 passTextureCoordinateAttribute;"
		"void main()"
		"{"
		"gl_Position = projectionTransform * viewTransform * modelTransform * vec4(positionAttribute, 1.0);"
		//"passColorAttribute = colorAttribute;"
		"passTextureCoordinateAttribute = textureCoordinateAttribute;"
		"}";
	Text_vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(Text_vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(Text_vertexShader);
}

void make_fragmentShader() {
	fragmentsource = filetobuf("fragment.glsl");
	//--- �����׸�Ʈ ���̴� ��ü �����
	fragmentshader = glCreateShader(GL_FRAGMENT_SHADER);
	//--- ���̴� �ڵ带 ���̴� ��ü�� �ֱ�
	glShaderSource(fragmentshader, 1, (const GLchar**)&fragmentsource, 0);
	//--- �����׸�Ʈ ���̴� ������
	glCompileShader(fragmentshader);
	//--- �������� ����� ���� ���� ���: ������ ���� üũ
	GLchar errorLog[512];
	glGetShaderiv(fragmentshader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(fragmentshader, 512, NULL, errorLog);
		std::cerr << "ERROR: fragment shader ������ ����\n" << errorLog << std::endl;
		return;
	}

	const GLchar* fragmentShaderSource =
		"#version 330 core\n"
		"in vec3 passColorAttribute;"
		"in vec2 passTextureCoordinateAttribute;"
		"out vec4 fragmentColor;"
		"uniform sampler2D tex;"
		"void main()"
		"{"
		//�÷��� ���
		//"fragmentColor = vec4(passColorAttribute, 1.0);"
		//�ؽ�ó�� ���
		"fragmentColor = texture(tex, passTextureCoordinateAttribute);"
		//�ؽ�ó�� �÷� ���� ���
		//"fragmentColor = texture(tex, passTextureCoordinateAttribute)*vec4(passColorAttribute, 1.0); "
		"}";
	Text_fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(Text_fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(Text_fragmentShader);

	glGetShaderiv(Text_fragmentShader, GL_COMPILE_STATUS, &result);
}

void InitShader()
{
	make_vertexShader(); //--- ���ؽ� ���̴� �����
	make_fragmentShader(); //--- �����׸�Ʈ ���̴� �����
	//-- shader Program
	s_program = glCreateProgram();
	glAttachShader(s_program, vertexshader);
	glAttachShader(s_program, fragmentshader);
	glLinkProgram(s_program);

	GLchar errorLog[512];
	if (!result)
	{
		glGetShaderInfoLog(fragmentshader, 512, NULL, errorLog);
		std::cerr << "ERROR: fragment shader ������ ����\n" << errorLog << std::endl;
		return;
	}
	//--- ���̴� �����ϱ�
	glDeleteShader(vertexshader);
	glDeleteShader(fragmentshader);
	//--- Shader Program ����ϱ�

	triangleShaderProgramID = glCreateProgram();

	glAttachShader(triangleShaderProgramID, Text_vertexShader);
	glAttachShader(triangleShaderProgramID, Text_fragmentShader);

	glLinkProgram(triangleShaderProgramID);


	glDeleteShader(Text_vertexShader);
	glDeleteShader(Text_fragmentShader);


	glGetProgramiv(triangleShaderProgramID, GL_LINK_STATUS, &result);
}

void InitBuffer()
{
	glGenVertexArrays(1, &vao); //--- VAO �� �����ϰ� �Ҵ��ϱ�
	glBindVertexArray(vao); //--- VAO�� ���ε��ϱ�
	glGenBuffers(2, vbo); //--- 2���� VBO�� �����ϰ� �Ҵ��ϱ�
	//--- 1��° VBO�� Ȱ��ȭ�Ͽ� ���ε��ϰ�, ���ؽ� �Ӽ� (��ǥ��)�� ����
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vPositionList), vPositionList, GL_STATIC_DRAW);
	glGenBuffers(1, &EBO[0]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[0]); //--- GL_ELEMENT_ARRAY_BUFFER ���� �������� ���ε�
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index), index, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glEnableVertexAttribArray(0);

	glGenVertexArrays(1, &linevao); //--- VAO �� �����ϰ� �Ҵ��ϱ�
	glBindVertexArray(linevao); //--- VAO�� ���ε��ϱ�
	glGenBuffers(2, linevbo); //--- 2���� VBO�� �����ϰ� �Ҵ��ϱ�
	glBindBuffer(GL_ARRAY_BUFFER, linevbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(line), line, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	defineVertexArrayObject();

	cameraPos = glm::vec3(Camerax, 17.0f, Cameraz);
	cameraDirection = glm::vec3(0.0f, 0.0f, -1.0f); //--- ī�޶� �ٶ󺸴� ����
	cameraUp = glm::vec3(0.0f, 1.0f, 0.0f); //--- ī�޶� ���� ����

}

void drawScene() {

	glUseProgram(s_program);
	glClearColor(1.0, 1.0, 1.0, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
	
	Play_state();

	glutSwapBuffers();
}

GLvoid Reshape(int w, int h) //--- �ݹ� �Լ�: �ٽ� �׸��� �ݹ� �Լ�
{
	glViewport(0, 0, w, h);
}

char* filetobuf(const char* file)
{
	FILE* fptr;
	long length;
	char* buf;
	fptr = fopen(file, "rb"); // Open file for reading 
	if (!fptr) // Return NULL on failure 
		return NULL;
	fseek(fptr, 0, SEEK_END); // Seek to the end of the file 
	length = ftell(fptr); // Find out how many bytes into the file we are 
	buf = (char*)malloc(length + 1); // Allocate a buffer for the entire length of the file and a null terminator 
	fseek(fptr, 0, SEEK_SET); // Go back to the beginning of the file 
	fread(buf, length, 1, fptr); // Read the contents of the file in to the buffer 
	fclose(fptr); // Close the file 
	buf[length] = 0; // Null terminator 
	return buf; // Return the buffer 
}

void TimerFunction(int value) {

	if (!Game_over) {
		for (int i = 0; i < 2; i++) {
			player[i].Move[0] += cos(glm::radians(-player[i].lotate)) * player[i].speed * player[i].x;			// �÷��̾� �¿� ������ �Ҷ� ���� ���
			player[i].Move[2] += sin(glm::radians(-player[i].lotate)) * player[i].speed * player[i].x;
			player[i].Move[0] += sin(glm::radians(player[i].lotate)) * player[i].speed * player[i].z;			// �÷��̾� �� �Ʒ� ������ �Ҷ� ���� ���
			player[i].Move[2] += cos(glm::radians(player[i].lotate)) * player[i].speed * player[i].z;
			player[i].Move[1] -= player[i].gravity;

			int temp = 0;

			glm::mat4 TR = glm::mat4(1.0f);
			glm::mat4 Tx = glm::mat4(1.0f);
			glm::mat4 Scale = glm::mat4(1.0f);

			TR = glm::mat4(1.0f);					// �÷��̾ �׷��ִ� �κ�.
			Tx = glm::mat4(1.0f);
			Scale = glm::mat4(1.0f);


			Scale = glm::scale(Scale, player[i].Pscale); //		�÷��̾�
			Tx = glm::translate(Tx, player[i].Plocate + player[i].Move);
			TR = Tx * Scale;

			for (int a = 0; a < xcount; a++) {
				for (int k = 0; k < Ycount; k++) {
					temp = collide(&player[i], All_Box[a][k], TR);
					if (temp != -1) {
						Crash(&player[i], temp);
						a = xcount;
						break;
					}
				}
			}
			// �����̰��� �ش� ��ġ ��� üũ	(����� �迭�� üũ�ϴ� ��. �迭�� üũ�� �� ���������� ���� ��ġ ����������ҵ�?)
			Player_camera(&player[i]);																// �÷��̾� ī�޶� ��ġ ���.
			time_t u = time(NULL);
			Now_time = u - start_time;

			if (Now_time == 60)
				Game_over = true;
		}
	}

	glutTimerFunc(50, TimerFunction, 1);
	glutPostRedisplay();
}

GLvoid KeyDownboard(unsigned char key, int x, int y) {
	if (key == 'a') {
		/*player[0].lotate += 10;		*/			// �÷��̾� ������
		player[1].x = -1;
	}
	else if (key == 'd') {
		player[1].x = +1;
	}
	else if (key == 'w') {
		player[1].z = -1;
	}
	else if (key == 's') {
		player[1].z = 1;
	}
	else if (key == 'q')
		player[0].lotate += 10;
	glutPostRedisplay();
}

GLvoid KeyUpboard(unsigned char key, int x, int y) {
	if (key == 'a') {
		player[1].x = 0;
	}
	else if (key == 'd') {
		player[1].x = 0;
	}
	else if (key == 'w') {
		player[1].z = 0;
	}
	else if (key == 's') {
		player[1].z = 0;
	}
}

void SKeyDownboard(int key, int x, int y) {
	switch (key) {
	
	case GLUT_KEY_LEFT:
		player[0].x = -1;
		break;
	case GLUT_KEY_RIGHT:
		player[0].x = +1;
		break;
	case GLUT_KEY_UP:
		player[0].z = -1;
		break;
	case GLUT_KEY_DOWN:
		player[0].z = +1;
	}
}

void SKeyUpboard(int key, int x, int y) {
	switch (key) {

	case GLUT_KEY_LEFT:
		player[0].x = 0;

		break;
	case GLUT_KEY_RIGHT:
		player[0].x = 0;
		break;
	case GLUT_KEY_UP:
		player[0].z = 0;

		break;
	case GLUT_KEY_DOWN:
		player[0].z = 0;

	}
}

void Boxinit(int x, int y) {				// �ڽ� ���� ���Ŀ� 25/25�� �÷��� �ڽ� �迭�� �������ְ� x,z���� 25�� �Ѱ��ָ� 25*25 �ڽ����� �������
	xScale = (float)10 / x;
	yScale = (float)10 / y;
	zScale = (float)10 / y;
	float xlocate = 5 - (xScale / 2);
	float zlocate = 5 - (zScale / 2);

	glm::vec3 bound_scale = { xScale / 2, yScale, zScale / 2 };

	int a = 0;
	for (int i = 0; i < Ycount; i = i++,a++) {
		for (int k = 0; k < x; k++) {
			All_Box[i][k].Bscale[0] = xScale;
			All_Box[i][k].Bscale[1] = yScale;
			All_Box[i][k].Bscale[2] = zScale;
			All_Box[i][k].Color = 0;
			All_Box[i][k].Blocate[0] = xlocate - xScale * k;
			All_Box[i][k].Blocate[1] = -yScale;
			All_Box[i][k].Blocate[2] = zlocate - zScale * a;

			All_Box[i][k].offset = i * x + k;

			All_Box[i][k].Bounding_box[0] = { (All_Box[i][k].Blocate - bound_scale), 1.f };
			All_Box[i][k].Bounding_box[1] = { (All_Box[i][k].Blocate + bound_scale), 1.f };
		}

	}

	playerinit();
}

void playerinit() {
	for (int i = 0; i < 2; i++) {
		player[i].PColor = { 1.0,1.0 * i ,0.0 };
		player[i].Plocate = All_Box[19*i][19 * i].Blocate;
		All_Box[19 * i][19 * i].Color = 1;
		player[i].Plocate[1] = 0.0f;
		player[i].Pscale = { 0.2,0.2,0.2 };
		player[i].lotate = 180 * i;
		player[i].prev_locate[0] = 0;
		player[i].prev_locate[1] = 0;
		player[i].x = 0;
		player[i].y = 0;
		player[i].z = 0;
		player[i].gravity = 0.01;
		player[i].speed = 0.05f;
	}
}

void Box_crash(Player* p) {								// �ڽ� �浹üũ
	glm::mat4 TR = glm::mat4(1.0f);
	glm::mat4 Tx = glm::mat4(1.0f);
	glm::mat4 Scale = glm::mat4(1.0f);
	glm::mat4 Rotate = glm::mat4(1.0f);

	Rotate = glm::rotate(Rotate, glm::radians(p->lotate), glm::vec3(0.0, 1.0, 0.0)); //--- z�࿡ ���Ͽ� ȸ�� ���
	Scale = glm::scale(Scale, p->Pscale); //		�÷��̾�
	Tx = glm::translate(Tx, p->Plocate + p->Move);
	TR = Tx * Rotate * Scale * TR;

	glm::vec4 left_front = TR * glm::vec4(-0.5f, 0.0f, -0.5f, 1.0f);					// �÷��̾� ��,����,��,��  ���� ��ǥ�� ���� vec�� ����
	glm::vec4 right_front = TR * glm::vec4(0.5f, 0.0f, -0.5f, 1.0f);
	glm::vec4 left_back = TR * glm::vec4(-0.5f, 0.0f, 0.5f, 1.0f);
	glm::vec4 right_back = TR * glm::vec4(0.5f, 0.0f, 0.5f, 1.0f);

	left_front[0] += 5.0f + 0.01,left_front[2] += 5.0f + 0.01;							// ���� ��ǥ�� ��� ��, �ش� ��ǥ�� �迭 �ε����� ��ȯ 
	left_front[0] = left_front[0] / xScale, left_front[2] = left_front[2] / zScale;

	right_front[0] += 5.0f + 0.01, right_front[2] += 5.0f + 0.01;
	right_front[0] = right_front[0] / xScale, right_front[2] = right_front[2] / zScale;

	left_back[0] += 5.0f + 0.01, left_back[2] += 5.0f + 0.01;
	left_back[0] = left_back[0] / xScale, left_back[2] = left_back[2] / zScale;

	right_back[0] += 5.0f + 0.01, right_back[2] += 5.0f + 0.01;
	right_back[0] = right_back[0] / xScale, right_back[2] = right_back[2] / zScale;

	
	if (19 - int(left_front[2]) != p->prev_locate[0] || 19 - int(left_front[0]) != p->prev_locate[1]) {			// ������ ��ȯ�� �ε����� ���� �ε����� ������ �� �ٸ���, ���� �ε����� ���� �� �ش� �ε��� �ڽ� �� ����
		All_Box[19 - int(left_front[2])][19 - int(left_front[0])].Color = 1;
		p->prev_locate[0] = 19 - int(left_front[2]);
		p->prev_locate[1] = 19 - int(left_front[0]);
	}
	if (19 - int(right_front[2]) != p->prev_locate[0] || 19 - int(right_front[0]) != p->prev_locate[1]) {
		All_Box[19 - int(right_front[2])][19 - int(right_front[0])].Color = 1;
		p->prev_locate[0] = 19 - int(right_front[2]);
		p->prev_locate[1] = 19 - int(right_front[0]);
	}
	if (19 - int(left_back[2]) != p->prev_locate[0] || 19 - int(left_back[0]) != p->prev_locate[1]) {
		All_Box[19 - int(left_back[2])][19 - int(left_back[0])].Color = 1;
		p->prev_locate[0] = 19 - int(left_back[2]);
		p->prev_locate[1] = 19 - int(left_back[0]);
	}
	if (19 - int(right_back[2]) != p->prev_locate[0] || 19 - int(right_back[0]) != p->prev_locate[1]) {
		All_Box[19 - int(right_back[2])][19 - int(right_back[0])].Color = 1;
		p->prev_locate[0] = 19 - int(right_back[2]);
		p->prev_locate[1] = 19 - int(right_back[0]);
	}
}

void Player_camera(Player* p) {
	glm::mat4 TR = glm::mat4(1.0f);
	glm::mat4 Tx = glm::mat4(1.0f);
	glm::mat4 Scale = glm::mat4(1.0f);
	glm::mat4 Rotate = glm::mat4(1.0f);

	Rotate = glm::rotate(Rotate, glm::radians(p->lotate), glm::vec3(0.0, 1.0, 0.0));
	Scale = glm::scale(Scale, p->Pscale); //		�÷��̾�
	Tx = glm::translate(Tx, p->Plocate + p->Move);
	TR = Tx * Rotate * Scale * TR;

	p->cameraPos = TR * glm::vec4(0.0f, Viewy, 0.0f, 1.0f);
	p->cameraDirection = glm::vec3(0.0f, 0.5f, -1.0f);

	p->cameraDirection[0] = sin(glm::radians(p->lotate)) * -1.0;
	p->cameraDirection[2] = cos(glm::radians(p->lotate)) * -1.0;
}


GLuint CreateTexture(char const* filename)
{
	FREE_IMAGE_FORMAT format = FreeImage_GetFileType(filename, 0);

	if (format == -1)
	{
		cout << "Could not find image: " << filename << " - Aborting." << endl;
		exit(-1);
	}

	if (format == FIF_UNKNOWN)
	{
		cout << "Couldn't determine file format - attempting to get from file extension..." << endl;

		format = FreeImage_GetFIFFromFilename(filename);

		if (!FreeImage_FIFSupportsReading(format))
		{
			cout << "Detected image format cannot be read!" << endl;
			exit(-1);
		}
	}

	FIBITMAP* bitmap = FreeImage_Load(format, filename);

	int bitsPerPixel = FreeImage_GetBPP(bitmap);

	FIBITMAP* bitmap32;
	if (bitsPerPixel == 32)
	{
		cout << "Source image has " << bitsPerPixel << " bits per pixel. Skipping conversion." << endl;
		bitmap32 = bitmap;
	}
	else
	{
		cout << "Source image has " << bitsPerPixel << " bits per pixel. Converting to 32-bit colour." << endl;
		bitmap32 = FreeImage_ConvertTo32Bits(bitmap);
	}

	// Some basic image info - strip it out if you don't care
	int imageWidth = FreeImage_GetWidth(bitmap32);
	int imageHeight = FreeImage_GetHeight(bitmap32);
	cout << "Image: " << filename << " is size: " << imageWidth << "x" << imageHeight << "." << endl;


	GLubyte* textureData = FreeImage_GetBits(bitmap32);

	GLuint tempTextureID;
	glGenTextures(1, &tempTextureID);
	glBindTexture(GL_TEXTURE_2D, tempTextureID);


	glTexImage2D(GL_TEXTURE_2D,    // Type of texture
		0,                // Mipmap level (0 being the top level i.e. full size)
		GL_RGBA,          // Internal format
		imageWidth,       // Width of the texture
		imageHeight,      // Height of the texture,
		0,                // Border in pixels
		GL_BGRA,          // Data format
		GL_UNSIGNED_BYTE, // Type of texture data
		textureData);     // The image data to use for this texture

						  // Specify our minification and magnification filters

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);


	glBindTexture(GL_TEXTURE_2D, 0);


	GLenum glError = glGetError();
	if (glError)
	{
		cout << "There was an error loading the texture: " << filename << endl;

		switch (glError)
		{
		case GL_INVALID_ENUM:
			cout << "Invalid enum." << endl;
			break;

		case GL_INVALID_VALUE:
			cout << "Invalid value." << endl;
			break;

		case GL_INVALID_OPERATION:
			cout << "Invalid operation." << endl;

		default:
			cout << "Unrecognised GLenum." << endl;
			break;
		}

		cout << "See https://www.opengl.org/sdk/docs/man/html/glTexImage2D.xhtml for further details." << endl;
	}

	FreeImage_Unload(bitmap32);

	if (bitsPerPixel != 32)
	{
		FreeImage_Unload(bitmap);
	}

	return tempTextureID;
}

// ���� �ؽ��ĸ� ���� �Լ�
void defineVertexArrayObject() {

	//#1
	//�ﰢ���� �����ϴ� vertex ������ - position�� color
	float position[] = {
	-0.5f,  -0.5f, 0.0f, //vertex 1 : Top-left
	0.5f, -0.5f, 0.0f, //vertex 2 : Top-right
	0.5f, 0.5f, 0.0f, //vertex 3 : Bottom-right
	0.5f, 0.5f, 0.0f, //vertex 4 : Bottom-right
	-0.5f, 0.5f, 0.0f, //vertex 5 : Bottom-left
	-0.5f,  -0.5f, 0.0f //vertex 6 : Top-left
	};

	float color[] = {
		1.0f, 1.0f, 1.0f, //vertex 1 : RED (1,0,0)
		1.0f, 1.0f, 1.0f, //vertex 2 : GREEN (0,1,0) 
		1.0f, 1.0f, 1.0f,  //vertex 3 : BLUE (0,0,1)
		1.0f, 1.0f, 1.0f,  //vertex 4 : BLUE (0,0,1)
		1.0f, 1.0f, 1.0f,  //vertex 5 : WHITE (1,1,1)
		1.0f, 1.0f, 1.0f //vertex 6 : RED (1,0,0)
	};

	float textureCoordinate[] = {
	0.0f, 0.0f,  //vertex 1  
	1.0f, 0.0f,  //vertex 2
	1.0f, 1.0f,   //vertex 3        
	1.0f, 1.0f,  //vertex 1  
	0.0f, 1.0f,  //vertex 2
	0.0f, 0.0f   //vertex 3        
	};



	//#2
	//Vertex Buffer Object(VBO)�� �����Ͽ� vertex �����͸� �����Ѵ�.
	glGenBuffers(1, &trianglePositionVertexBufferObjectID);
	glBindBuffer(GL_ARRAY_BUFFER, trianglePositionVertexBufferObjectID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(position), position, GL_STATIC_DRAW);

	glGenBuffers(1, &triangleColorVertexBufferObjectID);
	glBindBuffer(GL_ARRAY_BUFFER, triangleColorVertexBufferObjectID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(color), color, GL_STATIC_DRAW);

	glGenBuffers(1, &triangleTextureCoordinateBufferObjectID);
	glBindBuffer(GL_ARRAY_BUFFER, triangleTextureCoordinateBufferObjectID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(textureCoordinate), textureCoordinate, GL_STATIC_DRAW);


	//#6
	glGenVertexArrays(1, &triangleVertexArrayObject);
	glBindVertexArray(triangleVertexArrayObject);


	GLint positionAttribute = glGetAttribLocation(triangleShaderProgramID, "positionAttribute");
	if (positionAttribute == -1) {
		cerr << "position �Ӽ� ���� ����" << endl;
	}
	glBindBuffer(GL_ARRAY_BUFFER, trianglePositionVertexBufferObjectID);
	glVertexAttribPointer(positionAttribute, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glEnableVertexAttribArray(positionAttribute);


	/*
	GLint colorAttribute = glGetAttribLocation(triangleShaderProgramID, "colorAttribute");
	if (colorAttribute == -1) {
		cerr << "color �Ӽ� ���� ����" << endl;
		return false;
	}
	glBindBuffer(GL_ARRAY_BUFFER, triangleColorVertexBufferObjectID);
	glVertexAttribPointer(colorAttribute, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(colorAttribute);
	*/


	GLint textureCoordinateAttribute = glGetAttribLocation(triangleShaderProgramID, "textureCoordinateAttribute");
	if (textureCoordinateAttribute == -1) {
		cerr << "Texture Coordinate �Ӽ� ���� ����" << endl;
	}
	glBindBuffer(GL_ARRAY_BUFFER, triangleTextureCoordinateBufferObjectID);
	glVertexAttribPointer(textureCoordinateAttribute, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(textureCoordinateAttribute);


	glBindVertexArray(0);
}

void Texture_init() {
	Timecount[0].Tlocate = glm::vec3(0.5f, 0.0f, 0.0f);
	Timecount[1].Tlocate = glm::vec3(-0.5f, 0.0f, 0.0f);

	for (int i = 0; i < 2; i++) {
		glm::mat4 Tx = glm::mat4(1.0f);
		Timecount[i].Ttr = glm::mat4(1.0f);

		Tx = glm::translate(Tx, Timecount[i].Tlocate);
		Timecount[i].Ttr = Tx * Timecount[i].Ttr;
	}


	texureId[0] = CreateTexture("0.jpg");
	texureId[1] = CreateTexture("1.jpg");
	texureId[2] = CreateTexture("2.jpg");
	texureId[3] = CreateTexture("3.jpg");
	texureId[4] = CreateTexture("4.jpg");
	texureId[5] = CreateTexture("5.jpg");
	texureId[6] = CreateTexture("6.jpg");
	texureId[7] = CreateTexture("7.jpg");
	texureId[8] = CreateTexture("8.jpg");
	texureId[9] = CreateTexture("9.jpg");
	texureId[10] = CreateTexture("comg_bg.jpg");

	Worldbox[0].Tlocate = glm::vec3(0.0f, 0.0f,8.0f);
	Worldbox[1].Tlocate = glm::vec3(8.0f, 0.0f, 0.0f);
	Worldbox[2].Tlocate = glm::vec3(0.0f, 0.0f, -8.0f);
	Worldbox[3].Tlocate = glm::vec3(-8.0f, 0.0f, 0.0f);

	for (int i = 0; i < 4; i++) {
		Worldbox[i].Tscale = glm::vec3(20.0f, 15.0f, 20.0f);
		Worldbox[i].Trotate = 90 * i;
		Worldbox[i].Ttr = glm::mat4(1.0f);
		glm::mat4 Tx = glm::mat4(1.0f);
		glm::mat4 Scale = glm::mat4(1.0f);
		glm::mat4 Rotate = glm::mat4(1.0f);
		Scale = glm::scale(Scale, glm::vec3(Worldbox[i].Tscale)); //		�� �簢�� ũ�� 
		Rotate = glm::rotate(Rotate, glm::radians(float(Worldbox[i].Trotate)), glm::vec3(0.0, 1.0, 0.0));
		Tx = glm::translate(Tx, Worldbox[i].Tlocate);
		Worldbox[i].Ttr = Tx * Rotate * Scale * Worldbox[i].Ttr;
	}
}

int collide(Player* p, Box b, glm::mat4 TR)
{

	glm::vec4 a1 = TR * glm::vec4(-0.5f, 0.0f, -0.5f, 1.0f);					// �÷��̾� ��,����,��,��  ���� ��ǥ�� ���� vec�� ����
	glm::vec4 a2 = TR * glm::vec4(0.5f, 1.0f, 0.5f, 1.0f);
	glm::vec4 a3 = TR * glm::vec4(-0.5f, 0.0f, 0.5f, 1.0f);
	glm::vec4 a4 = TR * glm::vec4(0.5f, 1.0f, -0.5f, 1.0f);

	glm::vec4 player_bounding_box[2] = { a1, a2 }; // 0�� min, 1�� max

	if (b.Bounding_box[0][0] <= player_bounding_box[1][0] && b.Bounding_box[1][0] >= player_bounding_box[0][0] &&
		b.Bounding_box[0][1] <= player_bounding_box[1][1] && b.Bounding_box[1][1] >= player_bounding_box[0][1] &&
		b.Bounding_box[0][2] <= player_bounding_box[1][2] && b.Bounding_box[1][2] >= player_bounding_box[0][2]) {
		return b.offset;
	}
	return -1;
}

void Crash(Player* p, int inspection)
{
	All_Box[inspection / 20][inspection % 20].Bcolor = p->PColor;
	p->Move[1] += p->gravity;
}

// Ÿ���� �׷��ִ� �Լ�
void Drawtime() {
	glUseProgram(triangleShaderProgramID);
	glBindVertexArray(triangleVertexArrayObject);

	glUniform1i(glGetUniformLocation(triangleShaderProgramID, "tex"), 0);

	glm::mat4 projection = glm::mat4(1.0f);
	unsigned int projectionLocation = glGetUniformLocation(triangleShaderProgramID, "projectionTransform"); //--- ���� ��ȯ �� ����
	glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &projection[0][0]);

	glm::mat4 view = glm::mat4(1.0f);
	unsigned int viewLocation = glGetUniformLocation(triangleShaderProgramID, "viewTransform");
	glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);

	unsigned int modelLocation = glGetUniformLocation(s_program, "modelTransform"); //--- ���ؽ� ���̴����� �𵨸� ��ȯ ��ġ ��������
	glActiveTexture(GL_TEXTURE0);						// ���ڸ� ����
	glBindTexture(GL_TEXTURE_2D, texureId[int(Now_time) % 10]);
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(Timecount[0].Ttr)); //--- modelTransform ������ ��ȯ �� �����ϱ�
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindTexture(GL_TEXTURE_2D, texureId[int(Now_time) / 10]);			// ���ڸ� ����
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(Timecount[1].Ttr)); //--- modelTransform ������ ��ȯ �� �����ϱ�
	glDrawArrays(GL_TRIANGLES, 0, 6);
}
// �ʵ带 �׸��� �Լ�
void Draw_filed(BOOL View_draw_background) {
	glUseProgram(s_program);

	unsigned int modelLocation = glGetUniformLocation(s_program, "modelTransform"); //--- ���ؽ� ���̴����� �𵨸� ��ȯ ��ġ ��������
	unsigned int modelLocation1 = glGetUniformLocation(s_program, "in_Color"); //--- ���ؽ� ���̴����� �𵨸� ��ȯ ��ġ ��������

	glBindVertexArray(vao);

	for (int i = 0; i < Ycount; i = i++) {					// �ڽ��� �׷��ִ� �κ�.
		for (int j = 0; j < xcount; j++) {
			glm::mat4 TR = glm::mat4(1.0f);
			glm::mat4 Tx = glm::mat4(1.0f);
			glm::mat4 Scale = glm::mat4(1.0f);



			glUniform3f(modelLocation1, All_Box[i][j].Bcolor[0], All_Box[i][j].Bcolor[1], All_Box[i][j].Bcolor[2]);
			Scale = glm::scale(Scale, glm::vec3(All_Box[i][j].Bscale)); //		�� �簢�� ũ�� 
			Tx = glm::translate(Tx, All_Box[i][j].Blocate);

			TR = Tx * Scale * TR;

			glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(TR)); //--- modelTransform ������ ��ȯ �� �����ϱ�
			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (GLvoid*)(sizeof(GLuint) * 0));
		}
	}

	glm::mat4 TR = glm::mat4(1.0f);
	glm::mat4 Tx = glm::mat4(1.0f);
	glm::mat4 Scale = glm::mat4(1.0f);


	for (int i = 0; i < 2; i++) {
		TR = glm::mat4(1.0f);					// �÷��̾ �׷��ִ� �κ�.
		Tx = glm::mat4(1.0f);
		Scale = glm::mat4(1.0f);
		glm::mat4 Rotate = glm::mat4(1.0f);

		glUniform3f(modelLocation1, player[i].PColor[0], player[i].PColor[1], player[i].PColor[2]);

		Rotate = glm::rotate(Rotate, glm::radians(player[i].lotate), glm::vec3(0.0, 1.0, 0.0)); //--- z�࿡ ���Ͽ� ȸ�� ���
		Scale = glm::scale(Scale, player[i].Pscale); //		�÷��̾�
		Tx = glm::translate(Tx, player[i].Plocate + player[i].Move);
		TR = Tx * Rotate * Scale * TR;

		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(TR)); //--- modelTransform ������ ��ȯ �� �����ϱ�
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (GLvoid*)(sizeof(GLuint) * 0));
	}

	if (View_draw_background) {
		glUseProgram(triangleShaderProgramID);
		glBindVertexArray(triangleVertexArrayObject);

		glUniform1i(glGetUniformLocation(triangleShaderProgramID, "tex"), 0);

		unsigned int modelLocation2 = glGetUniformLocation(triangleShaderProgramID, "modelTransform"); //--- ���ؽ� ���̴����� �𵨸� ��ȯ ��ġ ��������
		for (int i = 0; i < 4; i++) {
			glUniformMatrix4fv(modelLocation2, 1, GL_FALSE, glm::value_ptr(Worldbox[i].Ttr)); //--- modelTransform ������ ��ȯ �� �����ϱ�

			glActiveTexture(GL_TEXTURE0);						// ���ڸ� ����
			glBindTexture(GL_TEXTURE_2D, texureId[10]);
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}
	}
	glUseProgram(s_program);
}

void Play_state() {
	unsigned int projectionLocation = glGetUniformLocation(s_program, "projectionTransform"); //--- ���� ��ȯ �� ����
	unsigned int viewLocation = glGetUniformLocation(s_program, "viewTransform"); //--- ���� ��ȯ ����


	glm::mat4 Prev_rotation = glm::mat4(1.0f);

	view = glm::mat4(1.0f);




	projection = glm::mat4(1.0f);
	projection = glm::perspective(glm::radians(45.0f), 1.0f, 0.01f, 100.0f);
	projection = glm::rotate(projection, glm::radians(45.0f), glm::vec3(1.0, 0.0, 0.0));
	projection = glm::translate(projection, glm::vec3(0.0, 0.0, -3)); //--- ������ �ణ �ڷ� �̷���
	glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projection));


	view = glm::lookAt(player[0].cameraPos, player[0].cameraPos + player[0].cameraDirection, cameraUp);
	glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);

	glUseProgram(triangleShaderProgramID);

	unsigned int Texture_viewlocation = glGetUniformLocation(triangleShaderProgramID, "viewTransform");
	unsigned int Teture_projectionlocation = glGetUniformLocation(triangleShaderProgramID, "projectionTransform");

	glUniformMatrix4fv(Texture_viewlocation, 1, GL_FALSE, &view[0][0]);

	glUniformMatrix4fv(Teture_projectionlocation, 1, GL_FALSE, &projection[0][0]);

	glViewport(0, 0, 630, 700);
	Draw_filed(true);


	view = glm::lookAt(player[1].cameraPos, player[1].cameraPos + player[1].cameraDirection, cameraUp);
	glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);

	glUseProgram(triangleShaderProgramID);
	glUniformMatrix4fv(Texture_viewlocation, 1, GL_FALSE, &view[0][0]);


	glViewport(630, 0, 630, 700);
	Draw_filed(true);

	view = glm::lookAt(cameraPos, cameraPos + cameraDirection, cameraUp);
	glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);


	glm::mat4 projection = glm::mat4(1.0f);
	projection = glm::perspective(glm::radians(60.0f), 1.0f, 0.01f, 100.0f);
	projection = glm::rotate(projection, glm::radians(90.0f), glm::vec3(1.0, 0.0, 0.0)); //--- z�࿡ ���Ͽ� ȸ�� ���
	projection = glm::translate(projection, glm::vec3(0.0, -3.0, 15.0)); //--- ������ �ణ �ڷ� �̷���
	glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &projection[0][0]);

	glUseProgram(triangleShaderProgramID);

	glUniformMatrix4fv(Texture_viewlocation, 1, GL_FALSE, &view[0][0]);

	glUniformMatrix4fv(Teture_projectionlocation, 1, GL_FALSE, &projection[0][0]);

	glDisable(GL_DEPTH_TEST);
	glViewport(830, 500, 200, 200);
	Draw_filed(false);


	glViewport(180, 500, 200, 200);
	Draw_filed(false);


	glViewport(570, 550, 100, 100);
	Drawtime();
}