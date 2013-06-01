#include "utils.h"

#include <string>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>

#ifndef max
#define max(a, b) ((a) > (b) ? (a) : (b))
#endif//max

buffer_t::buffer_t(GLuint attribute, GLfloat *vertices, unsigned int count, unsigned int size): 
    attribute(attribute), count(count), size(size)
{
    glGenBuffers(1, &buffer); // Generate our Vertex Buffer Object  
    glBindBuffer(GL_ARRAY_BUFFER, buffer); // Bind our Vertex Buffer Object  
    glBufferData(GL_ARRAY_BUFFER, count * sizeof(GLfloat), vertices, GL_STATIC_DRAW); // Set the size and data of our VBO and set it to STATIC_DRAW  
}

buffer_t::buffer_t(GLushort *indices, unsigned int count, unsigned int size): attribute(-1), count(count) {
    glGenBuffers( 1, &buffer );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, buffer );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, count*sizeof(unsigned short), 
			      indices, GL_STATIC_DRAW );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
}

void buffer_t::bind() {
    if (attribute == (GLuint)-1) {
        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, buffer );
        return;
    }
    glEnableVertexAttribArray(attribute);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glVertexAttribPointer(
		attribute,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
		size,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);
}

model_t::model_t(shader_t *shader_param): shader(shader_param) {
}
model_t::~model_t() {
    while (!buffers.size()) {
        delete buffers[buffers.size() - 1];
        buffers.pop_back();
    }
}

void model_t::draw(glm::mat4 matrix, GLuint primitive_kind, GLuint count) {

    shader->bind();
    shader->apply_model_matrix(matrix);

    for (int i = 0; i < buffers.size(); ++i) {
        buffers[i]->bind();
    }
    glDrawArrays(primitive_kind, 0, count); // 3 indices starting at 0 -> 1 triangle
    glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_SHORT, 0);
}

void model_t::draw_indexed(glm::mat4 matrix, GLuint primitive_kind, GLuint count) {

    shader->bind();
    shader->apply_model_matrix(matrix);
    shader->apply_normal_matrix(glm::inverseTranspose(glm::mat3(matrix)));


    for (int i = 0; i < buffers.size(); ++i) {
        buffers[i]->bind();
    }
    //glDrawArrays(primitive_kind, 0, count); // 3 indices starting at 0 -> 1 triangle
    glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_SHORT, 0);
}
shader_t::shader_t(const char * vertex_file_path,const char * fragment_file_path){
 
    // Create the shaders
    GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
 
    // Read the Vertex Shader code from the file
    std::string VertexShaderCode;
    std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
    if(VertexShaderStream.is_open())
    {
        std::string Line = "";
        while(getline(VertexShaderStream, Line))
            VertexShaderCode += "\n" + Line;
        VertexShaderStream.close();
    }
 
    // Read the Fragment Shader code from the file
    std::string FragmentShaderCode;
    std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
    if(FragmentShaderStream.is_open()){
        std::string Line = "";
        while(getline(FragmentShaderStream, Line))
            FragmentShaderCode += "\n" + Line;
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
    std::vector<char> VertexShaderErrorMessage(InfoLogLength);
    glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
    fprintf(stdout, "%s\n", &VertexShaderErrorMessage[0]);
 
    // Compile Fragment Shader
    printf("Compiling shader : %s\n", fragment_file_path);
    char const * FragmentSourcePointer = FragmentShaderCode.c_str();
    glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , NULL);
    glCompileShader(FragmentShaderID);
 
    // Check Fragment Shader
    glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    std::vector<char> FragmentShaderErrorMessage(InfoLogLength);
    glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
    fprintf(stdout, "%s\n", &FragmentShaderErrorMessage[0]);
 
    // Link the program
    fprintf(stdout, "Linking program\n");
    GLuint ProgramID = glCreateProgram();
    glAttachShader(ProgramID, VertexShaderID);
    glAttachShader(ProgramID, FragmentShaderID);
    glLinkProgram(ProgramID);
 
    // Check the program
    glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
    glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    std::vector<char> ProgramErrorMessage( max(InfoLogLength, int(1)) );
    glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
    fprintf(stdout, "%s\n", &ProgramErrorMessage[0]);
 
    glDeleteShader(VertexShaderID);
    glDeleteShader(FragmentShaderID);
        
    program = ProgramID;
    projection_matrix = glGetUniformLocation(ProgramID, "projection_matrix");;
    model_view_matrix = glGetUniformLocation(ProgramID, "model_view_matrix");;
    normal_matrix = glGetUniformLocation(ProgramID, "normal_matrix");;
}

void shader_t::bind() {
    glUseProgram(program);
    glUniformMatrix4fv(projection_matrix, 1, GL_FALSE, &projection[0][0]);
}

void shader_t::set_projection_matrix(const glm::mat4 &matrix) {
    projection = matrix;
}
void shader_t::set_view_matrix(const glm::mat4 &matrix) {
    view = matrix;
}

void shader_t::apply_model_matrix(const glm::mat4 &matrix) {
    glUniformMatrix4fv(model_view_matrix, 1, GL_FALSE, &matrix[0][0]);
}

void shader_t::apply_normal_matrix(const glm::mat3 &matrix) {
    glUniformMatrix3fv(normal_matrix, 1, GL_FALSE, &matrix[0][0]);
}