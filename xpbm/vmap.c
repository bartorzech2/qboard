#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>
#include <gdk_imlib.h>
#include "xpbm.h"

extern GdkPixmap *mapback, *drawbuf, *c_pix;
extern GtkWidget *mapcanvas, *c_canvas;

struct vmap_data *vmap = 0;
struct vmap_unit *u;
char *vmap_dir;

/* defines for parsing vmap files */
#define VMS_MAP "MAP"
#define VMS_TILES "TILES"
#define VMS_UNIT "UNIT"
#define VMS_LINE "LINE"
#define VMS_STACKING "STACKING"

enum {
  VMT_NO_TOKEN = -1,
  VMT_MAP = 0,
  VMT_TILES,
  VMT_UNIT,
  VMT_LINE,
  VMT_STACKING,
  N_VMTOKENS,
  VMT_FIRST = VMT_MAP,
};
char *vmap_keyw[] = { VMS_MAP, VMS_TILES, VMS_UNIT, VMS_LINE, VMS_STACKING };


/* fixme write a real parser */
int
parse_vmap_file(char *s)
{
  int i;
  char buf[1000], *cp;
  char keyword[1000], str[1000];
  int iarg[10];                  /* temp arg holders */
  int nargs;
  int c_wid, c_ht;
  FILE *vmf;
  int token;
  GdkImlibImage *im;
  struct vmap_unit *vu;
  struct vmap_line *vl;

  if(vmap)
    delete_vmap_data(vmap);

  if(!(vmap = get_vmap_data()))
    return -1;

  if(!(vmf = fopen(s, "r"))) {
    free(vmap);
    return -1;
  }
  vmap_dir = dirname(s);
  chdir(vmap_dir);
  while(1) {
    if(!fgets(buf, 999, vmf))
      break;
    sscanf(buf, "%999s", keyword);

    buf[strlen(buf) - 1] = 0;
    cp = &buf[0];

    while( *cp && isspace(*cp))
      cp ++;

    if(! *cp || *cp == '#' || *cp == '\n')
      continue;


    for(token = VMT_NO_TOKEN, i = VMT_FIRST; i < N_VMTOKENS; i++) {
      if(! strcmp(vmap_keyw[i], keyword))
	token = i;
    }

    if(token < 0) {
      continue;           /* fixme:  is this a good idea? */
      fclose(vmf);
      return -1;
    }

    switch(token) {
    case VMT_MAP:
      nargs = sscanf(buf, "%*999s %d %d %d %999s", 
		     &iarg[0], &iarg[1], &iarg[2], str);
      if(nargs < 4 || str[0] == 0 || ! (vmap->map = strdup(str))) {
	fclose(vmf);
	delete_vmap_data(vmap);
	vmap = 0;
	return -1;
      }
      vmap->step_size = iarg[0];
      vmap->max_top = iarg[1];
      vmap->max_left = iarg[2];


      /* 
       * set up the "mapback" blank map and the drawing buffer map 
       */
      if(! (im = gdk_imlib_load_image(vmap->map))) {
	/* fixme buffer overflows */
	char path[1024];
	sprintf(path, "%s/%s", vmap_dir, vmap->map);
	if(! (im = gdk_imlib_load_image(path))) { 
	  free(vmap);
	  vmap = 0;
	  return - 1;
	}
      }
      vmap->map_wid = im->rgb_width;
      vmap->map_ht = im->rgb_height;
      
      gtk_drawing_area_size(GTK_DRAWING_AREA(mapcanvas), vmap->map_wid, vmap->map_ht);
      
      if(mapback)
	gdk_imlib_free_pixmap(mapback);
      gdk_imlib_render(im, vmap->map_wid, vmap->map_ht);
      mapback = gdk_imlib_move_image(im);
      gdk_imlib_destroy_image(im);
      
      if(drawbuf)
	gdk_imlib_free_pixmap(drawbuf);
      
      drawbuf = gdk_pixmap_new(mapcanvas->window, vmap->map_wid, vmap->map_ht,
			       -1);
      copy_mapback_to_drawbuf();
      
      break;

    case VMT_TILES:
      nargs = sscanf(buf, "%*999s %d %d %999s", &iarg[0], &iarg[1], str);
      if(nargs < 3 || ! str[0] || ! (vmap->tiles = strdup(str))) {
	fclose(vmf);
	delete_vmap_data(vmap);
	vmap = 0;
	return -1;
      }
      vmap->c_rows = iarg[0];
      vmap->c_columns = iarg[1];
      /* 
       * set up the tiles pixmap 
       */
      if(! (im = gdk_imlib_load_image(vmap->tiles))) {
	/* fixme buffer overflows */
	char path[1024];
	sprintf(path, "%s/%s", vmap_dir, vmap->tiles);
	if(! (im = gdk_imlib_load_image(path))) { 
	  free(vmap);
	  vmap = 0;
	  return - 1;
	}
      }

      c_wid = im->rgb_width;
      c_ht = im->rgb_height;
      
      vmap->u_wid = c_wid / vmap->c_columns;
      vmap->u_ht = c_ht / vmap->c_rows;
      gtk_drawing_area_size(GTK_DRAWING_AREA(c_canvas), c_wid, c_ht);
      
      if(c_pix)
	gdk_imlib_free_pixmap(c_pix);
      gdk_imlib_render(im, c_wid, c_ht);
      c_pix = gdk_imlib_move_image(im);
      
      gdk_imlib_destroy_image(im);
      
      
      break;
    case VMT_UNIT:
      str[0] = 0;
      nargs = sscanf(buf, "%*999s %d %d %d %d %d %d %999[^\n\r]", 
	     &iarg[0], &iarg[1], &iarg[2], &iarg[3], &iarg[4], &iarg[5], str);
      if(nargs < 4) {
	fclose(vmf);
	delete_vmap_data(vmap);
	vmap = 0;
	return -1;
      }
      if(!(vu = get_vmap_unit()))
	return -1;
      vu->y = iarg[0];
      vu->x = iarg[1]; 
      vu->width = vmap->u_wid;
      vu->height = vmap->u_ht;
      vu->idx = iarg[3];
      if(str)
	vu->comment = strdup(str);
      
      put_tile_on_map_top(vmap, vu, 0);
      break;
    case VMT_LINE:
      nargs = sscanf(buf, "%*999s %d %d %d %d %d %d", 
	     &iarg[0], &iarg[1], &iarg[2], &iarg[3], &iarg[4], &iarg[5]);
      if(nargs != 6 ) {
	fclose(vmf);
	delete_vmap_data(vmap);
	vmap = 0;
	return -1;
      }
      if(!(vl = get_vmap_line()))
	return -1;
      vl->x[0] = iarg[0]; 
      vl->y[0] = iarg[1];
      vl->x[1] = iarg[2]; 
      vl->y[1] = iarg[3];
      vl->style = iarg[4]; 
      vl->color = iarg[5];

      
      vl->n = vmap->lines;
      vmap->lines = vl;
      break;
    case VMT_STACKING:
      sscanf(buf, "%*999s %999s", str);
      if(! strcmp(str, "ON"))
	vmap->stacking = 1;
      break;
    default:      /* should I do more error checking ? */
      break;
    }
    if(feof(vmf))
      break;
  }

  fclose(vmf);
}

