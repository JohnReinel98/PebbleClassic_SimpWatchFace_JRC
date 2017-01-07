#include <pebble.h>
static Window *s_main_window;
static TextLayer *s_time_layer;
static TextLayer *s_date_layer;
static TextLayer *s_day_layer;
static TextLayer *s_ampm_layer;
static TextLayer *s_developer_layer;
static BitmapLayer *s_background_layer;
static GFont s_time_font;
static GFont s_date_font;
static GFont s_day_font;
static GFont s_ampm_font;
static int s_battery_level;
static Layer *s_battery_layer;

static void battery_update_proc(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(layer);

  // Find the width of the bar (total width = 114px)
  int width = (s_battery_level * 114) / 100;

  // Draw the background
  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_fill_rect(ctx, bounds, 0, GCornerNone);

  // Draw the bar
  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_fill_rect(ctx, GRect(0, 0, width, bounds.size.h), 0, GCornerNone);
}
static void main_window_load(Window *window) {
  // Get information about the Window
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  // Create battery meter Layer
  s_battery_layer = layer_create(GRect(17, 15, 115, 2));
  layer_set_update_proc(s_battery_layer, battery_update_proc);
  // Create the TextLayer with specific bounds
  //s_time_layer = text_layer_create(GRect(0, PBL_IF_ROUND_ELSE(58, 52), bounds.size.w, 50));
  window_set_background_color(s_main_window, GColorBlack);
  s_time_layer = text_layer_create(GRect(0,10,144,50));
  s_date_layer = text_layer_create(GRect(0,105,144,37));
  s_day_layer = text_layer_create(GRect(0,83,144,20));
  s_ampm_layer = text_layer_create(GRect(0,61,144,20));
  s_developer_layer = text_layer_create(GRect(0,150,144,20));
  s_background_layer = bitmap_layer_create(bounds);
  // Improve the layout to be more like a watchface
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, GColorWhite);
  text_layer_set_background_color(s_developer_layer, GColorClear);
  text_layer_set_text_color(s_developer_layer, GColorWhite);
  text_layer_set_background_color(s_day_layer, GColorClear);
  text_layer_set_text_color(s_day_layer, GColorWhite);
  text_layer_set_background_color(s_ampm_layer, GColorClear);
  text_layer_set_text_color(s_ampm_layer, GColorWhite);
  text_layer_set_text(s_time_layer, "00:00");
  text_layer_set_text(s_developer_layer, "© John Reinel Caluag");
  text_layer_set_font(s_developer_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD));
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
  text_layer_set_text_alignment(s_developer_layer, GTextAlignmentCenter);
  text_layer_set_text_alignment(s_date_layer, GTextAlignmentCenter);
  text_layer_set_text_alignment(s_day_layer, GTextAlignmentCenter);
  text_layer_set_text_alignment(s_ampm_layer, GTextAlignmentCenter);
  
  // Create GFont
  s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_DATA_CONTROL_48));
  s_date_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_DATA_CONTROL_26));
  s_day_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_SEMI_CODER_18));
  s_ampm_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_SEMI_CODER_18));
  
  // Apply to TextLayer
  text_layer_set_font(s_time_layer, s_time_font);
  text_layer_set_font(s_date_layer, s_date_font);
  text_layer_set_font(s_day_layer, s_day_font);
  text_layer_set_font(s_ampm_layer, s_ampm_font);
  
  // Add it as a child layer to the Window's root layer
  layer_add_child(window_layer, text_layer_get_layer(s_time_layer));
  layer_add_child(window_layer, text_layer_get_layer(s_day_layer));
  layer_add_child(window_layer, text_layer_get_layer(s_date_layer));
  layer_add_child(window_layer, text_layer_get_layer(s_ampm_layer));
  layer_add_child(window_layer, text_layer_get_layer(s_developer_layer));
  layer_add_child(window_get_root_layer(window), s_battery_layer);
  layer_add_child(window_layer, bitmap_layer_get_layer(s_background_layer));
}

static void main_window_unload(Window *window) {
  // Destroy TextLayer
  text_layer_destroy(s_time_layer);
  text_layer_destroy(s_day_layer);
  text_layer_destroy(s_date_layer);
  text_layer_destroy(s_ampm_layer);
  text_layer_destroy(s_developer_layer);
  layer_destroy(s_battery_layer);
  bitmap_layer_destroy(s_background_layer);
  fonts_unload_custom_font(s_time_font);
}
static void battery_callback(BatteryChargeState state) {
  // Record the new battery level
  s_battery_level = state.charge_percent;
  // Update meter
  layer_mark_dirty(s_battery_layer);
}
static void update_time() {
  // Get a tm structure
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);

  // Write the current hours and minutes into a buffer
  static char s_buffer[8];
  static char date_buffer[10];
  static char day_buffer[10];
  static char ampm_buffer[10];
  
  //strftime(s_buffer, sizeof(s_buffer), clock_is_24h_style() ? "%H:%M" : "%I:%M", tick_time); //24hr format
  strftime(s_buffer, sizeof(s_buffer), "%I:%M", tick_time);
  strftime(date_buffer, sizeof(date_buffer), "%D", tick_time);
  strftime(day_buffer, sizeof(day_buffer), "%A", tick_time);
  strftime(ampm_buffer, sizeof(ampm_buffer), "%p", tick_time);
  
  // Display this time on the TextLayer
  text_layer_set_text(s_time_layer, s_buffer);
  text_layer_set_text(s_date_layer, date_buffer);
  text_layer_set_text(s_day_layer, day_buffer);
  text_layer_set_text(s_ampm_layer, ampm_buffer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}

static void init() {
  // Create main Window element and assign to pointer
  s_main_window = window_create();
  
  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  // Show the Window on the watch, with animated=true
  window_stack_push(s_main_window, true);
  // Register with TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  // Register for battery level updates
  battery_state_service_subscribe(battery_callback);
  battery_callback(battery_state_service_peek());
}

static void deinit() {
  // Destroy Window
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}