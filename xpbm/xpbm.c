/* Copyright (C) 2000 Free Software Foundation, Inc. */

/* This file contains the gui initialization for the main window and 
   other initialization.
*/


#include <gtk/gtk.h>
#include <gdk_imlib.h>

#include "xpbm.h"

static GtkItemFactoryEntry menu_items[] = {
  { "/_File",          NULL,            NULL,          0,    "<Branch>" },
#if 0
  { "/File/_New",      NULL,            NULL,          0,    NULL       },
  { "/File/_Open ...",     NULL,            NULL,          0,    NULL },
#endif
  { "/File/_Import VMap ...",     NULL,  import_vmap_file_selection,          0,    NULL },
  { "/File/E_xport VMap ...",     NULL,  export_vmap_file_selection,          0,    NULL },
  /*  { "/File/_Mail VMap ...",     NULL,  mail_vmap_file,          0,    NULL },*/
#if 0
  { "/File/_Save",     NULL,            NULL,          0,    NULL },
  { "/File/Save _As ...",  NULL,            NULL,          0,    NULL },
  { "/File/sep1",      NULL,            NULL,          0,    "<Separator>" },
  { "/File/_Print",  NULL,            NULL,          0,    NULL },
#endif
  { "/File/sep2",      NULL,            NULL,          0,    "<Separator>" },
  { "/File/_Erase Lines",     NULL,      erase_lines,          0,    NULL },
  { "/File/_Clear Comments",     NULL,      clear_comments,    0,    NULL },
  { "/File/sep3",      NULL,            NULL,          0,    "<Separator>" },
  { "/File/Quit",      NULL,  xpbm_quit_gui,          0,    NULL },
#if 1
  { "/_Turn",         NULL,            NULL,          0,    "<Branch>" },
  { "/Turn/Roll _Dice", NULL,     NULL, 0,    NULL },
  /*  { "/Turn/Send _Turn", NULL,     NULL,          0,    NULL },
  { "/Turn/_Restart Turn", NULL,     NULL, 0,    NULL },
  { "/Turn/_Previous",  NULL,            NULL,          0,    NULL },
  { "/Turn/_Next",  NULL,            NULL,          0,    NULL },
  */
#endif

  { "/_Options",      NULL,  NULL,          0,   "<Branch>" },
  { "/Options/_Settings",      NULL,  settings_dialog,          0,   NULL },

#if 0
  { "/_Help",          NULL,            NULL,          0,    "<LastBranch>" },
  { "/Help/_About",    NULL,            NULL,          0,    NULL },
  { "/Help/Topics",    NULL,            NULL,          0,    NULL },
  { "/Help/T_utorial", NULL,            NULL,          0,    NULL },
#endif
};


GtkWidget *mainwin, *mb, *main_vb, *c_prop_vb;
GtkWidget *viewport;
GtkWidget *mapcanvas;
GtkWidget *mail_widg;

extern GtkWidget *c_dialog, *c_scr, *c_canvas;
extern GtkWidget *c_prop_d, *c_prop_del, *c_prop_flip, 
  *c_prop_change, *c_prop_note;
GtkWidget *l_dialog, *l_hbox, *lt_vbox, *lc_vbox;
GtkWidget *l_l[8];
GtkWidget *l_ldec[8];       /* decorations, display line */
GtkWidget *l_c[8];
GtkWidget *l_cdec[8];       /* display colors */
GtkWidget *l_done;
GtkWidget *settings_widg;

