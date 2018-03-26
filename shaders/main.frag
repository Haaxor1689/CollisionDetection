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

in vec3 vertex_position_ws;
in vec3 vertex_normal_ws;

out vec4 final_color;

void main() {
    vec3 N = normalize(vertex_normal_ws);
    vec3 L = normalize(light_position.xyz - vertex_position_ws * light_position.w);
    vec3 E = normalize(eye_position - vertex_position_ws);
    vec3 H = normalize(E + L);

    float specular_intensity = pow(max(dot(N, H), 0.0), material_shininess);
    float diffuse_intensity = max(dot(N, L), 0);
    if (light_position.w != 0) {
        diffuse_intensity /= pow(distance(light_position.xyz, vertex_position_ws), 2);
    }

    vec3 color;

    // Calculate the ambient part of the light
    color += light_ambient_color * material_ambient_color;

    // Calculate the diffuse part of the light
    color += light_diffuse_color * material_diffuse_color * diffuse_intensity;

    // Calculate the specular part of the light
    color += light_specular_color * material_specular_color * specular_intensity;

    final_color = vec4(1.0, 0.0, 0.0, 1.0);
}
