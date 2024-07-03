
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
#include <stdio.h>

/*********************
 *      DEFINES
 *********************/
#define POSSIBLE_ENEMIES 11
#define NUM_COMBATANTS 3
#define FINAL_COMBATANTS 3


/**********************
 *      TYPEDEFS
 **********************/
typedef struct part{
  char *name;
  bool uniqueChar; // True for player character or Unique Enemy
  int init;
  int initSpot;
  int turnCount;
  int ac;
  char * hp;
  struct part *next;
}part;

typedef struct num_enemy{
  part * enemy;
  int qty;
}num_enemy;

/***** ENEMIES *****/
part orc = {"Orc", false, 0, 0, 0, 13, "15", NULL};
part orog = {"Orog", false, 0, 0, 0, 18, "53", &orc};
part magmin = {"Magmin", false, 0, 0, 0, 15, "9", &orog};
part bandit = {"Bandit", false, 0, 0, 0, 12, "11", &magmin};
part banditCaptain = {"Bandit Captain", false, 0, 0, 0, 15, "65", &bandit};
part tribalWarrior = {"Tribal Warrior", false, 0, 0, 0, 12, "15", &banditCaptain};
part berserker = {"Berserker", false, 0, 0, 0, 13, "63", &tribalWarrior};
part fireGiant = {"Fire Giant", false, 0, 0, 0, 18, "163", &berserker};
part frostGiant = {"Frost Giant", false, 0, 0, 0, 15, "138", &fireGiant};
part hillGiant = {"Hill Giant", false, 0, 0, 0, 13, "105", &frostGiant};
part stoneGiant = {"Stone Giant", false, 0, 0, 0, 17, "126", &hillGiant};

/***** ARRAY OF COMBATANTS ******/
/* ENEMIES MUST BE ADDED TO ARRAY IN THE SAME ORDER THEY APPEAR IN THE ABOVE LINKED LIST */
part * enemies[POSSIBLE_ENEMIES] = {&stoneGiant, &hillGiant, &frostGiant, &fireGiant, &berserker,
                                    &tribalWarrior, &banditCaptain, &bandit, &magmin, &orog, &orc};

/***** GRID SETTINGS *****/
static int32_t select_col_dsc[] = {50, 39, 39 , LV_GRID_TEMPLATE_LAST};
static int32_t select_row_dsc[] = {50, LV_GRID_TEMPLATE_LAST};


/**********************
 *  STATIC PROTOTYPES
 **********************/
static lv_display_t * hal_init(int32_t w, int32_t h);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_obj_t * scr;

/* SET UP SCREEN */
static lv_obj_t * builder_menu;
static lv_obj_t * build_win;
static lv_obj_t * start_label;
static lv_obj_t * start_btn;

/* SET UP SCREEN, LEFT SIDE */
static lv_obj_t * sub_selectPage;
static lv_obj_t * select_cont;

/* SET UP SCREEN, RIGHT SIDE */
static lv_obj_t * selected_page;
static lv_obj_t * selected_cont;
static lv_obj_t * selected_label;
static lv_obj_t * selected_num;

/* COMBAT SCREEN */
static lv_obj_t * combat_menu;
static lv_obj_t * combat_win;

/* COMBAT SCREEN, RIGHT SIDE */
static lv_obj_t * stat_page;
static lv_obj_t * stat_section;
static lv_obj_t * stat_cont;


/* COMBAT SCREEN, LEFT SIDE */
static uint16_t parts_counter = 0;
static lv_obj_t * sub_combatPage;
static lv_obj_t * combat_section;
static lv_obj_t * combat_cont;

 /* GENERIC REUSE */
static lv_obj_t * label;
static lv_obj_t * entry;
static lv_obj_t * btn;
static lv_obj_t * cont;
static lv_obj_t * head_cont;
static lv_obj_t * select_arr_cont[POSSIBLE_ENEMIES];
static num_enemy final_parts[POSSIBLE_ENEMIES] = { /* RESERVING MEMORY FOR POSSIBLE ENEMIES */
                                                  {NULL, 0}, {NULL, 0}, {NULL, 0}, {NULL, 0}, {NULL, 0}, {NULL, 0},
                                                  {NULL, 0}, {NULL, 0}, {NULL, 0}, {NULL, 0}, {NULL, 0}
                                                  };

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
 *      STYLES
 **********************/

