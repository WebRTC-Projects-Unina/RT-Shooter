#include <iostream>
#include <fstream>
#include <string>
#include <sstream>


#include <GL/gl.h>
#include <GLES3/gl3.h>
#include "renderer.h"







unsigned int CompileShader(unsigned int type, const std::string source) {
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    //qui facciamo error handling
    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE) {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);

        //perchè non posso definire un array nell'heap usando come dimensioe una varibiale ("length" nel nostro caso)
        //ma C++ vuole una costante -> lui lo aggira così
        char* message = (char*) alloca(length * sizeof(char)); 
        glGetShaderInfoLog(id, length, &length, message);
        
        std::cout << "Compilazione del " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << "shader fallita!" << std::endl;
        std::cout << message << std::endl;

        glDeleteShader(id); //cancello lo shader che ha fallito la compilazione
        return 0;

    }

    return id;
}

/*
* Si prende gli shader come stringe dai file in cui li definisco 
* (potrei definirli direttamente come strinfa in questo codice)
*/
unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader) {

    unsigned int program = glCreateProgram();
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader); // vs sta per vertex shader
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader); // fs sta per fragment shader

    glAttachShader(program, vs);
    glAttachShader(program, fs);

    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vs); //come se cancellassi glo .obj (le parti intermedie della compilazione)
    glDeleteShader(fs); 

    return program;

}