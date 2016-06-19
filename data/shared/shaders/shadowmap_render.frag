$out vec4 o_color;
$in vec2 v_texcoord;

uniform sampler2D u_shadowmap;

void main()
{
	o_color = $texture2D(u_shadowmap, v_texcoord);
}
