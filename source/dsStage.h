
/**
* NCK-DEMO SCENE
* https://github.com/nczeroshift/nck-demo
*/

#ifndef DS_SCENE_H
#define DS_SCENE_H

#include "dsData.h"

_DS_BEGIN

class StageValue {
public:
	StageValue();
	StageValue(float value);
	~StageValue();
	void Parse(const JSONValue & value);
	float GetValue(float time);
private:
	float m_Value;
	Scene::AnimationGraph * m_BezierValue;
};

class StageProxy;
class Stage
{
public:
    Stage(Data * data) { m_Data = data; /*m_Order = 0;*/ };
    virtual ~Stage() {};
    virtual void Load() = 0;
    virtual void Render(StageProxy * proxy, double start, double end, double time) = 0;
    virtual void RenderFBO(StageProxy * proxy, double start, double end, double time) {};
    virtual void Update(StageProxy * proxy, double start, double end, double time) {};
	virtual void Parse(const JSONObject & node) {};

   // virtual void AddStage(double start, double end, Stage * stage, int layer = 0);
   // virtual void BuildTimeline();
    //virtual void SetOrder(int order) { m_Order = order; }
    //virtual int GetOrder() { return m_Order; }
    Data * GetData() { return m_Data; }
protected:
	Data * m_Data;
};

class StageProxy : public Stage{
public:
    StageProxy(Data * data);
    StageProxy(Stage * stage, double offset = 0);
    ~StageProxy();
    void Load();
    void Render(StageProxy * proxy, double start, double end, double time);
    void RenderFBO(StageProxy * proxy, double start, double end, double time);
    void Update(StageProxy * proxy, double start, double end, double time);

	void Parse(const JSONObject & node);
    void SetOffset(double offset);
    void SetStage(Stage * stage);
	void AddStage(Math::TimelineItem<DS::StageProxy*> stage);
    //void SetOrder(int order) { m_Stage->SetOrder(order); }
    //int GetOrder() { return m_Stage->GetOrder(); }
	void SetOrder(int order) { m_Order = order; }
	int GetOrder() { return m_Order; }
    void SetRepeat(double duration) { m_RepeatDuration = duration;}
	double SetRepeat() { return m_RepeatDuration; }
	void BuildTimeline();
	static Math::TimelineItem<DS::StageProxy*> Parse(const JSONObject & entry, const std::map<std::string, DS::Stage*> & stages);
	static bool compareTL(const Math::TimelineItem<DS::StageProxy*> & a, const Math::TimelineItem<DS::StageProxy*> & b);
	int GetItems(double time, std::list<Math::TimelineItem<DS::StageProxy*>> * proxy);
protected:
	double m_RepeatDuration;
	double m_Offset;
    Stage * m_Stage;
	int m_Order;
	Math::TimelineNode<DS::StageProxy*> m_Stages;
};

_DS_END

#endif