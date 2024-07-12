## Jib

Jib is a simple web-browser for Maemo Leste.

<img src="https://i.imgur.com/kDeQoVJ.jpg" width="300"/>

- Chromium based
- Qt 5.15, CMake, CCache
- QtWidgets
- Adblock (uBlockOrigin)

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

### Adblock

The following filters are available and can be configured individually:

- Filters: The regular filters from uBlockOrigin
- ~~I don't care about cookies: Dismiss cookie popups~~ (bug: white screen, disabled for now)
- Privacy: Filters for enhanced privacy
- Resource Abuse: foil sites potentially abusing CPU/bandwidth resources without informed consent
- Unbreak: Unbreak sites broken as a result of 3rd-party filter lists enabled by default

To update filters, see: `src/assets/adblock/rules/update.sh`

### Manual installation

Compilation on a Droid 4 is quite fast, around 3min.

```text
sudo apt install -y qtbase5-dev ccache cmake libx11-dev zlib1g-dev libpng-dev libssl-dev libqt5maemo5-dev libqt5svg5-dev libqt5x11extras5-dev libqt5charts5-dev qtmultimedia5-dev libqt5network5 qtquickcontrols2-5-dev qtdeclarative5-dev qtdeclarative5-dev-tools qtwebengine5-dev libqt5webengine5 
cmake -Bbuild .
make -Cbuild -j2
./build/bin/jib
```

### To-do

The following feature(s) are not supported, but to be implemented:

- Password Manager
