#version 150

uniform mat4 u_Model;
uniform mat4 u_ViewProj;

in vec4 vs_Pos;
in vec4 vs_Col;

out vec3 fs_Pos;
out vec4 fs_Col;

void main() {
  fs_Col = vs_Col;
  
  vec4 modelposition = u_Model * vs_Pos;
  fs_Pos = modelposition.xyz;

  gl_Position = u_ViewProj * modelposition;
}