#include <adwaita.h>
#include <gtk/gtk.h>

#include <algorithm>
#include <cmath>
#include <string>
#include <vector>

#include "PrototypeModel.h"

using tenacity::gnome_prototype::ProjectViewModel;
using tenacity::gnome_prototype::TrackViewModel;

namespace {

constexpr double PI = 3.14159265358979323846;

const char *CSS = R"CSS(
.window-root {
    background: @window_bg_color;
}

.header-toolbar {
    padding: 8px 10px;
    border-bottom: 1px solid alpha(@borders, 0.55);
    background: @window_bg_color;
}

.transport-box,
.tool-box,
.device-box,
.project-strip {
    padding: 6px 8px;
    border-radius: 12px;
    background: alpha(@card_bg_color, 0.82);
    border: 1px solid alpha(@borders, 0.50);
}

.editor-background {
    background: @view_bg_color;
}

.track-shell {
    margin: 14px;
    border-radius: 14px;
    border: 1px solid alpha(@borders, 0.60);
    background: @card_bg_color;
}

.track-left-column {
    background: alpha(@view_bg_color, 0.68);
    border-right: 1px solid alpha(@borders, 0.50);
    border-top-left-radius: 14px;
    border-bottom-left-radius: 14px;
}

.track-header {
    padding: 8px 10px;
    border-bottom: 1px solid alpha(@borders, 0.45);
    background: alpha(@card_bg_color, 0.92);
    border-top-left-radius: 14px;
}

.track-control-panel {
    padding: 8px 10px 10px 10px;
    background: transparent;
}

.track-title-pill {
    padding: 3px 9px;
    border-radius: 999px;
    color: @accent_color;
    background: alpha(@accent_bg_color, 0.12);
}

.waveform-scroll {
    background: @view_bg_color;
    border-top-right-radius: 14px;
    border-bottom-right-radius: 14px;
}

.side-deck {
    background: alpha(@card_bg_color, 0.92);
    border-left: 1px solid alpha(@borders, 0.55);
}

.side-tab-rail {
    padding: 8px 6px;
    border-right: 1px solid alpha(@borders, 0.35);
    background: alpha(@view_bg_color, 0.52);
}

.side-deck-body {
    padding: 12px;
}

.property-card {
    padding: 10px;
    border-radius: 12px;
    background: alpha(@view_bg_color, 0.72);
    border: 1px solid alpha(@borders, 0.42);
}

.bottom-status {
    padding: 7px 10px;
    border-top: 1px solid alpha(@borders, 0.55);
    background: @window_bg_color;
}

.status-section {
    padding: 5px 10px;
    border-radius: 10px;
    background: alpha(@card_bg_color, 0.72);
    border: 1px solid alpha(@borders, 0.38);
}

.status-title {
    font-size: 10px;
    font-weight: 700;
    opacity: 0.68;
}

.status-value {
    font-family: monospace;
    font-size: 13px;
}

.heading {
    font-weight: 700;
}

.muted {
    opacity: 0.68;
}

.compact-button {
    padding: 4px 8px;
}
)CSS";

GtkWidget *label(const char *text, const char *css_class = nullptr)
{
    GtkWidget *item = gtk_label_new(text);
    gtk_label_set_xalign(GTK_LABEL(item), 0.0f);

    if (css_class)
        gtk_widget_add_css_class(item, css_class);

    return item;
}

GtkWidget *icon_button(const char *icon_name, const char *tooltip)
{
    GtkWidget *button = gtk_button_new_from_icon_name(icon_name);
    gtk_widget_add_css_class(button, "compact-button");
    gtk_widget_set_tooltip_text(button, tooltip);
    return button;
}

