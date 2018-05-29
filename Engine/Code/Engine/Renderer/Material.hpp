#pragma once

#include "Engine/Renderer/Shader.hpp"
#include "Engine/Renderer/CubeMap.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/Sampler.hpp"

#include <vector>


class MaterialProperty {
public:
	virtual void Bind() = 0;
	virtual MaterialProperty* GetCopy() = 0;

public:
	std::string name;
};

class MaterialPropertyFloat : public MaterialProperty {
public:
	MaterialPropertyFloat( const std::string& n, float val ) {
		name = n;
		value = val;
	}
	virtual void Bind() override;
	virtual MaterialProperty* GetCopy() override;

public:
	float value;
};

class Material {
public:
	Material( const tinyxml2::XMLElement& xml );
	Material( Shader* shader );
	Material( Material* mat );
	~Material();

	void SetProperty( const std::string& name, float value );
	void RemoveProperty( const std::string& name );

	void SetTexture( unsigned int bindPoint, Texture* tex, Sampler* sampler = nullptr);

	unsigned int GetTextureCount() const;
	unsigned int GetSamplerCount() const;
	unsigned int GetPropertyCount() const;
	unsigned int GetQueue() const;


public:
	Shader* shader;
	std::vector<MaterialProperty*> properties;
	std::vector<Texture*> textures;
	std::vector<Sampler*> samplers;

};