int 
main (int argc, char **argv)
{
  int i;
  
  gtk_set_locale();
  
  gtk_init (&argc, &argv);
  gdk_imlib_init();
   

  gtk_widget_push_visual(gdk_imlib_get_visual());
  gtk_widget_push_colormap(gdk_imlib_get_colormap());
  mainwin = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  
  main_vb = gtk_vbox_new(FALSE, 1);
  gtk_container_add(GTK_CONTAINER(mainwin), main_vb);
  gtk_widget_show(main_vb);

  /* 
   *                            set up menus 
   */
  get_main_menu(mainwin, &mb);
  gtk_box_pack_start(GTK_BOX(main_vb), mb, FALSE, TRUE, 0);
  gtk_widget_show(mb);

  /* 
   *                 set up scrolling map viewing area 
   */
  /* fixme:  have a nice arrow button feature for easier scrolling */
  viewport = gtk_scrolled_window_new(NULL, NULL);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(viewport), 
				 GTK_POLICY_ALWAYS, GTK_POLICY_ALWAYS);
  gtk_box_pack_start(GTK_BOX(main_vb), viewport, TRUE, TRUE, 0);

  /* fixme: get map dims from rc file */
  gtk_widget_set_usize(viewport, 640, 480);
  mapcanvas = gtk_drawing_area_new();

  gtk_drawing_area_size(GTK_DRAWING_AREA(mapcanvas), 640, 480);

  gtk_signal_connect(GTK_OBJECT(mapcanvas), "expose_event",
		     (GtkSignalFunc)map_expose_event, NULL);
  gtk_widget_set_events(mapcanvas, 
			GDK_EXPOSURE_MASK |
			GDK_POINTER_MOTION_MASK | 
			GDK_POINTER_MOTION_HINT_MASK |
			GDK_BUTTON_MOTION_MASK | GDK_BUTTON_PRESS_MASK |
			GDK_BUTTON_RELEASE_MASK);

  
  gtk_signal_connect(GTK_OBJECT(mapcanvas), "button_press_event",
		     (GtkSignalFunc)map_button_press_event, NULL);
  gtk_signal_connect(GTK_OBJECT(mapcanvas), "button_release_event",
		     (GtkSignalFunc)map_button_release_event, NULL);
  gtk_signal_connect(GTK_OBJECT(mapcanvas), "motion_notify_event",
		     (GtkSignalFunc)map_motion_event, NULL);

  gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(viewport), mapcanvas);
  gtk_widget_show(mapcanvas);
  gtk_widget_show(viewport);

  /* 
   *                     set up the counter selection dialog 
   */

  c_dialog = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW(c_dialog), "Available Counters");
  c_scr = gtk_scrolled_window_new(NULL, NULL);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(c_scr), 
				 GTK_POLICY_ALWAYS, GTK_POLICY_ALWAYS);
  gtk_container_add(GTK_CONTAINER(c_dialog), c_scr);

  c_canvas = gtk_drawing_area_new();

  gtk_drawing_area_size(GTK_DRAWING_AREA(c_canvas), 400, 300);
  gtk_widget_set_usize(c_scr, 640, 480);


  gtk_signal_connect(GTK_OBJECT(c_dialog), "delete_event",
		     (GtkSignalFunc)popdown_c_select_tile, NULL);
  
  gtk_signal_connect(GTK_OBJECT(c_canvas), "expose_event",
		     (GtkSignalFunc)tiles_expose_event, NULL);

  gtk_widget_set_events(c_canvas,
			GDK_EXPOSURE_MASK | GDK_BUTTON_PRESS_MASK |
			GDK_BUTTON_RELEASE_MASK |
			GDK_KEY_PRESS_MASK);

  gtk_signal_connect(GTK_OBJECT(c_canvas), "button_press_event",
		     (GtkSignalFunc)tiles_button_press_event, NULL);
  /* the dialog box will have the keyboard focus, send key events to it. */
  gtk_signal_connect(GTK_OBJECT(c_dialog), "key_press_event",
		     (GtkSignalFunc)tiles_key_press_event, NULL);

  gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(c_scr), c_canvas);

  gtk_widget_show(c_scr);
  gtk_widget_show(c_canvas);


  /* 
   *                   set up the counter properties dialog
   */
  c_prop_d = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW(c_prop_d), "Counter Properites");
  c_prop_vb     = gtk_vbox_new(TRUE, 0);
  c_prop_del    = gtk_button_new_with_label("Delete Counter");
  c_prop_change = gtk_button_new_with_label("Change Counter");
  c_prop_flip   = gtk_button_new_with_label("Flip Counter");
  c_prop_note   = gtk_entry_new();

  gtk_signal_connect(GTK_OBJECT(c_prop_d), "delete_event",
		     (GtkSignalFunc)popdown_c_prop, NULL);
  gtk_signal_connect(GTK_OBJECT(c_prop_d), "key_press_event",
		     (GtkSignalFunc)c_prop_key_press_event, NULL);
  gtk_signal_connect(GTK_OBJECT(c_prop_flip), "clicked",
		     (GtkSignalFunc)flip_counter, NULL);
  gtk_signal_connect(GTK_OBJECT(c_prop_del), "clicked",
		     (GtkSignalFunc)delete_counter, NULL);
  gtk_signal_connect(GTK_OBJECT(c_prop_change), "clicked",
		     (GtkSignalFunc)change_counter, NULL);

  gtk_container_add(GTK_CONTAINER(c_prop_d), c_prop_vb);

  gtk_box_pack_start(GTK_BOX(c_prop_vb), c_prop_del,    FALSE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(c_prop_vb), c_prop_change, FALSE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(c_prop_vb), c_prop_flip,   FALSE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(c_prop_vb), c_prop_note,   FALSE, TRUE, 0);

  gtk_widget_show(c_prop_del   );
  gtk_widget_show(c_prop_change);
  gtk_widget_show(c_prop_flip  );
  gtk_widget_show(c_prop_note  );
  gtk_widget_show(c_prop_vb);

  /* 
   *                     Now set up the line options dialog 
   */
  l_dialog = gtk_dialog_new();
  gtk_window_set_title(GTK_WINDOW(l_dialog), "Line Options");
  l_done = gtk_button_new_with_label("Done");
  gtk_signal_connect_object (GTK_OBJECT (l_done), "clicked",
			     GTK_SIGNAL_FUNC (gtk_widget_destroy), 
			     GTK_OBJECT(l_dialog));
  gtk_container_add (GTK_CONTAINER (GTK_DIALOG(l_dialog)->action_area),
		     l_done);
  l_hbox = gtk_hbox_new(TRUE, TRUE);
  lt_vbox = gtk_vbox_new(TRUE, TRUE);
  lc_vbox = gtk_vbox_new(TRUE, TRUE);
  gtk_container_add (GTK_CONTAINER (GTK_DIALOG(l_dialog)->vbox),
		     l_hbox);
  gtk_container_add (GTK_CONTAINER (l_hbox),
		     lt_vbox);
  gtk_container_add (GTK_CONTAINER (l_hbox),
		     lc_vbox);
  l_l[0] = gtk_radio_button_new(NULL);
  l_c[0] = gtk_radio_button_new(NULL);
  gtk_box_pack_start(GTK_BOX(lt_vbox), l_l[0], TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(lc_vbox), l_c[0], TRUE, TRUE, 0);
  for(i = 1; i < 8; i++) {
    l_l[i] = gtk_radio_button_new_from_widget(GTK_RADIO_BUTTON(l_l[0]));
    l_c[i] = gtk_radio_button_new_from_widget(GTK_RADIO_BUTTON(l_c[0]));
    /* fixme:  put in the pixmaps here w/ dashed lines and colors */
    gtk_box_pack_start(GTK_BOX(lt_vbox), l_l[i], TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(lc_vbox), l_c[i], TRUE, TRUE, 0);
  }
  gtk_widget_show_all(l_hbox);
  
  gtk_widget_show(l_done);
  /*gtk_widget_show(l_dialog);*/

  /* Set up settings */

  settings_widg = create_set_window2 ();
  mail_widg = create_mail_window1();
  gtk_widget_show(mainwin);
  
  gtk_main();
  
  gtk_exit (0);
}

static void 
get_main_menu(GtkWidget *w, GtkWidget **mb)
{
  GtkItemFactory *item_f;
  GtkAccelGroup *accel_g;
  gint n_mitems = sizeof(menu_items) / sizeof(menu_items[0]);

  accel_g = gtk_accel_group_new();
  item_f = gtk_item_factory_new(GTK_TYPE_MENU_BAR, "<main>", accel_g);
  gtk_item_factory_create_items(item_f, n_mitems, menu_items, NULL);
  gtk_window_add_accel_group(GTK_WINDOW(w), accel_g);

  if(mb)
    *mb = gtk_item_factory_get_widget(item_f, "<main>");
}


static void 
xpbm_quit_gui(GtkWidget *w, gpointer data)
{
  /* save state, close files etc. */

  gtk_main_quit();
}

