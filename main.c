#include <stdio.h>
#include <unistd.h>

#define GLEW_STATIC
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include "defs.h"

const float vertices[9] = {
    //    X      Y     Z
        -0.5f, -0.5f, 0.0f, // left  
         0.5f, -0.5f, 0.0f, // right 
         0.0f,  0.5f, 0.0f  // top   
};

const char* vertexSource = 
    "#version 330 core\n"
    "layout(location = 0) in vec4 pos; \n"
    "void main(){ \n"
    "gl_Position = pos; }\n";

const char* fragmentSource = 
    "#version 330 core\n"
    "out vec4 color;\n"
    "void main(){ color = vec4(0.1f, 0.5f, 0.5f, 1.0f); }";

int main(void){

    int pid = getpid();
    printf("process pid: %i\n", pid);

    // initialization
    if(SDL_Init(SDL_INIT_VIDEO) != 0){
        SDL_Log("%s", "failed to initialize SDL2 !\n");
    } else{ SDL_Log("%s", "SDL2 initialized successfully!\n");}

    setattrb();

    SDL_Event* e = (SDL_Event* ) malloc(sizeof(SDL_Event));
    SDL_Window* w = SDL_CreateWindow("OpenGL Triangle With SDL2", 
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    SDL_GLContext* c = SDL_GL_CreateContext(w);
    SDL_GL_MakeCurrent(w, c);

    glewExperimental = GL_TRUE;
    GLenum glew_status = glewInit();
    if(glew_status != GLEW_OK){
        SDL_Log("%s", "Failed to initialize GLEW\n");
        SDL_Log("%s", glewGetErrorString(glew_status));
        SDL_Quit();
        return -1;
    } else{ SDL_Log("%s", "Glew Initialized successfully!\n"); }

    printf("GL version:%s\n", glGetString(GL_VERSION));
    printf("GLSL version:%s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

    // end of initialization //

    // shader creation
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(vertexShader, 1, &vertexSource, NULL);
    glShaderSource(fragmentShader, 1, &fragmentSource, NULL);

    GLint compile_status;
    int buff_length = 0;
    glCompileShader(vertexShader);
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &compile_status);    // get compile status of vertexShader
    if(compile_status != GL_TRUE){
        glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &buff_length);
        char buffer[buff_length];
        glGetShaderInfoLog(vertexShader, buff_length, NULL, buffer);
        SDL_Log("\n%s\n", buffer);

        glDeleteShader(vertexShader);
        SDL_Quit();
        return -1;
    }else {SDL_Log("%s", "vertex shader compiled!\n"); }

    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &compile_status);  // get compile status of fragmentShader
    if(compile_status != GL_TRUE){
        glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &buff_length);
        char buffer[buff_length];
        glGetShaderInfoLog(fragmentShader, buff_length, NULL, buffer);
        SDL_Log("%s", buffer);
        deleteShaders(vertexShader, fragmentShader);
        SDL_Quit();
        return -1;
    } else {SDL_Log("%s", "fragment shader compiled!\n"); }

    // linking shaders together into a program
    int validate_status;
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glValidateProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_VALIDATE_STATUS, &validate_status);
    if(validate_status != GL_TRUE){
        SDL_Log("%s", "Program validation failed!\n");
        glDetachShader(shaderProgram, vertexShader);
        glDetachShader(shaderProgram, fragmentShader);
        deleteShaders(vertexShader, fragmentShader);
        SDL_Quit();
    }

    // check up
    GLint link_status;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &link_status);
    if(!link_status){
        char buffer[512];
        SDL_Log("%s", "Failed to link shaders into program!\n");
        glGetProgramInfoLog(shaderProgram, 512, NULL, buffer);
        SDL_Log("%s", buffer);
        deleteShaders(vertexShader, fragmentShader);
        glDeleteProgram(shaderProgram);
        SDL_Quit();
        return -1;
    }
    // cleaning up
    glDetachShader(shaderProgram, vertexShader);
    glDetachShader(shaderProgram, fragmentShader);
    deleteShaders(vertexShader, fragmentShader);

    GLuint vbo; // Vertex Buffer Objects
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(float), vertices, GL_STATIC_DRAW); // copying data to buffer

    GLuint vao; // Vertex Array Objecs
    glGenVertexArrays(1, &vao);

    glBindVertexArray(vao);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL); // specifying to OpenGL how to interpret vertex data 
    
    setColor(0xB0E0E6);
    
    int quit = 0;
    while(!quit){

        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(shaderProgram);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        SDL_WaitEvent(e);
        switch(e->type){

            case SDL_KEYDOWN:
                switch(e->key.keysym.sym){
                    case SDL_QUIT:
                        quit = 1;
                    break;
                    case SDLK_q:
                        quit = 1;
                    break;
                }
            break;
        }
        
        SDL_GL_SwapWindow(w);
    }

    // cleaning up
    free(e);
    glDeleteProgram(shaderProgram);
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
    SDL_DestroyWindow(w);
    SDL_GL_DeleteContext(c);
    SDL_Log("%s", "Bye!\n");
    SDL_Quit();
    return 0;
}

void setattrb(void){
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE,8);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
    SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
}

void setColor(Uint32 color){

    float r = ((color >> 16) & 0xFF) / 255.0f;
    float g = ((color >> 8) & 0xFF) / 255.0f;
    float b = (color & 0xFF) / 255.0f;
    
    glClearColor(r, g, b, 1.0f);
}

void deleteShaders(unsigned int vs, unsigned int fs){
    glDeleteShader(vs);
    glDeleteShader(fs);
}