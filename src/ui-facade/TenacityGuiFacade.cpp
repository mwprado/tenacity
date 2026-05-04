/**********************************************************************

Tenacity GUI Facade

Initial implementation over the existing wxWidgets-based Tenacity UI.

**********************************************************************/

#include "ui-facade/TenacityGuiFacade.h"

#include "ProjectAudioManager.h"
#include "ProjectWindow.h"
#include "TrackPanel.h"
#include "Viewport.h"
#include "ViewInfo.h"

#include <iomanip>
#include <sstream>

namespace tenacity::ui_facade {

namespace {

std::string FormatSeconds(double seconds)
{
   std::ostringstream out;
   out << std::fixed << std::setprecision(3) << seconds << "s";
   return out.str();
}

} // namespace

TenacityGuiFacade::TenacityGuiFacade(AudacityProject& project) noexcept
   : mProject{project}
{
}

AudacityProject& TenacityGuiFacade::Project() const noexcept
{
   return mProject;
}

ProjectWindow& TenacityGuiFacade::Window() const
{
   return ProjectWindow::Get(mProject);
}

TrackPanel& TenacityGuiFacade::Panel() const
{
   return TrackPanel::Get(mProject);
}

ViewInfo& TenacityGuiFacade::View() const
{
   return ViewInfo::Get(mProject);
}

Viewport& TenacityGuiFacade::TimelineViewport() const
{
   return Viewport::Get(mProject);
}

ProjectAudioManager& TenacityGuiFacade::Audio() const
{
   return ProjectAudioManager::Get(mProject);
}

ViewportGeometry TenacityGuiFacade::GetViewportGeometry() const
{
   auto& window = Window();
   const auto [width, height] = window.ViewportSize();

   return {
      .widthPx = width,
      .heightPx = height,
      .horizontalThumbPosition = window.GetHorizontalThumbPosition(),
      .horizontalThumbSize = window.GetHorizontalThumbSize(),
      .horizontalRange = window.GetHorizontalRange(),
      .verticalThumbPosition = window.GetVerticalThumbPosition(),
      .verticalThumbSize = window.GetVerticalThumbSize(),
      .verticalRange = window.GetVerticalRange(),
   };
}

TimelineState TenacityGuiFacade::GetTimelineState() const
{
   auto& window = Window();

   return {
      .horizontalPixelPosition = window.GetHorizontalThumbPosition(),
      .horizontalPixelRange = window.GetHorizontalRange(),
      .horizontalVisiblePixels = window.GetHorizontalThumbSize(),
      .verticalPixelPosition = window.GetVerticalThumbPosition(),
      .verticalPixelRange = window.GetVerticalRange(),
      .verticalVisiblePixels = window.GetVerticalThumbSize(),
   };
}

void TenacityGuiFacade::RefreshAll(bool eraseBackground) const
{
   Panel().Refresh(eraseBackground);
}

void TenacityGuiFacade::RefreshTrack(Track& track, bool refreshBacking) const
{
   Panel().RefreshTrack(&track, refreshBacking);
}

void TenacityGuiFacade::UpdateLayout() const
{
   Window().UpdateLayout();
}

void TenacityGuiFacade::SetDefaultWindowSize() const
{
   TimelineViewport().SetToDefaultSize();
}

void TenacityGuiFacade::SetHorizontalPixelPosition(int positionPx) const
{
   Window().SetHorizontalThumbPosition(positionPx);
   TimelineViewport().DoScroll();
}

void TenacityGuiFacade::ScrollHorizontalByPixels(int deltaPx) const
{
   TimelineViewport().ScrollHorizontalByPixels(deltaPx);
}

void TenacityGuiFacade::ZoomIn(double multiplier) const
{
   TimelineViewport().ZoomBy(multiplier);
}

void TenacityGuiFacade::ZoomOut(double multiplier) const
{
   TimelineViewport().ZoomBy(multiplier);
}

void TenacityGuiFacade::ZoomFitHorizontally() const
{
   TimelineViewport().ZoomFitHorizontally();
}

void TenacityGuiFacade::ZoomFitVertically() const
{
   TimelineViewport().ZoomFitVertically();
}

SelectionState TenacityGuiFacade::GetSelection() const
{
   const auto& selected = View().selectedRegion;

   return {
      .startSeconds = selected.t0(),
      .endSeconds = selected.t1(),
      .isPoint = selected.isPoint(),
   };
}

void TenacityGuiFacade::SetSelection(TimeRange range) const
{
   range.Normalize();
   View().selectedRegion.setTimes(range.startSeconds, range.endSeconds);
   RefreshAll(false);
}

void TenacityGuiFacade::CollapseSelectionToStart() const
{
   View().selectedRegion.collapseToT0();
   RefreshAll(false);
}

void TenacityGuiFacade::CollapseSelectionToEnd() const
{
   View().selectedRegion.collapseToT1();
   RefreshAll(false);
}

TransportState TenacityGuiFacade::GetTransportState() const
{
   const auto& audio = Audio();

   if (audio.Recording())
      return TransportState::Recording;
   if (audio.Playing())
      return TransportState::Playing;
   if (audio.Paused())
      return TransportState::Paused;

   return TransportState::Stopped;
}

void TenacityGuiFacade::Play() const
{
   Audio().PlayCurrentRegion();
}

void TenacityGuiFacade::Pause() const
{
   Audio().OnPause();
}

void TenacityGuiFacade::Stop(bool stopStream) const
{
   Audio().Stop(stopStream);
}

void TenacityGuiFacade::Record(bool altAppearance) const
{
   Audio().OnRecord(altAppearance);
}

TrackViewState TenacityGuiFacade::GetTrackViewState(const Track& track) const
{
   return {
      .track = &track,
      .rect = GetTrackRect(track),
      .visualHeightPx = GetTrackHeight(track),
      .minimized = IsTrackMinimized(track),
   };
}

Rect TenacityGuiFacade::GetTrackRect(const Track& track) const
{
   const auto wxRect = Panel().FindTrackRect(&track);

   return {
      .x = wxRect.x,
      .y = wxRect.y,
      .width = wxRect.width,
      .height = wxRect.height,
   };
}

int TenacityGuiFacade::GetTrackHeight(const Track& track) const
{
   return Window().GetTrackHeight(track);
}

void TenacityGuiFacade::SetTrackHeight(Track& track, unsigned heightPx) const
{
   Window().SetChannelHeights(track, heightPx);
   TimelineViewport().UpdateScrollbarsForTracks();
   RefreshTrack(track, true);
}

bool TenacityGuiFacade::IsTrackMinimized(const Track& track) const
{
   return Window().IsTrackMinimized(track);
}

void TenacityGuiFacade::SetTrackMinimized(Track& track, bool minimized) const
{
   Window().SetMinimized(track, minimized);
   TimelineViewport().UpdateScrollbarsForTracks();
   RefreshTrack(track, true);
}

void TenacityGuiFacade::ShowTrack(const Track& track) const
{
   TimelineViewport().ShowTrack(track);
}

std::string TenacityGuiFacade::FormatSelection() const
{
   const auto selection = GetSelection();

   return "Start " + FormatSeconds(selection.startSeconds) +
          "  End " + FormatSeconds(selection.endSeconds) +
          "  Length " + FormatSeconds(selection.endSeconds - selection.startSeconds);
}

std::string TenacityGuiFacade::FormatTransport() const
{
   switch (GetTransportState()) {
      case TransportState::Stopped:
         return "Stopped";
      case TransportState::Playing:
         return "Playing";
      case TransportState::Paused:
         return "Paused";
      case TransportState::Recording:
         return "Recording";
      case TransportState::Unknown:
         return "Unknown";
   }

   return "Unknown";
}

} // namespace tenacity::ui_facade
