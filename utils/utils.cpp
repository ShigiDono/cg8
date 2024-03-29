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
    //dirty hack
    if (shader->normal_matrix < 32) {
        shader->apply_normal_matrix(glm::inverseTranspose(glm::mat3(matrix)));
    }

    for (int i = 0; i < buffers.size(); ++i) {
        buffers[i]->bind();
    }
    glDrawArrays(primitive_kind, 0, count); // 3 indices starting at 0 -> 1 triangle
//    glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_SHORT, 0);
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



model_t *loadOBJ(const char * path, shader_t *shader) {
	std::vector<glm::vec3> out_vertices; 
	std::vector<glm::vec2> out_uvs;
	std::vector<glm::vec3> out_normals;
	printf("Loading OBJ file %s...\n", path);

	std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
	std::vector<glm::vec3> temp_vertices; 
	std::vector<glm::vec2> temp_uvs;
	std::vector<glm::vec3> temp_normals;


	FILE * file = fopen(path, "r");
	if( file == NULL ){
		printf("Impossible to open the file ! Are you in the right path ? See Tutorial 1 for details\n");
		return NULL;
	}

	while( 1 ){

		char lineHeader[128];
		// read the first word of the line
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF)
			break; // EOF = End Of File. Quit the loop.

		// else : parse lineHeader
		
		if ( strcmp( lineHeader, "v" ) == 0 ){
			glm::vec3 vertex;
			fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z );
			temp_vertices.push_back(vertex);
		}else if ( strcmp( lineHeader, "vt" ) == 0 ){
			glm::vec2 uv;
			fscanf(file, "%f %f\n", &uv.x, &uv.y );
			uv.y = -uv.y; // Invert V coordinate since we will only use DDS texture, which are inverted. Remove if you want to use TGA or BMP loaders.
			temp_uvs.push_back(uv);
		}else if ( strcmp( lineHeader, "vn" ) == 0 ){
			glm::vec3 normal;
			fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z );
			temp_normals.push_back(normal);
		}else if ( strcmp( lineHeader, "f" ) == 0 ){
			std::string vertex1, vertex2, vertex3;
			unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
			int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2] );
			if (matches != 9 && matches != 6){
				printf("File can't be read by our simple parser :-( Try exporting with other options\n");
				return NULL;
			}
			vertexIndices.push_back(vertexIndex[0] - 1);
			vertexIndices.push_back(vertexIndex[1] - 1);
			vertexIndices.push_back(vertexIndex[2] - 1);
			uvIndices    .push_back(uvIndex[0]);
			uvIndices    .push_back(uvIndex[1]);
			uvIndices    .push_back(uvIndex[2]);
			normalIndices.push_back(normalIndex[0]);
			normalIndices.push_back(normalIndex[1]);
			normalIndices.push_back(normalIndex[2]);
		}else{
			// Probably a comment, eat up the rest of the line
			char stupidBuffer[1000];
			fgets(stupidBuffer, 1000, file);
		}

	}

	// For each vertex of each triangle
	for( unsigned int i=0; i<vertexIndices.size(); i++ ){

		// Get the indices of its attributes
		//glm::vec3 a = temp_vertices[vertexIndices[i*3 + 0]];
		//glm::vec3 b = temp_vertices[vertexIndices[i*3 + 1]];
		//glm::vec3 c = temp_vertices[vertexIndices[i*3 + 2]];


		//unsigned int uvIndex = uvIndices[i];
		unsigned int normalIndex = normalIndices[i];
		
		// Get the attributes thanks to the index
		glm::vec3 vertex = temp_vertices[ vertexIndices[i] ];
		//glm::vec2 uv = temp_uvs[ uvIndex-1 ];
		glm::vec3 normal = /*glm::cross(c - a, b - a);/*/temp_normals[ normalIndex-1 ];
        /*if (normal.x && normal.y && normal.z) {
            normal = glm::normalize(normal);
        }*/
		
		// Put the attributes in buffers
		out_vertices.push_back(vertex);
		//out_uvs     .push_back(uv);
		out_normals.push_back(normal);
		//out_normals.push_back(normal);
		//out_normals.push_back(normal);
	
	}
    model_t *model = new model_t(shader);
    model->buffers.push_back(new buffer_t(0, &(out_vertices[0].x), out_vertices.size()*3));
    model->buffers.push_back(new buffer_t(1, &(out_normals[0].x), out_normals.size()*3));
    //model->buffers.push_back(new buffer_t(&(vertexIndices[0]), vertexIndices.size()));
    model->count = out_vertices.size();
	return model;
}

