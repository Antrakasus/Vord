#include <iostream>
#include <fstream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

std::string shaderNames[]={"Vertex", "Fragment", "Tesselation Control", "Tesselation Evaluation", "Geometry", "Compute"};
std::string shaderSuffix[]={"vert","frag","tesc", "tese", "geom", "comp"};
GLuint shaderCodes[]={GL_VERTEX_SHADER, GL_FRAGMENT_SHADER, GL_TESS_EVALUATION_SHADER, GL_TESS_CONTROL_SHADER, GL_GEOMETRY_SHADER, GL_COMPUTE_SHADER};
bool activeShaders[]={true, true, false, false, false, false};

struct vec3{
    float x,y,z;
    vec3(float _x=0, float _y=0, float _z=0): 
    x(_x) , y(_y) , z(_z){}
};

struct Tri{
    vec3 normal,v1,v2,v3;
    short attribute;
    Tri(vec3 _normal=vec3(), vec3 _v1=vec3(), vec3 _v2=vec3(), vec3 _v3=vec3(), short _attr=0) :
    normal(_normal), v1(_v1), v2(_v2), v3(_v3), attribute(_attr){}
};

GLFWwindow* window;
Tri * triangles;
GLuint VBO;
GLuint shaderProgram;

void renderFrame(){
    glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glDisableVertexAttribArray(0);
    glfwSwapBuffers(window);
}

int generateShaders(std::string shader="main"){
    if(shaderProgram){
        glDeleteProgram(shaderProgram);
        shaderProgram = glCreateProgram();
    }
    GLuint shaders[sizeof(activeShaders)/sizeof(bool)];
    std::fstream shaderStream;
    int streamLength;
    
    for(short int i=0; i<sizeof(activeShaders)/sizeof(bool);i++){
        if(!activeShaders[i]){
            continue;
        }
        shaderStream.open("./shaders/"+shader+"."+shaderSuffix[i], std::ios_base::in);
        if(!shaderStream.is_open()){
            std::cout<<"Couldn't open ./shaders/" +shader+"."+shaderSuffix[i]+"\n";
            return 1;
        }
        shaderStream.seekg(0,shaderStream.end);
        streamLength=shaderStream.tellg();
        shaderStream.seekg(0, shaderStream.beg);
        std::cout<<streamLength<<" a "<<shaderStream.tellg()<< " "<< shaderStream.good()<<"\n";
        std::cout<<"Test 1\n";
        char* streamBuffer[streamLength];
        std::cout<<"Test 2\n";
        shaderStream.read(NULL,1);
        std::cout<<"Test 3\n";
        shaders[i] = glCreateShader(shaderCodes[i]);
        std::cout<<"Test 4\n";
        glShaderSource(shaders[i],1,streamBuffer,NULL);
        std::cout<<"Test 5\n";
        glCompileShader(shaders[i]);
        GLint result; 
        glGetShaderiv( shaders[i], GL_COMPILE_STATUS, &result ); 
        std::cout<<"Test 6\n";
        if( GL_FALSE == result ) { 
            std::cerr << shaderNames[i] << " shader compilation failed!" << std::endl;
            GLint logLen; 
            glGetShaderiv(shaders[i], GL_INFO_LOG_LENGTH, &logLen); 
            if( logLen > 0 ) { 
                std::string log(logLen, ' '); 
                GLsizei written; 
                glGetShaderInfoLog(shaders[i], logLen, &written, &log[0]); 
                std::cerr << "Shader log: " << std::endl << log;
            } 
            glDeleteShader(shaders[i]);
        }else{
            glAttachShader( shaderProgram, shaders[i] );
        }
    }
    
    glLinkProgram(shaderProgram);
    GLint isLinked = 0;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, (int *)&isLinked);
    if (isLinked == GL_FALSE)
    {
        GLint maxLength = 0;
        glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &maxLength);

        // The maxLength includes the NULL character
        GLchar infoLog[maxLength];
        glGetProgramInfoLog(shaderProgram, maxLength, &maxLength, &infoLog[0]);

        std::cout<<infoLog <<"\n";
    }
    for(short int i=0; i<sizeof(activeShaders)/sizeof(bool);i++){
        if(!activeShaders[i]){
            continue;
        }
        glDeleteShader(shaders[i]);
        glDetachShader(shaderProgram,shaders[i]);
    }
    return 0;
}

int openglSetup(){
    if(!glfwInit()){
        std::cout << "GLFW Init failed\n";
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,6);
    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(800,800, "Test",NULL, NULL);
    if(window==NULL){
        std::cout<<"Window Init failed\n";
        glfwTerminate();
        return 1;
    }

    glfwMakeContextCurrent(window);

    gladLoadGL();

    glViewport(0, 0, 800, 800);

    if(generateShaders()){
        return 1;
    }
    std::cout<<"Compiled Shaders Successfully!\n";

    return 0;
}

int main(){
    if(openglSetup()){
        return 1;
    }

    std::string stlPath;
    std::fstream strm;
    /*std::cout<<"Please provide a path to the binary STL file: ";
    std::cin>>stlPath;
    std::cout<<"\n";*/
    stlPath="C:\\Users\\Antrakasus\\Downloads\\Adult Black Dragon Head (88.2%).stl";
    
    strm.open(stlPath, std::ios_base::in | std::ios::binary);
    if(!strm.is_open()){
        std::cout<<"Couldn't open file, sorry!\n";
        return 1;
    }
    strm.seekg(80);
    char stlBuffer[50];
    unsigned int streamLength;
    strm.read(stlBuffer,4);
    memcpy(&streamLength,&stlBuffer,sizeof(int));
    std::cout<<"Stl triangle count: " << streamLength << "\n";

    triangles = (Tri*) malloc(streamLength*sizeof(Tri));
    for(short i=0; i<=streamLength; i++){
        strm.read(stlBuffer,50);
        memcpy(&triangles[i],&stlBuffer, 50);
    }
    std::cout <<"Done!\n";

    vec3 Vertices[1];
    Vertices[0] = vec3(-1.0f, -1.0f, 0.0f);
    Vertices[1] = vec3(1.0f, -1.0f, 0.0f);
    Vertices[2] = vec3(0.0f, 1.0f, 0.0f);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);
    while(!glfwWindowShouldClose(window)){
        glfwPollEvents();
        renderFrame();
    }
    strm.close();
    if(strm.is_open()){
        std::cout<<"Couldn't close filestream!\n";
        return 1;
    }
    glfwDestroyWindow(window);
    glfwTerminate();
    free(triangles);
    return 0;
}