/* MAKING STYLES CONST TO AVOID DYNAMIC ALLOCATION */

const lv_style_const_prop_t text_style_props[] = {
  LV_STYLE_CONST_TEXT_FONT(&lv_font_montserrat_12),
  LV_STYLE_CONST_ALIGN(LV_ALIGN_CENTER),
  LV_STYLE_CONST_TEXT_ALIGN(LV_TEXT_ALIGN_CENTER),
  LV_STYLE_CONST_WIDTH(58),
  LV_STYLE_CONST_PROPS_END
};

LV_STYLE_CONST_INIT(text_style, text_style_props);

const lv_style_const_prop_t btn_style_props[] = {
  LV_STYLE_CONST_HEIGHT(20),
  LV_STYLE_CONST_WIDTH(35),
  LV_STYLE_CONST_PROPS_END
};

LV_STYLE_CONST_INIT(btn_style, btn_style_props);

const lv_style_const_prop_t ta_style_props[] = {
  LV_STYLE_CONST_TEXT_FONT(&lv_font_montserrat_10),
  LV_STYLE_CONST_HEIGHT(20),
  LV_STYLE_CONST_WIDTH(35),
  LV_STYLE_CONST_PROPS_END
};

LV_STYLE_CONST_INIT(ta_style, ta_style_props);

/**********************
 *      VARIABLES
 **********************/
int monitor_hor_res = 320, monitor_ver_res = 240;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void setup_screen(void);
static void fill_select_page(lv_obj_t * page);
static void read_combatant(lv_event_t * e);
static void add_combatant(part * combatant, const char * num);
static void remove_combatant(lv_event_t * e);

static void start_fight(lv_event_t * e);

static void combat_screen(void);
static void fill_combat_screen(void);

static void exit_combat(lv_event_t * e);

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
  setup_screen();

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

/**********************
 *       SCREENS
 **********************/
static void setup_screen(void)
{
  build_win = lv_win_create(lv_screen_active());

  head_cont = lv_win_get_header(build_win);
  lv_obj_set_height(head_cont, 40);

  label = lv_label_create(head_cont);
  lv_label_set_text(label, "Combat Builder");
  lv_obj_set_style_pad_right(label, 125, 0);

  start_btn = lv_btn_create(head_cont);
  lv_obj_set_align(start_btn, LV_ALIGN_RIGHT_MID);
  lv_obj_add_event_cb(start_btn, start_fight, LV_EVENT_ALL, NULL);

  start_label = lv_label_create(start_btn);
  lv_label_set_text(start_label, "Start!");
  lv_obj_center(start_label);

  cont = lv_win_get_content(build_win);
  lv_obj_set_style_pad_all(cont, 0, 0);

  builder_menu = lv_menu_create(cont);
  lv_obj_set_size(builder_menu, lv_pct(100), lv_pct(100));
  lv_obj_center(builder_menu);
  lv_menu_set_mode_header(builder_menu, LV_MENU_HEADER_TOP_UNFIXED);

  selected_page = lv_menu_page_create(builder_menu, NULL);
  lv_menu_set_page(builder_menu, selected_page);
  lv_menu_separator_create(selected_page);

  selected_cont = lv_menu_cont_create(selected_page);
  lv_obj_set_align(selected_cont, LV_ALIGN_BOTTOM_RIGHT);
  lv_obj_set_width(selected_cont, lv_pct(100));

  sub_selectPage = lv_menu_page_create(builder_menu, NULL);
  lv_obj_set_width(sub_selectPage, lv_pct(100));
  lv_menu_set_sidebar_page(builder_menu, sub_selectPage);

  fill_select_page(sub_selectPage);
  lv_obj_scroll_to_y(sub_selectPage, 0, LV_ANIM_OFF);

  lv_menu_t* menu = (lv_menu_t*)builder_menu;
  // set sidebar width
  lv_obj_set_width(menu->sidebar, LV_PCT(50));
}

