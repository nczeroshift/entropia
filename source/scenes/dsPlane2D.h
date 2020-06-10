#ifndef DS_PLANE_2D_H
#define DS_PLANE_2D_H

#include "dsItem2D.h"

class dsPlane2D : public dsItem2D {
public:
	dsPlane2D(DS::Data * data);
    virtual ~dsPlane2D();
    void Load();
    void Render(DS::StageProxy * proxy, double start, double end, double time);
	void Parse(const JSONObject & node);
private:
	Math::Vec2 m_Anchor;
	Graph::Program * m_ShaderProgram;
	Graph::Texture * m_Texture;
	std::vector<Graph::Texture*> m_Textures;
	DS::StageValue m_R, m_G, m_B;
};

#endif