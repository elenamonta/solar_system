#version 330 core

in vec4 fragColor;
out vec4 FragColor;

uniform bool viewBB; 

void main() {
    if(viewBB){
        FragColor = fragColor;
    }else{
        discard; 
    }
}
