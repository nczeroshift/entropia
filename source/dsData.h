
/**
* NCK-DEMO SCENE
* https://github.com/nczeroshift/nck-demo
*/

#ifndef DS_DATA_H
#define DS_DATA_H

#include "dsCompound.h"
#include <tinyxml.h>
#include "../codecs/JSON.h"

_DS_BEGIN

class ReloadResult {
public:
    ReloadResult() { total = 0; success = 0; failure = 0; }
    ReloadResult(int total, int success, int failure) {
        this->total = total; this->success = success; this->failure = failure;
    }
    int GetTotal() const { return total; };
    int GetSuccess() const { return success; };
    int GetFailure() const { return failure; };
private:
    int total;
    int success;
    int failure;
};

class Data {
public:
    Data(Core::Window * wnd, Graph::Device * gDev);
    ~Data();

    ReloadResult ReloadTextures();
    ReloadResult ReloadCompounds();
    ReloadResult ReloadShaders();

    Graph::Device * GetGraphicsDevice();
    Gui::ShapeRenderer * GetShapeRenderer();
    Gui::WidgetRenderer * GetWidgetRenderer();

    void GetProgramKeys(std::vector<std::string> * names);
    void GetTextureKeys(std::vector<std::string> * names);
    void GetCompoundsKeys(std::vector<std::string> * names);

	Graph::Program * LoadProgramFromText(const std::string & name, const std::string & source);
    Graph::Program * LoadProgram(const std::string & filename);
    Graph::Texture * LoadTexture(const std::string & filename);
    Compound * LoadCompound(const std::string & filename);
    Gui::FontMap * LoadFontMap(const std::string & filename);
    Compound * LoadCompound(const std::string & filename, Scene::Processor * processor);

    Graph::Program * GetProgram(const std::string & filename);
    Graph::Texture * GetTexture(const std::string & filename);
    Compound * GetCompound(const std::string & filename);
    Gui::FontMap * GetFontMap(const std::string & filename);
      
	void SetTexture(const std::string & filename, Graph::Texture * texture);

    float GetAspect();
    float GetHeight();
    float GetWidth();

	int GetVirtualDisplayWidth();
	int GetVirtualDisplayHeight();
	void SetupVirtualDisplay(int width, int height);

	void GetShaderErrors(std::list<std::string>  * errors);

	void LockAccess();
	void UnlockAccess();

	void SetVirtualViewport();
	Math::Vec2 GetVirtualViewportDimensions();
private:
	float m_VirtualDisplayWidth, m_VirtualDisplayHeight;
	Core::Mutex * accessMutex;
	std::list<std::string> shadersErrors;
    int64_t lastTimelineChange;
    TiXmlDocument sceneTimeline;
    Core::Window * window;
    Graph::Device * gDevice;
    Gui::WidgetRenderer * widgetRenderer;
    Gui::ShapeRenderer * shapeRenderer;
    std::map<std::string, Gui::FontMap*> m_FontMap;
    std::map<std::string, Compound*> m_Compounds;
    std::map<std::string, Graph::Program*> m_Programs;
    std::map<std::string, Graph::Texture*> m_Textures;
};

_DS_END

#endif