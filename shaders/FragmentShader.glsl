#version 330

in vec2 texcoord;

uniform sampler2D texture;
uniform bool isSnow;
uniform vec2 texModif;

layout(location = 0) out vec4 out_color;


void main()
{
    vec2 textureCoord = texcoord;

    if (isSnow) {
        textureCoord.x += texModif.x;
        textureCoord.y += texModif.y;
    }

    out_color = texture2D(texture, textureCoord);

    if (out_color.a < 0.5f) {
        discard;
    }

}
