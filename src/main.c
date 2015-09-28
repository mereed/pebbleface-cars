#include <pebble.h>

#define ANIM_DURATION 3000

static AppSync sync;
static uint8_t sync_buffer[128];

static const uint32_t WEATHER_ICONS[] = {
  RESOURCE_ID_CLEAR_DAY,
  RESOURCE_ID_CLEAR_NIGHT,
  RESOURCE_ID_WINDY,
  RESOURCE_ID_COLD,
  RESOURCE_ID_PARTLY_CLOUDY_DAY,
  RESOURCE_ID_PARTLY_CLOUDY_NIGHT,
  RESOURCE_ID_HAZE,
  RESOURCE_ID_CLOUD,
  RESOURCE_ID_RAIN,
  RESOURCE_ID_SNOW,
  RESOURCE_ID_HAIL,
  RESOURCE_ID_CLOUDY,
  RESOURCE_ID_STORM,
  RESOURCE_ID_FOG,
  RESOURCE_ID_NA,
};

static int bluetoothvibe;
static int hourlyvibe;
static int hidesec;

static bool appStarted = false;

enum WeatherKey {
  WEATHER_ICON_KEY = 0x0,
  BLUETOOTHVIBE_KEY = 0x1,
  HOURLYVIBE_KEY = 0x2,
  WEATHER_TEMPERATURE_KEY = 0x3,
};

TextLayer *time_layer;
TextLayer *ampm_layer;
TextLayer *layer_date_text;
TextLayer *layer_daynum_text;
TextLayer *layer_secs_text;
TextLayer *layer_bt_text;

static GBitmap *background_image;
static BitmapLayer *background_layer;

BitmapLayer *layer_conn_img;
GBitmap *img_bt_connect;
GBitmap *img_bt_disconnect;

Layer* weather_holder;
BitmapLayer *icon_layer;
GBitmap *icon_bitmap = NULL;
TextLayer *temp_layer;

int charge_percent = 0;
TextLayer *battery_text_layer;
static GFont *custom_font;
static GFont *custom_font2;
static GFont *custom_font3;
static GFont *custom_font4;
static GFont *custom_font5;

static Window *window;
static Layer *window_layer;

static PropertyAnimation *animation1, *animation2, *animation3;

BitmapLayer *block1_layer;
GBitmap *block1_bitmap;

BitmapLayer *block2_layer;
GBitmap *block2_bitmap;

BitmapLayer *block2a_layer;
GBitmap *block2a_bitmap;

BitmapLayer *block3_layer;
GBitmap *block3_bitmap;


GBitmap *img_battery_100;
GBitmap *img_battery_90;
GBitmap *img_battery_80;
GBitmap *img_battery_70;
GBitmap *img_battery_60;
GBitmap *img_battery_50;
GBitmap *img_battery_40;
GBitmap *img_battery_30;
GBitmap *img_battery_20;
GBitmap *img_battery_10;
GBitmap *img_battery_charge;
BitmapLayer *layer_batt_img;


static void run_animation() {

  GRect from_frame1 = GRect(-30, 43, 21, 11);
  GRect to_frame1 = GRect(160, 43, 21, 11);
  // Schedule the next animation
  animation1 = property_animation_create_layer_frame(bitmap_layer_get_layer(block1_layer), &from_frame1, &to_frame1);
  animation_set_duration((Animation*)animation1, ANIM_DURATION);
  animation_set_delay((Animation*)animation1, 0);
  animation_set_curve((Animation*)animation1, AnimationCurveEaseIn);
  animation_schedule((Animation*)animation1);

  GRect from_frame2 = GRect(96, 190, 11, 22);
  GRect to_frame2 = GRect(96, -30, 11, 22);
  // Schedule the next animation
  animation2 = property_animation_create_layer_frame(bitmap_layer_get_layer(block2_layer), &from_frame2, &to_frame2);
  animation_set_duration((Animation*)animation2, ANIM_DURATION);
  animation_set_delay((Animation*)animation2, 800);
  animation_set_curve((Animation*)animation2, AnimationCurveEaseIn);
  animation_schedule((Animation*)animation2);



  GRect from_frame3 = GRect(170, 102, 21, 11);
  GRect to_frame3 = GRect(-30, 102, 21, 11);
  // Schedule the next animation
  animation3 = property_animation_create_layer_frame(bitmap_layer_get_layer(block3_layer), &from_frame3, &to_frame3);
  animation_set_duration((Animation*)animation3, ANIM_DURATION);
  animation_set_delay((Animation*)animation3, 2000);
  animation_set_curve((Animation*)animation3, AnimationCurveEaseIn);
  animation_schedule((Animation*)animation3);
	
}

