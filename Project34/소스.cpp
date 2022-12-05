#include <iostream>
#include <gl/glew.h> // 필요한 헤더파일 include
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <gl/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <gl/glm/glm.hpp>
#include <gl/glm/ext.hpp>
#include <gl/glm/gtc/matrix_transform.hpp>
#include <random>

void make_vertexShader();
void make_fragmentShader();
void InitBuffer();
void InitShader();
void TimerFunction(int value);
//void SKeyboard(int key, int x, int y);
GLvoid Reshape(int w, int h);
GLvoid drawScene();
GLvoid Keyboard(unsigned char key, int x, int y);
char* filetobuf(const char* file);
void Boxinit(int x, int y);
void Mazeinit();
void Draw_filed();

typedef struct Box {
	glm::vec3 Bscale;
	glm::vec3 Blocate;
	glm::vec3 BColor;

	float Max_height;
	float Speed;

	int Yplus;
}Box;


GLchar* vertexsource, * fragmentsource; //--- 소스코드 저장 변수
GLuint vertexshader, fragmentshader; //--- 세이더 객체
GLuint s_program;
GLint result;
GLuint vao, vbo[2], EBO[2], linevbo[2], linevao;

float Cameraz = 15.0f;
float Camerax = 0.0f;
float ySelfRot = 0.0f;
float yZeroRot = 0.0f;

bool ra, Drop = true, What_view, Mani,Vani,rani,rminus;
int xcount = 0, Ycount = 0;
float yz = 0.0f;
glm::vec3 Bscale[25][25];
glm::vec3 Blocate[25][25];
glm::vec3 BColor[25][25];
bool* maze;
Box* All_Box;
float dropboard = 3.0, drop = 3.0;

GLUquadricObj* qobj = gluNewQuadric();

glm::vec3 cameraPos;
glm::vec3 cameraPos1;
glm::vec3 cameraDirection; //--- 카메라 바라보는 방향
glm::vec3 cameraUp; //--- 카메라 위쪽 방향

float vPositionList[] = {
0.5f, 1.0f, 0.5f, // 앞 우측 상단
0.5f, 1.0f, -0.5f, // 뒤 우측 상단
-0.5f, 1.0f, -0.5f, // 뒤 좌측 상단
-0.5f, 1.0f, 0.5f, // 앞 좌측 상단
-0.5f,0.0f,0.5f, // 앞 좌측 하단
-0.5f,0.0f,-0.5f,	// 뒤 좌측 하단
0.5f,0.0f,-0.5f,	// 뒤 우측 하단
0.5f,0.0f,0.5f,		// 앞 우측 하단

0.5f,0.0f,0.5f,
0.5f,0.0f,-0.5f,
-0.5f,0.0f,0.5f, // 앞 좌측 하단
-0.5f,0.0f,-0.5f,	// 뒤 좌측 하단
0.0f,0.5f,0.0f
};

unsigned int index[] = {
1, 3, 0, // 첫 번째 삼각형			// 위 사각형
1, 2, 3, // 두 번째 삼각형
5, 4, 2,									// 왼쪽 사각형
3, 2, 4,
0, 7, 6,							// 오른쪽 사각형
6, 1, 0,
3, 4, 7,							// 앞 사각형
7, 0, 3,
6, 5, 2,								// 뒤 사각형
6, 2, 1,
6, 7, 4,	// 세 번째 삼각형			// 아래 사각형
6, 4, 5,	// 네 번째 삼각형

8,9,10,
10,9,11,
10,8,12,		// 앞 삼각형
8,9,12,			// 오른쪽
9,11,12,		// 뒤쪽
11,10,12		// 왼쪽
};

float line[]{
5.0f,0.0f,0.0f,
-5.0f,0.0f,0.0f,
0.0f,5.0f,0.0f,
0.0f,-5.0f,0.0f,
0.0f,0.0f,5.0f,
0.0f,0.0f,-5.0f
};

void main(int argc, char** argv) //--- 윈도우 출력하고 콜백함수 설정
{
	//--- 윈도우 생성하기
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(800, 600);
	glutCreateWindow("Example1");
	//--- GLEW 초기화하기
	glewExperimental = GL_TRUE;

	printf("x X y");
	scanf("%d %d", &xcount,&Ycount);
	Boxinit(xcount,Ycount);

	glewInit();
	InitShader();
	InitBuffer();
	glutDisplayFunc(drawScene);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(Keyboard); // 키보드 입력 콜백함수 지정
	//glutSpecialFunc(SKeyboard);
	glutTimerFunc(50, TimerFunction, 1);
	glutMainLoop();
}

