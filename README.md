## Jib

Jib is a simple web-browser for Maemo Leste.

![https://i.imgur.com/NTGFhFH.jpg](https://i.imgur.com/NTGFhFH.jpg | width=240)

- Chromium based
- Qt 5.15, CMake, CCache
- QtWidgets

```text
sudo apt install -y jib
```

### Settings

| Name                        | Description                                                                                                                                              |
|-----------------------------|----------------------------------------------------------------------------------------------------------------------------------------------------------|
| JavascriptEnabled           | Enables the running of JavaScript                                                                                                                        |
| userAgent                   | Switch between a desktop and mobile User-Agent.                                                                                                          |
| ZoomFactor                  | Web page zoom. Available: 75%, 100%, 125%, 150%, 200%                                                                                                    |
| AllowRunningInsecureContent | By default, HTTPS pages cannot run JavaScript, CSS, plugins or  web-sockets from HTTP URLs. This provides an override to get the old  insecure behavior. |
| PdfViewerEnabled            | Specifies that PDF documents will be opened in the internal PDF viewer                                                                                   |
| ShowScrollBars              | Show/hide scroll bars in the webview.                                                                                                                    |
| WebGLEnabled                | Enables WebGL, default off                                                                                                                               |

### Manual installation

Compilation on a Droid 4 is quite fast, around 3min.

```text
sudo apt install -y qtbase5-dev ccache cmake libx11-dev zlib1g-dev libpng-dev libssl-dev libqt5maemo5-dev libqt5svg5-dev libqt5x11extras5-dev libqt5charts5-dev qtmultimedia5-dev libqt5network5 qtquickcontrols2-5-dev qtdeclarative5-dev qtdeclarative5-dev-tools qtwebengine5-dev libqt5webengine5 
cmake -Bbuild .
make -Cbuild -j2
./build/bin/jib
```

### Notes

- The application takes 6 seconds to boot on Droid 4
- Tabs/multiple-windows not supported
- Browser extensions/plugins are not supported (See [QWebEngineView](https://doc.qt.io/qt-5/qtwebengine-overview.html)).

### To-do

The following features are not supported, but maybe they'll be implemented at a later date:

- AdBlock
- Password Manager