double fake_sample(double x, unsigned seed)
{
    double env = 0.20 + 0.12 * std::sin(2.0 * PI * (1.7 * x + 0.05 + seed * 0.0007));
    env += 0.08 * std::sin(2.0 * PI * (6.5 * x + 0.13));

    if (x > 0.12 && x < 0.28) env *= 1.30;
    if (x > 0.36 && x < 0.58) env *= 1.65;
    if (x > 0.64 && x < 0.83) env *= 1.45;
    if (x > 0.28 && x < 0.36) env *= 0.25;

    const double carrier = std::abs(std::sin(2.0 * PI * (34.0 * x + 0.02 * seed)));
    return std::clamp(env * (0.35 + 0.65 * carrier), 0.03, 0.96);
}

void draw_wave_channel(
    cairo_t *cr,
    int left_pad,
    int usable_w,
    double center,
    double amp,
    unsigned seed)
{
    constexpr int n = 1600;

    cairo_set_source_rgba(cr, 0.03, 0.33, 0.68, 0.72);
    cairo_move_to(cr, left_pad, center);

    for (int i = 0; i < n; ++i) {
        const double x = static_cast<double>(i) / (n - 1);
        const double value = fake_sample(x, seed);
        cairo_line_to(cr, left_pad + usable_w * x, center - value * amp);
    }

    for (int i = n - 1; i >= 0; --i) {
        const double x = static_cast<double>(i) / (n - 1);
        const double value = fake_sample(x, seed);
        cairo_line_to(cr, left_pad + usable_w * x, center + value * amp);
    }

    cairo_close_path(cr);
    cairo_fill(cr);
}

void draw_waveform(GtkDrawingArea *, cairo_t *cr, int width, int height, gpointer user_data)
{
    const auto seed = static_cast<unsigned>(GPOINTER_TO_UINT(user_data));

    const int ruler_h = 38;
    const int left_pad = 54;
    const int right_pad = 14;
    const int bottom_pad = 12;
    const int lane_gap = 10;
    const int usable_w = width - left_pad - right_pad;
    const int usable_h = height - ruler_h - bottom_pad;

    const double channel_h = (usable_h - lane_gap) / 2.0;
    const double left_center = ruler_h + channel_h / 2.0;
    const double right_center = ruler_h + channel_h + lane_gap + channel_h / 2.0;
    const double amp = channel_h * 0.44;

    cairo_set_source_rgb(cr, 0.985, 0.985, 0.975);
    cairo_rectangle(cr, 0, 0, width, height);
    cairo_fill(cr);

    const std::vector<std::string> marks {
        "00:00", "15:00", "30:00", "45:00", "01:00:00", "01:15:00", "01:30:00"
    };

    for (std::size_t i = 0; i < marks.size(); ++i) {
        const double x = left_pad + usable_w * (static_cast<double>(i) / (marks.size() - 1));

        cairo_set_source_rgba(cr, 0.42, 0.42, 0.42, 0.32);
        cairo_move_to(cr, x, 0);
        cairo_line_to(cr, x, height - bottom_pad);
        cairo_stroke(cr);

        cairo_set_source_rgba(cr, 0.10, 0.10, 0.10, 0.78);
        cairo_move_to(cr, x - 18, 20);
        cairo_show_text(cr, marks[i].c_str());
    }

    cairo_set_source_rgba(cr, 0.42, 0.42, 0.42, 0.28);
    cairo_move_to(cr, left_pad, left_center);
    cairo_line_to(cr, width - right_pad, left_center);
    cairo_move_to(cr, left_pad, right_center);
    cairo_line_to(cr, width - right_pad, right_center);
    cairo_stroke(cr);

    draw_wave_channel(cr, left_pad, usable_w, left_center, amp, seed + 101);
    draw_wave_channel(cr, left_pad, usable_w, right_center, amp, seed + 202);

    const double play_x = left_pad + 2;
    cairo_set_source_rgba(cr, 0.10, 0.42, 0.82, 0.92);
    cairo_set_line_width(cr, 2.0);
    cairo_move_to(cr, play_x, ruler_h);
    cairo_line_to(cr, play_x, height - bottom_pad);
    cairo_stroke(cr);
}

GtkWidget *make_scale(double min, double max, double value)
{
    GtkWidget *scale = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, min, max, 0.01);
    gtk_range_set_value(GTK_RANGE(scale), value);
    gtk_scale_set_draw_value(GTK_SCALE(scale), FALSE);
    gtk_widget_set_hexpand(scale, TRUE);
    return scale;
}

