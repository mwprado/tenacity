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
   const auto transport = facade.FormatTransport();

   wxLogMessage(
      "TenacityGuiFacade smoke test: viewport=%dx%d hpos=%d hrange=%d "
      "selection=[%.3f, %.3f] transport=%s",
      geometry.widthPx,
      geometry.heightPx,
      geometry.horizontalThumbPosition,
      geometry.horizontalRange,
      selection.startSeconds,
      selection.endSeconds,
      transport
   );
#else
   (void)project;
#endif
}

} // namespace tenacity::ui_facade
