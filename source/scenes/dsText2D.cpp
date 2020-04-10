
#include "dsText2D.h"

dsText2D::dsText2D(DS::Data * data) : dsItem2D(data), m_R(255), m_G(255), m_B(255), m_FontSize(32){
	m_FontAlignment = Gui::FontAlignment::FONT_ALIGNMENT_LEFT;
	m_FontMap = NULL;
	m_FontTexture = NULL;
	m_ShaderProgram = NULL;
}

dsText2D::~dsText2D() {
   
}

void dsText2D::Parse(const JSONObject & node) {
	dsItem2D::Parse(node);

	if (node.find("font") != node.end()){
		std::string fontId = node.find("font")->second->AsString();
		m_FontTexture = m_Data->LoadTexture("texture://tex2d_" + fontId + ".png");
		m_FontMap = m_Data->LoadFontMap("script://fmap_" + fontId + ".txt");
	}

	if (node.find("shader") != node.end()) {
		m_ShaderProgram = m_Data->LoadProgram("shader://" + node.find("shader")->second->AsString());
	}

	if (node.find("text") != node.end()) {
		m_Text = node.find("text")->second->AsString();
	}

	if (node.find("color") != node.end()) {
		const JSONObject & colorNode = node.find("color")->second->AsObject();
		if (colorNode.find("r") != colorNode.end())
			m_R.Parse(colorNode.find("r")->second);
		if (colorNode.find("g") != colorNode.end())
			m_R.Parse(colorNode.find("g")->second);
		if (colorNode.find("b") != colorNode.end())
			m_R.Parse(colorNode.find("b")->second);
	}
}

void dsText2D::Load() {
    Graph::Device * const dev = m_Data->GetGraphicsDevice();
}

void dsText2D::Render(DS::StageProxy * proxy, double start, double end, double time) {
	if (m_FontTexture == NULL || m_FontMap == NULL)
		return;

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

	dev->Color(m_R.GetValue(t), m_G.GetValue(t), m_B.GetValue(t));

	if (m_ShaderProgram) m_ShaderProgram->Enable();
	
	m_FontTexture->Enable();
	m_FontMap->Draw(m_CurrentX, m_CurrentY, m_FontSize.GetValue(t), m_Text, true, m_FontAlignment);
	m_FontTexture->Disable();

	if (m_ShaderProgram) m_ShaderProgram->Disable();
}
