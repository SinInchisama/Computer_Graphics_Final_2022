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

typedef struct Box {
	glm::vec3 Bscale;
	glm::vec3 Blocate;

	float Color;
}Box;

typedef struct Player {
	glm::vec3 Pscale;
	glm::vec3 Plocate;
	glm::vec3 PColor;
	glm::vec3 Move;

	glm::vec3 cameraPos;
	glm::vec3 cameraDirection;

	int x, z;
	int prev_locate[2];
	float lotate;
	bool view;
}Player;

void make_vertexShader();
void make_fragmentShader();
void InitBuffer();
void InitShader();
void TimerFunction(int value);
void SKeyDownboard(int key, int x, int y);
void SKeyUpboard(int key, int x, int y);
GLvoid Reshape(int w, int h);
GLvoid drawScene();
GLvoid Keyboard(unsigned char key, int x, int y);
char* filetobuf(const char* file);
void Boxinit(int x, int y);
void Draw_filed();
void playerinit();
void Box_crash(Player* p);
void player_Camera(Player* p);



GLchar* vertexsource, * fragmentsource; //--- �ҽ��ڵ� ���� ����
GLuint vertexshader, fragmentshader; //--- ���̴� ��ü
GLuint s_program;
GLint result;
GLuint vao, vbo[2], EBO[2], linevbo[2], linevao;

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

bool ra, Drop = true, What_view, Mani,Vani,rani,rminus,ani_1, ani_3;
int xcount = 20, Ycount = 20;
float yz = 0.0f;
Box All_Box[20][20];
Player player[2];

GLUquadricObj* qobj = gluNewQuadric();

glm::vec3 cameraPos;
glm::vec3 cameraPos_1;
glm::vec3 cameraDirection; //--- ī�޶� �ٶ󺸴� ����
glm::vec3 cameraDirection_1;
glm::vec3 cameraUp; //--- ī�޶� ���� ����

glm::vec3 originPos;

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

	Boxinit(xcount, Ycount);
	glewInit();
	InitShader();
	InitBuffer();
	glutDisplayFunc(drawScene);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(Keyboard); // Ű���� �Է� �ݹ��Լ� ����
	/*glutKeyboardUpFunc();*/
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
	glUseProgram(s_program);
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

	cameraPos = glm::vec3(Camerax, 17.0f, Cameraz);
	cameraDirection = glm::vec3(0.0f, 0.0f, -1.0f); //--- ī�޶� �ٶ󺸴� ����
	cameraUp = glm::vec3(0.0f, 1.0f, 0.0f); //--- ī�޶� ���� ����

}

void drawScene() {
	glUseProgram(s_program);
	glClearColor(1.0, 1.0, 1.0, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);


	glm::mat4 Prev_rotation = glm::mat4(1.0f);


	glm::mat4 view = glm::mat4(1.0f);

	unsigned int viewLocation = glGetUniformLocation(s_program, "viewTransform"); //--- ���� ��ȯ ����
	view = glm::lookAt(cameraPos, cameraPos + cameraDirection, cameraUp);
	glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);

	glm::mat4 projection = glm::mat4(1.0f);
	projection = glm::perspective(glm::radians(60.0f), 1.0f, 0.01f, 100.0f);
	projection = glm::rotate(projection, glm::radians(90.0f), glm::vec3(1.0, 0.0, 0.0)); //--- z�࿡ ���Ͽ� ȸ�� ���
	projection = glm::translate(projection, glm::vec3(0.0, -3.0, 15.0)); //--- ������ �ణ �ڷ� �̷���
	unsigned int projectionLocation = glGetUniformLocation(s_program, "projectionTransform"); //--- ���� ��ȯ �� ����
	glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &projection[0][0]);

	Draw_filed();
	glViewport(830, 500, 200, 200);

	Draw_filed();
	glViewport(630, 0, 630, 700);


	view = glm::mat4(1.0f);


	projection = glm::mat4(1.0f);
	projection = glm::perspective(glm::radians(45.0f), 1.0f, 0.01f, 100.0f);
	projection = glm::rotate(projection, glm::radians(45.0f), glm::vec3(1.0, 0.0, 0.0)); 
	projection = glm::translate(projection, glm::vec3(0.0, 0.0, -3)); //--- ������ �ణ �ڷ� �̷���
	projectionLocation = glGetUniformLocation(s_program, "projectionTransform"); //--- ���� ��ȯ �� ����
	glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &projection[0][0]);


	
	view = glm::lookAt(player[0].cameraPos, player[0].cameraPos + player[0].cameraDirection, cameraUp);
	

	glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);

	Draw_filed();

	glViewport(0, 0, 630, 700);

	view = glm::lookAt(player[1].cameraPos, player[1].cameraPos + player[1].cameraDirection, cameraUp);


	glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);

	Draw_filed();
	glViewport(430, 500, 200, 200);



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

	for (int i = 0; i < 2; i++) {
		player[i].Move[0] += cos(glm::radians(-player[i].lotate)) * 0.02 * player[i].x;			// �÷��̾� �¿� ������ �Ҷ� ���� ���
		player[i].Move[2] += sin(glm::radians(-player[i].lotate)) * 0.02 * player[i].x;
		player[i].Move[0] += sin(glm::radians(player[i].lotate)) * 0.02 * player[i].z;			// �÷��̾� �� �Ʒ� ������ �Ҷ� ���� ���
		player[i].Move[2] += cos(glm::radians(player[i].lotate)) * 0.02 * player[i].z;
		Box_crash(&player[i]);																// �����̰��� �ش� ��ġ ��� üũ	(����� �迭�� üũ�ϴ� ��. �迭�� üũ�� �� ���������� ���� ��ġ ����������ҵ�?)
		player_Camera(&player[i]);																// �÷��̾� ī�޶� ��ġ ���.
	}

	glutTimerFunc(50, TimerFunction, 1);
	glutPostRedisplay();
}

GLvoid Keyboard(unsigned char key, int x, int y) {
	if (key == 'a') {
			player[0].lotate += 10;					// �÷��̾� ������
	}
	else if (key == 'd') {
		player[0].lotate -= 10;
	}
	else if (key == 'q')
		exit(0);
	glutPostRedisplay();
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

		}

	}

	playerinit();
}


void Draw_filed() {
	unsigned int modelLocation = glGetUniformLocation(s_program, "modelTransform"); //--- ���ؽ� ���̴����� �𵨸� ��ȯ ��ġ ��������
	unsigned int modelLocation1 = glGetUniformLocation(s_program, "in_Color"); //--- ���ؽ� ���̴����� �𵨸� ��ȯ ��ġ ��������

	glBindVertexArray(vao);

	for (int i = 0; i < Ycount; i = i++) {					// �ڽ��� �׷��ִ� �κ�.
		for (int j = 0; j < xcount; j++) {
			glm::mat4 TR = glm::mat4(1.0f);							
			glm::mat4 Tx = glm::mat4(1.0f);
			glm::mat4 Scale = glm::mat4(1.0f);

			if (All_Box[i][j].Color == 0) {
				glUniform3f(modelLocation1, 0.0f, 0.0f, 0.0f);
			}
			else if (All_Box[i][j].Color == 1) {
				glUniform3f(modelLocation1, 1.0f, 0.0f, 0.0f);
			}
			else {
				glUniform3f(modelLocation1, .0f, 1.0f, 1.0f);
			}
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
		player[i].z = 0;
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

void player_Camera(Player* p) {
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