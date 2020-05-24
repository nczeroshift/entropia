# Manual
## 1. About
**Entropy** is an opensource compatible application for 3D graphics, audio and IO experimentation, mainly thought for the demoscene and "pc demo" category competitions, it can have other aplications for VJ or realtime/interactive art performances. 
    
## 2. Requirements
Device capable of running **OpenGL 2.0** / **OpenGLES 2.0** applications and audio output, running **Windows 10** / **Linux** / **MacOS X 10.9.5**.

### 2.1. Development from binaries
The minimum required tools to develop with Entropy are a text editor, [Blender](https://download.blender.org/release/Blender2.79/), image editor and optionaly, a web browser. 

The text editor is used to edit the configuration files (**Plain Text**, [JSON](https://en.wikipedia.org/wiki/JSON)), shaders ([GLSL](https://www.khronos.org/opengles/sdk/docs/reference_cards/OpenGL-ES-2_0-Reference-card.pdf)) and timeline scripts (**JSON**).

Content created with **Blender** can be exported with [bxon for Blender 2.79](https://github.com/nczeroshift/bxon-3d) and loaded to **Entropy**, the script supports exporting of model geometry, materials, textures, lamps, cameras, armatures and animations. 

A web browser can be used as a real-time editing tool for shaders and timeline, open the browser at the url [http://localhost:8080](http://localhost:8080).

### 2.2. Development from source
To compile from source, the required enviroment needs to be able to compile C++ aplications prior to C++11 standard. Project files are included to build with Visual Studio C++, XCode, and Python + GCC on linux.

## 3. File formats
* Audio streams: OGG (44100 Hz, Stereo)
* Textures: JPG, PNG (RGB 24 Bit and RGBA), BMP
* Models: BXON
* Scripts: GLSL, JSON, XML
* Package: TAR

## 4. Shortcuts
* **Space** - Play/Pause playback
* **C** - Show/Hide UI
* **X** - Reload resources (textures, models, shadders)
* **Z** - Reload timeline script
* **Escape** - Exit Entropy

## 5. Folder structure
The aplication folder structure has the following layout:
|Path|Filename|Description|
|-|-|-|
|.|entropy.exe|Aplication binary|
|.|data|Aplication data folder|
|data|log.txt|Created by aplication on exception|
|data|models|Models folder|
|data|textures|Textures folder|
|data/textures|ui|UI texture resources, required|
|data|scripts|Scripts folder|
|data|shaders|GLSL shaders folder|
|data/shaders|include|Shaders headers, required|
|data/shaders|ui|Shaders source for UI, required|
|data|audio|Audio streams folder|
|data|httpd|HTTP server folder, optional if web server is not used|

### 5.1 TAR data folder
The data folder can be packaged as a tar file and used as the application virtual file system.

## 6. Configuration

## 7. Timeline
The Timeline file can be found in the path scripts/timeline.json and manages the application scenes timing and configuration.

The JSON file has two main sections "scenes" and "timeline". The node "scenes" is used to instance and configure the used scenes, the node
"timeline" setups the scenes timing configuration. The same "scene" node can be used as many times as necessary in the "timeline" node.

```json
{
    "scenes":[{
            "type": "text",
            "name": "txt_1",
            "x": 256, "y": 256,
            "text": "hello",
            "font": "sans_serif"
    }],
    "timeline":[{
            "name": "txt_1",
            "start": 0,
            "end": 120,
            "track": 1
    }]
```
Example: Showing a text string in the screen
