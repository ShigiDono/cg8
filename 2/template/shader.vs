#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec3 vertex_normal;

uniform mat4 projection_matrix;
uniform mat4 model_view_matrix;
uniform mat3 normal_matrix;

//out vec3 gl_Color; // color of vertex to pass in fragment shader
out vec3 normal; // vertex normal
out vec3 eyeVec;
out vec3 lightDir;
//comment this
out vec3 pos;
//this too
const vec3 light_pos = vec3(100.0f, 10.0f, 10.0f);

void main(){
	normal = normal_matrix * vertex_normal;
	
	vec3 vVertex = vec3(model_view_matrix * vec4(vertex_position, 1.0));

	lightDir = vec3(light_pos.xyz - vVertex);
	eyeVec = -vVertex;

	//comment this
	pos = vertex_position;
	//until

    gl_Position = projection_matrix * model_view_matrix * vec4(vertex_position, 1.0);
}