static void combat_screen(void)
{
  combat_win = lv_win_create(lv_screen_active());

  head_cont = lv_win_get_header(combat_win);
  lv_obj_set_height(head_cont, 40);

  label = lv_label_create(head_cont);
  lv_label_set_text(label, "Fight!");
  lv_obj_set_style_pad_right(label, 190, 0);

  btn = lv_btn_create(head_cont);
  lv_obj_set_align(btn, LV_ALIGN_RIGHT_MID);
  lv_obj_add_event_cb(btn, exit_combat, LV_EVENT_ALL, NULL);

  label = lv_label_create(btn);
  lv_label_set_text(label, "Finish!");
  lv_obj_center(label);

  cont = lv_win_get_content(combat_win);
  lv_obj_set_style_pad_all(cont, 0, 0);

  combat_menu = lv_menu_create(cont);
  lv_obj_set_size(combat_menu, lv_pct(100), lv_pct(100));
  lv_obj_center(combat_menu);
  lv_menu_set_mode_header(combat_menu, LV_MENU_HEADER_TOP_UNFIXED);

  stat_page = lv_menu_page_create(combat_menu, NULL);
  lv_menu_set_page(combat_menu, stat_page);
  lv_menu_separator_create(stat_page);
  stat_cont = lv_menu_cont_create(stat_page);

  sub_combatPage = lv_menu_page_create(combat_menu, NULL);
  lv_obj_set_width(sub_combatPage, lv_pct(100));
  lv_menu_set_sidebar_page(combat_menu, sub_combatPage);

  lv_menu_t* menu_t = (lv_menu_t*)combat_menu;
  // set sidebar width
  lv_obj_set_width(menu_t->sidebar, LV_PCT(60));
}


/**********************
 *  HELPER FUNCTIONS
 **********************/

static void fill_select_page(lv_obj_t * page)
{
  part * temp = &stoneGiant;
  int i = 0;

  while(temp != NULL){
    select_cont = lv_menu_cont_create(page);
    select_arr_cont[i] = select_cont; /*Keep track of each group to be able to add them to the selected_page ***** SWITCH TO LINKED LIST*/
    lv_obj_set_width(select_cont, lv_pct(100));
    lv_obj_set_layout(select_cont, LV_LAYOUT_GRID);
    lv_obj_set_grid_dsc_array(select_cont, select_col_dsc, select_row_dsc);

    label = lv_label_create(select_cont);
    lv_obj_add_style(label, &text_style, 0);
    lv_label_set_text(label, temp->name);
    // lv_obj_set_width(label, 57);
    lv_obj_set_grid_cell(label, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_START, 0, 1);

    entry = lv_textarea_create(select_cont);
    lv_obj_add_style(entry, &ta_style, 0);
    lv_textarea_set_one_line(entry, true);
    lv_textarea_set_max_length(entry, 1);
    lv_obj_set_grid_cell(entry, LV_GRID_ALIGN_CENTER, 1, 1, LV_GRID_ALIGN_START, 0, 1);

    btn = lv_btn_create(select_cont);
    lv_obj_add_style(btn, &btn_style, 0);
    lv_obj_set_grid_cell(btn, LV_GRID_ALIGN_CENTER, 2, 1, LV_GRID_ALIGN_START, 0, 1);
    lv_obj_add_event_cb(btn, read_combatant, LV_EVENT_ALL, entry);

    label = lv_label_create(btn);
    lv_obj_add_style(label, &text_style, 0);
    lv_label_set_text(label, "Add");
    lv_obj_center(label);

    temp = temp->next;
    i++;
  }
}

static void fill_combat_screen(void)
{
  int total_enemies = 0;
  for(int i = 0; i < FINAL_COMBATANTS; i++){
    total_enemies = total_enemies + final_parts[i].qty; // SWITCH TO LINKED LIST
  }

  for(int i = 0; i < FINAL_COMBATANTS; i++){
    for(int j = 0; j < final_parts[i].qty; j++){
      combat_cont = lv_menu_cont_create(sub_combatPage);
      lv_obj_set_layout(combat_cont, LV_LAYOUT_FLEX);
      lv_obj_set_flex_flow(combat_cont, LV_FLEX_FLOW_ROW);

      label = lv_label_create(combat_cont);
      lv_obj_add_style(label, &text_style, 0);
      lv_label_set_text(label, "Init: ");

      btn = lv_btn_create(combat_cont);
      lv_obj_set_size(btn, 60, 30);

      label = lv_label_create(btn);
      lv_obj_add_style(label, &text_style, 0);
      lv_label_set_text(label, final_parts[i].enemy->name);
      // lv_obj_set_width(label, 57);
      lv_obj_center(label);

      entry = lv_textarea_create(combat_cont);
      lv_obj_add_style(entry, &ta_style, 0);
      lv_textarea_set_one_line(entry, true);
      lv_textarea_set_max_length(entry, 3);
      //lv_obj_set_size(entry, 55, 45);

      label = lv_label_create(combat_cont);
      lv_obj_add_style(label, &text_style, 0);
      lv_label_set_text(label, final_parts[i].enemy->hp);
    }
  }
}

