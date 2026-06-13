# Tenacity GUI Facade — local integration files

Copy these files into your local Tenacity fork:

```text
src/ui-facade/TenacityGuiFacade.h
src/ui-facade/TenacityGuiFacade.cpp
```

Then edit `src/CMakeLists.txt` and add the two files inside the existing `set( SOURCES PRIVATE ... )` list, for example near `ProjectWindow.cpp` / `ProjectWindow.h`:

```cmake
        ui-facade/TenacityGuiFacade.cpp
        ui-facade/TenacityGuiFacade.h
```

Suggested first local test:

```bash
git switch -c experiment/gnome-ui-facade
mkdir -p src/ui-facade
cp /path/to/TenacityGuiFacade.h src/ui-facade/
cp /path/to/TenacityGuiFacade.cpp src/ui-facade/
# edit src/CMakeLists.txt
cmake --build build
```

If the build reports an API mismatch, the likely places are:

1. `ProjectAudioManager::PlayCurrentRegion()`
2. `ProjectAudioManager::OnPause()`
3. `ProjectAudioManager::Stop(bool)`
4. `ProjectAudioManager::OnRecord(bool)`
5. `TrackPanel::FindTrackRect(const Track*)`
6. `TrackPanel::RefreshTrack(Track*, bool)`

These names were selected from the current fork headers and should be close, but the local branch/build configuration is the final authority.
