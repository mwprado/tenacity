#pragma once

/**********************************************************************

Tenacity GUI Facade smoke test

Small runtime probe for the facade. It is intentionally compiled only as
ordinary application code and does not change project behavior.

**********************************************************************/

class AudacityProject;

namespace tenacity::ui_facade {

void SmokeTestGuiFacade(AudacityProject& project);

} // namespace tenacity::ui_facade
