#include <stdlib.h>
#include <gtk/gtk.h>

#include "xpbm.h"

extern GtkWidget *mapcanvas;
extern struct vmap_data *vmap;
extern GdkPixmap *drawbuf, *mapback;
struct vmap_line *lines;

void
setup_line_gc(void)
{
  mapcanvas->window;
}

struct vmap_line *
get_vmap_line(void)
{
  struct vmap_line *vl;

  vl = malloc(sizeof(struct vmap_line));
  vl->x[0] = vl->x[1] = -1;
  vl->y[0] = vl->y[1] = -1;
  vl->color = VLC_BLACK;
  vl->style = VLS_WIDTH1;
  return vl;
};

void
delete_vmap_line(struct vmap_line *l)
{
  free(l);
}

void
draw_vmap_line(struct vmap_line *vl)
{
  gdk_draw_line(drawbuf,
		mapcanvas->style->fg_gc[GTK_WIDGET_STATE(mapcanvas)],
		vl->x[0], vl->y[0], vl->x[1], vl->y[1]);
  vmap->display_dirty = 1;
}

void 
erase_lines(void) 
{
  struct vmap_line *vl;
  undraw_lines(vl);
  for(vl = vmap->lines; vl; ) {
    struct vmap_line *n = vl->n;
    delete_vmap_line(vl);
    vl = n;
  }
  vmap->lines = 0;
  map_redisplay();
  vmap->display_dirty = 1;
}

void
undraw_lines(struct vmap_line *vl)
{
  int ulx, uly, lrx, lry;
  for(vl = vmap->lines; vl; vl = vl->n) {
    if(vl->x[0] < vl->x[1]) {
      ulx = vl->x[0]; lrx = vl->x[1];
    }
    else {
      ulx = vl->x[1]; lrx = vl->x[0];
    }
    if(vl->y[0] < vl->y[1]) {
      uly = vl->y[0]; lry = vl->y[1];
    }
    else {
      uly = vl->y[1]; lry = vl->y[0];
    }
    gdk_draw_pixmap(drawbuf,
		    mapcanvas->style->fg_gc[GTK_WIDGET_STATE(mapcanvas)],
		    mapback,
		    ulx, uly, ulx, uly, lrx - ulx + 1, lry - uly + 1);
  }
}
