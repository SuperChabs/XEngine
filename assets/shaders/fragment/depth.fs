#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture1;

// float near = 0.1;
// float far = 100.0;

// float LinearizeDepth(float depth) 
// {
//     float z = depth * 2.0 - 1.0;
//     return (2.0 * near * far) / (far * near - z * (far - near));
// }

void main()
{
    // float depth = LinearizeDepth(gl_FragCoord.z) / far;
    // FragColor = vec4(vec3(depth), 1.0);

    vec4 texColor = texture(texture1, TexCoords);
    if(texColor.a < 0.1)
        discard;
    FragColor = texColor;

    // if(gl_FragCoord.x < 400)
    //     FragColor = vec4(1.0, 0.0, 0.0, 1.0);
    // else
    //     FragColor = vec4(0.0, 1.0, 0.0, 1.0); 

    // FragColor = vec4(1.0);
    // gl_FragDepth = gl_FragCoord.z + 0.1;
}