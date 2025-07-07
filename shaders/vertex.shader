#version 330 core

// layout (location = 0) in vec3 aPos;
// layout (location = 1) in vec2 aTexture;

in vec3 aPos;
in vec2 aTexture;

out vec2 TexCoord;

uniform mat4 model;
uniform mat4 vp;

void main()
{
  // gl_Position = vp * (model * vec4(aPos, 1.0f));
  gl_Position = vp * (model * vec4(aPos, 1.0f));
  TexCoord = vec2(aTexture.x, aTexture.y);
}
