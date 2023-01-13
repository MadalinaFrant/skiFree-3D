#version 330

// Input
in vec2 texcoord;

// Uniform properties
uniform sampler2D texture_1;
uniform sampler2D texture_2;
// TODO(student): Declare various other uniforms
uniform bool mixTextures;
uniform vec2 texModif;

// Output
layout(location = 0) out vec4 out_color;


void main()
{
    vec2 textureCoord = texcoord;

    textureCoord.x += texModif.x * 0.02f;
    textureCoord.y += texModif.y * 0.02f;

    // TODO(student): Calculate the out_color using the texture2D() function.
    vec4 color1 = texture2D(texture_1, textureCoord);
    vec4 color2 = texture2D(texture_2, textureCoord);
    
    if (mixTextures) {
        out_color = mix(color1, color2, 0.25f);
    } else {
        out_color = color1;
    }

    if (out_color.a < 0.5f) {
        discard;
    }
}
