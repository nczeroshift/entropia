
#ifndef DS_ITEM_2D_H
#define DS_ITEM_2D_H

#include "../dsStage.h"

class dsItem2D : public DS::Stage {
public:
	dsItem2D(DS::Data * data);
    virtual ~dsItem2D();
    void Load();
    void Render(DS::StageProxy * proxy, double start, double end, double time);
    void Update(DS::StageProxy * proxy, double start, double end, double time);
	void Parse(const JSONObject & node);
protected:
	DS::StageValue m_X, m_Y;
	DS::StageValue m_Width, m_Height;
	DS::StageValue m_Alpha;

	float m_CurrentX, m_CurrentY;
	float m_CurrentWidth, m_CurrentHeight;
	float m_CurrentAlpha;
private:
};

#endif