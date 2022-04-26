#version 330 core

in vec2 TexCoord;

out vec4 color;

uniform sampler2D ourTexture1;
uniform sampler2D ourTexture2;
out vec4 fragColor;
void main()
{
    fragColor = vec4(1,0,0,1);
    //color = mix(texture(ourTexture1, TexCoord), texture(ourTexture2, TexCoord), 0.2);//vec4(1,1,1,1);//
}
