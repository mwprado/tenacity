/**********************************************************************

Tenacity GUI Facade

**********************************************************************/

#include "ui-facade/TenacityGuiFacade.h"

#include "ProjectWindow.h"

namespace tenacity::ui_facade {

TenacityGuiFacade::TenacityGuiFacade(AudacityProject& project)
   : mProject{ project }
{
}

ViewportGeometry TenacityGuiFacade::GetViewportGeometry() const
{
   const auto pWindow = ProjectWindow::Find(&mProject);
   if (!pWindow)
      return {};

   const auto [widthPx, heightPx] = pWindow->ViewportSize();

   return {
      widthPx,
      heightPx,
      pWindow->GetHorizontalThumbPosition(),
      pWindow->GetHorizontalRange()
   };
}

SelectionState TenacityGuiFacade::GetSelection() const
{
   return {};
}

TransportState TenacityGuiFacade::GetTransportState() const
{
   return {};
}

} // namespace tenacity::ui_facade
