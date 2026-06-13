#pragma once

/**********************************************************************

Tenacity GUI Facade

Minimal state-extraction facade for experiments with alternative UI
frontends. This file must not depend on GTK, libadwaita, or wxWidgets UI
types unless strictly necessary.

**********************************************************************/

class AudacityProject;

namespace tenacity::ui_facade {

struct ViewportGeometry {
   int widthPx {};
   int heightPx {};
   int horizontalThumbPosition {};
   int horizontalRange {};
};

struct SelectionState {
   double startSeconds {};
   double endSeconds {};
};

struct TransportState {
   bool playing {};
   bool recording {};
   bool paused {};
};

class TenacityGuiFacade {
public:
   explicit TenacityGuiFacade(AudacityProject& project);

   ViewportGeometry GetViewportGeometry() const;
   SelectionState GetSelection() const;
   TransportState GetTransportState() const;

private:
   AudacityProject& mProject;
};

} // namespace tenacity::ui_facade
