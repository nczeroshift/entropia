
#include "dsItem3D.h"
#include "../dsUtils.h"

dsItem3D::dsItem3D(DS::Data * data) : DS::Stage(data), m_X(0), m_Y(0), m_Z(0) {

}

dsItem3D::~dsItem3D() {

}

void dsItem3D::Load() {
  
}


void dsItem3D::Update(DS::StageProxy * proxy, double start, double end, double time) {
	const double off = time - start;
	m_CurrentX = m_X.GetValue(off);
	m_CurrentY = m_Y.GetValue(off);
	m_CurrentZ = m_Z.GetValue(off);
}

void dsItem3D::Render(DS::StageProxy * proxy, double start, double end, double time) {

}

void dsItem3D::Parse(const JSONObject & node) {
	if (node.find("x") != node.end())
		m_X.Parse(*node.find("x")->second);

	if (node.find("y") != node.end())
		m_Y.Parse(*node.find("y")->second);

	if (node.find("z") != node.end())
		m_Z.Parse(*node.find("z")->second);
}