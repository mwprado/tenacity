#pragma once

#include <string>
#include <vector>

namespace tenacity::gnome_prototype {

struct TrackViewModel {
   int number {};
   std::string name;
   std::string channelLayout;
   std::string sampleRate;
   std::string sampleFormat;
   unsigned waveformSeed {};
};

struct ProjectViewModel {
   std::string title;
   std::string subtitle;
   std::string audioHost;
   std::string inputDevice;
   std::string outputDevice;
   std::vector<TrackViewModel> tracks;
};

ProjectViewModel MakeMockProjectViewModel();

} // namespace tenacity::gnome_prototype
