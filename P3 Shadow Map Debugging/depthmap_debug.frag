#version 330

in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D directionalShadowMap;

void main()
{
      float depth = texture(directionalShadowMap, TexCoord).r;
      FragColor = vec4(vec3(depth), 1.0);
}
