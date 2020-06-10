
#ifndef DS_TEXTURE_CAPTURE_H
#define DS_TEXTURE_CAPTURE_H

#include "../dsStage.h"

class dsTextureCapture : public DS::Stage {
public:
	dsTextureCapture(DS::Data * data);
    virtual ~dsTextureCapture();
    void Load();
    void Render(DS::StageProxy * proxy, double start, double end, double time);
    void RenderFBO(DS::StageProxy * proxy, double start, double end, double time);
    void Update(DS::StageProxy * proxy, double start, double end, double time);
	void Parse(const JSONObject & obj);
protected:
    Graph::Texture2D * fboTexture;
    Graph::RTManager * fboManager;
    Graph::Program * aaProgram;
};

#endif