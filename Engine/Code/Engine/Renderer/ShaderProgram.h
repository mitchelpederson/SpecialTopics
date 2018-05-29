#pragma once


class ShaderProgram {
	friend class Renderer;
public:
	ShaderProgram();
	~ShaderProgram();

	bool LoadFromFiles(char const *root );
	
private:
	unsigned int program_handle = 0;
};