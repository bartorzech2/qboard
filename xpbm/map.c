#include <stdio.h>
#include <gtk/gtk.h>
#include <gdk_imlib.h>
#include <gdk/gdk.h>
#include "xpbm.h"
extern struct vmap_data *vmap;

/* fixme put these in a struct */
GdkPixmap *mapback = 0, *drawbuf = 0;
extern GtkWidget *mapcanvas, *c_prop_d, *viewport;
extern GdkPixmap *c_pix;
int map_wid, map_ht;
int place_x = -1, place_y = -1;
int show_vmap_lines = 1;
struct vmap_unit *vusel = 0;
GdkCursor *move_cursor = 0;
extern struct  xpbm_settings *pset;

gint
map_expose_event(GtkWidget *w, GdkEventExpose *e)
{
  /* fixme: display something cool before a map is loaded? */
  if(! mapback || ! drawbuf || ! vmap)
    return FALSE;

  if(vmap->display_dirty)
    update_drawbuf();

  gdk_draw_pixmap(w->window,
		  w->style->fg_gc[GTK_WIDGET_STATE(w)],
		  drawbuf,
		  e->area.x, e->area.y,
		  e->area.x, e->area.y,
		  e->area.width, e->area.height
		  );
  
  return FALSE;
}

void
copy_mapback_to_drawbuf(void)
{
  if(! vmap)
    return;
  gdk_draw_pixmap(drawbuf,
		  mapcanvas->style->fg_gc[GTK_WIDGET_STATE(mapcanvas)],
		  mapback,
		  0, 0, 
		  0, 0, 
		  vmap->map_wid, vmap->map_ht);
}

void
update_drawbuf(void)
{
  struct vmap_unit *vu;
  struct vmap_line *vl;
  int ulx, lrx, uly, lry;

  GdkEventExpose e;           /* this is to send to the map widget */

  if(! vmap->display_dirty)
    return ;
  vmap->display_dirty = 0;
#if 0
  /* this is the really slow way */
  gdk_draw_pixmap(drawbuf,
		  mapcanvas->style->fg_gc[GTK_WIDGET_STATE(mapcanvas)],
		  mapback,
		  0, 0, 
		  0, 0, 
		  vmap->map_wid, vmap->map_ht);
#endif
  /* erase units */
  for(vu = vmap->ubot; vu; vu = vu->below)
      gdk_draw_pixmap(drawbuf,
		  mapcanvas->style->fg_gc[GTK_WIDGET_STATE(mapcanvas)],
		  mapback,
		  vu->x - 4, vu->y - 4, 
		  vu->x - 4, vu->y - 4,
		  vu->width + 8, vu->height + 8);


  undraw_lines(vmap->lines);
  
  if(show_vmap_lines)
    for(vl = vmap->lines; vl; vl = vl->n) {
      draw_vmap_line(vl);
    }

  for(vu = vmap->ubot; vu; vu = vu->above) {
    place_tile_on_map(vu);
  }

  if(vmap->utop && ! vusel) {
    highlight_unit(vmap->utop);
  }

  return;
}

gint
map_button_press_event(GtkWidget *w, GdkEventButton *e)
{
  int x= (int)e->x, y = (int)e->y;
  struct vmap_unit *vu;
  struct vmap_unit *above, *below;
  struct vmap_unit *newsel;

  if(! vmap)
    return TRUE;
  if(x >= vmap->map_wid || y > vmap->map_ht)
    return TRUE;
  if(vusel)            /* if a counter is selected, no other action till
			* it is replaced. */
    return TRUE;
                  
  if(e->state & GDK_SHIFT_MASK){   /* If shift is down, a line is to be drawn. */
    vmap->newline = get_vmap_line();
    vmap->newline->x[0]= x;
    vmap->newline->y[0]= y;

    return TRUE;
  }
  /* see if click is on a counter.  
   * button 1: select unit, change cursor, no highlighted counter
   * button 2: unit properties.
   */
  newsel = 0;
  for(vu = vmap->utop; vu; vu = vu->below) {
    if(x > vu->x && x < vu->x + vu->width  &&
       y > vu->y  && y < vu->y + vu->height) {
      newsel = vu;
      break;
    }
  }
  if(newsel) {
    unhighlight_unit(vmap->utop);
    switch(e->button) {
    case 1:                    /* pop unit off top, change cursor */
      if(! move_cursor)
	move_cursor = gdk_cursor_new(GDK_DOTBOX);
      gdk_window_set_cursor(mapcanvas->window, move_cursor);

      if(vusel)
	delete_vmap_unit(vusel);

      /* remove unit from in play chain */
      if(newsel != remove_unit_from_vmap_list(vmap, newsel))
	fprintf(stderr, "System error:%s:%d, please report to maintainer\n",
		__FUNCTION__, __LINE__);
      vusel = newsel;
      gdk_draw_pixmap(drawbuf,
		      mapcanvas->style->fg_gc[GTK_WIDGET_STATE(mapcanvas)],
		      mapback,
		      vu->x - 4, vu->y - 4, 
		      vu->x - 4, vu->y - 4,
		      vu->width + 8, vu->height + 8);
      map_redisplay();
      
      return TRUE;
    case 3:                     /* unit properties */
      /* fixme write function move_unit_top_map_top */
      remove_unit_from_vmap_list(vmap, newsel);
      put_tile_on_map_top(vmap, newsel, 1);
      highlight_unit(newsel);
      map_redisplay();
      popup_c_prop(NULL, NULL);
      return TRUE;
    } 
    
  }
  /* if we get to this point then the user has clicked a blank spot on 
   * the map        Pop up the counter selection or hex property
   * dialog  box????
   */

  switch(e->button) {
  case 1:
    place_x = x - vmap->u_wid / 2;
    place_y = y - vmap->u_ht / 2;
    place_x = place_x < 0 ? 0: place_x;
    place_y = place_y < 0 ? 0: place_y;
    
    popup_c_select_tile(NULL, NULL);
    return TRUE;
    break;
  case 2:
    default:
      break;
  }
  return TRUE;
}

