#ifndef DS_LOADING_H
#define DS_LOADING_H

#include "../dsStage.h"

class dsLoading : public DS::Stage {
public:
	dsLoading(DS::Data * data);
    virtual ~dsLoading();
    void Load();
	void Render(DS::StageProxy * proxy, double start, double end, double time);
private:
    Graph::Texture * logo;
    Graph::Program * prog;
};

#endif