void make_vertexShader() {
	vertexsource = filetobuf("vertex.glsl");//--- 버텍스 세이더 객체 만들기
	vertexshader = glCreateShader(GL_VERTEX_SHADER);
	//--- 세이더 코드를 세이더 객체에 넣기
	glShaderSource(vertexshader, 1, (const GLchar**)&vertexsource, 0);
	//--- 버텍스 세이더 컴파일하기
	glCompileShader(vertexshader);
	//--- 컴파일이 제대로 되지 않은 경우: 에러 체크
	GLchar errorLog[512];
	glGetShaderiv(vertexshader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(vertexshader, 512, NULL, errorLog);
		std::cerr << "ERROR: vertex shader 컴파일 실패\n" << errorLog << std::endl;
		return;
	}
}

void make_fragmentShader() {
	fragmentsource = filetobuf("fragment.glsl");
	//--- 프래그먼트 세이더 객체 만들기
	fragmentshader = glCreateShader(GL_FRAGMENT_SHADER);
	//--- 세이더 코드를 세이더 객체에 넣기
	glShaderSource(fragmentshader, 1, (const GLchar**)&fragmentsource, 0);
	//--- 프래그먼트 세이더 컴파일
	glCompileShader(fragmentshader);
	//--- 컴파일이 제대로 되지 않은 경우: 컴파일 에러 체크
	GLchar errorLog[512];
	glGetShaderiv(fragmentshader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(fragmentshader, 512, NULL, errorLog);
		std::cerr << "ERROR: fragment shader 컴파일 실패\n" << errorLog << std::endl;
		return;
	}
}

void InitShader()
{
	make_vertexShader(); //--- 버텍스 세이더 만들기
	make_fragmentShader(); //--- 프래그먼트 세이더 만들기
	//-- shader Program
	s_program = glCreateProgram();
	glAttachShader(s_program, vertexshader);
	glAttachShader(s_program, fragmentshader);
	glLinkProgram(s_program);

	GLchar errorLog[512];
	if (!result)
	{
		glGetShaderInfoLog(fragmentshader, 512, NULL, errorLog);
		std::cerr << "ERROR: fragment shader 컴파일 실패\n" << errorLog << std::endl;
		return;
	}
	//--- 세이더 삭제하기
	glDeleteShader(vertexshader);
	glDeleteShader(fragmentshader);
	//--- Shader Program 사용하기
	glUseProgram(s_program);
}

void InitBuffer()
{
	glGenVertexArrays(1, &vao); //--- VAO 를 지정하고 할당하기
	glBindVertexArray(vao); //--- VAO를 바인드하기
	glGenBuffers(2, vbo); //--- 2개의 VBO를 지정하고 할당하기
	//--- 1번째 VBO를 활성화하여 바인드하고, 버텍스 속성 (좌표값)을 저장
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vPositionList), vPositionList, GL_STATIC_DRAW);
	glGenBuffers(1, &EBO[0]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[0]); //--- GL_ELEMENT_ARRAY_BUFFER 버퍼 유형으로 바인딩
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index), index, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glEnableVertexAttribArray(0);

	glGenVertexArrays(1, &linevao); //--- VAO 를 지정하고 할당하기
	glBindVertexArray(linevao); //--- VAO를 바인드하기
	glGenBuffers(2, linevbo); //--- 2개의 VBO를 지정하고 할당하기
	glBindBuffer(GL_ARRAY_BUFFER, linevbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(line), line, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	cameraPos1 = glm::vec3(Camerax, 0.0f, Cameraz);
	cameraPos = glm::vec3(Camerax, 15.0f, Cameraz);
	cameraDirection = glm::vec3(0.0f, 0.0f, -1.0f); //--- 카메라 바라보는 방향
	cameraUp = glm::vec3(0.0f, 1.0f, 0.0f); //--- 카메라 위쪽 방향

}

