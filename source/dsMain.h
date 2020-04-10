
/**
* NCK-DEMO SCENE
* https://github.com/nczeroshift/nck-demo
*/

#include "dsData.h"
#include "dsUtils.h"
#include "dsSettings.h"
#include "dsSubtitles.h"
#include "dsAudioPlayer.h"
#include "dsStage.h"
#include "tinyxml.h"
#include "dsSceneLoading.h"
#include "dsServer.h"

_DS_BEGIN

class dsMain : public virtual Core::Threadable
{
public:
    dsMain(Core::Window * window);
    virtual ~dsMain();

    void AddSub(float start, float duration, const std::string & text);

    void Run();

	virtual void LoadTimeline() {};

	virtual void LoadAudio() {};

	virtual double GetMaxDuration() { return m_MaxDuration; }

    double UpdateFPS();
    
    void ReleaseStuff();

    int64_t GetTime();

	void RenderUI(const ReloadResult & rCompounds,const ReloadResult & rShaders, const ReloadResult & rTextures, float fps);
    void DrawText(float x, float y, float size, bool bold, const std::string & str, const Math::Color4ub & col = Math::Color4ub(255, 255, 255, 255));
    
    void DrawTimeText(float y);

    void DrawFrameRate(float x, float y, float fps);

    void DrawSeekbar(float y, float height);

    void DrawFFT(float x, float y, float width, float height);

    void DrawFPS(float x, float y, float width, float height);

    void PlayPauseToggle();

    void SeekTo(int64_t pos);

	void RequestReloadTimeline();

    void RequestReloadResources();

    void ToggleUI();

    void UpdateWndInput();

    void renderLoading(float progress);

	Data * GetData() { return data; }

	void SetMaxDuration(double value) { m_MaxDuration = value; }

	void SetAudioSettings(int audioSampleRate,int audioFFTSize, const std::string & audioStream);
protected:
    int reloadTextureCounter[3];
    int reloadCompoundCounter[3];
    int reloadShaderFlag[3];

    bool reloadTimeline;
    bool reloadResources;

	bool m_AudioEnable;
	std::string m_AudioStreamFilename;
	int m_AudioSampleRate;
	int m_AudioFTTSize;

    bool showUI;
    Data * data;
    int64_t lastTime;               // Onde começou a tocar 
    std::list<float>    fpsQueue;   // Queue para filtrar os fps
    Core::Chronometer * frTime;     // Timer para a framerate
    Core::Chronometer * timer;      // Tempo passado desde começou a reproduzir
    double m_MaxDuration;            // Duração total
    Math::TimelineNode<Subtitle*> subtitlesTimeline; // Suporte de desenho de legendas
	Math::TimelineNode<DS::StageProxy*> mainTimeline; // Scenes timeline.
    bool mutePlayback;              // Modo de reprodução sem pista de som
    AudioPlayer * player;           // Dispositivo de som e FFT
    float sDensity;                 // Densidade do ecrã
    float hScale;                   // Factor de escala vertical
    Gui::ShapeRenderer * shpRender;
    Graph::Texture	* fontTexture, * fontTimeTexture;   // Textura da fonte
    Gui::FontMap    * fontMap, * fontTimeMap;           // Mapa de caracteres da font
    Graph::Device   * dev;          // Dispositivo de gráfico
    Core::Window    * wnd;
    dsSceneLoading * loading;
    Server * server;
	Graph::Texture * iconTexture;
	Graph::Texture * iconCompound;
	Graph::Texture * iconShader;
};

JSONValue * ReadFileAsJSON(const std::string & fileName);

_DS_END