gint
map_button_release_event(GtkWidget *w, GdkEventButton *e)
{
  int x= (int)e->x, y = (int)e->y;
  struct vmap_unit *vu;
  struct vmap_unit *above, *below;
  struct vmap_unit *newsel;
  
  if(! vmap )
    return TRUE;
  
  if(! vusel && ! vmap->newline) 
    return TRUE;
  if(vmap->newline){
    /* see if we are in the midst of drawing a line */
    if(!(e->state & GDK_SHIFT_MASK)){   /* If shift is up cancel line. */
      delete_vmap_line(vmap->newline);
      vmap->newline = 0;
      return TRUE;
    }
    vmap->newline->x[1] = x;
    vmap->newline->y[1] = y;
    /*    draw_vmap_line(vmap->newline);*/

    vmap->newline->n = vmap->lines;
    vmap->lines = vmap->newline;
    vmap->newline = 0;
    vmap->display_dirty = 1;
    map_redisplay();
    
    return TRUE;
  }
  
  if(x >= vmap->map_wid || y > vmap->map_ht)
    return TRUE;
    
  /* if button release, check if a counter is selected if so 
   * deposit it and highlight it, force a redraw.
   */
  switch(e->button){
  case 1:
    unhighlight_unit(vmap->utop);
    vusel->x = x - vusel->width / 2;
    vusel->y = y - vusel->height / 2;
    put_tile_on_map_top(vmap, vusel, 1);
    vusel = 0;
    highlight_unit(vmap->utop);
    vmap->display_dirty = 1;
    map_redisplay();

    gdk_window_set_cursor(mapcanvas->window, 0);
    return TRUE;
  }
  return TRUE;
}

int
place_tile_on_map(struct vmap_unit *vu)
     /* put tile on map but don't update display */
{
  int cx, cy;

  if(vu->x < 0)
    vu->x = 0;
  if(vu->y < 0)
    vu->y = 0;

  cx = unit_ulc_x_from_index(vmap, vu->idx);
  cy = unit_ulc_y_from_index(vmap, vu->idx);

  gdk_draw_pixmap(drawbuf, 
		  mapcanvas->style->fg_gc[GTK_WIDGET_STATE(mapcanvas)],
		  c_pix,
		  cx, cy, 
		  vu->x, vu->y,
		  vmap->u_wid, vmap->u_ht);
  vmap->display_dirty = 1;
}

gint
map_motion_event(GtkWidget *w, GdkEventMotion *e)
{
  int x, y;
  int vx, vy;       /* coords relative to viewport origin */
  int nvx, nvy;       /* coords relative to viewport max, reversed */
  int xnew = -1, ynew = -1;/* if we are going to scroll, these are the 
			    *values to scroll to.
			    */
  static int b3x = -1, b3y = -1;
  GdkModifierType state;

  if(! vmap)
    return TRUE;

  x = e->x;
  y = e->y;
  state = e->state;

  if(e->is_hint)
    gdk_window_get_pointer(e->window, &x, &y, NULL);

  if(!(e->state & GDK_SHIFT_MASK) && vmap->newline){   /* If shift is up 
							* cancel line. */
    delete_vmap_line(vmap->newline);
    vmap->newline = 0;

    return TRUE;
  }

  /* Right button drags map, see if pressed otherwise reset b3 coords */
  if(e->state & GDK_BUTTON3_MASK ){
    if(b3x == -1) {
      b3x = x;
      b3y = y;
    }
    else {
      int d_x = 1 * (b3x - x);
      int d_y = 1 * (b3y - y);
      if(d_x ) {
	xnew = d_x +     gtk_scrolled_window_get_hadjustment(GTK_SCROLLED_WINDOW(viewport))->value;
	if(xnew < 0)
	  xnew = 0;
	b3x  = x + d_x;
	}
      if(d_y) {
	ynew = d_y  + gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(viewport))->value;;
	if(ynew < 0)
	  ynew = 0;
	b3y = y  + d_y;
	}
    }
  }
  else {
    b3x = b3y = -1;
    return TRUE;
  }
