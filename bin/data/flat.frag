uniform sampler2DRect offsetTexture;
uniform sampler2DRect sourceTexture;
uniform float height;
uniform float horizon;
uniform float transitionSmoothing;

void main(){
	vec2 st = gl_TexCoord[0].st;

	float horizonLine = horizon * height;

	float actualOffset = texture2DRect(offsetTexture, vec2(st.x,0.5)).r;

	float horizonOffset = actualOffset * height;

	horizonOffset = clamp(horizonOffset,0.0,height);
	float offSet = st.y + horizonOffset - horizonLine;

	vec4 color = texture2DRect(sourceTexture, vec2(st.x, offSet));

/*     if (st.y > horizonLine){
          
          float yMapped =  (st.y - horizonOffset ) / (height);
          color = texture2DRect(sourceTexture, vec2(st.x, horizonLine + yMapped*height*1.0));
          //color.r = 1.0;
      }
*/

	if ( (offSet > height) || (offSet < 0.0 )){
	color = vec4(0.0,0.0,0.0,0.0);
}

gl_FragColor = color;
}