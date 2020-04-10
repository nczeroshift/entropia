
#include "dsColor2D.h"

dsColor2D::dsColor2D(DS::Data * data) : dsItem2D(data), m_R(255), m_G(255), m_B(255){
	m_ShaderProgram = NULL;
	m_Texture = NULL;
}

dsColor2D::~dsColor2D() {
   
}

void dsColor2D::Parse(const JSONObject & node) {
	dsItem2D::Parse(node);

	if (node.find("texture") != node.end()){
		m_Texture = m_Data->LoadTexture("texture://" + node.find("texture")->second->AsString());
	}

	if (node.find("shader") != node.end()) {
		m_ShaderProgram = m_Data->LoadProgram("shader://" + node.find("shader")->second->AsString());
	}
	
	if (node.find("color") != node.end()) {
		const JSONObject & colorNode = node.find("color")->second->AsObject();
		if (colorNode.find("r") != colorNode.end())
			m_R.Parse(*colorNode.find("r")->second);

		if (colorNode.find("g") != colorNode.end())
			m_G.Parse(*colorNode.find("g")->second);

		if (colorNode.find("b") != colorNode.end())
			m_B.Parse(*colorNode.find("b")->second);
	}
}

void dsColor2D::Load() {
	
}

void dsColor2D::Render(DS::StageProxy * proxy, double start, double end, double time) {
	Graph::Device * const dev = m_Data->GetGraphicsDevice();
    Gui::ShapeRenderer * const shp = m_Data->GetShapeRenderer();

    const float aspect = m_Data->GetAspect();
    const float width = m_Data->GetWidth();
    const float height = m_Data->GetHeight();

    const float t = (time - start);
   
	dev->Viewport(0, 0, width, height);

    dev->MatrixMode(Graph::MATRIX_PROJECTION);
    dev->Identity();
    dev->Ortho2D(width, height);

    dev->MatrixMode(Graph::MATRIX_VIEW);
    dev->Identity();

    dev->MatrixMode(Graph::MATRIX_MODEL);
    dev->Identity();

	dev->Disable(Graph::STATE_DEPTH_TEST);
	dev->Disable(Graph::STATE_CULL_FACE);

	Math::Color4ub col = Math::Color4ub(m_R.GetValue(t), m_G.GetValue(t), m_B.GetValue(t), m_CurrentAlpha);

	if (m_ShaderProgram) m_ShaderProgram->Enable();
	if(m_Texture != NULL) m_Texture->Enable();
	m_Data->GetShapeRenderer()->Square(m_CurrentX, m_CurrentY, m_CurrentWidth, m_CurrentHeight, col);
	if (m_Texture != NULL) m_Texture->Disable();
	if (m_ShaderProgram) m_ShaderProgram->Disable();
}