void drawScene() {
	glUseProgram(s_program);
	glClearColor(1.0, 1.0, 1.0, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);


	glm::mat4 Prev_rotation = glm::mat4(1.0f);



	if (!What_view) {
		glm::mat4 projection = glm::mat4(1.0f);
		projection = glm::perspective(glm::radians(60.0f), 1.0f, 0.01f, 100.0f);
		projection = glm::rotate(projection, glm::radians(45.0f), glm::vec3(1.0, 0.0, 0.0)); //--- z축에 대하여 회전 행렬
		//projection = glm::translate(projection, glm::vec3(0.0, 0.0, -5.0)); //--- 공간을 약간 뒤로 미뤄줌
		unsigned int projectionLocation = glGetUniformLocation(s_program, "projectionTransform"); //--- 투영 변환 값 설정
		glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &projection[0][0]);
	}
	else{
		glm::mat4 projection = glm::mat4(1.0f);
		projection = glm::ortho(-15.0f, 15.0f, -15.0f, 15.0f, -15.0f, 15.0f); //--- 투영 공간을 [-100.0, 100.0] 공간으로 설정
		unsigned int projectionLocation = glGetUniformLocation(s_program, "projectionTransform"); //--- 투영 변환 값 설정
		glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &projection[0][0]);
	}


	glm::mat4 view = glm::mat4(1.0f);

	view = glm::lookAt(cameraPos, cameraPos + cameraDirection, cameraUp);
	unsigned int viewLocation = glGetUniformLocation(s_program, "viewTransform"); //--- 뷰잉 변환 설정
	glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);


	Draw_filed();
	glViewport(600, 400, 200, 200);
	

	view = glm::mat4(1.0f);

	view = glm::lookAt(cameraPos, cameraPos + cameraDirection, cameraUp);
	glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);

	glm::mat4 projection = glm::mat4(1.0f);
	projection = glm::perspective(glm::radians(60.0f), 1.0f, 0.01f, 100.0f);
	projection = glm::rotate(projection, glm::radians(90.0f), glm::vec3(1.0, 0.0, 0.0)); //--- z축에 대하여 회전 행렬
	projection = glm::translate(projection, glm::vec3(0.0, -3.0, 15.0)); //--- 공간을 약간 뒤로 미뤄줌
	unsigned int projectionLocation = glGetUniformLocation(s_program, "projectionTransform"); //--- 투영 변환 값 설정
	glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &projection[0][0]);
	Draw_filed();
	glViewport(0, 0, 800, 600);

	glutSwapBuffers();
}

GLvoid Reshape(int w, int h) //--- 콜백 함수: 다시 그리기 콜백 함수
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
	if (Drop) {
		if (drop >= -0.2)
			drop -= 0.2;
		if (dropboard >= -10.0)
			dropboard -= 0.2;
		else
			Drop = false;
	}
	else{
		if (Mani) {
			for (int i = 0; i < Ycount * Ycount; i = i + Ycount) {
				for (int k = 0; k < xcount; k++) {

						if (!rani || maze[i + k]) {
							if (All_Box[i + k].Yplus == 1) {
								All_Box[i + k].Bscale[1] += All_Box[i + k].Speed;
								if (All_Box[i + k].Bscale[1] >= All_Box[i + k].Max_height)
									All_Box[i + k].Yplus = 2;
							}
							else if (All_Box[i + k].Yplus == 2) {
								All_Box[i + k].Bscale[1] -= All_Box[i + k].Speed;
								if (All_Box[i + k].Bscale[1] <= 1)
									All_Box[i + k].Yplus = 1;
							}
						}
					}
				
			}
		}
		else if (Vani) {
			for (int i = 0; i < Ycount * Ycount; i = i + Ycount) {
				for (int k = 0; k < xcount; k++) {
					if (maze[i + k]) {
						if (All_Box[i + k].Bscale[1] > 1)
							All_Box[i + k].Bscale[1] -= All_Box[i + k].Speed;
						else
							All_Box[i + k].Bscale[1] = 1;
					}
				}
			}
		}
		
		if (rminus) {
			rminus = false;
			for (int i = 0; i < Ycount * Ycount; i = i + Ycount) {
				for (int k = 0; k < xcount; k++) {
					if (!maze[i + k]) {
						if (All_Box[i + k].Bscale[1] > -2.0) {
							All_Box[i + k].Bscale[1] -= All_Box[i + k].Speed;
							rminus = true;
						}
					}
				}
			}
		}
	}
	glutTimerFunc(50, TimerFunction, 1);
	glutPostRedisplay();
}

