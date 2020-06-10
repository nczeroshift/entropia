
/**
* NCK-DEMO SCENE
* https://github.com/nczeroshift/nck-demo
*/

#include "dsMain.h"

_DS_BEGIN

class dsMainWithTimeline : public dsMain
{
public:
	dsMainWithTimeline(Core::Window * window);
	virtual ~dsMainWithTimeline();
    void LoadTimeline();
private:
	bool m_EnableMusic;
	std::string m_MusicFilename;
	int m_SoundSamplerate;
	int m_SoundFFTSize;
	int64_t m_LastTimelineDate;
	std::map<std::string, DS::Stage*> m_InstancedStages;
};

_DS_END

