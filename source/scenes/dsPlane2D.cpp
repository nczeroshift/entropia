
#include "dsPlane2D.h"

dsPlane2D::dsPlane2D(DS::Data * data) : dsItem2D(data), m_R(255), m_G(255), m_B(255), m_Anchor(0.0, 0.0){
	m_ShaderProgram = NULL;
	m_Texture = NULL;
}

dsPlane2D::~dsPlane2D() {
   
}

void dsPlane2D::Parse(const JSONObject & node) {
	dsItem2D::Parse(node);

	if (node.find("texture") != node.end())
		m_Texture = m_Data->LoadTexture("texture://" + node.find("texture")->second->AsString());

	if (node.find("textures") != node.end()) {
		const JSONArray & arrNode = node.find("textures")->second->AsArray();
		m_Textures.reserve(arrNode.size());
		for (int i = 0; i < arrNode.size(); i++) 
			m_Textures.push_back(m_Data->LoadTexture("texture://" + arrNode[i]->AsString()));
	}

	if (node.find("shader") != node.end()) 
		m_ShaderProgram = m_Data->LoadProgram("shader://" + node.find("shader")->second->AsString());
	
	if (node.find("color") != node.end()) {
		const JSONObject & colorNode = node.find("color")->second->AsObject();
		if (colorNode.find("r") != colorNode.end())
			m_R.Parse(*colorNode.find("r")->second);

		if (colorNode.find("g") != colorNode.end())
			m_G.Parse(*colorNode.find("g")->second);

		if (colorNode.find("b") != colorNode.end())
			m_B.Parse(*colorNode.find("b")->second);
	}

	if (node.find("anchor") != node.end()) {
		const JSONArray & arrNode = node.find("anchor")->second->AsArray();
		m_Anchor = Math::Vec2(arrNode[0]->AsNumber(), arrNode[1]->AsNumber());
	}
}

void dsPlane2D::Load() {
	
}

void dsPlane2D::Render(DS::StageProxy * proxy, double start, double end, double time) {
	Graph::Device * const dev = m_Data->GetGraphicsDevice();
    Gui::ShapeRenderer * const shp = m_Data->GetShapeRenderer();

    const float aspect = m_Data->GetAspect();
    const float width = m_Data->GetVirtualDisplayWidth();
    const float height = m_Data->GetVirtualDisplayHeight();

    const float t = (time - start);

    dev->MatrixMode(Graph::MATRIX_PROJECTION);
    dev->Identity();
    dev->Ortho2D(width, height);

    dev->MatrixMode(Graph::MATRIX_VIEW);
    dev->Identity();

    dev->MatrixMode(Graph::MATRIX_MODEL);
    dev->Identity();

	dev->Disable(Graph::STATE_DEPTH_TEST);
	dev->Disable(Graph::STATE_CULL_FACE);
	dev->Enable(Graph::STATE_BLEND);
	dev->BlendFunc(Graph::BLEND_SRC_ALPHA, Graph::BLEND_INV_SRC_ALPHA);

	Math::Color4ub col = Math::Color4ub(m_R.GetValue(t), m_G.GetValue(t), m_B.GetValue(t), m_CurrentAlpha);

	if (m_ShaderProgram) m_ShaderProgram->Enable();
	if(m_Texture != NULL) m_Texture->Enable();
	for (int i = 0; i < m_Textures.size(); i++)
		m_Textures[i]->Enable(i);

	dev->PushMatrix();
	
	dev->Translate(m_CurrentX, m_CurrentY, 0);
	dev->Scale(m_CurrentScaleX, m_CurrentScaleY, 1);
	
	dev->Translate(-m_Anchor.GetX(), -m_Anchor.GetY(), 0);

	m_Data->GetShapeRenderer()->Square(0,0, m_CurrentWidth, m_CurrentHeight, col);
	
	dev->PopMatrix();
	for (int i = 0; i < m_Textures.size(); i++)
		m_Textures[i]->Disable(i);
	if (m_Texture != NULL) m_Texture->Disable();
	if (m_ShaderProgram) m_ShaderProgram->Disable();
}
