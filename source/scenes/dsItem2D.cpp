
#include "dsItem2D.h"
#include "../dsUtils.h"

dsItem2D::dsItem2D(DS::Data * data) : DS::Stage(data), m_Alpha(255), m_ScaleX(1.0), m_ScaleY(1.0){

}

dsItem2D::~dsItem2D() {

}

void dsItem2D::Load() {
  
}


void dsItem2D::Update(DS::StageProxy * proxy, double start, double end, double time) {
	const double off = time - start;
	m_CurrentX = m_X.GetValue(off);
	m_CurrentY = m_Y.GetValue(off);
	m_CurrentWidth = m_Width.GetValue(off);
	m_CurrentHeight = m_Height.GetValue(off);
	m_CurrentAlpha = m_Alpha.GetValue(off);
	m_CurrentScaleX = m_ScaleX.GetValue(off);
	m_CurrentScaleY = m_ScaleY.GetValue(off);
}

void dsItem2D::Render(DS::StageProxy * proxy, double start, double end, double time) {

}

void dsItem2D::Parse(const JSONObject & node) {
	if (node.find("x") != node.end())
		m_X.Parse(*node.find("x")->second);

	if (node.find("y") != node.end())
		m_Y.Parse(*node.find("y")->second);

	if (node.find("sx") != node.end())
		m_ScaleX.Parse(*node.find("sx")->second);

	if (node.find("sy") != node.end())
		m_ScaleY.Parse(*node.find("sy")->second);

	if (node.find("width") != node.end())
		m_Width.Parse(*node.find("width")->second);

	if (node.find("height") != node.end())
		m_Height.Parse(*node.find("height")->second);

	if (node.find("alpha") != node.end())
		m_Alpha.Parse(*node.find("alpha")->second);
}