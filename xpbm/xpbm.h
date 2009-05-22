/* Copyright (C) 2000 Free Software Foundation, Inc. */

#include <stdio.h>

/* System wide header file for X Game Mapper, gtk.h must be included before 
   this header.
 */

/* some defines to read v_map format files */
#define VMAP_FORMAT_VERSION "1.6"
#define VMAP_PATTERN "*.vmp"

struct vmap_data {
  char *name;
  char *map;
  int step_size;        /* I think I will just ignore this */
  int max_top;
  int max_left;
  char *tiles;
  int map_wid;
  int map_ht;
  int c_rows;
  int c_columns;
  int u_wid;
  int u_ht;
  int stacking;
  int display_dirty;
  struct vmap_unit *utop;      /* when seeing what unit to select, use utop. */
  struct vmap_unit *ubot;      /* redraw from bottom. */
  struct vmap_line *lines;
  struct vmap_line *newline;
};

struct vmap_unit {
  int idx;
  char *name;
  char *comment;
  int x;
  int y;
  int width;
  int height;
  struct vmap_unit *above;
  struct vmap_unit *below;
};

struct vmap_line {
  int x[2];
  int y[2];
  int style;
  int color;
  struct vmap_line *n;
};
enum {
  VLS_WIDTH1,
  VLS_WIDTH2,
  VLS_WIDTH3,
  VLS_WIDTH4,
  VLS_DASH1,
  VLS_DASH2,
  VLS_DASH3,
  VLS_DASH4,
};

enum {
  VLC_BLACK,
  VLC_COLOR2,
  VLC_COLOR3,
  VLC_COLOR4,
  VLC_COLOR5,
  VLC_COLOR6,
  VLC_COLOR7,
  VLC_COLOR8,
};

struct xpbm_settings {
  int stack_off_x;
  int stack_off_y;
  int autoscroll_marg;
};

/* dpick.c */
void popup_drawing_picker(GtkWidget *w, gpointer data);
void popdown_drawing_picker(GtkWidget *w, gpointer data);

/* files.c */
void import_vmap_file_selection(void);
void export_vmap_file_selection(void);
void mail_vmap_file(void);
void write_vmap_to_file_pointer(FILE *f);
void on_mail_cancel_clicked(GtkButton *button, gpointer user_data);
void on_mail_checkbutton1_toggled (GtkToggleButton *togglebutton, 
				   gpointer user_data);
void on_mail_ok_clicked(GtkButton *button, gpointer user_data);
GtkWidget* create_mail_window1 (void);

/* lines.c */
struct vmap_line *get_vmap_line(void);
void delete_vmap_line(struct vmap_line *l);
void draw_vmap_line(struct vmap_line *vl);
void erase_lines(void);
void undraw_lines(struct vmap_line *vl);

/* map.c */
gint map_expose_event(GtkWidget *w, GdkEventExpose *e);
gint map_button_press_event(GtkWidget *w, GdkEventButton *e);
gint map_button_release_event(GtkWidget *w, GdkEventButton *e);
gint popup_c_select_tile(GtkWidget *w, gpointer d);
gint popdown_c_select_tile(GtkWidget *w, gpointer d);
gint popup_c_prop(GtkWidget *w, gpointer d);
gint popdown_c_prop(GtkWidget *w, gpointer d);
gint map_motion_event(GtkWidget *w, GdkEventMotion *e);
gint tiles_key_press_event(GtkWidget *w, GdkEventKey *e);
int place_tile_on_map(struct vmap_unit *vu);
int put_tile_on_map_top(struct vmap_data *vmp, struct vmap_unit *vu, int draw);
void copy_mapback_to_drawbuf(void);
void update_drawbuf(void);
void map_redisplay(void);
void highlight_unit(struct vmap_unit *vu);
void unhighlight_unit(struct vmap_unit *vu);
void undraw_vmap_unit(struct vmap_unit *vu);

/* node.c */
struct node * get_node(int type);

/* settings.c */
void settings_dialog(void);
GtkWidget *create_set_window2(void);
gint accept_settings(GtkWidget *w, gpointer d);
gint cancel_settings(GtkWidget *w, gpointer d);

/* tiles.c */
gint tiles_expose_event(GtkWidget *w, GdkEventExpose *e);
gint tiles_button_press_event(GtkWidget *w, GdkEventButton *e);
gint popdown_select_tile(GtkWidget *w, gpointer d);
int unit_ulc_x_from_index(struct vmap_data *vmd, int u);
int unit_ulc_y_from_index(struct vmap_data *vmd, int u);
struct vmap_unit *remove_unit_from_vmap_list(struct vmap_data *vm, 
					     struct vmap_unit *vu);
gint c_prop_key_press_event(GtkWidget *w, GdkEventKey *e);
void apply_c_prop(void);
gint flip_counter(GtkWidget *w, gpointer  data);
gint delete_counter(GtkWidget *w, gpointer  data);
gint change_counter(GtkWidget *w, gpointer  data);
void clear_comments(void);
void get_stacked_coords(struct vmap_data *vmp, struct vmap_unit *vu);

/* vmap.c */
int parse_vmap_file(char *s);
struct vmap_data *get_vmap_data(void);
void              delete_vmap_data(struct vmap_data *vm);
struct vmap_unit *get_vmap_unit(void);
void            delete_vmap_unit(struct vmap_unit *vu);
void            delete_vmap_unit_list(struct vmap_unit *vu);
void debug_dump_vmap_unit_chain(struct vmap_data *vmap);

/* xpbm.c */
static void get_main_menu(GtkWidget *w, GtkWidget **mb);
int main (int argc, char **argv);
static void xpbm_quit_gui(GtkWidget *w, gpointer data);





