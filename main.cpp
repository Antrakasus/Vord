#include "main.hpp"

struct Object{
	glm::vec3 scale;
	glm::vec3 position;
	glm::vec4 rotation;
	glm::vec2 uv;
	glm::vec3* mesh;
	uint id;
	uint count;
};


struct stlTri{
    glm::vec3 normal,v1,v2,v3;
    short attribute;
    stlTri(glm::vec3 _normal=glm::vec3(), glm::vec3 _v1=glm::vec3(), glm::vec3 _v2=glm::vec3(), glm::vec3 _v3=glm::vec3(), short _attr=0) :
    normal(_normal), v1(_v1), v2(_v2), v3(_v3), attribute(_attr){}
};


struct stlObject{
	stlTri* triangles;
	uint count;
};

struct objObject{
	
};


Object triangle;
Object triangle2;


void frame();
void draw(const Object o);
void glSetup();
void addToDraw(Object* o);
int removeFromDraw(Object* o);
void keyHandler(GLFWwindow* window, int key, int scancode, int action, int mods);
int windowSetup();
Object stlToMesh(stlObject STL);
stlObject readStl(std::string path);
GLuint LoadShaders(const char * vertex_file_path,const char * fragment_file_path);

std::queue<Object*> toDraw;
GLFWmonitor** monitors;
GLFWwindow* window;
GLuint programID;
GLuint VertexArrayID;
GLuint vertexBuffer;
glm::vec3 camera;
Object dragon;

float* draw_buffer;

glm::mat4 myScalingMatrix = glm::scale(glm::vec3(0.5f,0.5f,0.5f));

float timeRandom(float x=0, float y=9){
	return sin(x+std::chrono::system_clock::now().time_since_epoch().count()*pow(0.1,y));
}


int main(){
	triangle.mesh = new glm::vec3[3]{
	glm::vec3(-1.0f, -1.0f, 0.0f),
   	glm::vec3(1.0f, -1.0f, 0.0f),
   	glm::vec3(0.0f,  1.0f, 0.0f)
	};

	triangle.position=glm::vec3(0,0,0);
	triangle.count=3;
	triangle.id=0;
	triangle2=triangle;
	triangle2.id=1;
	dragon=stlToMesh(readStl("/home/zhynx/Downloads/Adult Black Dragon Head (88.2%).stl"));
	if(windowSetup()!=0){
        return -1;
    }
    glSetup();
    programID = LoadShaders( "mainVert.glsl", "mainFrag.glsl" );
	glUseProgram(programID);
	glfwSetKeyCallback(window,keyHandler);
	addToDraw(&triangle);
	addToDraw(&triangle2);
    while(!glfwWindowShouldClose(window)){
        glfwPollEvents();
        frame();
    }
	glfwDestroyWindow(window);
    glfwTerminate();
	free(dragon.mesh);
    return 0;
}


void frame(){
    glClearColor(0.0f,0.1f,0.2f,1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	camera.x=timeRandom(1)*5;
	camera.y=timeRandom(2)*5;
	camera.z=timeRandom(3)*5-10;
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	Object* current;
	for(int i=0; i<toDraw.size();i++){
		current=toDraw.front();
		current->position.x=timeRandom(current->id);
		draw(*current);
		toDraw.pop();
		toDraw.push(current);
	}
    glfwSwapBuffers(window);
}


void draw(const Object o){
    glEnableVertexAttribArray(0);
    glBufferData(GL_ARRAY_BUFFER, o.count*3*sizeof(float), o.mesh, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(
        0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0
    );
	glm::mat4 translation= glm::translate(o.position);
	glUniformMatrix4fv(glGetUniformLocation(programID,"translation"),1,GL_FALSE,&translation[0][0]);
    glDrawArrays(GL_TRIANGLES, 0, o.count);
    glDisableVertexAttribArray(0);
};


void addToDraw(Object* o){
	toDraw.push(o);
}


int removeFromDraw(Object* o){
	uint count=0;
	uint size=toDraw.size();
	Object* current;
	for(uint i=0; i<size;i++){
		current=toDraw.front();
		toDraw.pop();
		if(current->id==o->id){
			count++;
		}else{
			toDraw.push(current);
		}
	}
	return count;
}


void keyHandler(GLFWwindow* window, int key, int scancode, int action, int mods){

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS){
		glfwSetWindowShouldClose(window,GL_TRUE);
	}

}


Object stlToMesh(stlObject STL){
	Object out;
	out.count=STL.count;
	out.mesh=(glm::vec3*)malloc(out.count*9*sizeof(float));
	for(int i=0; i<STL.count;i++){
		out.mesh[i*3]=STL.triangles[i].v1;
		out.mesh[i*3+1]=STL.triangles[i].v2;
		out.mesh[i*3+2]=STL.triangles[i].v3;
	}
	return out;
}


stlObject readStl(std::string path){

	stlObject STL;
    std::fstream strm;
    strm.open(path, std::ios_base::in | std::ios::binary);
    if(!strm.is_open()){
        std::cout<<"Couldn't open file, sorry!\n";
        return stlObject();
    }
	char header[80];
    strm.read(header,80);
    char stlBuffer[50];
    strm.read(stlBuffer,4);
    memcpy(&STL.count,&stlBuffer,sizeof(int));
    STL.triangles = (stlTri*) malloc(STL.count*sizeof(stlTri));
    for(int i=0; i<STL.count; i++){
        strm.read(stlBuffer,50);
        memcpy(&STL.triangles[i],&stlBuffer,50);
    }
	return STL;
}


objObject readObj(std::string path){
	objObject out;
	return out;
}


void glSetup(){
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);
    glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glEnable(GL_CULL_FACE);
}


int windowSetup(){
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
        //fprintf(stdout,"Found %i monitor(s)\n",count);
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
    //fprintf(stdout, "Window setup successful!\n");
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
	//printf("Compiling shader : %s\n", vertex_file_path);
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
	//printf("Compiling shader : %s\n", fragment_file_path);
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
	//printf("Linking program\n");
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