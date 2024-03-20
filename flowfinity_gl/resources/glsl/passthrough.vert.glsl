#version 150

uniform mat4 u_Model;
uniform mat4 u_ViewProj;

in vec4 vs_Pos;
in vec4 vs_Col;
in vec4 vs_Nor;

out vec3 fs_Pos;
out vec4 fs_Col;
out vec4 fs_Nor;

void main() {
  fs_Col = vs_Col;
  fs_Nor = vs_Nor;
  
  vec4 modelposition = u_Model * vs_Pos;
  fs_Pos = modelposition.xyz;

  gl_Position = u_ViewProj * modelposition;
}