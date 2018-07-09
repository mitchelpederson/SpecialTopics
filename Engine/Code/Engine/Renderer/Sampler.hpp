#pragma once

enum eSamplerModes {
	SAMPLER_NEAREST,
	SAMPLER_LINEAR,
	SAMPLER_NEAREST_MIPMAP_LINEAR,
	SAMPLER_LINEAR_MIPMAP_LINEAR,
	SAMPLER_SHADOW
};


class Sampler {
public:
	Sampler();
	~Sampler();

	bool Create( eSamplerModes mode = SAMPLER_NEAREST_MIPMAP_LINEAR );
	void Destroy();

	unsigned int GetHandle() const;

	void SetSamplerMode( eSamplerModes mode );

private:
	unsigned int m_handle = 0;
};