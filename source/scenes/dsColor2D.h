#ifndef DS_COLOR_2D_H
#define DS_COLOR_2D_H

#include "dsItem2D.h"

class dsColor2D : public dsItem2D {
public:
	dsColor2D(DS::Data * data);
    virtual ~dsColor2D();
    void Load();
    void Render(DS::StageProxy * proxy, double start, double end, double time);
	void Parse(const JSONObject & node);
private:
	Graph::Program * m_ShaderProgram;
	Graph::Texture * m_Texture;
	DS::StageValue m_R, m_G, m_B;
};

#endif