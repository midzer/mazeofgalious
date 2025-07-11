# Emscripten

## Build

```
emmake make
```

## Link

```
em++ -flto -O3 -fno-rtti -fno-exceptions sources/*.o -o index.html -sUSE_SDL=2 -sUSE_SDL_IMAGE=2 -sSDL2_IMAGE_FORMATS=pcx -sUSE_SDL_MIXER=2 -sSDL2_MIXER_FORMATS=ogg -sASYNCIFY -sENVIRONMENT=web -sTOTAL_MEMORY=256mb --preload-file graphics/ --preload-file rooms/ --preload-file sound/ --closure 1 -sEXPORTED_RUNTIME_METHODS=['allocate','ALLOC_NORMAL']
```
