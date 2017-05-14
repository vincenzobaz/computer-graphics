#version 330 core
out vec3 color;
in vec2 uv;
in vec3 Normal;
in vec3 Position;
in vec3 view_dir;

uniform sampler2D tex_wave;
uniform sampler2D tex_terrain;
uniform sampler2D tex_sky;

void main() {
    vec2 Reflect = reflect(uv, normalize(Normal).xy);

    vec3 terrain_reflection_color = texture(tex_terrain, Reflect.xy).rgb;
    vec3 sky_reflection_color = texture(tex_sky, Reflect.xy).rgb;
    vec3 reflexion_mix = mix(terrain_reflection_color, sky_reflection_color, vec3(.8));

    vec3 water_color = vec3(57.0f/255.0f, 88.0f/255.0f, 121.0f/255.0f);

    //color = mix(water_color, reflexion_mix, vec3(.15));
    color = terrain_reflection_color;
}