GLvoid Keyboard(unsigned char key, int x, int y) {
	if (key == 'o') {
		What_view = true;					 // 직각 투영
		cameraPos = glm::vec3(Camerax, 0.0f, 10);
	}
	else if (key == 'r') {
		rani = rani ? false : true;
		rminus = true;
	}
	else if (key == 'p') {
		What_view = false;					// 원근 투영
		cameraPos = glm::vec3(Camerax, 15.0f, Cameraz);
	}
	else if (key == 'z') {
		if (What_view == false) {
			Cameraz -= 0.1;					// 원근 투영일시 Camerz 줄이기
			cameraPos = glm::vec3(Camerax, 15.0f, Cameraz);
			cameraPos1 = glm::vec3(Camerax, 0.0f, Cameraz);
		}
	}
	else if (key == 'Z') {
		if (What_view == false) {
			Cameraz += 0.1;					// 원근 투영일시 Camerz 늘리기
			cameraPos = glm::vec3(Camerax, 15.0f, Cameraz);
			cameraPos1 = glm::vec3(Camerax, 0.0f, Cameraz);
		}
	}
	else if (key == 'm') {
		Mani = Mani ? false : true;			// 사각형 오르락내리락 애니메이션 출력
		Vani = false;
	}
	else if (key == 'v') {
		if (Vani == false) {
			Vani = true;
			Mani = false;
		}
		else if (Vani == true) {
			Vani = false;
			Mani = true;
		}
	}
	else if (key == 'y') {
		yZeroRot += 1.0f;					// 바닥 기준 Y 돌리기 시계반대방향
		glm::vec3 front;
		front.x = 0 * cos(glm::radians(yZeroRot)) + Cameraz * sin(glm::radians(yZeroRot));
		front.y = 0.0f;
		front.z = 0 * (-sin(glm::radians(yZeroRot))) + Cameraz * cos(glm::radians(yZeroRot));

		cameraPos = front;

		glm::vec3 target = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::vec3 direction = glm::normalize(cameraPos - target);
		glm::vec3 right = glm::normalize(glm::cross(glm::vec3(0.0, 1.0, 0.0), direction));
		
		if (What_view)
			cameraPos.y = 0;
		else
			cameraPos.y = 15.0f;

		cameraDirection = -direction;
	}
	else if (key == 'Y') {
		yZeroRot -= 1.0f;							// 바닥 기준 Y 돌리기 시계방향
		glm::vec3 front;
		front.x = 0 * cos(glm::radians(yZeroRot)) + Cameraz * sin(glm::radians(yZeroRot));
		front.y = 0.0f;
		front.z = 0 * (-sin(glm::radians(yZeroRot))) + Cameraz * cos(glm::radians(yZeroRot));

		cameraPos = front;

		glm::vec3 target = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::vec3 direction = glm::normalize(cameraPos - target);
		glm::vec3 right = glm::normalize(glm::cross(glm::vec3(0.0, 1.0, 0.0), direction));

		if (What_view)
			cameraPos.y = 0;
		else
			cameraPos.y = 15.0f;
		cameraDirection = -direction;
	}
	else if (key == 'c') {
		cameraPos.y = 15.0f;
		Camerax = 0;
		Cameraz = 15.0f;
		Vani = false;
		Mani = false;
		What_view == false;
		yZeroRot = 0;

		for (int i = 0; i < Ycount * Ycount; i = i + Ycount) {
			for (int k = 0; k < xcount; k++) {
				All_Box[i + k].Bscale[1] = All_Box[i + k].Max_height;
			}
		}
	}
	else if (key == 'q')
		exit(0);
	glutPostRedisplay();
}

void Boxinit(int x, int y) {

	All_Box = (Box*)malloc(sizeof(Box)* x * y);		// 원하는 xz개수만큼 배열 생성
	maze = (bool*)malloc(sizeof(bool) * x * y);		// 원하는 xz개수만큼 미로에 사용할 배열 생성.

	srand(time(NULL));
	Mazeinit();
	

	float xScale = (float)10 / x;
	float yScale = (float)10 / y;
	float xlocate = -5 + (xScale / 2);
	float ylocate = -5 + (yScale / 2);

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<> dist(4, 8);
	std::uniform_real_distribution<> Speed(0.05, 0.3);

	int a = 0;
	for (int i = 0; i < Ycount * Ycount; i = i + Ycount,a++) {
		for (int k = 0; k < x; k++) {
			All_Box[i  + k].Bscale[0] = xScale;
			All_Box[i + k].Bscale[1] = dist(gen);
			All_Box[i + k].Bscale[2] = yScale;
			All_Box[i + k].Blocate[0] = xlocate + xScale * k;
			All_Box[i + k].Blocate[1] = 0.0f;
			All_Box[i + k].Blocate[2] = ylocate + yScale * a;
			All_Box[i + k].BColor[0] = rand() % 10;
			All_Box[i + k].BColor[1] = rand() % 10;
			All_Box[i + k].BColor[2] = rand() % 10;
			All_Box[i + k].Max_height = All_Box[i + k].Bscale[1];
			All_Box[i + k].Yplus = rand() %2 + 1;
			All_Box[i + k].Speed = Speed(gen);
		}
	}

}

