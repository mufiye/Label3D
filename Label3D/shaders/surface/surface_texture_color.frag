#version 330 core
// please send comments or report bug to: liangliang.nan@gmail.com

uniform vec3    wLightPos;
uniform vec3    wCamPos;

layout(std140) uniform Material {
    vec3    ambient;// in [0, 1], r==g==b;
    vec3    specular;// in [0, 1], r==g==b;
    float    shininess;
};

uniform bool lighting = true;
uniform bool two_sides_lighting = true;
uniform bool smooth_shading;

// backside color
uniform bool distinct_back_color = true;
uniform vec3 backside_color = vec3(0.8f, 0.4f, 0.4f);

uniform bool highlight;
uniform int  hightlight_id_min;
uniform int  hightlight_id_max;

in Data{
    vec3 texcoord;
    vec3 normal;
    vec3 position;
	vec3 color;
} DataIn;

uniform sampler2D textureID0;
uniform sampler2D textureID1;
uniform sampler2D textureID2;
uniform sampler2D textureID3;
uniform sampler2D textureID4;

uniform float texture_repeat = 1.0f;
uniform float fractional_repeat = 0.0f;

//add by xyh 2020-5-2
uniform float transparency = 0.5f;

out vec4 outputF;// Ouput data


//#define ENABLE_ALPHA

void main() {
    float repeat = texture_repeat + fractional_repeat / 100.0f;
	float map_diffuse = DataIn.texcoord.z;

#ifndef ENABLE_ALPHA
    vec3 color = vec3(0.0, 0.0, 0.0);
    if(map_diffuse < 0.5)
		color = texture(textureID0, vec2(DataIn.texcoord.x, DataIn.texcoord.y) * repeat).rgb;
	else if(map_diffuse < 1.5)
		color = texture(textureID1, vec2(DataIn.texcoord.x, DataIn.texcoord.y) * repeat).rgb;
	else if(map_diffuse < 2.5)
		color = texture(textureID2, vec2(DataIn.texcoord.x, DataIn.texcoord.y) * repeat).rgb;
	else if(map_diffuse < 3.5)
		color = texture(textureID3, vec2(DataIn.texcoord.x, DataIn.texcoord.y) * repeat).rgb;
	else if(map_diffuse < 4.5)
		color = texture(textureID4, vec2(DataIn.texcoord.x, DataIn.texcoord.y) * repeat).rgb;
	else
		color = vec3(0.0, 0.0, 0.0);
    float alpha = 1.0f;
#else
    vec4 tmp = vec3(0.0, 0.0, 0.0);
	if(map_diffuse < 0.5)
		tmp = texture(textureID0, vec2(DataIn.texcoord.x, DataIn.texcoord.y) * repeat).rgb;
	else if(map_diffuse < 1.5)
		tmp = texture(textureID1, vec2(DataIn.texcoord.x, DataIn.texcoord.y) * repeat).rgb;
	else if(map_diffuse < 2.5)
		tmp = texture(textureID2, vec2(DataIn.texcoord.x, DataIn.texcoord.y) * repeat).rgb;
	else if(map_diffuse < 3.5)
		tmp = texture(textureID3, vec2(DataIn.texcoord.x, DataIn.texcoord.y) * repeat).rgb;
	else if(map_diffuse < 4.5)
		tmp = texture(textureID4, vec2(DataIn.texcoord.x, DataIn.texcoord.y) * repeat).rgb;
	else
		tmp = vec3(0.0, 0.0, 0.0);
		
    if (tmp.a <= 0)
        discard;
    vec3 color = tmp.rgb;
    float alpha = tmp.a;
#endif

//add by xyh 2020-5-2
	if (DataIn.color.x < 1.5f){
		color = mix(color, DataIn.color, transparency);
	}

	if (!lighting) {
		outputF = vec4(color, alpha);
		return;
	}

    if (!gl_FrontFacing && distinct_back_color)
        color = backside_color;

    if (highlight) {
        if (gl_PrimitiveID >= hightlight_id_min && gl_PrimitiveID <= hightlight_id_max)
        color = mix(color, vec3(1.0, 0.0, 0.0), 0.8);
    }

    vec3 normal;
    if (smooth_shading)
        normal = normalize(DataIn.normal);
    else {
        normal = normalize(cross(dFdx(DataIn.position), dFdy(DataIn.position)));
        if (dot(normal, DataIn.normal) < 0)
                normal = -normal;
    }

    vec3 view_dir = normalize(wCamPos - DataIn.position);
    vec3 light_dir = normalize(wLightPos);

    float df = 0.0;// diffuse factor
    if (two_sides_lighting)
        df = abs(dot(light_dir, normal));
    else
        df = max(dot(light_dir, normal), 0);

    float sf = 0.0;// specular factor
    if (df > 0.0) { // if the vertex is lit compute the specular color
        vec3 half_vector = normalize(light_dir + view_dir);// compute the half vector

        if (two_sides_lighting)
        sf = abs(dot(half_vector, normal));
        else
        sf = max(dot(half_vector, normal), 0.0);

        sf = pow(sf, shininess);
    }

    //		if (ssaoEnabled) {
    //			vec2 texCoord = gl_FragCoord.xy / textureSize(ssaoTexture, 0);
    //			float coeff = texture(ssaoTexture, texCoord).r;
    //			outputF = vec4(vec3(color * df + specular * sf + ambient) * coeff, alpha);
    //		}
    //		else
    outputF = vec4(color * df + specular * sf + ambient, alpha);
}
