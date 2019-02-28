# MahjongAI-socket

## 安裝說明

請參考 [INSTALL.md](./INSTALL.md) 安裝 Boost

我個人是裝 mingw-win64 的 boost 1.69.0，載下來是 dll 檔，可以直接使用

MahjongAI-socket 的 .sin 檔在 `example/MahjongAI-socket/MahjongAI-socket.sin`

打開專案，在設定的地方把 Boost 的 lib 路徑加進去，應該就可以 compile 了

目前只有 Debug win32 版本可以 compile，release 跟 x64 都會在 linking 階段出錯，還在 debug 中

## 更改 AI

目前的 AI 是 IRB01，如果需要更改的話，要把需要用到的檔案放到 `example/` 目錄下，在 VS 專案中設定，並把 `expample/main.cpp` 第 318 行的 player 改成新的 AI