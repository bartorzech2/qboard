#include <stdlib.h> /* free() */
#include <stdio.h>
#include <string.h>
#include <gtk/gtk.h>
#include <gdk_imlib.h>
#include <gdk/gdkkeysyms.h>
#include "xpbm.h"

/* fixme put these in a struct */
GtkWidget *c_canvas, *c_dialog, *c_scr, *c_vbox, *c_prop_d;
GtkWidget *c_prop_d, *c_prop_del, *c_prop_flip, 
  *c_prop_change, *c_prop_note;
GdkPixmap *c_pix;
int c_wid, c_ht;


extern struct xpbm_settings *pset;
extern struct vmap_data *vmap;
extern struct vmap_unit *vusel;
extern int place_x, place_y;

/*static int uidx;*/
static int c_popped_up = 0, c_prop_popped_up = 0;

gint
tiles_expose_event(GtkWidget *w, GdkEventExpose *e)
{
  if(! c_pix )
    return FALSE;

  gdk_draw_pixmap(w->window,
		  w->style->fg_gc[GTK_WIDGET_STATE(w)],
		  c_pix,
		  e->area.x, e->area.y,
		  e->area.x, e->area.y,
		  e->area.width, e->area.height);

  return FALSE;
}

gint 
popup_c_select_tile(GtkWidget *w, gpointer d)
{
  if(! vmap || c_popped_up)
    return TRUE;
  c_popped_up = 1;
  gtk_widget_show(c_dialog);
  gtk_window_set_modal(GTK_WINDOW(c_dialog), TRUE);
  return TRUE;
}

gint
popdown_c_select_tile(GtkWidget *w, gpointer d)
{
  if(! vmap || ! c_popped_up)
    return TRUE;
  
  gtk_window_set_modal(GTK_WINDOW(c_dialog), FALSE);
  gtk_widget_hide(c_dialog);
  c_popped_up = 0;
  return TRUE;
}

gint 
popup_c_prop(GtkWidget *w, gpointer d)
{
  if(! vmap || c_prop_popped_up)
    return TRUE;
  c_prop_popped_up = 1;
  if(vmap->utop) {
    if(vmap->utop->comment)
      gtk_entry_set_text(GTK_ENTRY(c_prop_note), vmap->utop->comment);
    else
      gtk_entry_set_text(GTK_ENTRY(c_prop_note), "");
  }

  gtk_widget_show(c_prop_d);
  gtk_window_set_modal(GTK_WINDOW(c_prop_d), TRUE);
  return TRUE;
}

gint
popdown_c_prop(GtkWidget *w, gpointer d)
{
  if(! vmap || ! c_prop_popped_up)
    return TRUE;
  gtk_window_set_modal(GTK_WINDOW(c_prop_d), FALSE);
  gtk_widget_hide(c_prop_d);
  c_prop_popped_up = 0;
  return TRUE;
}

gint
tiles_button_press_event(GtkWidget *w, GdkEventButton *e)
{
  int r, c, u;
  struct vmap_unit *vu;
  popdown_c_select_tile(NULL, NULL);
  if(! vmap)
    return TRUE;

  /* figure out which counter here */
  r = (int)e->y / vmap->u_ht;
  c = (int)e->x / vmap->u_wid;
  if(r >= vmap->c_rows || c >= vmap->c_columns)
    return TRUE;
  u = r * vmap->c_columns + c;
  if(!(vu = get_vmap_unit()))
    return TRUE;
  vu->x = place_x; 
  vu->y = place_y;
  vu->width = vmap->u_wid;
  vu->height = vmap->u_ht;
  vu->idx = u;
  put_tile_on_map_top(vmap, vu, 1);
  highlight_unit(vmap->utop);
  map_redisplay();
  place_x = place_y = -1;
  return TRUE;

}

gint
tiles_key_press_event(GtkWidget *w, GdkEventKey *e)
{
  /* c_dialog gets the key events. */
  int k = e->keyval;
  if(k == GDK_Escape)
    popdown_c_select_tile(NULL, NULL);
  return TRUE;
}

gint
c_prop_key_press_event(GtkWidget *w, GdkEventKey *e)
{
  /* c_dialog gets the key events. */
  int k = e->keyval;
  switch(e->keyval) {
  case GDK_Escape:
    popdown_c_prop(NULL, NULL);
    return TRUE;
  case GDK_Return:
    apply_c_prop();
    popdown_c_prop(NULL, NULL);
    return TRUE;
  }
  return TRUE;
}

void
apply_c_prop(void)
{
  struct vmap_unit *vu;

  if(! (vu = vmap->utop))
    return;
  if(vu->comment)
    free(vu->comment);
  vu->comment = strdup(gtk_entry_get_text(GTK_ENTRY(c_prop_note)));
}

