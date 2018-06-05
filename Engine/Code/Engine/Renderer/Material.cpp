#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/XmlUtilities.hpp"
#include "Engine/Renderer/Material.hpp"

Material::Material( Shader* s ) {
	shader = s;
}


Material::Material( Material* mat ) {
	textures.resize( mat->GetTextureCount() );
	samplers.resize( mat->GetSamplerCount() );
	properties.resize( mat->GetPropertyCount() );
	shader = new Shader( mat->shader->GetProgram() );

	for (unsigned int i = 0; i < mat->GetTextureCount(); i++) {
		if (mat->textures[i] != nullptr) {
			textures[i] = mat->textures[i];
		}

		if (mat->samplers[i] != nullptr) {
			samplers[i] = mat->samplers[i];
		}
	}
}


Material::Material( const tinyxml2::XMLElement& xml ) {

	std::string shaderName = "error";
	shaderName = ParseXmlAttribute(xml, "shader", shaderName);
	if (shaderName == "error") {
		ERROR_AND_DIE("SHADER NAME INVALID IN MATERIAL");
	}
	shader = g_theRenderer->GetShader(shaderName);

	const tinyxml2::XMLElement* propertiesNode = xml.FirstChildElement("properties");
	if (propertiesNode != nullptr) {
		const tinyxml2::XMLElement* prop = propertiesNode->FirstChildElement();

		while(prop != nullptr) {
			std::string propName = ParseXmlAttribute(*prop, "name", propName);

			if (strcmp(prop->Name(), "float")) {
				float value = -100.f;
				value = ParseXmlAttribute(*prop, "value", value);
				SetProperty(propName, value);
			}
			prop = prop->NextSiblingElement();
		}
	}

	const tinyxml2::XMLElement* texturesNode = xml.FirstChildElement("textures");
	const tinyxml2::XMLElement* texture = texturesNode->FirstChildElement("texture");
	if (texture != nullptr) {
		int slot = -1;
		slot = ParseXmlAttribute(*texture, "slot", slot);
		std::string texPath = ParseXmlAttribute(*texture, "path", texPath);
		SetTexture(slot, g_theRenderer->CreateOrGetTexture(texPath));
		texture->NextSiblingElement("texture");
	}
}


Material::~Material() {

}


void Material::SetProperty( const std::string& name, float value ) {
	RemoveProperty( name );
	MaterialPropertyFloat* prop = new MaterialPropertyFloat( name, value );
	properties.push_back(prop);
}


void Material::RemoveProperty( const std::string& name ) {
	
	std::vector<MaterialProperty*>::iterator it = properties.begin();

	while (it != properties.end()) {
		if ((*it)->name == name) {
			delete *it;
			properties.erase(it);
		}
		return;
	}
}


void Material::SetTexture( unsigned int bindPoint, Texture* tex, Sampler* sampler ) {
	textures.resize(bindPoint + 1);
	textures[bindPoint] = tex;
	
	samplers.resize(bindPoint + 1);
	samplers[bindPoint] = sampler;
}


unsigned int Material::GetTextureCount() const {
	return (unsigned int) textures.size();
}

unsigned int Material::GetSamplerCount() const {
	return (unsigned int) samplers.size();
}

unsigned int Material::GetPropertyCount() const {
	return (unsigned int) properties.size();
}


void MaterialPropertyFloat::Bind() {
	g_theRenderer->SetUniform(name, &value);
}

MaterialProperty* MaterialPropertyFloat::GetCopy() {
	return new MaterialPropertyFloat(name, value);
}


unsigned int Material::GetQueue() const {
	return shader->GetQueue();
}