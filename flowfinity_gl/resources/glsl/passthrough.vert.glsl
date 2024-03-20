#version 150

uniform mat4 u_Model;
uniform mat4 u_ViewProj;
uniform mat3 u_ModelInvTr;
uniform vec3 u_CamPos;

in vec4 vs_Pos;
in vec4 vs_Col;
in vec4 vs_Nor;

out vec3 fs_Pos;
out vec4 fs_Col;
out vec4 fs_Nor;
out vec4 fs_LightVec;

void main() {
  fs_Col = vs_Col;
  fs_Nor = normalize(vec4(u_ModelInvTr * vec3(vs_Nor), 0));
  
  vec4 modelposition = u_Model * vs_Pos;
  fs_Pos = modelposition.xyz;

  fs_LightVec = (vec4(u_CamPos, 1)) - modelposition;  // Compute the direction in which the light source lies

  gl_Position = u_ViewProj * modelposition;
}