void update_battery_state(BatteryChargeState charge_state) {
    static char battery_text[] = "x100%";

    if (charge_state.is_charging) {
        bitmap_layer_set_bitmap(layer_batt_img, img_battery_charge);
        snprintf(battery_text, sizeof(battery_text), "+%d%%", charge_state.charge_percent);
    } else {
        snprintf(battery_text, sizeof(battery_text), "%d%%", charge_state.charge_percent);       
	
    //    if (charge_state.charge_percent <= 10) {
    //        bitmap_layer_set_bitmap(layer_batt_img, img_battery_10);
      if (charge_state.charge_percent <= 20) {
            bitmap_layer_set_bitmap(layer_batt_img, img_battery_20);
	//	} else if (charge_state.charge_percent <= 30) {
    //        bitmap_layer_set_bitmap(layer_batt_img, img_battery_30);
		} else if (charge_state.charge_percent <= 40) {
            bitmap_layer_set_bitmap(layer_batt_img, img_battery_40);
	//	} else if (charge_state.charge_percent <= 50) {
    //        bitmap_layer_set_bitmap(layer_batt_img, img_battery_50);
        } else if (charge_state.charge_percent <= 60) {
            bitmap_layer_set_bitmap(layer_batt_img, img_battery_60);
	//	} else	if (charge_state.charge_percent <= 70) {
    //        bitmap_layer_set_bitmap(layer_batt_img, img_battery_70);
		} else if (charge_state.charge_percent <= 80) {
            bitmap_layer_set_bitmap(layer_batt_img, img_battery_80);
	//	} else if (charge_state.charge_percent <= 90) {
    //        bitmap_layer_set_bitmap(layer_batt_img, img_battery_90);
		} else if (charge_state.charge_percent <= 100) {
            bitmap_layer_set_bitmap(layer_batt_img, img_battery_100);
		} else {
            bitmap_layer_set_bitmap(layer_batt_img, img_battery_100);
        } 
    } 
    charge_percent = charge_state.charge_percent;   
    text_layer_set_text(battery_text_layer, battery_text);
} 

static void toggle_bluetooth(bool connected) {

if (!connected) {
		static char bt_text[] = "xxxxxx";  
	    snprintf(bt_text, sizeof(bt_text), "NO BT ");
	    text_layer_set_text(layer_bt_text, bt_text);
		bitmap_layer_set_bitmap(layer_conn_img, img_bt_disconnect);
     } else {
		static char bt_text[] = "xxxxxx";
        snprintf(bt_text, sizeof(bt_text), "BT Ok");
        text_layer_set_text(layer_bt_text, bt_text);
		bitmap_layer_set_bitmap(layer_conn_img, img_bt_connect);
	}
	
   if (appStarted && bluetoothvibe) {
        vibes_short_pulse();
	}
}

void bluetooth_connection_callback(bool connected) {
  toggle_bluetooth(connected);
}

void tick_handler(struct tm *t, TimeUnits units){
  static char time_buffer[] = "00:00";
    static char date_text[] = "wednesday";
    static char daynum_text[] = "xxx 00";
    static char secs_text[] = "00";
    static char ampm_text[] = "xx";

   strftime(date_text, sizeof(date_text), "%A", t);
   text_layer_set_text(layer_date_text, date_text);

   strftime(daynum_text, sizeof(daynum_text), "%b %e", t);
   text_layer_set_text(layer_daynum_text, daynum_text);
	
  // strftime(secs_text, sizeof(secs_text), "%S", t);
  // text_layer_set_text(layer_secs_text, secs_text);
	
  if(clock_is_24h_style()){
    strftime(time_buffer, sizeof(time_buffer), "%R", t);
  }

  else{
    strftime(time_buffer, sizeof(time_buffer), "%l:%M", t);
    strftime(ampm_text, sizeof(ampm_text), "%p", t);
	text_layer_set_text(ampm_layer, ampm_text);
  }
  text_layer_set_text(time_layer, time_buffer);
	
  run_animation();

}