/* fixme: initialize to more sensible defaults */
struct vmap_data *
get_vmap_data(void)
{
  struct vmap_data *vmd;

  vmd = malloc(sizeof(struct vmap_data));
  
  vmd->name = 0;
  vmd->map = 0;
  vmd->tiles = 0;
  vmd->stacking = 0;
  vmd->display_dirty = 1;
  vmd->utop = vmd->ubot = 0;
  vmd->newline = 0;
  vmd->lines = 0;
  return vmd;
}

void
delete_vmap_data(struct vmap_data *vm)
{

  if(! vm)
    return;

  if(vm->name)
    free(vm->name);
  if(vm->map) 
    free(vm->map);
  if(vm->tiles)
    free(vm->tiles);
  if(vm->utop)         /* just delete from top to bottom. */
    delete_vmap_unit_list(vm->utop);

  free(vm);
}

struct vmap_unit *
get_vmap_unit(void)
{
  struct vmap_unit *vu;

  vu = malloc(sizeof(struct vmap_unit));
  vu->name = 0;
  vu->comment = 0;
  vu->x = -1;
  vu->y = -1;
  vu->above = 0;
  vu->below = 0;

  if(vmap) {
    vu->width = vmap->u_wid;
    vu->height = vmap->u_ht;
  }
  else {
    vu->width = 0;
    vu->height = 0;
  }

  return vu;
}

void
delete_vmap_unit(struct vmap_unit *vu)
{
  if(! vu)
    return;

  if(vu->name)
    free(vu->name);
  if(vu->comment)
    free(vu->comment);
  free(vu);
}

void
delete_vmap_unit_list(struct vmap_unit *vu)
{
  struct vmap_unit *u, *t;
  if(! vu)
    return;

  for(u = vu; u; u = t) {
    t = u->below;
    delete_vmap_unit(u);
  }
}
void
debug_dump_vmap_unit_chain(struct vmap_data *vmap)
{
  struct vmap_unit *vu;
  fprintf(stderr, "vmap->uidx chain: ");
  for(vu = vmap->utop; vu; vu = vu->below)
    fprintf(stderr, "%d, ", vu->idx);
  fprintf(stderr, "\n");
}
