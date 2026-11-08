uniform sampler2D tex;
uniform sampler2D videoTex;
uniform float superMix; // 0: alpha, 1: opaque at 50%, 2: 50/50 mix
float superAlpha(float a) { return superMix > 1.5 ? 0.5 : (superMix > 0.5 ? a * 0.5 : a); }

varying vec2 texCoord;
varying vec2 videoCoord;

void main()
{
#if SUPERIMPOSE
	vec4 col = texture2D(tex, texCoord);
	vec4 vid = texture2D(videoTex, videoCoord);
	gl_FragColor = mix(vid, col, superAlpha(col.a));
#else
	gl_FragColor = texture2D(tex, texCoord);
#endif
}