static void sync_tuple_changed_callback(const uint32_t key,
                                        const Tuple* new_tuple,
                                        const Tuple* old_tuple,
                                        void* context) {

  // App Sync keeps new_tuple in sync_buffer, so we may use it directly
  switch (key) {
    case WEATHER_ICON_KEY:
      if (icon_bitmap) {
        gbitmap_destroy(icon_bitmap);
      }
      icon_bitmap = gbitmap_create_with_resource(
          WEATHER_ICONS[new_tuple->value->uint8]);
      bitmap_layer_set_bitmap(icon_layer, icon_bitmap);
      break;

   case WEATHER_TEMPERATURE_KEY:
      text_layer_set_text(temp_layer, new_tuple->value->cstring);  
   break;
	
    case BLUETOOTHVIBE_KEY:
      bluetoothvibe = new_tuple->value->uint8 != 0;
	  persist_write_bool(BLUETOOTHVIBE_KEY, bluetoothvibe);
      break;      
	  
    case HOURLYVIBE_KEY:
      hourlyvibe = new_tuple->value->uint8 != 0;
	  persist_write_bool(HOURLYVIBE_KEY, hourlyvibe);	  
      break;	  
  }
}

void force_update(void) {
  update_battery_state(battery_state_service_peek());
  toggle_bluetooth(bluetooth_connection_service_peek());

  struct tm *t;
  time_t temp;        
  temp = time(NULL);        
  t = localtime(&temp);

  tick_handler(t, MINUTE_UNIT);
}

static void main_window_load(Window *window) {
	
  Layer *window_layer = window_get_root_layer(window);

  background_image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BACKGROUND);
  background_layer = bitmap_layer_create(layer_get_frame(window_layer));
  bitmap_layer_set_bitmap(background_layer, background_image);
  layer_add_child(window_layer, bitmap_layer_get_layer(background_layer));
	
  img_battery_100   = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BATT_100);
  //img_battery_90   = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BATT_90);
  img_battery_80   = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BATT_80);
  //img_battery_70   = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BATT_70);
  img_battery_60   = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BATT_60);
  //img_battery_50   = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BATT_50);
  img_battery_40   = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BATT_40);
  //img_battery_30   = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BATT_30);
  img_battery_20    = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BATT_20);
  //img_battery_10    = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BATT_10);
  img_battery_charge = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BATT_CHARGING);

  layer_batt_img  = bitmap_layer_create(GRect(110, 0, 27, 26));
  bitmap_layer_set_bitmap(layer_batt_img, img_battery_100);
  layer_add_child(window_layer, bitmap_layer_get_layer(layer_batt_img));

   img_bt_connect     = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_CONNECT);
    img_bt_disconnect  = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_DISCONNECT);
	
  layer_conn_img  = bitmap_layer_create(GRect(111, 60, 29, 25));
  bitmap_layer_set_bitmap(layer_conn_img, img_bt_connect);
  layer_add_child(window_layer, bitmap_layer_get_layer(layer_conn_img)); 
	
  block1_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BLOCK1);
  block1_layer = bitmap_layer_create(GRect(-30, 43, 21, 11));
  bitmap_layer_set_bitmap(block1_layer, block1_bitmap);
  layer_add_child(window_layer, bitmap_layer_get_layer(block1_layer));	
	
  block2_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BLOCK2);
  block2_layer = bitmap_layer_create(GRect(96, 190, 11, 22));
  bitmap_layer_set_bitmap(block2_layer, block2_bitmap);
  layer_add_child(window_layer, bitmap_layer_get_layer(block2_layer));	
	
  block3_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BLOCK3);
  block3_layer = bitmap_layer_create(GRect(-30, 102, 21, 11));
  bitmap_layer_set_bitmap(block3_layer, block3_bitmap);
  layer_add_child(window_layer, bitmap_layer_get_layer(block3_layer));	
	

  custom_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_CUSTOM_14));
 // custom_font2 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_CUSTOM_18));
 //custom_font3 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_CUSTOM_22));
  custom_font5 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_CUSTOM_18));
  custom_font4 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_CUSTOM_38));

  Layer *weather_holder = layer_create(GRect(0, 0, 144, 168 ));
  layer_add_child(window_layer, weather_holder);

  icon_layer = bitmap_layer_create(GRect(108, 137, 21, 21));
  layer_add_child(weather_holder, bitmap_layer_get_layer(icon_layer));

  temp_layer = text_layer_create(GRect(110, 118, 50, 30));
  text_layer_set_text_color(temp_layer, GColorWhite);
