/* fixme: THis leaks like crazy!  Free those file selectors, or keep them
 * around! */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>
#include <gdk_imlib.h>
#include "xpbm.h"

extern struct vmap_data *vmap;
static GtkWidget *file_selector;
extern GtkWidget *mail_widg;
GtkWidget *mail_entry1;
GtkWidget *mail_entry2;
GtkWidget *mail_checkbutton1;

static void cancel_fileselection(GtkWidget *w, gpointer data);
static void load_vmap_file(GtkWidget *w, gpointer data);
static int save_vmap_file(GtkWidget *w, gpointer data);


static int selection_popped_up = 0;

/* fixme:  unify all of the silly different file selector things */
void 
import_vmap_file_selection(void) 
{
  if(selection_popped_up)
    return;
  selection_popped_up = 1;

  /* Create the selector */
  
  file_selector = gtk_file_selection_new("Import a VMAP file");
  
  /* Ensure that the dialog box is destroyed when the user clicks a button. */
  
  gtk_signal_connect_object (GTK_OBJECT (GTK_FILE_SELECTION(file_selector)->ok_button),
			     "clicked", GTK_SIGNAL_FUNC (load_vmap_file),
			     (gpointer) file_selector);

  /* get rid of that filename if cancelled or window manager closes */
  gtk_signal_connect_object (GTK_OBJECT (GTK_FILE_SELECTION(file_selector)->cancel_button),
			     "clicked", GTK_SIGNAL_FUNC (cancel_fileselection),
			     (gpointer) file_selector);
  
  /* get rid of that filename if cancelled or window manager closes */
  gtk_signal_connect_object (GTK_OBJECT (GTK_FILE_SELECTION(file_selector)->cancel_button),
			     "destroy", GTK_SIGNAL_FUNC (cancel_fileselection),
			     (gpointer) file_selector);

  /* fixme:  attach this to the appropriate list objects */
  gtk_file_selection_complete(GTK_FILE_SELECTION(file_selector), VMAP_PATTERN);
  gtk_file_selection_hide_fileop_buttons(GTK_FILE_SELECTION(file_selector));

  /* Display that dialog */
  gtk_widget_show (file_selector);
}

static void
cancel_fileselection(GtkWidget *w, gpointer data)
{
  gtk_widget_destroy(w);
  selection_popped_up = 0;
}

static void
load_vmap_file(GtkWidget *w, gpointer data)
{
  char *selected_filename = 
    (char *)strdup(gtk_file_selection_get_filename (GTK_FILE_SELECTION(w)));
  selection_popped_up = 0;

  parse_vmap_file(selected_filename);
  free(selected_filename);
       /* fixme, error dialog here on failure */
  gtk_widget_destroy(w);
}

void 
export_vmap_file_selection(void) {
  if(selection_popped_up)
    return;
  selection_popped_up = 1;

  /* Create the selector */
  
  file_selector = gtk_file_selection_new("Export a VMAP file");
  
  /* Ensure that the dialog box is destroyed when the user clicks a button. */
  
  gtk_signal_connect_object (GTK_OBJECT (GTK_FILE_SELECTION(file_selector)->ok_button),
			     "clicked", GTK_SIGNAL_FUNC (save_vmap_file),
			     (gpointer) file_selector);

  /* get rid of that filename if cancelled or window manager closes */
  gtk_signal_connect_object (GTK_OBJECT (GTK_FILE_SELECTION(file_selector)->cancel_button),
			     "clicked", GTK_SIGNAL_FUNC (cancel_fileselection),
			     (gpointer) file_selector);
  
  /* get rid of that filename if cancelled or window manager closes */
  gtk_signal_connect_object (GTK_OBJECT (GTK_FILE_SELECTION(file_selector)->cancel_button),
			     "destroy", GTK_SIGNAL_FUNC (cancel_fileselection),
			     (gpointer) file_selector);

  /* fixme:  attach this to the appropriate list objects */
  gtk_file_selection_complete(GTK_FILE_SELECTION(file_selector), VMAP_PATTERN);
  gtk_file_selection_hide_fileop_buttons(GTK_FILE_SELECTION(file_selector));

  /* Display that dialog */
  gtk_widget_show (file_selector);
}

void
mail_vmap_file(void)
{
  if(! vmap)
    return;
  gtk_widget_show(mail_widg);
}

static int 
save_vmap_file(GtkWidget *w, gpointer data)
{
  FILE *f;
  char *selected_filename = 
    (char *)strdup(gtk_file_selection_get_filename (GTK_FILE_SELECTION(w)));

  if(! vmap)
    return -1;
  selection_popped_up = 0;

  if(! (f = fopen(selected_filename, "w"))) {
    return -1;
  }
  
  free(selected_filename);
       /* fixme, error dialog here on failure */
  write_vmap_to_file_pointer(f);
  fclose(f);
  gtk_widget_destroy(w);
}