/**********************
 *   BUTTON COMMANDS
 **********************/
static void start_fight(lv_event_t * e)
{
  lv_event_code_t code = lv_event_get_code(e);
  lv_obj_t * obj = lv_event_get_target(e);

  if (code == LV_EVENT_CLICKED){
    combat_screen();
    fill_combat_screen();
    for(int i = 0; i < POSSIBLE_ENEMIES; i++){
      lv_obj_del(select_arr_cont[i]);
    }
    //lv_obj_del(build_win);
  }
}

static void read_combatant(lv_event_t * e)
{
  lv_event_code_t code = lv_event_get_code(e);
  lv_obj_t * obj = lv_event_get_target(e);
  lv_obj_t * num_enemies_ta = lv_event_get_user_data(e);
  const char * num_enemies = lv_textarea_get_text(num_enemies_ta);
  uint16_t selector = 0;
  if (code == LV_EVENT_CLICKED){
    for(int i = 0; i < POSSIBLE_ENEMIES; i++){
      if (select_arr_cont[i] == obj->parent){ // LINKED LIST INSTEAD
        selector = i;
        break;
      }
    }
    add_combatant(enemies[selector], num_enemies);
  }
}

static void add_combatant(part * combatant, const char * num)
{
  for(int i = 0; i < POSSIBLE_ENEMIES; i++){ // LINKED LIST INSTEAD
    if(final_parts[i].enemy == NULL){
      final_parts[i].enemy = combatant;
      final_parts[i].qty = atoi(num);
      parts_counter++;
      break;
    }else if(parts_counter >= POSSIBLE_ENEMIES){
      return;
    }
  }

  selected_cont = lv_menu_cont_create(selected_page);
  lv_obj_set_width(selected_cont, lv_pct(100));
  lv_obj_set_layout(selected_cont, LV_LAYOUT_GRID);
  lv_obj_set_grid_dsc_array(selected_cont, select_col_dsc, select_row_dsc);

  selected_label = lv_label_create(selected_cont);
  lv_obj_add_style(selected_label, &text_style, 0);
  lv_label_set_text(selected_label, combatant->name);
  lv_obj_set_size(selected_label, 50, 50);
  lv_obj_set_grid_cell(selected_label, LV_GRID_ALIGN_END, 0, 1, LV_GRID_ALIGN_END, 0, 1);

  selected_num = lv_label_create(selected_cont);
  lv_label_set_text(selected_num, num);
  lv_obj_add_style(selected_num, &text_style, 0);
  lv_obj_set_grid_cell(selected_num, LV_GRID_ALIGN_START, 1, 1, LV_GRID_ALIGN_START, 0, 1);

  btn = lv_btn_create(selected_cont);
  lv_obj_set_size(btn, 30, 20);
  lv_obj_set_grid_cell(btn, LV_GRID_ALIGN_START, 2, 1, LV_GRID_ALIGN_START, 0, 1);
  lv_obj_add_event_cb(btn, remove_combatant, LV_EVENT_ALL, NULL);

  label = lv_label_create(btn);
  lv_label_set_text(label, "-");
  lv_obj_center(label);
}

static void remove_combatant(lv_event_t * e)
{
  lv_event_code_t code = lv_event_get_code(e);
  lv_obj_t * obj = lv_event_get_target(e);
  lv_obj_t * to_delete = obj->parent;

  if(code == LV_EVENT_CLICKED){
    lv_obj_del(to_delete);
  }
}

static void exit_combat(lv_event_t * e)
{
  lv_event_code_t code = lv_event_get_code(e);

  if(code == LV_EVENT_CLICKED){
    for(int i = 0; i < FINAL_COMBATANTS; i++){
      final_parts[i].enemy = NULL;
      final_parts[i].qty = 0;
    }
    parts_counter = 0;
    setup_screen();
    lv_obj_del(combat_win);
  }
  return;
}
