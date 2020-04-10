#ifndef DS_SCENE_LOADING_H
#define DS_SCENE_LOADING_H

#include "../dsStage.h"

class dsSceneLoading : public DS::Stage {
public:
    dsSceneLoading(DS::Data * data);
    virtual ~dsSceneLoading();
    void Load();
	void Render(DS::StageProxy * proxy, double start, double end, double time);
private:
    Graph::Texture * logo;
    Graph::Program * prog;
};

#endif