#version 330

in vec2 texcoord; // primeste coordonatele de texturare de la vertex shader

uniform sampler2D texture;
uniform vec2 textureModif; // valorile cu care se modifica coordonatele de texturare
uniform bool isSnow; // modificarile se vor realiza doar asupra texturii terenului

layout(location = 0) out vec4 out_color;


void main()
{
    vec2 textureCoord = texcoord;

    /* In cazul texturii de pe plan se va aplica modificarea coordonatelor de texturare
    pentru a crea iluzia de miscare */
    if (isSnow) {
        textureCoord.x += textureModif.x;
        textureCoord.y += textureModif.y;
    }

    out_color = texture2D(texture, textureCoord);
}