GtkWidget *make_track_controls()
{
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 7);
    gtk_widget_add_css_class(box, "track-control-panel");
    gtk_widget_set_size_request(box, 218, 200);

    gtk_box_append(GTK_BOX(box), label("Stereo · 44.1 kHz", "muted"));
    gtk_box_append(GTK_BOX(box), label("32-bit float", "muted"));

    GtkWidget *buttons = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    GtkWidget *mute = gtk_toggle_button_new_with_label("Mute");
    GtkWidget *solo = gtk_toggle_button_new_with_label("Solo");
    gtk_widget_set_hexpand(mute, TRUE);
    gtk_widget_set_hexpand(solo, TRUE);
    gtk_box_append(GTK_BOX(buttons), mute);
    gtk_box_append(GTK_BOX(buttons), solo);
    gtk_box_append(GTK_BOX(box), buttons);

    gtk_box_append(GTK_BOX(box), label("Gain"));
    gtk_box_append(GTK_BOX(box), make_scale(-36.0, 36.0, 0.0));

    gtk_box_append(GTK_BOX(box), label("Pan"));
    gtk_box_append(GTK_BOX(box), make_scale(-1.0, 1.0, 0.0));

    GtkWidget *frame = gtk_frame_new(nullptr);
    GtkWidget *channels = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
    gtk_widget_set_margin_top(channels, 6);
    gtk_widget_set_margin_bottom(channels, 6);
    gtk_widget_set_margin_start(channels, 7);
    gtk_widget_set_margin_end(channels, 7);
    gtk_box_append(GTK_BOX(channels), label("L: active", "muted"));
    gtk_box_append(GTK_BOX(channels), label("R: active", "muted"));
    gtk_frame_set_child(GTK_FRAME(frame), channels);
    gtk_box_append(GTK_BOX(box), frame);

    return box;
}

GtkWidget *make_track_lane(
    const TrackViewModel& track,
    GtkAdjustment *shared_adjustment)
{
    GtkWidget *shell = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_add_css_class(shell, "track-shell");
    gtk_widget_set_hexpand(shell, TRUE);
    gtk_widget_set_vexpand(shell, FALSE);
    gtk_widget_set_size_request(shell, -1, 248);

    GtkWidget *left = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_add_css_class(left, "track-left-column");
    gtk_widget_set_size_request(left, 218, 248);
    gtk_box_append(GTK_BOX(shell), left);

    GtkWidget *header = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
    gtk_widget_add_css_class(header, "track-header");
    gtk_widget_set_size_request(header, 218, 48);
    gtk_box_append(GTK_BOX(left), header);

    GtkWidget *header_top = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    gtk_box_append(GTK_BOX(header), header_top);

    std::string title = "Track " + std::to_string(track.number);
    GtkWidget *pill = gtk_label_new(title.c_str());
    gtk_widget_add_css_class(pill, "track-title-pill");
    gtk_box_append(GTK_BOX(header_top), pill);
    gtk_box_append(GTK_BOX(header_top), icon_button("view-more-symbolic", "Track options"));

    GtkWidget *name = gtk_label_new(track.name.c_str());
    gtk_label_set_xalign(GTK_LABEL(name), 0.0f);
    gtk_label_set_ellipsize(GTK_LABEL(name), PANGO_ELLIPSIZE_END);
    gtk_widget_add_css_class(name, "muted");
    gtk_box_append(GTK_BOX(header), name);

    gtk_box_append(GTK_BOX(left), make_track_controls());

    GtkWidget *wave_scroll = gtk_scrolled_window_new();
    gtk_widget_add_css_class(wave_scroll, "waveform-scroll");
    gtk_widget_set_hexpand(wave_scroll, TRUE);
    gtk_widget_set_vexpand(wave_scroll, FALSE);
    gtk_widget_set_size_request(wave_scroll, -1, 248);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(wave_scroll), GTK_POLICY_AUTOMATIC, GTK_POLICY_NEVER);
    gtk_scrolled_window_set_hadjustment(GTK_SCROLLED_WINDOW(wave_scroll), shared_adjustment);

    GtkWidget *wave = gtk_drawing_area_new();
    gtk_widget_set_size_request(wave, 2400, 220);
gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(wave), draw_waveform, GUINT_TO_POINTER(track.waveformSeed), nullptr);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(wave_scroll), wave);
    gtk_box_append(GTK_BOX(shell), wave_scroll);

    return shell;
}

struct SidePanelState {
    GtkWidget *deck {};
    GtkWidget *panel {};
};

void on_side_panel_toggled(GtkToggleButton *button, gpointer user_data)
{
    auto *state = static_cast<SidePanelState *>(user_data);
    const gboolean expanded = gtk_toggle_button_get_active(button);

    gtk_widget_set_visible(state->panel, expanded);
    gtk_widget_set_size_request(state->deck, expanded ? 200 : 44, -1);
}

GtkWidget *make_side_deck()
{
    GtkWidget *deck = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_add_css_class(deck, "side-deck");
    gtk_widget_set_size_request(deck, 200, -1);
    gtk_widget_set_hexpand(deck, FALSE);
    gtk_widget_set_vexpand(deck, TRUE);

    GtkWidget *rail = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    gtk_widget_add_css_class(rail, "side-tab-rail");
    gtk_box_append(GTK_BOX(deck), rail);

    gtk_box_append(GTK_BOX(rail), icon_button("document-properties-symbolic", "Properties"));
    gtk_box_append(GTK_BOX(rail), icon_button("audio-card-symbolic", "Audio"));
    gtk_box_append(GTK_BOX(rail), icon_button("applications-graphics-symbolic", "Effects"));
    gtk_box_append(GTK_BOX(rail), icon_button("bookmark-new-symbolic", "Markers"));

    GtkWidget *panel = gtk_box_new(GTK_ORIENTATION_VERTICAL, 12);
    gtk_widget_add_css_class(panel, "side-deck-body");
    gtk_widget_set_size_request(panel, 156, -1);
    gtk_box_append(GTK_BOX(deck), panel);

    gtk_box_append(GTK_BOX(panel), label("Properties", "heading"));

    GtkWidget *card = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_widget_add_css_class(card, "property-card");
    gtk_box_append(GTK_BOX(card), label("Selected Track", "heading"));
    gtk_box_append(GTK_BOX(card), label("Name: 16ABR_SAMPLE_PART1", "muted"));
    gtk_box_append(GTK_BOX(card), label("Type: stereo", "muted"));
    gtk_box_append(GTK_BOX(card), label("Rate: 44.1 kHz", "muted"));
    gtk_box_append(GTK_BOX(card), label("Format: 32-bit float", "muted"));
    gtk_box_append(GTK_BOX(panel), card);

    g_object_set_data(G_OBJECT(deck), "side-panel", panel);
    return deck;
}

GtkWidget *make_status_section(const char *title, const char *value, bool expand = false)
{
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 1);
    gtk_widget_add_css_class(box, "status-section");
    gtk_widget_set_hexpand(box, expand);

    GtkWidget *title_label = gtk_label_new(title);
    gtk_label_set_xalign(GTK_LABEL(title_label), 0.0f);
    gtk_widget_add_css_class(title_label, "status-title");

    GtkWidget *value_label = gtk_label_new(value);
    gtk_label_set_xalign(GTK_LABEL(value_label), 0.0f);
    gtk_label_set_ellipsize(GTK_LABEL(value_label), PANGO_ELLIPSIZE_END);
    gtk_widget_add_css_class(value_label, "status-value");

    gtk_box_append(GTK_BOX(box), title_label);
    gtk_box_append(GTK_BOX(box), value_label);
    return box;
}

