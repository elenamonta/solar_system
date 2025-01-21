#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

uniform mat4 projection;
uniform mat4 view;

void main(){	
  TexCoords = aPos;
  mat4 Static_View=mat4(mat3(view));  
  gl_Position = projection * Static_View * vec4(aPos, 1.0);
}


