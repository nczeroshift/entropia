
#ifndef DS_ITEM_3D_H
#define DS_ITEM_3D_H

#include "../dsStage.h"

class dsItem3D : public DS::Stage {
public:
	dsItem3D(DS::Data * data);
    virtual ~dsItem3D();
    void Load();
    void Render(DS::StageProxy * proxy, double start, double end, double time);
    void Update(DS::StageProxy * proxy, double start, double end, double time);
	void Parse(const JSONObject & node);
protected:
	DS::StageValue m_X, m_Y, m_Z;
	float m_CurrentX, m_CurrentY, m_CurrentZ;
private:
};

#endif