#if 0
  if(pset->autoscroll_marg > 0 && b3x == -1) { /* Do autoscrolling */

  vx = x -
    gtk_scrolled_window_get_hadjustment(GTK_SCROLLED_WINDOW(viewport))->value;
  vy = y -
    gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(viewport))->value;
  nvx = viewport->allocation.width + gtk_scrolled_window_get_hadjustment(GTK_SCROLLED_WINDOW(viewport))->value- x;

    nvy = viewport->allocation.height + gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(viewport))->value- y;
   

  if(vx < pset->autoscroll_marg) {
    xnew = gtk_scrolled_window_get_hadjustment(GTK_SCROLLED_WINDOW(viewport))->value - gtk_scrolled_window_get_hadjustment(GTK_SCROLLED_WINDOW(viewport))->step_increment;
  
  }

  if(nvx < pset->autoscroll_marg + 23) {
    xnew 
      = gtk_scrolled_window_get_hadjustment(GTK_SCROLLED_WINDOW(viewport))->value 
      +
      gtk_scrolled_window_get_hadjustment(GTK_SCROLLED_WINDOW(viewport))->step_increment;
  }

  if(vy < pset->autoscroll_marg) {
    ynew = gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(viewport))->value - gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(viewport))->step_increment;
  }

  if(nvy < pset->autoscroll_marg + 23) {
    ynew 
      = gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(viewport))->value 
      +
      gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(viewport))->step_increment;
  }


  }
#endif
  /* Update the map position */
  if(xnew > gtk_scrolled_window_get_hadjustment(GTK_SCROLLED_WINDOW(viewport))->upper - gtk_scrolled_window_get_hadjustment(GTK_SCROLLED_WINDOW(viewport))->page_size)
    xnew = gtk_scrolled_window_get_hadjustment(GTK_SCROLLED_WINDOW(viewport))->upper - gtk_scrolled_window_get_hadjustment(GTK_SCROLLED_WINDOW(viewport))->page_size;

  if(ynew > gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(viewport))->upper - gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(viewport))->page_size)
    ynew = gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(viewport))->upper - gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(viewport))->page_size;
  
  if(xnew > -1)
    gtk_adjustment_set_value(gtk_scrolled_window_get_hadjustment(GTK_SCROLLED_WINDOW(viewport)), xnew);
  
  if(ynew > -1)
    gtk_adjustment_set_value(gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(viewport)), ynew);

  return TRUE;
}

void
highlight_unit(struct vmap_unit *vu)
{
  if(! vmap || ! vu)
    return;

  gdk_draw_rectangle(drawbuf, mapcanvas->style->black_gc,
		     TRUE, vu->x - 4, vu->y -4, 
		     vu->width + 8, vu->height + 8);
  gdk_draw_rectangle(drawbuf, mapcanvas->style->white_gc,
		     TRUE, vu->x - 2, vu->y -2, 
		     vu->width + 4, vu->height + 4);
  place_tile_on_map(vu);
}
void
unhighlight_unit(struct vmap_unit *vu)
{
  if(! vu)
    return;
  gdk_draw_pixmap(drawbuf,
		  mapcanvas->style->fg_gc[GTK_WIDGET_STATE(mapcanvas)],
		  mapback,
		  vu->x - 4, vu->y - 4, 
		  vu->x - 4, vu->y - 4,
		  vu->width + 8, vu->height + 8);
  place_tile_on_map(vu);
}
void
map_redisplay(void)
{
  GdkEventExpose e;
  if(! vmap)
    return ;
  e.send_event = TRUE;
  e.window = mapcanvas->window;
  e.area.x = 0;
  e.area.y = 0;
  e.area.width = vmap->map_wid;
  e.area.height = vmap->map_ht;
  vmap->display_dirty = 1;
  map_expose_event(mapcanvas, &e);
}
void
undraw_vmap_unit(struct vmap_unit *vu)
{
  if(!vu)
    return;
  gdk_draw_pixmap(drawbuf,
		  mapcanvas->style->fg_gc[GTK_WIDGET_STATE(mapcanvas)],
		  mapback,
		  vu->x - 4, vu->y - 4, 
		  vu->x - 4, vu->y - 4,
		  vu->width + 8, vu->height + 8);
  vmap->display_dirty = 1;
}
