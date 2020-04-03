# Manual

## 1. Application structure
The application is compromised of a data folder with the following structure:
* models
* textures
* scripts
* shaders
* audio
* httpd

If the data folder is not available but a TAR file with data contents placed in the same folder as the application folder, it will use the TAR has the data source.
### 1.1 Models folder
This is where the 3d model files(*.bxon) can be placed.

### 1.2 Textures folder
Where image files(*.png, *.jpg) for 3D models texture files and photos can be placed.

### 1.3 Scripts folder
Where script files( *.json, *.txt) can be placed.

### 1.4 Shaders folder
Where shader files (*.cpp) can be placed.

### 1.5 Audio folder
Where audio files (*.wav, *.ogg) can be placed.

### 1.5 HTTPD folder
Where the webserver files are stored, can be removed if the webserver is not used.

## 2. Timeline
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

### Timeline "scenes" node
