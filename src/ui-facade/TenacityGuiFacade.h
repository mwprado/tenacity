#pragma once

/**********************************************************************

Tenacity GUI Facade

A small toolkit-neutral bridge for future non-wx frontends.

This header intentionally avoids wxWidgets and GTK includes. The .cpp file
connects these methods to the existing Tenacity objects: ProjectWindow,
TrackPanel, Viewport, ViewInfo, and ProjectAudioManager.

**********************************************************************/

#include <algorithm>
#include <string>
#include <utility>

class AudacityProject;
class ProjectWindow;
class TrackPanel;
class Track;
class ViewInfo;
class Viewport;
class ProjectAudioManager;

namespace tenacity::ui_facade {

struct Rect final {
   int x {0};
   int y {0};
   int width {0};
   int height {0};
};

struct ViewportGeometry final {
   int widthPx {0};
   int heightPx {0};
   int horizontalThumbPosition {0};
   int horizontalThumbSize {0};
   int horizontalRange {0};
   int verticalThumbPosition {0};
   int verticalThumbSize {0};
   int verticalRange {0};
};

struct TimeRange final {
   double startSeconds {0.0};
   double endSeconds {0.0};

   double LengthSeconds() const noexcept
   {
      return std::max(0.0, endSeconds - startSeconds);
   }

   void Normalize() noexcept
   {
      if (endSeconds < startSeconds)
         std::swap(startSeconds, endSeconds);
   }
};

struct TimelineState final {
   int horizontalPixelPosition {0};
   int horizontalPixelRange {0};
   int horizontalVisiblePixels {0};
   int verticalPixelPosition {0};
   int verticalPixelRange {0};
   int verticalVisiblePixels {0};
};

struct SelectionState final {
   double startSeconds {0.0};
   double endSeconds {0.0};
   bool isPoint {true};
};

enum class TransportState {
   Stopped,
   Playing,
   Paused,
   Recording,
   Unknown
};

struct TrackViewState final {
   const Track* track {nullptr};
   Rect rect {};
   int visualHeightPx {0};
   bool minimized {false};
};

class TenacityGuiFacade final {
public:
   explicit TenacityGuiFacade(AudacityProject& project) noexcept;

   TenacityGuiFacade(const TenacityGuiFacade&) = delete;
   TenacityGuiFacade& operator=(const TenacityGuiFacade&) = delete;

   AudacityProject& Project() const noexcept;
   ProjectWindow& Window() const;
   TrackPanel& Panel() const;
   ViewInfo& View() const;
   Viewport& TimelineViewport() const;
   ProjectAudioManager& Audio() const;

   ViewportGeometry GetViewportGeometry() const;
   TimelineState GetTimelineState() const;

   void RefreshAll(bool eraseBackground = false) const;
   void RefreshTrack(Track& track, bool refreshBacking = true) const;
   void UpdateLayout() const;
   void SetDefaultWindowSize() const;

   void SetHorizontalPixelPosition(int positionPx) const;
   void ScrollHorizontalByPixels(int deltaPx) const;
   void ZoomIn(double multiplier = 1.25) const;
   void ZoomOut(double multiplier = 0.80) const;
   void ZoomFitHorizontally() const;
   void ZoomFitVertically() const;

   SelectionState GetSelection() const;
   void SetSelection(TimeRange range) const;
   void CollapseSelectionToStart() const;
   void CollapseSelectionToEnd() const;

   TransportState GetTransportState() const;
   void Play() const;
   void Pause() const;
   void Stop(bool stopStream = true) const;
   void Record(bool altAppearance = false) const;

   TrackViewState GetTrackViewState(const Track& track) const;
   Rect GetTrackRect(const Track& track) const;
   int GetTrackHeight(const Track& track) const;
   void SetTrackHeight(Track& track, unsigned heightPx) const;
   bool IsTrackMinimized(const Track& track) const;
   void SetTrackMinimized(Track& track, bool minimized) const;
   void ShowTrack(const Track& track) const;

   std::string FormatSelection() const;
   std::string FormatTransport() const;

private:
   AudacityProject& mProject;
};

} // namespace tenacity::ui_facade