void
write_vmap_to_file_pointer(FILE *f)
{
  struct vmap_unit *vu;
  struct vmap_line *vl;
  fprintf(f, "# V_MAP save file\r\n");
  fprintf(f, "# VERSION ver\r\n");
  fprintf(f, "VERSION %s\r\n", VMAP_FORMAT_VERSION);
  fprintf(f, "# MAP step_size max_top max_left path/name\r\n");
  fprintf(f, "MAP %d %d %d %s\r\n", vmap->step_size, vmap->max_top,
	  vmap->max_left, vmap->map);
  fprintf(f, "\r\n");
  fprintf(f, "# TILES rows columns path/name\r\n");
  fprintf(f, "TILES %d %d %s\r\n", vmap->c_rows, vmap->c_columns, vmap->tiles);
  fprintf(f, "\r\n");
  fprintf(f, "# STACKING ON=auto_stack\r\n");
  fprintf(f, "STACKING OFF\r\n");
  fprintf(f, "\r\n");
  fprintf(f, "# UNIT top left in_play bitmap_index comment\r\n");
  for(vu = vmap->ubot; vu; vu = vu->above)
    fprintf(f, "UNIT %-6d %-6d %-4d %-5d %-4d %-4d %s\r\n", 
	    vu->y, vu->x, 1, vu->idx, 
	    /* Don't know what these are for, new fields in VMAP def. */
	    0, 0, vu->comment ? vu->comment : "");
  fprintf(f, "\r\n");
  fprintf(f, "# LINE x0 y0 x1 y1\r\n");
  for(vl = vmap->lines; vl; vl = vl->n)
    fprintf(f, "LINE %d %d %d %d %d %d\r\n", 
	    vl->x[0], vl->y[0], vl->x[1], vl->y[1], vl->style, vl->color);
}

