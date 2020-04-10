
/**
* NCK-DEMO SCENE
* https://github.com/nczeroshift/nck-demo
*/

#include "dsMainWithTimeline.h"


void Core::Application_Main(const std::vector<std::string> & CmdLine)
{
	// Where settings are stored, optional.
	const std::string fileName = "script://config.json";

	// Initialization variables.
	std::string windowTitle = "Entropy";
	bool fullScreen = false;
	bool showConfig = false;
    int width = 1920 * 0.6, height = 1080 * 0.6;
	int virtualWidth = 1920, virtualHeight = 1080;
	bool audioEnable = false;
	std::string audioStream = "";
	int audioSampleRate = AUDIO_SAMPLERATE;
	int audioFFTSize = AUDIO_FFT;
	double maxDuration = 10 * 60; // seconds

	try{
		JSONValue * obj = DS::ReadFileAsJSON(fileName);

		const JSONObject & root = obj->AsObject();
		
		if (root.find("showConfig") != root.end())
			showConfig = root.find("showConfig")->second->AsBool();

		if (root.find("maxDuration") != root.end()) {
			maxDuration = root.find("maxDuration")->second->AsNumber();
		}

		if (root.find("fullScreen") != root.end())
			fullScreen = root.find("fullScreen")->second->AsBool();

		if (root.find("windowTitle") != root.end())
			windowTitle = root.find("windowTitle")->second->AsString();

		if (root.find("virtualWidth") != root.end())
			virtualWidth = root.find("virtualWidth")->second->AsNumber();

		if (root.find("virtualHeight") != root.end())
			virtualHeight = root.find("virtualHeight")->second->AsNumber();

		if (root.find("windowWidth") != root.end())
			width = root.find("windowWidth")->second->AsNumber();

		if (root.find("windowHeight") != root.end())
			height = root.find("windowHeight")->second->AsNumber();

		if (root.find("audio") != root.end()) {
			const JSONObject & audio = root.find("audio")->second->AsObject();
			audioEnable = true;

			if (audio.find("stream") != audio.end())
				audioStream = audio.find("stream")->second->AsString();
			
			if (audio.find("sampleRate") != audio.end())
				audioSampleRate = audio.find("sampleRate")->second->AsNumber();

			if (audio.find("fftSize") != audio.end())
				audioFFTSize = audio.find("fftSize")->second->AsNumber();
		}
		
		SafeDelete(obj);
	}
	catch (const Core::Exception & e) {
		e.PrintStackTrace();
	}

	bool runDemo = true;

	if (showConfig) {
		DS::DemoSettings * conf = new DS::DemoSettings();
		runDemo = conf->Run(&width, &height, &fullScreen);
		SafeDelete(conf);
	}

    if (runDemo) 
    {
        Core::Window * wnd = Core::CreateWindow(windowTitle, width, height, fullScreen);

        DS::dsMain * grThread = new DS::dsMainWithTimeline(wnd);

		grThread->SetMaxDuration(maxDuration);
		grThread->GetData()->SetScreenWidth(virtualWidth);
		grThread->GetData()->SetScreenHeight(virtualHeight);

		if (audioEnable) {
			grThread->SetAudioSettings(audioSampleRate, audioFFTSize, audioStream);
		}

        grThread->Start();
        while (wnd->Peek(true)) 
        {
            if(fullScreen)
                wnd->SetCursorVisiblity(false);

            if (!grThread->IsRunning())
                break;
            
            grThread->UpdateWndInput();
        }
        grThread->TearDownAndJoin();

        SafeDelete(grThread);
        SafeDelete(wnd);
    }
}

