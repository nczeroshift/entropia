
#ifndef DS_MODEL_3D_H
#define DS_MODEL_3D_H

#include "dsItem3D.h"

class dsModel3D : public dsItem3D {
public:
	dsModel3D(DS::Data * data);
    virtual ~dsModel3D();
    void Load();
    void Render(DS::StageProxy * proxy, double start, double end, double time);
    void Update(DS::StageProxy * proxy, double start, double end, double time);
	void Parse(const JSONObject & node);
protected:
	// 3D Model with objects, models, materials, lamps, armatures...
	DS::Compound * m_Model;
	// Single camera object resolved.
	Scene::Camera * m_Camera;
	// Camera and timeline is defined from a compound.
	DS::Compound * m_CameraModel;
	Graph::Program * m_ShaderProgram;
private:
};

#endif