GtkWidget *make_status_bar()
{
    GtkWidget *status = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    gtk_widget_add_css_class(status, "bottom-status");

    gtk_box_append(GTK_BOX(status), make_status_section(
        "SELECTION",
        "Start 00:00:00.000   End 00:00:00.000   Length 00:00:00.000",
        true));

    gtk_box_append(GTK_BOX(status), make_status_section("CURSOR", "00:00:00.000"));
    gtk_box_append(GTK_BOX(status), make_status_section("ZOOM", "100% · fit off"));
    gtk_box_append(GTK_BOX(status), make_status_section("PROJECT", "44,100 Hz · stereo · 32-bit float"));
    gtk_box_append(GTK_BOX(status), make_status_section("AUDIO I/O", "PipeWire/JACK · default in/out"));
    gtk_box_append(GTK_BOX(status), make_status_section("LOAD", "CPU 03% · RT safe"));

    return status;
}

void load_css()
{
    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_css_provider_load_from_string(provider, CSS);

    GdkDisplay *display = gdk_display_get_default();
    gtk_style_context_add_provider_for_display(
        display,
        GTK_STYLE_PROVIDER(provider),
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    g_object_unref(provider);
}

void activate(GtkApplication *application, gpointer)
{
   const ProjectViewModel project_model = tenacity::gnome_prototype::MakeMockProjectViewModel();

    GtkAdjustment *shared_adjustment =
        gtk_adjustment_new(0.0, 0.0, 2400.0, 24.0, 240.0, 800.0);

    GtkWidget *window = adw_application_window_new(GTK_APPLICATION(application));
    gtk_window_set_title(GTK_WINDOW(window), "Tenacity GNOME Prototype");
    gtk_window_set_default_size(GTK_WINDOW(window), 1320, 760);
    gtk_widget_set_size_request(window, 980, 560);

    GtkWidget *toolbar_view = adw_toolbar_view_new();
    adw_application_window_set_content(ADW_APPLICATION_WINDOW(window), toolbar_view);

    GtkWidget *header = adw_header_bar_new();
    GtkWidget *title = gtk_label_new("Tenacity");
    gtk_widget_add_css_class(title, "heading");
    adw_header_bar_set_title_widget(ADW_HEADER_BAR(header), title);
    adw_toolbar_view_add_top_bar(ADW_TOOLBAR_VIEW(toolbar_view), header);

    GtkWidget *root = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_add_css_class(root, "window-root");
    adw_toolbar_view_set_content(ADW_TOOLBAR_VIEW(toolbar_view), root);

    GtkWidget *top = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 9);
    gtk_widget_add_css_class(top, "header-toolbar");
    gtk_box_append(GTK_BOX(root), top);

    GtkWidget *transport = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_widget_add_css_class(transport, "transport-box");
    gtk_box_append(GTK_BOX(transport), icon_button("media-skip-backward-symbolic", "Rewind"));
    gtk_box_append(GTK_BOX(transport), icon_button("media-playback-start-symbolic", "Play"));
    gtk_box_append(GTK_BOX(transport), icon_button("media-playback-pause-symbolic", "Pause"));
    gtk_box_append(GTK_BOX(transport), icon_button("media-playback-stop-symbolic", "Stop"));
    gtk_box_append(GTK_BOX(transport), icon_button("media-record-symbolic", "Record"));
    gtk_box_append(GTK_BOX(top), transport);

    GtkWidget *tools = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_widget_add_css_class(tools, "tool-box");
    gtk_box_append(GTK_BOX(tools), icon_button("edit-cut-symbolic", "Cut"));
    gtk_box_append(GTK_BOX(tools), icon_button("edit-copy-symbolic", "Copy"));
    gtk_box_append(GTK_BOX(tools), icon_button("edit-paste-symbolic", "Paste"));
    gtk_box_append(GTK_BOX(tools), icon_button("edit-undo-symbolic", "Undo"));
    gtk_box_append(GTK_BOX(tools), icon_button("edit-redo-symbolic", "Redo"));
    gtk_box_append(GTK_BOX(tools), icon_button("zoom-in-symbolic", "Zoom In"));
    gtk_box_append(GTK_BOX(tools), icon_button("zoom-out-symbolic", "Zoom Out"));
    gtk_box_append(GTK_BOX(top), tools);

    GtkWidget *project = gtk_box_new(GTK_ORIENTATION_VERTICAL, 1);
    gtk_widget_add_css_class(project, "project-strip");
    gtk_widget_set_hexpand(project, TRUE);
    gtk_box_append(GTK_BOX(project), label(project_model.title.c_str(), "heading"));
    gtk_box_append(GTK_BOX(project), label(project_model.subtitle.c_str(), "muted"));

    gtk_box_append(GTK_BOX(top), project);

    GtkWidget *devices = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    gtk_widget_add_css_class(devices, "device-box");
    gtk_box_append(GTK_BOX(devices), gtk_image_new_from_icon_name("audio-card-symbolic"));
    gtk_box_append(GTK_BOX(devices), gtk_label_new(project_model.audioHost.c_str()));    
    gtk_box_append(GTK_BOX(devices), gtk_separator_new(GTK_ORIENTATION_VERTICAL));
    gtk_box_append(GTK_BOX(devices), gtk_image_new_from_icon_name("audio-input-microphone-symbolic"));
    gtk_box_append(GTK_BOX(devices), gtk_label_new(project_model.inputDevice.c_str()));    
    gtk_box_append(GTK_BOX(top), devices);

    GtkWidget *work_area = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_hexpand(work_area, TRUE);
    gtk_widget_set_vexpand(work_area, TRUE);
    gtk_box_append(GTK_BOX(root), work_area);

    GtkWidget *editor_scroll = gtk_scrolled_window_new();
    gtk_widget_add_css_class(editor_scroll, "editor-background");
    gtk_widget_set_hexpand(editor_scroll, TRUE);
    gtk_widget_set_vexpand(editor_scroll, TRUE);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(editor_scroll), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
    gtk_box_append(GTK_BOX(work_area), editor_scroll);

    GtkWidget *tracks_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_hexpand(tracks_box, TRUE);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(editor_scroll), tracks_box);
    
    for (const auto& track : project_model.tracks) {
        gtk_box_append(GTK_BOX(tracks_box), make_track_lane(track, shared_adjustment));
    }

    GtkWidget *add_track = gtk_button_new_with_label("Add Track");
    gtk_button_set_icon_name(GTK_BUTTON(add_track), "list-add-symbolic");
    gtk_widget_set_halign(add_track, GTK_ALIGN_START);
    gtk_widget_set_margin_start(add_track, 14);
    gtk_widget_set_margin_bottom(add_track, 14);
    gtk_box_append(GTK_BOX(tracks_box), add_track);

    GtkWidget *side_deck = make_side_deck();
    gtk_box_append(GTK_BOX(work_area), side_deck);

    GtkWidget *side_toggle = gtk_toggle_button_new_with_label("Side Panel");
    gtk_button_set_icon_name(GTK_BUTTON(side_toggle), "sidebar-show-right-symbolic");
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(side_toggle), TRUE);
    gtk_widget_set_tooltip_text(side_toggle, "Show or collapse track properties");
    gtk_box_append(GTK_BOX(top), side_toggle);

    auto *side_state = new SidePanelState {
        side_deck,
        GTK_WIDGET(g_object_get_data(G_OBJECT(side_deck), "side-panel"))
    };

    g_signal_connect(side_toggle, "toggled", G_CALLBACK(on_side_panel_toggled), side_state);

    gtk_box_append(GTK_BOX(root), make_status_bar());

    gtk_window_present(GTK_WINDOW(window));
}

} // namespace

int main(int argc, char **argv)
{
    g_autoptr(AdwApplication) app =
        adw_application_new("br.dev.formalis.TenacityGnomePrototype", G_APPLICATION_DEFAULT_FLAGS);

    g_signal_connect(app, "startup", G_CALLBACK(load_css), nullptr);
    g_signal_connect(app, "activate", G_CALLBACK(activate), nullptr);

    return g_application_run(G_APPLICATION(app), argc, argv);
}