void Mazeinit() {

	for (int i = 0; i < Ycount; i++) {
		for (int j = 0; j < xcount; j++) {
			maze[i * Ycount + j] = 1;
		}
	}

	for (int i = 1; i < Ycount - 1; i++)
	{
		for (int j = 1; j < xcount - 1; j += 2)
		{
			if (i % 2 == 0)
				maze[i *Ycount + xcount - 2] = 0;
			else {
				//현재 셀이 우측 최하단 이라면(즉 마지막 셀) 스킵
				if (i == Ycount - 2 && j == xcount - 2) {
					for (int k = 0; k < 2; k++)
						maze[i * Ycount + k + j] = 0;
					continue;
				}
				//현재 셀이 맨 아래 셀들이라면 오른쪽으로 벽 뚫기
				if (i == Ycount - 2) {
					for (int k = 0; k < 2; k++)
						maze[i * Ycount + k + j] = 0;
					continue;
				}
				//현재 셀이 맨 오른쪽 셀들이라면 아래쪽으로 벽 뚫기
				if (j == xcount - 2) {
					maze[i * Ycount + j] = 0;
					continue;
				}

				//별다른 특이사항인 셀이 아니면 오른쪽 또는 아래 벽 랜덤 뚫기
				if (rand() % 2 == 0)
					for (int k = 0; k < 2; k++)
						maze[i * Ycount + k + j] = 0;
				else
					for (int k = 0; k < 2; k++)
						maze[(i + k) * Ycount + j] = 0;
			}
		}
	}

	//if (xcount % 2 == 0) {
	//	for (int i = 1; i < Ycount - 1; i++) {
	//		maze[i * Ycount + xcount - 2] = 0;
	//	}
	//}

	for (int i = 0; i < Ycount; i++) {
		for (int j = 0; j < xcount; j++)
			printf("%d ", maze[i * Ycount + j]);
		}
		printf("\n");
	}




void Draw_filed() {
	unsigned int modelLocation = glGetUniformLocation(s_program, "modelTransform"); //--- 버텍스 세이더에서 모델링 변환 위치 가져오기
	unsigned int modelLocation1 = glGetUniformLocation(s_program, "in_Color"); //--- 버텍스 세이더에서 모델링 변환 위치 가져오기

	glBindVertexArray(vao);

	for (int i = 0; i < Ycount * Ycount; i = i + Ycount) {
		for (int j = 0; j < xcount; j++) {
			glm::mat4 TR = glm::mat4(1.0f);
			glm::mat4 Tx = glm::mat4(1.0f);
			glm::mat4 Scale = glm::mat4(1.0f);

			glUniform3f(modelLocation1, 0.1 * All_Box[i + j].BColor[0], 0.1 * All_Box[i + j].BColor[1], 0.1 * All_Box[i + j].BColor[2]);
			Scale = glm::scale(Scale, glm::vec3(All_Box[i + j].Bscale)); //		각 사각형 크기 
			Tx = glm::translate(Tx, All_Box[i + j].Blocate);
			Tx = glm::translate(Tx, glm::vec3(0.0f, drop, 0.0f));

			TR = Tx * Scale * TR;

			glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(TR)); //--- modelTransform 변수에 변환 값 적용하기
			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (GLvoid*)(sizeof(GLuint) * 0));
		}
	}

	glm::mat4 TR = glm::mat4(1.0f);
	glm::mat4 Tx = glm::mat4(1.0f);
	glm::mat4 Scale = glm::mat4(1.0f);
	Tx = glm::translate(Tx, glm::vec3(0.0f, dropboard, 0.0f));
	Scale = glm::scale(Scale, glm::vec3(11.0f, 10.0f, 11.0f));
	TR = Tx * Scale * TR;
	glUniform3f(modelLocation1, 0.0f, 0.0f, 0.0f);
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(TR)); //--- modelTransform 변수에 변환 값 적용하기
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (GLvoid*)(sizeof(GLuint) * 0));

}