#version 330 core

in vec2 TexCoord;

out vec4 FragColor;

uniform vec4 shaderColor;
uniform sampler2D ourTexture;
uniform int animationFrame;
uniform int animationFrameMax;

uniform int recOnly;
uniform vec4 textureOffset;

void main()
{
  // FragColor = vec4(0.0f, 0.3f, 1.0f, 1.0f);
  // FragColor = shaderColor;
  // vec3 x = vec3(textureOffset.z, 0.0, 0.5);
  // vec3 y = vec3(0.0, 1.0, 1.0);
  // vec3 z = vec3(0.0, 0.0, 1.0);

  // mat3 matrix = mat3(x,y,z);

  vec2 x = vec2(textureOffset.z, 0.0);
  vec2 y = vec2(0.0, textureOffset.w);

  mat2 matrix = mat2(x,y);

  // vec2 qwe = vec2(0.5, 0.5);

  if (recOnly == 1) {
    FragColor = vec4(shaderColor.rgb, 0.1f);
  } else {
    // FragColor = texture(ourTexture, vec2((TexCoord.x + animationFrame) / animationFrameMax, TexCoord.y)) * shaderColor;
    // vec3 asd = matrix * vec3(TexCoord.x + 0.5, TexCoord.y + 0.5, 1.0f);
    vec2 asd = matrix * TexCoord + textureOffset.xy;
    // vec3 asd = matrix * vec3(TexCoord, 1.0f);
    FragColor = texture(ourTexture, asd.xy) * shaderColor;

    // FragColor.a *= 0.5;
  }
}
