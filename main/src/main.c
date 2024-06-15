
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
enum {
    LV_MENU_ITEM_BUILDER_VARIANT_1,
    LV_MENU_ITEM_BUILDER_VARIANT_2
};

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
typedef uint8_t lv_menu_builder_variant_t;

/**********************
 *      VARIABLES
 **********************/
int monitor_hor_res = 800, monitor_ver_res = 480;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void combat_screen(void);
static lv_obj_t * create_text(lv_obj_t * parent, const char * icon, const char * txt, lv_menu_builder_variant_t builder_variant);
static lv_obj_t * create_slider(lv_obj_t * parent, const char * icon, const char * txt, int32_t min, int32_t max, int32_t val);

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
  // lv_screen_load(scr);

  combat_menu = lv_menu_create(lv_screen_active());
  lv_obj_set_size(combat_menu, lv_display_get_horizontal_resolution(NULL), lv_display_get_vertical_resolution(NULL));
  lv_obj_center(combat_menu);

  lv_obj_t * section;
  lv_obj_t * cont;
  lv_obj_t * label;

  /***** START SAMPLE CODE *****/
  /*Create a sub page*/
  /*lv_obj_t * sub_page = lv_menu_page_create(combat_menu, NULL);

  cont = lv_menu_cont_create(sub_page);
  label = lv_label_create(cont);
  lv_label_set_text(label, "Hello, I am hiding here");*/

  /*Create a main page*/
 /* lv_obj_t * main_page = lv_menu_page_create(combat_menu, NULL);

  cont = lv_menu_cont_create(main_page);
  label = lv_label_create(cont);
  lv_label_set_text(label, "Item 1");

  cont = lv_menu_cont_create(main_page);
  label = lv_label_create(cont);
  lv_label_set_text(label, "Item 2");

  cont = lv_menu_cont_create(main_page);
  label = lv_label_create(cont);
  lv_label_set_text(label, "Item 3 (Click me!)");
  lv_menu_set_load_page_event(combat_menu, cont, sub_page);

  lv_menu_set_page(combat_menu, main_page);*/

  /***** END SAMPLE CODE *****/

  lv_obj_t * main_page = lv_menu_page_create(combat_menu, NULL);
  lv_menu_set_page(combat_menu, main_page);


  lv_obj_t * sub_statPage = lv_menu_page_create(combat_menu, "Combatants");
  lv_obj_set_style_pad_hor(sub_statPage, lv_obj_get_style_pad_left(lv_menu_get_main_header(combat_menu), 0), 0);
  lv_menu_set_sidebar_page(combat_menu, sub_statPage);
  lv_menu_separator_create(main_page);
  lv_menu_cont_create(main_page);
  section = lv_menu_section_create(sub_statPage);
  create_slider(section, LV_SYMBOL_SETTINGS, "Velocity", 0, 150, 120);
  create_slider(section, LV_SYMBOL_SETTINGS, "Acceleration", 0, 150, 50);
  create_slider(section, LV_SYMBOL_SETTINGS, "Weight limit", 0, 150, 80);
}

static lv_obj_t * create_text(lv_obj_t * parent, const char * icon, const char * txt,
                              lv_menu_builder_variant_t builder_variant)
{
    lv_obj_t * obj = lv_menu_cont_create(parent);

    lv_obj_t * img = NULL;
    lv_obj_t * label = NULL;

    if(icon) {
        img = lv_image_create(obj);
        lv_image_set_src(img, icon);
    }

    if(txt) {
        label = lv_label_create(obj);
        lv_label_set_text(label, txt);
        lv_label_set_long_mode(label, LV_LABEL_LONG_SCROLL_CIRCULAR);
        lv_obj_set_flex_grow(label, 1);
    }

    if(builder_variant == LV_MENU_ITEM_BUILDER_VARIANT_2 && icon && txt) {
        lv_obj_add_flag(img, LV_OBJ_FLAG_FLEX_IN_NEW_TRACK);
        lv_obj_swap(img, label);
    }

    return obj;
}

static lv_obj_t * create_slider(lv_obj_t * parent, const char * icon, const char * txt, int32_t min, int32_t max,
                                int32_t val)
{
    lv_obj_t * obj = create_text(parent, icon, txt, LV_MENU_ITEM_BUILDER_VARIANT_2);

    lv_obj_t * slider = lv_slider_create(obj);
    lv_obj_set_flex_grow(slider, 1);
    lv_slider_set_range(slider, min, max);
    lv_slider_set_value(slider, val, LV_ANIM_OFF);

    if(icon == NULL) {
        lv_obj_add_flag(slider, LV_OBJ_FLAG_FLEX_IN_NEW_TRACK);
    }

    return obj;
}
