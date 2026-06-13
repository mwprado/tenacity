/**********************************************************************

Tenacity GUI Facade smoke test

**********************************************************************/

#include "ui-facade/TenacityGuiFacadeSmokeTest.h"

#include "ui-facade/TenacityGuiFacade.h"

#include <wx/log.h>

namespace tenacity::ui_facade {

void SmokeTestGuiFacade(AudacityProject& project)
{
#if defined(_DEBUG)
   static bool once = false;
   if (once)
      return;
   once = true;

   TenacityGuiFacade facade{ project };

   const auto geometry = facade.GetViewportGeometry();
   const auto selection = facade.GetSelection();
   const auto transport = facade.GetTransportState();

   wxLogMessage(
      "TenacityGuiFacade smoke test: viewport=%dx%d hpos=%d hrange=%d "
      "selection=[%.3f, %.3f] transport={playing=%d recording=%d paused=%d}",
      geometry.widthPx,
      geometry.heightPx,
      geometry.horizontalThumbPosition,
      geometry.horizontalRange,
      selection.startSeconds,
      selection.endSeconds,
      static_cast<int>(transport.playing),
      static_cast<int>(transport.recording),
      static_cast<int>(transport.paused)
   );
#else
   (void)project;
#endif
}

} // namespace tenacity::ui_facade