GtkWidget*
create_mail_window1 (void)
{
  GtkWidget *mail_window1;
  GtkWidget *mail_vbox1;
  GtkWidget *mail_table1;
  GtkWidget *mail_label1;
  GtkWidget *mail_label2;
  GtkWidget *mail_label3;
  GtkWidget *mail_vbox2;
  GtkWidget *mail_hseparator1;
  GtkWidget *mail_hbuttonbox1;
  GtkWidget *mail_ok;
  GtkWidget *mail_cancel;

  mail_window1 = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_object_set_data (GTK_OBJECT (mail_window1), "mail_window1", mail_window1);
  gtk_window_set_title (GTK_WINDOW (mail_window1), "Mail VMAP File");

  mail_vbox1 = gtk_vbox_new (FALSE, 0);
  gtk_widget_ref (mail_vbox1);
  gtk_object_set_data_full (GTK_OBJECT (mail_window1), "mail_vbox1", mail_vbox1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (mail_vbox1);
  gtk_container_add (GTK_CONTAINER (mail_window1), mail_vbox1);

  mail_table1 = gtk_table_new (3, 2, FALSE);
  gtk_widget_ref (mail_table1);
  gtk_object_set_data_full (GTK_OBJECT (mail_window1), "mail_table1", mail_table1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (mail_table1);
  gtk_box_pack_start (GTK_BOX (mail_vbox1), mail_table1, TRUE, TRUE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (mail_table1), 13);

  mail_entry1 = gtk_entry_new ();
  gtk_widget_ref (mail_entry1);
  gtk_object_set_data_full (GTK_OBJECT (mail_window1), "mail_entry1", mail_entry1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (mail_entry1);
  gtk_table_attach (GTK_TABLE (mail_table1), mail_entry1, 1, 2, 0, 1,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_widget_set_usize (mail_entry1, 300, -2);

  mail_entry2 = gtk_entry_new ();
  gtk_widget_ref (mail_entry2);
  gtk_object_set_data_full (GTK_OBJECT (mail_window1), "mail_entry2", mail_entry2,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (mail_entry2);
  gtk_table_attach (GTK_TABLE (mail_table1), mail_entry2, 1, 2, 1, 2,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_widget_set_usize (mail_entry2, 300, -2);

  mail_label1 = gtk_label_new ("Save VMAP file?");
  gtk_widget_ref (mail_label1);
  gtk_object_set_data_full (GTK_OBJECT (mail_window1), "mail_label1", mail_label1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (mail_label1);
  gtk_table_attach (GTK_TABLE (mail_table1), mail_label1, 0, 1, 2, 3,
                    (GtkAttachOptions) (0),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_padding (GTK_MISC (mail_label1), 15, 0);

  mail_label2 = gtk_label_new ("Send mail to:");
  gtk_widget_ref (mail_label2);
  gtk_object_set_data_full (GTK_OBJECT (mail_window1), "mail_label2", mail_label2,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (mail_label2);
  gtk_table_attach (GTK_TABLE (mail_table1), mail_label2, 0, 1, 0, 1,
                    (GtkAttachOptions) (0),
                    (GtkAttachOptions) (0), 0, 0);

  mail_label3 = gtk_label_new ("Mail command:");
  gtk_widget_ref (mail_label3);
  gtk_object_set_data_full (GTK_OBJECT (mail_window1), "mail_label3", mail_label3,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (mail_label3);
  gtk_table_attach (GTK_TABLE (mail_table1), mail_label3, 0, 1, 1, 2,
                    (GtkAttachOptions) (0),
                    (GtkAttachOptions) (0), 0, 0);

  mail_checkbutton1 = gtk_check_button_new_with_label ("");
  gtk_widget_ref (mail_checkbutton1);
  gtk_object_set_data_full (GTK_OBJECT (mail_window1), "mail_checkbutton1", mail_checkbutton1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (mail_checkbutton1);
  gtk_table_attach (GTK_TABLE (mail_table1), mail_checkbutton1, 1, 2, 2, 3,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (mail_checkbutton1), TRUE);

  mail_vbox2 = gtk_vbox_new (FALSE, 0);
  gtk_widget_ref (mail_vbox2);
  gtk_object_set_data_full (GTK_OBJECT (mail_window1), "mail_vbox2", mail_vbox2,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (mail_vbox2);
  gtk_box_pack_start (GTK_BOX (mail_vbox1), mail_vbox2, FALSE, TRUE, 0);

  mail_hseparator1 = gtk_hseparator_new ();
  gtk_widget_ref (mail_hseparator1);
  gtk_object_set_data_full (GTK_OBJECT (mail_window1), "mail_hseparator1", mail_hseparator1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (mail_hseparator1);
  gtk_box_pack_start (GTK_BOX (mail_vbox2), mail_hseparator1, TRUE, TRUE, 0);

  mail_hbuttonbox1 = gtk_hbutton_box_new ();
  gtk_widget_ref (mail_hbuttonbox1);
  gtk_object_set_data_full (GTK_OBJECT (mail_window1), "mail_hbuttonbox1", mail_hbuttonbox1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (mail_hbuttonbox1);
  gtk_box_pack_start (GTK_BOX (mail_vbox2), mail_hbuttonbox1, TRUE, TRUE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (mail_hbuttonbox1), 10);
  gtk_button_box_set_layout (GTK_BUTTON_BOX (mail_hbuttonbox1), GTK_BUTTONBOX_SPREAD);

  mail_ok = gtk_button_new_with_label ("OK");
  gtk_widget_ref (mail_ok);
  gtk_object_set_data_full (GTK_OBJECT (mail_window1), "mail_ok", mail_ok,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (mail_ok);
  gtk_container_add (GTK_CONTAINER (mail_hbuttonbox1), mail_ok);
  GTK_WIDGET_SET_FLAGS (mail_ok, GTK_CAN_DEFAULT);

  mail_cancel = gtk_button_new_with_label ("Cancel");
  gtk_widget_ref (mail_cancel);
  gtk_object_set_data_full (GTK_OBJECT (mail_window1), "mail_cancel", mail_cancel,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (mail_cancel);
  gtk_container_add (GTK_CONTAINER (mail_hbuttonbox1), mail_cancel);
  GTK_WIDGET_SET_FLAGS (mail_cancel, GTK_CAN_DEFAULT);

  gtk_signal_connect (GTK_OBJECT (mail_checkbutton1), "toggled",
                      GTK_SIGNAL_FUNC (on_mail_checkbutton1_toggled),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (mail_ok), "clicked",
                      GTK_SIGNAL_FUNC (on_mail_ok_clicked),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (mail_cancel), "clicked",
                      GTK_SIGNAL_FUNC (on_mail_cancel_clicked),
                      NULL);

  return mail_window1;
}

void
on_mail_cancel_clicked                 (GtkButton       *button,
                                        gpointer         user_data)
{
  gtk_widget_hide(mail_widg);
}


void
on_mail_checkbutton1_toggled           (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{

}


void
on_mail_ok_clicked                     (GtkButton       *button,
                                        gpointer         user_data)
{
  char *addr;
  char *comstr;
  int save 
    = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(mail_checkbutton1));
  FILE *f ;
  gtk_widget_hide(mail_widg);

  addr = strdup(gtk_entry_get_text(GTK_ENTRY(mail_entry1)));
  comstr = strdup(gtk_entry_get_text(GTK_ENTRY(mail_entry2)));

  printf("Sending mail to %s using %s\n", addr, comstr);
  /* fixme:  Pop up an error box */
  /* fixme: Make this work */
  if(!( f = popen(comstr, "w")))
    goto cleanup;

  write_vmap_to_file_pointer(f);
  pclose(f);

  if(save)
    export_vmap_file_selection();

 cleanup:
  free(addr);
  free(comstr);
}