#ifdef PBL_COLOR
 text_layer_set_text_color(temp_layer, GColorYellow  );
#endif	
  text_layer_set_background_color(temp_layer, GColorClear);
  text_layer_set_font(temp_layer, custom_font);
	// text_layer_set_font(temp_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  text_layer_set_text_alignment(temp_layer, GTextAlignmentLeft);
  layer_add_child(weather_holder, text_layer_get_layer(temp_layer));
	

  time_layer = text_layer_create(GRect(0, 47, 90, 40));
  text_layer_set_font(time_layer, custom_font4);
  text_layer_set_text_color(time_layer, GColorWhite);
#ifdef PBL_COLOR
 text_layer_set_text_color(time_layer, GColorYellow);
#endif
  text_layer_set_background_color(time_layer, GColorClear);
  text_layer_set_text_alignment(time_layer, GTextAlignmentRight);
  layer_add_child(window_layer, text_layer_get_layer(time_layer));

  ampm_layer = text_layer_create(GRect(0, 84, 90, 40));
  text_layer_set_font(ampm_layer, custom_font);
  text_layer_set_text_color(ampm_layer, GColorWhite);
#ifdef PBL_COLOR
 text_layer_set_text_color(ampm_layer, GColorYellow);
#endif
  text_layer_set_background_color(ampm_layer, GColorClear);
  text_layer_set_text_alignment(ampm_layer, GTextAlignmentRight);
  layer_add_child(window_layer, text_layer_get_layer(ampm_layer));
	
  layer_date_text = text_layer_create(GRect(0, 20, 90, 30));
  text_layer_set_text_color(layer_date_text, GColorWhite);
#ifdef PBL_COLOR
 text_layer_set_text_color(layer_date_text, GColorYellow);
#endif
  text_layer_set_background_color(layer_date_text, GColorClear);
  text_layer_set_font(layer_date_text, custom_font5);
  text_layer_set_text_alignment(layer_date_text, GTextAlignmentRight);
  layer_add_child(window_layer, text_layer_get_layer(layer_date_text));	

  layer_daynum_text = text_layer_create(GRect(0, 115, 90, 30));
  text_layer_set_text_color(layer_daynum_text, GColorWhite );
#ifdef PBL_COLOR
 text_layer_set_text_color(layer_daynum_text, GColorYellow);
#endif	
  text_layer_set_background_color(layer_daynum_text, GColorClear);
  text_layer_set_font(layer_daynum_text, custom_font5);
  text_layer_set_text_alignment(layer_daynum_text, GTextAlignmentRight);
  layer_add_child(window_layer, text_layer_get_layer(layer_daynum_text));	
/*
  layer_secs_text = text_layer_create(GRect(0, 84, 80, 30));
  text_layer_set_text_color(layer_secs_text, GColorWhite );
#ifdef PBL_COLOR
 text_layer_set_text_color(layer_secs_text, GColorYellow);
#endif	
  text_layer_set_background_color(layer_secs_text, GColorClear);
  text_layer_set_font(layer_secs_text, custom_font);
  text_layer_set_text_alignment(layer_secs_text, GTextAlignmentRight);
  layer_add_child(window_layer, text_layer_get_layer(layer_secs_text));		
	*/
	
  battery_text_layer = text_layer_create(GRect(110, 24, 50, 40));
  text_layer_set_text_color(battery_text_layer, GColorWhite);
#ifdef PBL_COLOR
 text_layer_set_text_color(battery_text_layer, GColorYellow);
#endif
  text_layer_set_background_color(battery_text_layer, GColorClear);
  text_layer_set_font(battery_text_layer, custom_font);
  text_layer_set_text_alignment(battery_text_layer, GTextAlignmentLeft);
  layer_add_child(window_layer, text_layer_get_layer(battery_text_layer));	
	
  layer_bt_text = text_layer_create(GRect(110, 84,50, 40));
  text_layer_set_text_color(layer_bt_text, GColorWhite);
#ifdef PBL_COLOR
 text_layer_set_text_color(layer_bt_text, GColorYellow);
#endif
  text_layer_set_background_color(layer_bt_text, GColorClear);
  text_layer_set_font(layer_bt_text, custom_font);
  text_layer_set_text_alignment(layer_bt_text, GTextAlignmentLeft);
  layer_add_child(window_layer, text_layer_get_layer(layer_bt_text));		

