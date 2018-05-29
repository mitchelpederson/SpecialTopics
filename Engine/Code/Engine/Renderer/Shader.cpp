#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/Shader.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/XmlUtilities.hpp"


Shader::Shader() {
	m_state = new RenderState();
}

Shader::Shader(ShaderProgram* program) {
	m_state = new RenderState();
	m_program = program;
}


Shader::Shader(  const tinyxml2::XMLElement& xml ) {
	m_state = new RenderState();
	std::string name = "error";
	name = ParseXmlAttribute(xml, "name", name);
	GUARANTEE_OR_DIE(name != "error", "Shader name must be defined");
	m_name = name;

	std::string cull = "default";
	std::string fill = "default";
	std::string frontface = "default";
	std::string queue = "opaque";

	cull = ParseXmlAttribute(xml, "cull", cull);
	fill = ParseXmlAttribute(xml, "fill", fill);
	frontface = ParseXmlAttribute(xml, "frontface", frontface);
	queue = ParseXmlAttribute(xml, "queue", queue);

	if (queue == "opaque") {
		m_queue = 0;
	} else if ( queue == "alpha" ) {
		m_queue = 1;
	}

	if (cull == "back") {
		m_state->cullMode = CULLMODE_BACK;
	} else if (cull == "front") {
		m_state->cullMode = CULLMODE_FRONT;
	}

	if (fill == "solid") {
		m_state->fillMode = FILLMODE_SOLID;
	} else if (fill == "point") {
		m_state->fillMode = FILLMODE_POINT;
	} else if (fill == "line") {
		m_state->fillMode = FILLMODE_WIRE;
	}

	if (frontface == "ccw") {
		m_state->windOrder = WIND_COUNTER_CLOCKWISE;
	} else if (frontface == "cw") {
		m_state->windOrder = WIND_CLOCKWISE;
	}

	const tinyxml2::XMLElement* vertex = xml.FirstChildElement("vertex");
	const tinyxml2::XMLElement* fragment = xml.FirstChildElement("fragment");
	std::string vsPath = ParseXmlAttribute(*vertex, "file", vsPath);
	std::string fsPath = ParseXmlAttribute(*fragment, "file", fsPath);

	m_program = new ShaderProgram();
	m_program->LoadFromFiles(vsPath.c_str(), fsPath.c_str());

	const tinyxml2::XMLElement* depth = xml.FirstChildElement("depth");
	m_state->depthWrite = ParseXmlAttribute(*depth, "write", m_state->depthWrite);
	std::string depthCompare = ParseXmlAttribute(*depth, "compare", depthCompare);

	if (depthCompare == "never") {
		m_state->compareMode = COMPARE_NEVER;
	} else if (depthCompare == "always") {
		m_state->compareMode = COMPARE_ALWAYS;
	} else if (depthCompare == "less") {
		m_state->compareMode = COMPARE_LESS;
	} 


	const tinyxml2::XMLElement* blend = xml.FirstChildElement("blend");
	const tinyxml2::XMLElement* blendColor = blend->FirstChildElement("color");
	std::string op	 = ParseXmlAttribute(*blendColor, "op", op);
	std::string src  = ParseXmlAttribute(*blendColor, "src", src);
	std::string dest = ParseXmlAttribute(*blendColor, "dest", dest);
	
	if (op == "add") {
		m_state->colorBlendOp = COMPARE_ADD;
	} else if (op == "subtract") {
		m_state->colorBlendOp = COMPARE_SUBTRACT;
	} else if (op == "reverse_subtract") {
		m_state->colorBlendOp = COMPARE_REVERSE_SUBTRACT;
	} else if (op == "min") {
		m_state->colorBlendOp = COMPARE_MIN;
	} else if (op == "max") {
		m_state->colorBlendOp = COMPARE_MAX;
	}

	if (src == "one") {
		m_state->colorSrcFactor = BLEND_ONE;
	} else if (src == "zero") {
		m_state->colorSrcFactor = BLEND_ZERO;
	} else if (src == "inv_src_alpha") {
		m_state->colorSrcFactor = BLEND_ONE_MINUS_SRC_ALPHA;
	} else if (src == "src_alpha") {
		m_state->colorSrcFactor = BLEND_SRC_ALPHA;
	} else if (src == "dst_alpha") {
		m_state->colorSrcFactor = BLEND_DST_ALPHA;
	}

	if (dest == "one") {
		m_state->colorDstFactor = BLEND_ONE;
	} else if (dest == "zero") {
		m_state->colorDstFactor = BLEND_ZERO;
	} else if (dest == "inv_src_alpha") {
		m_state->colorDstFactor = BLEND_ONE_MINUS_SRC_ALPHA;
	} else if (dest == "src_alpha") {
		m_state->colorDstFactor = BLEND_SRC_ALPHA;
	} else if (dest == "dst_alpha") {
		m_state->colorDstFactor = BLEND_DST_ALPHA;
	}

}


Shader::~Shader() {
	delete m_state;
	m_state = nullptr;
}


void Shader::SetProgram( ShaderProgram* program ) {
	m_program = program;
}

void Shader::EnableBlending( BlendOp op, BlendFactor src, BlendFactor dst ) {
	m_state->colorBlendOp = op;
	m_state->colorSrcFactor = src;
	m_state->colorDstFactor = dst;
	m_state->blend = true;
}


void Shader::DisasbleBlending() {
	m_state->blend = false;
}


void Shader::EnableDepth( DepthCompare compare, bool write ) {
	m_state->depthWrite = write;
	m_state->compareMode = compare;
}

void Shader::DisableDepth() {
	m_state->depthWrite = false;
	m_state->compareMode = COMPARE_ALWAYS;
}


void Shader::SetCullMode( CullMode mode ) {
	m_state->cullMode = mode;
}


void Shader::SetFillMode( FillMode mode ) {
	m_state->fillMode = mode;
}


void Shader::SetFrontFace( WindOrder order ) {
	m_state->windOrder = order;
}


ShaderProgram* Shader::GetProgram() const {
	return m_program;
}


RenderState* Shader::GetRenderState() const {
	return m_state;
}


const std::string& Shader::GetName() const {
	return m_name;
}


unsigned int Shader::GetQueue() const {
	return m_queue;
}