gint
flip_counter(GtkWidget *ignored, gpointer  data)
{
  struct vmap_unit *vu = vmap->utop;
  if(! vu || ! vmap)
    return TRUE;
  if(vu->idx % 2)
    vu->idx --;
  else
    vu->idx ++;

  /*put_tile_on_map_top(vmap, vusel);*/
  popdown_c_prop(NULL, NULL);
  place_tile_on_map(vu);
  map_redisplay();
  return TRUE;
  
}
gint
delete_counter(GtkWidget *w, gpointer  data)
{
  struct vmap_unit *vu;
  if(! (vu = remove_unit_from_vmap_list(vmap, vmap->utop)))
    return TRUE;
  popdown_c_prop(NULL, NULL);
  undraw_vmap_unit(vu);
  delete_vmap_unit(vu);
  map_redisplay();
  return TRUE;
}
gint
change_counter(GtkWidget *w, gpointer  data)
{
  struct vmap_unit *vu;
  if(! (vu = remove_unit_from_vmap_list(vmap, vmap->utop)))
      return TRUE;
  undraw_vmap_unit(vu);
  place_x = vu->x;
  place_y = vu->y;
  popdown_c_prop(NULL, NULL);

  popup_c_select_tile(NULL, NULL);
  /*
  undraw_vmap_unit(vu);
  delete_vmap_unit(vu);
  map_redisplay();
  popdown_c_select_tile(NULL, NULL);
  */
  vmap->display_dirty = 1;
  update_drawbuf();
  return TRUE;
}
int 
unit_ulc_x_from_index(struct vmap_data *vmd, int u)
{
  int x;
  int col;
  col = u % vmd->c_columns;
  return col * vmd->u_wid;
}

int 
unit_ulc_y_from_index(struct vmap_data *vmd, int u)
{
  int y;
  int row;
  row = u / vmd->c_columns;
  return row * vmd->u_ht;
}

void 
get_stacked_coords(struct vmap_data *vmp, struct vmap_unit *vu)
{
  struct vmap_unit *tu = 0;
  struct vmap_unit *atop = 0;
  /* Walk through list of units from botton and find the last one which
   * the current unit is stacked near the middle of  (w/in 75%)
   */
  int cx = vu->x + vu->width / 2;
  int cy = vu->y + vu->height / 2;

  if(! pset->stack_off_x && ! pset->stack_off_y) {
    return;
  }
  for(tu = vmp->ubot; tu; tu = tu->above) {
    int ctx = tu->x + tu->width / 2; /* Center of current tile */
    int cty = tu->y + tu->height / 2;

    if(abs(ctx - cx) < tu->width * .5
       && abs(cty - cy) < tu->width * .5 )
      atop = tu;
  }

  if(atop) {
    vu->x = atop->x + pset->stack_off_x;
    vu->y = atop->y + pset->stack_off_y;
  }
}

int
put_tile_on_map_top(struct vmap_data *vmp, struct vmap_unit *vu, int draw)
{
  int x, y;
  
  if(! vmp)
    return -1;
  if(!vu)
    return -1;
  get_stacked_coords(vmp, vu);

  x = vu->x;
  y = vu->y;

  if(x < 0 || y < 0 || x >= vmp->map_wid || y >= vmp->map_ht)
    return -1;
  if(vu->idx < 0 || vu->idx >= vmp->c_columns * vmp->c_rows)
    return -1;

  if(! vmap->utop) {
    vmap->utop = vu;
    vmap->ubot = vu;
  }
  else {
    unhighlight_unit(vmap->utop);
    vu->below = vmap->utop;
    vmap->utop->above = vu;
    vmap->utop = vu;
  }
  vu->x = x;
  vu->y = y;
  if(draw)
    place_tile_on_map(vu);

  return 0;
}

struct vmap_unit *
remove_unit_from_vmap_list(struct vmap_data *vm, struct vmap_unit *vu)
{
  struct vmap_unit *tu, *above, *below;
  for(tu = vmap->utop; tu; tu = tu->below) {
    if(tu == vu)
      break;
  }
  if(!tu)
    return 0;
  above = tu->above;
  below = tu->below;
  if(!above && !below) { /* only unit in chain */
    tu->above = tu->below = vmap->utop = vmap->ubot = 0;
    return tu;
  }
  tu->above = tu->below = 0;
  if(below)
    below->above = above;
  if(above)
    above->below = below;
  if(tu == vmap->utop)
    vmap->utop = below;
  if(tu == vmap->ubot)
    vmap->ubot = above;
  return tu;
}

void
clear_comments(void)
{

}
