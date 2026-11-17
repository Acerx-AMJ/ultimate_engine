#version 330
in vec2 fragTexCoord;
out vec4 finalColor;

uniform sampler2D texture0;
uniform vec2 canvasSize;

void main() {
   vec2 cell = fragTexCoord * canvasSize;
   vec2 id = floor(cell);
   vec2 local = fract(cell);
   vec3 c = texture(texture0, (id + 0.5) / canvasSize).rgb;

   vec3 m = vec3(0.0);
   m[int(local.x * 3.0)] = 1.0;

   finalColor = vec4(c * m, 1.0);
}
