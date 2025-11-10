#version 330 core
out vec4 FragColor;

//in vec2 TexCoord;

//uniform sampler2D texture1;
//uniform sampler2D texture2;

// uniform vec3 objectColor;
// uniform vec3 ligthColor;

void main()
{
    //FragColor = mix(texture(texture1, TexCoord), texture(texture2, vec2(-TexCoord.x, TexCoord.y)), 0.0);
    FragColor = vec4(1.0);
}