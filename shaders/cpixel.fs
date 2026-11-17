#version 330
in vec2 fragTexCoord;
out vec4 finalColor;

uniform sampler2D texture0;
uniform vec2 canvasSize;

void main() {
   vec2 cell = fragTexCoord * canvasSize;
   vec2 id = floor(cell);
   vec2 local = fract(cell) * 4.0;
   vec3 c = texture(texture0, (id + 0.5) / canvasSize).rgb;
   float aa = fwidth(local.x);
   vec3 m = vec3(
      1.0 - smoothstep(1.0 - aa, 1.0 + aa, length(local - vec2(1.0, 1.0))),
      1.0 - smoothstep(1.0 - aa, 1.0 + aa, length(local - vec2(3.0, 1.0))),
      1.0 - smoothstep(1.0 - aa, 1.0 + aa, length(local - vec2(2.0, 3.0)))
   );
   finalColor = vec4(c * m, 1.0);
}
