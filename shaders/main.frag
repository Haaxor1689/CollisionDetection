#version 330

uniform vec3 eye_position;

uniform vec4 light_position;
uniform vec3 light_ambient_color;
uniform vec3 light_diffuse_color;
uniform vec3 light_specular_color;

uniform vec3 material_ambient_color;
uniform vec3 material_diffuse_color;
uniform vec3 material_specular_color;
uniform float material_shininess;

uniform sampler2D obj_texture;

in vec3 vertex_position_ws;
in vec3 vertex_normal_ws;
in vec2 vertex_tex_coord;

out vec4 final_color;

void main() {
    vec3 L = normalize(light_position.xyz - vertex_position_ws * light_position.w);
    vec3 N = normalize(vertex_normal_ws);
    vec3 E = normalize(eye_position - vertex_position_ws);
    vec3 H = normalize(E + L);

    vec3 tex_color = texture(obj_texture, vertex_tex_coord).rgb;

    float intensity = max(dot(N, L), 0);
    float dist = light_position.w != 0 ? distance(light_position.xyz, vertex_position_ws) : 1.0;
    float atten = 1.0 / pow(dist, 2);
    float spec = pow(max(dot(N, H), 0.0), material_shininess);

    vec3 color = material_ambient_color * light_ambient_color * tex_color;
    color += material_diffuse_color * light_diffuse_color * intensity * atten * tex_color;
    color += material_specular_color * light_specular_color * spec * atten;

    final_color = vec4(color, 1.0);
}
