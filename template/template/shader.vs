#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec3 vertex_color;

uniform mat4 projection_matrix;
uniform mat4 model_view_matrix;

out vec3 gl_Color; // color of vertex to pass in fragment shader

void main(){
    gl_Position = projection_matrix * model_view_matrix * vec4(vertex_position, 1.0);
	gl_Color = vertex_color;
}

