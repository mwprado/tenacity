#include "PrototypeModel.h"

namespace tenacity::gnome_prototype {

ProjectViewModel MakeMockProjectViewModel()
{
   return {
      "16ABR_SAMPLE_PART1",
      "Current project · 44.1 kHz · stereo · 32-bit float",
      "PipeWire",
      "default",
      "default",
      {
         {
            1,
            "16ABR_SAMPLE_PART1",
            "Stereo",
            "44.1 kHz",
            "32-bit float",
            101
         },
         {
            2,
            "Voice - edited segment",
            "Stereo",
            "44.1 kHz",
            "32-bit float",
            501
         },
         {
            3,
            "Ambience / reference",
            "Stereo",
            "44.1 kHz",
            "32-bit float",
            901
         }
      }
   };
}

} // namespace tenacity::gnome_prototype
