#ifndef DS_TEXT_2D_H
#define DS_TEXT_2D_H

#include "dsItem2D.h"

class dsText2D : public dsItem2D {
public:
	dsText2D(DS::Data * data);
    virtual ~dsText2D();
    void Load();
    void Render(DS::StageProxy * proxy, double start, double end, double time);
	void Parse(const JSONObject & node);
private:
	Graph::Program * m_ShaderProgram;
	Graph::Texture * m_FontTexture;
	Gui::FontMap * m_FontMap;
	Gui::FontAlignment m_FontAlignment;
	DS::StageValue m_FontSize;
	DS::StageValue m_R, m_G, m_B;
    std::string m_Text;
};

#endif