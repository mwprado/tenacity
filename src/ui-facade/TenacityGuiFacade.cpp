/**********************************************************************

Tenacity GUI Facade

**********************************************************************/

#include "ui-facade/TenacityGuiFacade.h"

namespace tenacity::ui_facade {

TenacityGuiFacade::TenacityGuiFacade(AudacityProject& project)
   : mProject{ project }
{
}

ViewportGeometry TenacityGuiFacade::GetViewportGeometry() const
{
   return {};
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
