#version 460 core
out vec4 FragColor;

in VS_OUT
{
	vec4 color;
} fs_in;

void main()
{
	FragColor = fs_in.color;
}