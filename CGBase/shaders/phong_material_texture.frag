#version 330 core
#define NR_POINT_LIGHTS 4
#define NR_SPOT_LIGHTS 2

struct PositionalLight {
	vec3 Position;
	vec3 Ka;
	vec3 Kd;
	vec3 Ks;
	float Kc;
	float Kl;
	float Kq;
};

struct DirectionalLight {
	vec3 Position;
	vec3 Direction;
	vec3 Ka;
	vec3 Kd;
	vec3 Ks;
	float InnerCutOff;
	float OuterCutOff;
	float Kc;
	float Kl;
	float Kq;
};

struct Material {
	// NOTE(Jovan): Diffuse is used as ambient as well since the light source
	// defines the ambient colour
	sampler2D Kd;
	sampler2D Ks;
	float Shininess;
};

uniform PositionalLight uPointLights[NR_POINT_LIGHTS];
uniform DirectionalLight uSpotlights[NR_SPOT_LIGHTS];

uniform PositionalLight uPointLight;
uniform DirectionalLight uSpotlight;
uniform DirectionalLight uDirLight;
uniform Material uMaterial;
uniform vec3 uViewPos;

in vec2 UV;
in vec3 vWorldSpaceFragment;
in vec3 vWorldSpaceNormal;

out vec4 FragColor;

in vec4 fragPosLight;
uniform sampler2D shadowMap;

