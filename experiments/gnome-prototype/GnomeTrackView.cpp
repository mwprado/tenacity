#include "GnomeTrackView.h"

#include <cmath>

GnomeTrackView::GnomeTrackView()
{
    m_root = buildRoot();
}

GtkWidget* GnomeTrackView::widget() const
{
    return m_root;
}

GtkWidget* GnomeTrackView::buildRoot()
{
    auto* root = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);

    gtk_widget_set_hexpand(root, TRUE);
    gtk_widget_set_vexpand(root, FALSE);
    gtk_widget_set_size_request(root, -1, kTrackHeight);

    m_header = buildHeader();
    m_waveformScroller = buildWaveformScroller();

    gtk_box_append(GTK_BOX(root), m_header);
    gtk_box_append(GTK_BOX(root), m_waveformScroller);

    return root;
}

GtkWidget* GnomeTrackView::buildHeader()
{
    auto* header = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);

    gtk_widget_set_size_request(header, kHeaderWidth, -1);
    gtk_widget_set_hexpand(header, FALSE);
    gtk_widget_set_vexpand(header, TRUE);
    gtk_widget_set_margin_top(header, 6);
    gtk_widget_set_margin_bottom(header, 6);
    gtk_widget_set_margin_start(header, 6);
    gtk_widget_set_margin_end(header, 6);

    auto* title = gtk_label_new("Audio Track");
    gtk_widget_set_halign(title, GTK_ALIGN_START);

    auto* buttons = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);

    auto* muteButton = gtk_button_new_with_label("M");
    auto* soloButton = gtk_button_new_with_label("S");

    gtk_box_append(GTK_BOX(buttons), muteButton);
    gtk_box_append(GTK_BOX(buttons), soloButton);

    auto* gainLabel = gtk_label_new("Gain");
    gtk_widget_set_halign(gainLabel, GTK_ALIGN_START);

    auto* panLabel = gtk_label_new("Pan");
    gtk_widget_set_halign(panLabel, GTK_ALIGN_START);

    gtk_box_append(GTK_BOX(header), title);
    gtk_box_append(GTK_BOX(header), buttons);
    gtk_box_append(GTK_BOX(header), gainLabel);
    gtk_box_append(GTK_BOX(header), panLabel);

    return header;
}

GtkWidget* GnomeTrackView::buildWaveformScroller()
{
    auto* scroller = gtk_scrolled_window_new();

    gtk_widget_set_hexpand(scroller, TRUE);
    gtk_widget_set_vexpand(scroller, TRUE);

    gtk_scrolled_window_set_policy(
        GTK_SCROLLED_WINDOW(scroller),
        GTK_POLICY_AUTOMATIC,
        GTK_POLICY_NEVER
    );

    m_waveformArea = buildWaveformArea();

    gtk_scrolled_window_set_child(
        GTK_SCROLLED_WINDOW(scroller),
        m_waveformArea
    );

    return scroller;
}

GtkWidget* GnomeTrackView::buildWaveformArea()
{
    auto* area = gtk_drawing_area_new();

    gtk_widget_set_size_request(area, kWaveformVirtualWidth, kTrackHeight);
    gtk_widget_set_hexpand(area, TRUE);
    gtk_widget_set_vexpand(area, TRUE);

    gtk_drawing_area_set_draw_func(
        GTK_DRAWING_AREA(area),
        &GnomeTrackView::drawWaveform,
        this,
        nullptr
    );

    return area;
}

void GnomeTrackView::drawWaveform(
    GtkDrawingArea*,
    cairo_t* cr,
    int width,
    int height,
    gpointer
)
{
    const double mid = height / 2.0;
    const double amplitude = height * 0.32;

    cairo_set_line_width(cr, 1.0);

    cairo_move_to(cr, 0.0, mid);

    for (int x = 0; x < width; ++x) {
        const double t = static_cast<double>(x) / 24.0;
        const double y = mid + std::sin(t) * amplitude * std::sin(t / 9.0);
        cairo_line_to(cr, static_cast<double>(x), y);
    }

    cairo_stroke(cr);
}
