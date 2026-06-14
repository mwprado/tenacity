#pragma once

#include <gtk/gtk.h>

class GnomeTrackView final
{
public:
    GnomeTrackView();
    ~GnomeTrackView() = default;

    GnomeTrackView(const GnomeTrackView&) = delete;
    GnomeTrackView& operator=(const GnomeTrackView&) = delete;

    GnomeTrackView(GnomeTrackView&&) = delete;
    GnomeTrackView& operator=(GnomeTrackView&&) = delete;

    GtkWidget* widget() const;

private:
    GtkWidget* buildRoot();
    GtkWidget* buildHeader();
    GtkWidget* buildWaveformScroller();
    GtkWidget* buildWaveformArea();

    static void drawWaveform(
        GtkDrawingArea* area,
        cairo_t* cr,
        int width,
        int height,
        gpointer userData
    );

private:
    GtkWidget* m_root { nullptr };
    GtkWidget* m_header { nullptr };
    GtkWidget* m_waveformScroller { nullptr };
    GtkWidget* m_waveformArea { nullptr };

    static constexpr int kHeaderWidth = 150;
    static constexpr int kTrackHeight = 96;
    static constexpr int kWaveformVirtualWidth = 2400;
};