Tuplet initial_values[] = {
    TupletInteger(WEATHER_ICON_KEY, (uint8_t) 14),
    TupletCString(WEATHER_TEMPERATURE_KEY, ""),
    TupletInteger(BLUETOOTHVIBE_KEY, persist_read_bool(BLUETOOTHVIBE_KEY)),
    TupletInteger(HOURLYVIBE_KEY, persist_read_bool(HOURLYVIBE_KEY)),
  };

  app_sync_init(&sync, sync_buffer, sizeof(sync_buffer), initial_values,
                ARRAY_LENGTH(initial_values), sync_tuple_changed_callback,
                NULL, NULL);

  appStarted = true;
  

    // draw first frame
    force_update();

}

static void main_window_unload(Window *window) {
 text_layer_destroy(time_layer);
 text_layer_destroy(ampm_layer);
 text_layer_destroy(layer_daynum_text);
 text_layer_destroy(layer_secs_text);
 text_layer_destroy(layer_date_text);
 text_layer_destroy(battery_text_layer);
 text_layer_destroy( layer_bt_text );

}

static void init(void) {
	
	 // Setup messaging
  const int inbound_size = 128;
  const int outbound_size = 128;
  app_message_open(inbound_size, outbound_size);	
  // Create main Window
  window = window_create();
  window_set_background_color(window, GColorBlack);

  window_set_window_handlers(window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload,
  });
  window_stack_push(window, true);

  battery_state_service_subscribe(&update_battery_state);
  bluetooth_connection_service_subscribe(&toggle_bluetooth);
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
	
  // Start animation
  run_animation();
}

static void deinit(void) {

  // Stop any animation in progress
  animation_unschedule_all();
	
  tick_timer_service_unsubscribe();
  battery_state_service_unsubscribe();
  bluetooth_connection_service_unsubscribe();

  layer_remove_from_parent(bitmap_layer_get_layer(background_layer));
  bitmap_layer_destroy(background_layer);
  gbitmap_destroy(background_image);
  background_image = NULL;
	
  text_layer_destroy(temp_layer);
	
  layer_remove_from_parent(bitmap_layer_get_layer(icon_layer));
  bitmap_layer_destroy(icon_layer);
  gbitmap_destroy(icon_bitmap);
  icon_bitmap = NULL;
	
  layer_remove_from_parent(bitmap_layer_get_layer(layer_conn_img));
  bitmap_layer_destroy(layer_conn_img);
  gbitmap_destroy(img_bt_connect);
  gbitmap_destroy(img_bt_disconnect);
  img_bt_connect = NULL;
  img_bt_disconnect = NULL;

  layer_remove_from_parent(bitmap_layer_get_layer(block1_layer));
  bitmap_layer_destroy(block1_layer);
  gbitmap_destroy(block1_bitmap);
  block1_bitmap = NULL;
	
  layer_remove_from_parent(bitmap_layer_get_layer(block2_layer));
  bitmap_layer_destroy(block2_layer);
  gbitmap_destroy(block2_bitmap);
  block2_bitmap = NULL;  

  layer_remove_from_parent(bitmap_layer_get_layer(block3_layer));
  bitmap_layer_destroy(block3_layer);
  gbitmap_destroy(block3_bitmap);
  block3_bitmap = NULL;  
	
  layer_remove_from_parent(bitmap_layer_get_layer(layer_batt_img));
  bitmap_layer_destroy(layer_batt_img);
	
  gbitmap_destroy(img_battery_100);
  gbitmap_destroy(img_battery_90);
  gbitmap_destroy(img_battery_80);
  gbitmap_destroy(img_battery_70);
  gbitmap_destroy(img_battery_60);
  gbitmap_destroy(img_battery_50);
  gbitmap_destroy(img_battery_40);
  gbitmap_destroy(img_battery_30);
  gbitmap_destroy(img_battery_20);
  gbitmap_destroy(img_battery_10);
  gbitmap_destroy(img_battery_charge);	

  fonts_unload_custom_font(custom_font);
  fonts_unload_custom_font(custom_font2);
  fonts_unload_custom_font(custom_font3);
  fonts_unload_custom_font(custom_font4);
  fonts_unload_custom_font(custom_font5);

  layer_remove_from_parent(window_layer);
  layer_destroy(window_layer);
	
  // Destroy main Window
  window_destroy(window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}