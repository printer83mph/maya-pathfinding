#version 150

in vec4 fs_Col;
in vec4 fs_Nor;

out vec4 out_Col;

void main()
{
    out_Col = (fs_Nor + vec4(1.0))/2.0;
}