void main() {
	// Shadow value
	float shadow = 0.0f;
	// Sets lightCoords to cull space
	vec3 lightCoords = fragPosLight.xyz / fragPosLight.w;
	if(lightCoords.z <= 1.0f)
	{
		// Get from [-1, 1] range to [0, 1] range just like the shadow map
		lightCoords = (lightCoords + 1.0f) / 2.0f;
		float currentDepth = lightCoords.z;

		// Smoothens out the shadows
		int sampleRadius = 2;
		vec2 pixelSize = 1.0 / textureSize(shadowMap, 0);
		for(int y = -sampleRadius; y <= sampleRadius; y++)
		{
		    for(int x = -sampleRadius; x <= sampleRadius; x++)
		    {
		        float closestDepth = texture(shadowMap, lightCoords.xy + vec2(x, y) * pixelSize).r;
				if (currentDepth > closestDepth + 0.0015)
					shadow += 1.0f;     
		    }    
		}
		// Get average shadow
		shadow /= pow((sampleRadius * 2 + 1), 2);
	}

	vec3 ViewDirection = normalize(uViewPos - vWorldSpaceFragment);
	// NOTE(Jovan): Directional light
	vec3 DirLightVector = normalize(-uDirLight.Direction);
	float DirDiffuse = max(dot(vWorldSpaceNormal, DirLightVector), 0.0f);
	vec3 DirReflectDirection = reflect(-DirLightVector, vWorldSpaceNormal);
	// NOTE(Jovan): 32 is the specular shininess factor. Hardcoded for now
	float DirSpecular = pow(max(dot(ViewDirection, DirReflectDirection), 0.0f), uMaterial.Shininess);

	vec3 DirAmbientColor = uDirLight.Ka * vec3(texture(uMaterial.Kd, UV));
	vec3 DirDiffuseColor = uDirLight.Kd * DirDiffuse * vec3(texture(uMaterial.Kd, UV)) * (1.0 - shadow);
	vec3 DirSpecularColor = uDirLight.Ks * DirSpecular * vec3(texture(uMaterial.Ks, UV)) * (1.0 - shadow);
	vec3 DirColor = DirAmbientColor + DirDiffuseColor + DirSpecularColor;

	// NOTE(Jovan): Point light
	/*vec3 PtLightVector = normalize(uPointLight.Position - vWorldSpaceFragment);
	float PtDiffuse = max(dot(vWorldSpaceNormal, PtLightVector), 0.0f);
	vec3 PtReflectDirection = reflect(-PtLightVector, vWorldSpaceNormal);
	float PtSpecular = pow(max(dot(ViewDirection, PtReflectDirection), 0.0f), uMaterial.Shininess);

	vec3 PtAmbientColor = uPointLight.Ka * vec3(texture(uMaterial.Kd, UV));
	vec3 PtDiffuseColor = PtDiffuse * uPointLight.Kd * vec3(texture(uMaterial.Kd, UV));
	vec3 PtSpecularColor = PtSpecular * uPointLight.Ks * vec3(texture(uMaterial.Ks, UV));

	float PtLightDistance = length(uPointLight.Position - vWorldSpaceFragment);
	float PtAttenuation = 1.0f / (uPointLight.Kc + uPointLight.Kl * PtLightDistance + uPointLight.Kq * (PtLightDistance * PtLightDistance));
	vec3 PtColor = PtAttenuation * (PtAmbientColor + PtDiffuseColor + PtSpecularColor);*/

	vec3 PtLightsColor;
	bool first = true;
	for(int i = 0; i < NR_POINT_LIGHTS; i++) {
		vec3 PtLightVector = normalize(uPointLights[i].Position - vWorldSpaceFragment);
		float PtDiffuse = max(dot(vWorldSpaceNormal, PtLightVector), 0.0f);
		vec3 PtReflectDirection = reflect(-PtLightVector, vWorldSpaceNormal);
		float PtSpecular = pow(max(dot(ViewDirection, PtReflectDirection), 0.0f), uMaterial.Shininess);

		vec3 PtAmbientColor = uPointLights[i].Ka * vec3(texture(uMaterial.Kd, UV));
		vec3 PtDiffuseColor = PtDiffuse * uPointLights[i].Kd * vec3(texture(uMaterial.Kd, UV));
		vec3 PtSpecularColor = PtSpecular * uPointLights[i].Ks * vec3(texture(uMaterial.Ks, UV));

		float PtLightDistance = length(uPointLights[i].Position - vWorldSpaceFragment);
		float PtAttenuation = 1.0f / (uPointLights[i].Kc + uPointLights[i].Kl * PtLightDistance + uPointLights[i].Kq * (PtLightDistance * PtLightDistance));

		if(first){
			PtLightsColor = PtAttenuation * (PtAmbientColor + PtDiffuseColor + PtSpecularColor);
			first = false;
		}
		else {
			PtLightsColor += PtAttenuation * (PtAmbientColor + PtDiffuseColor + PtSpecularColor);
		}
	}

	// NOTE(Jovan): Spotlight
	vec3 SptLightsColor;
	first = true;
	for(int i = 0; i < NR_SPOT_LIGHTS; i++) {
		vec3 SpotlightVector = normalize(uSpotlights[i].Position - vWorldSpaceFragment);

		float SpotDiffuse = max(dot(vWorldSpaceNormal, SpotlightVector), 0.0f);
		vec3 SpotReflectDirection = reflect(-SpotlightVector, vWorldSpaceNormal);
		float SpotSpecular = pow(max(dot(ViewDirection, SpotReflectDirection), 0.0f), uMaterial.Shininess);

		vec3 SpotAmbientColor = uSpotlights[i].Ka * vec3(texture(uMaterial.Kd, UV));
		vec3 SpotDiffuseColor = SpotDiffuse * uSpotlights[i].Kd * vec3(texture(uMaterial.Kd, UV));
		vec3 SpotSpecularColor = SpotSpecular * uSpotlights[i].Ks * vec3(texture(uMaterial.Ks, UV));

		float SpotlightDistance = length(uSpotlights[i].Position - vWorldSpaceFragment);
		float SpotAttenuation = 1.0f / (uSpotlights[i].Kc + uSpotlights[i].Kl * SpotlightDistance + uSpotlights[i].Kq * (SpotlightDistance * SpotlightDistance));

		float Theta = dot(SpotlightVector, normalize(-uSpotlights[i].Direction));
		float Epsilon = uSpotlights[i].InnerCutOff - uSpotlights[i].OuterCutOff;
		float SpotIntensity = clamp((Theta - uSpotlights[i].OuterCutOff) / Epsilon, 0.0f, 1.0f);
		vec3 SpotColor = SpotIntensity * SpotAttenuation * (SpotAmbientColor + SpotDiffuseColor + SpotSpecularColor);
		
		if(first){
			SptLightsColor = SpotIntensity * SpotAttenuation * (SpotAmbientColor + SpotDiffuseColor + SpotSpecularColor);
			first = false;
		}
		else {
			SptLightsColor += SpotIntensity * SpotAttenuation * (SpotAmbientColor + SpotDiffuseColor + SpotSpecularColor);
		}
	}
	//Final
	vec3 FinalColor = DirColor + PtLightsColor + SptLightsColor;
	FragColor = vec4(FinalColor, 1.0f);
}
