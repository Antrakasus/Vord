#include "main.hpp"

typedef float vec3[3];
typedef vec3 tri[4];

void frame();
void draw();
void glSetup();
void keyHandler(GLFWwindow* window, int key, int scancode, int action, int mods);
int windowSetup();
tri* readStl(std::string path);
GLuint LoadShaders(const char * vertex_file_path,const char * fragment_file_path);


GLFWmonitor** monitors;
GLFWwindow* window;
GLuint programID;
GLuint VertexArrayID;
GLuint vertexbuffer;
glm::vec3 camera;
tri* dragon;

float g_vertex_buffer_data[] = {
   -1.0f, -1.0f, 0.0f,
   1.0f, -1.0f, 0.0f,
   0.0f,  1.0f, 0.0f,
};


float timeRandom(float x=0, float y=9){
	return sin(x+std::chrono::system_clock::now().time_since_epoch().count()*pow(0.1,y));
}


int main(){
	dragon = readStl("/home/zhynx/Downloads/Adult Black Dragon Head (88.2%).stl");
    if(windowSetup()!=0){
        return -1;
    }
    glSetup();
    programID = LoadShaders( "main.vs", "main.fs" );
	glUseProgram(programID);
	glfwSetKeyCallback(window,keyHandler);
    while(!glfwWindowShouldClose(window)){
        glfwPollEvents();
        frame();
    }
	glfwDestroyWindow(window);
    glfwTerminate();
	free(dragon);
    return 0;
}


void frame(){
    glClearColor(0.0f,0.1f,0.2f,0.5f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    draw();
    glfwSwapBuffers(window);
}


void draw(){
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	camera.x=timeRandom(1)*5;
	camera.y=timeRandom(2)*5;
	camera.z=timeRandom(3)*5-10;

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glVertexAttribPointer(
        0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0
    );
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glDisableVertexAttribArray(0);
};


void keyHandler(GLFWwindow* window, int key, int scancode, int action, int mods){

	if (key == GLFW_KEY_E && action == GLFW_PRESS){
		glfwSetWindowShouldClose(window,GL_TRUE);
	}

}


tri* readStl(std::string path){
    std::fstream strm;
    strm.open(path, std::ios_base::in | std::ios::binary);
    if(!strm.is_open()){
        std::cout<<"Couldn't open file, sorry!\n";
        return NULL;
    }
    strm.seekg(80);
    char stlBuffer[50];
    unsigned int streamLength;
    strm.read(stlBuffer,4);
    memcpy(&streamLength,&stlBuffer,sizeof(int));
    std::cout<<"Stl triangle count: " << streamLength << "\n";

    tri* triangles = (tri*) malloc(streamLength*sizeof(tri));
    for(short i=0; i<=streamLength; i++){
        strm.read(stlBuffer,50);
        memcpy(&triangles[i],&stlBuffer, 50);
    }
	return triangles;
}


void glSetup(){
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_DYNAMIC_DRAW);
}


int windowSetup(){
    fprintf(stdout,"Starting \n");
	glfwInit();
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_DECORATED,GL_FALSE);
    int count;
    monitors = glfwGetMonitors(&count);
    if(count==0){
        fprintf(stderr,"Couldn't find any monitor \n");
    }else{
        fprintf(stdout,"Found %i monitor(s)\n",count);
    }
    window = glfwCreateWindow( 960, 540, "Test", NULL, NULL);
    if( window == NULL ){
        fprintf( stderr, "Failed to open GLFW window \n" );
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glewExperimental=GL_TRUE;
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW \n");
        return -1;
    }
    fprintf(stdout, "Window setup successful!\n");
    //glfwSetWindowMonitor(window,NULL,100,100,1024,768,GLFW_DONT_CARE);
    return 0;
}




GLuint LoadShaders(const char * vertex_file_path,const char * fragment_file_path){

	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// Read the Vertex Shader code from the file
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
	if(VertexShaderStream.is_open()){
		std::stringstream sstr;
		sstr << VertexShaderStream.rdbuf();
		VertexShaderCode = sstr.str();
		VertexShaderStream.close();
	}else{
		printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", vertex_file_path);
		getchar();
		return 0;
	}

	// Read the Fragment Shader code from the file
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
	if(FragmentShaderStream.is_open()){
		std::stringstream sstr;
		sstr << FragmentShaderStream.rdbuf();
		FragmentShaderCode = sstr.str();
		FragmentShaderStream.close();
	}

	GLint Result = GL_FALSE;
	int InfoLogLength;

	// Compile Vertex Shader
	printf("Compiling shader : %s\n", vertex_file_path);
	char const * VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer , NULL);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		std::vector<char> VertexShaderErrorMessage(InfoLogLength+1);
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		printf("%s\n", &VertexShaderErrorMessage[0]);
	}

	// Compile Fragment Shader
	printf("Compiling shader : %s\n", fragment_file_path);
	char const * FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , NULL);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		std::vector<char> FragmentShaderErrorMessage(InfoLogLength+1);
		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
		printf("%s\n", &FragmentShaderErrorMessage[0]);
	}

	// Link the program
	printf("Linking program\n");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		std::vector<char> ProgramErrorMessage(InfoLogLength+1);
		glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		printf("%s\n", &ProgramErrorMessage[0]);
	}
	
	glDetachShader(ProgramID, VertexShaderID);
	glDetachShader(ProgramID, FragmentShaderID);
	
	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	return ProgramID;
}