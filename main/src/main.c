
/**
 * @file main
 *
 */

/*********************
 *      INCLUDES
 *********************/
#define _DEFAULT_SOURCE /* needed for usleep() */
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include "lvgl/lvgl.h"
#include "lvgl/examples/lv_examples.h"
#include "lvgl/demos/lv_demos.h"

/*********************
 *      DEFINES
 *********************/


/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static lv_display_t * hal_init(int32_t w, int32_t h);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_obj_t * scr;
static lv_obj_t * combat_menu;

static lv_obj_t * stat_page;
static lv_obj_t * stat_section;
static lv_obj_t * stat_cont;
static lv_obj_t * entry;
static lv_obj_t * btn;


static lv_obj_t * sub_combatPage;
static lv_obj_t * combat_section;
static lv_obj_t * combat_cont;
static int16_t combat_col = 4;
static int16_t combat_row;
static lv_obj_t * combat_win;

static lv_obj_t * label;
static uint16_t num_combat = 0;


/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/


/**********************
 *      VARIABLES
 **********************/
int monitor_hor_res = 800, monitor_ver_res = 480;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void combat_screen(void);
static void create_grid(int16_t col, int16_t row);
static void combatant(void);

static void get_number(lv_event_t * e);

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

int main(int argc, char **argv)
{
  (void)argc; /*Unused*/
  (void)argv; /*Unused*/

  /*Initialize LVGL*/
  lv_init();

  /*Initialize the HAL (display, input devices, tick) for LVGL*/
  hal_init(monitor_hor_res, monitor_ver_res);

  // lv_demo_widgets();
  combat_screen();

  while(1) {
    /* Periodically call the lv_task handler.
     * It could be done in a timer interrupt or an OS task too.*/
    lv_timer_handler();
    usleep(5 * 1000);
  }

  return 0;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Initialize the Hardware Abstraction Layer (HAL) for the LVGL graphics
 * library
 */
static lv_display_t * hal_init(int32_t w, int32_t h)
{

  lv_group_set_default(lv_group_create());

  lv_display_t * disp = lv_sdl_window_create(w, h);

  lv_indev_t * mouse = lv_sdl_mouse_create();
  lv_indev_set_group(mouse, lv_group_get_default());
  lv_indev_set_display(mouse, disp);
  lv_display_set_default(disp);

  LV_IMAGE_DECLARE(mouse_cursor_icon); /*Declare the image file.*/
  lv_obj_t * cursor_obj;
  cursor_obj = lv_image_create(lv_screen_active()); /*Create an image object for the cursor */
  lv_image_set_src(cursor_obj, &mouse_cursor_icon);           /*Set the image source*/
  lv_indev_set_cursor(mouse, cursor_obj);             /*Connect the image  object to the driver*/

  lv_indev_t * mousewheel = lv_sdl_mousewheel_create();
  lv_indev_set_display(mousewheel, disp);
  lv_indev_set_group(mousewheel, lv_group_get_default());

  lv_indev_t * kb = lv_sdl_keyboard_create();
  lv_indev_set_display(kb, disp);
  lv_indev_set_group(kb, lv_group_get_default());

  return disp;
}

static void combat_screen(void)
{

  combat_menu = lv_menu_create(lv_screen_active());
  lv_obj_set_size(combat_menu, lv_display_get_horizontal_resolution(NULL), lv_display_get_vertical_resolution(NULL));
  lv_obj_center(combat_menu);

  // stat_cont
  // stat_cont

  stat_page = lv_menu_page_create(combat_menu, NULL);
  lv_menu_set_page(combat_menu, stat_page);
  entry = lv_textarea_create(stat_page);
  lv_textarea_set_one_line(entry, true);
  btn = lv_btn_create(stat_page);
  lv_obj_add_event_cb(btn, get_number, LV_EVENT_ALL, entry);

  combat_row = 4; // Get from user on input screen
  // combat_col = 4 is always predefined for fields in combatant window

  // create_grid(stat_col, stat_row);

  sub_combatPage = lv_menu_page_create(combat_menu, "Combatants");
  lv_obj_set_style_pad_hor(sub_combatPage, lv_obj_get_style_pad_left(lv_menu_get_main_header(combat_menu), 0), 0);
  lv_menu_set_sidebar_page(combat_menu, sub_combatPage);
  lv_menu_separator_create(sub_combatPage);
  lv_menu_cont_create(sub_combatPage);

}

static void create_grid(int16_t col, int16_t row)
{
  // lv_obj_set_layout(stat_page, LV_LAYOUT_GRID);
}

static void get_number(lv_event_t * e)
{
  lv_event_code_t code = lv_event_get_code(e);
  lv_obj_t * obj = lv_event_get_target(e);
  lv_obj_t * ta = lv_event_get_user_data(e);

  if(code == LV_EVENT_CLICKED){
    num_combat = atoi(lv_textarea_get_text(ta));
    for(int i = 0; i < num_combat; i++){
      combatant();
    }
  }
  return;
}

static void combatant(void)
{
  combat_cont = lv_menu_cont_create(sub_combatPage);
  lv_obj_set_layout(combat_cont, LV_LAYOUT_FLEX);
  lv_obj_set_flex_flow(combat_cont, LV_FLEX_FLOW_ROW);

  label = lv_label_create(combat_cont);
  lv_label_set_text(label, LV_SYMBOL_CUT);

  btn = lv_btn_create(combat_cont);
  lv_obj_set_size(btn, 75, 30);

  label = lv_label_create(btn);
  lv_label_set_text(label, " Ogre");
  lv_obj_center(label);

  entry = lv_textarea_create(combat_cont);
  lv_textarea_set_one_line(entry, true);
  lv_textarea_set_max_length(entry, 3);
  lv_obj_set_size(entry, 50, 50);

  label = lv_label_create(combat_cont);
  lv_label_set_text(label, "/32");

  return;
}
