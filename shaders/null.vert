#version 330

uniform vec2 _dim;
uniform vec2 _spot;

out DETAILS
{
vec2 dim;
vec2 spot;
} details;

void main()
{
details.dim = _dim;
details.spot = _spot;
}