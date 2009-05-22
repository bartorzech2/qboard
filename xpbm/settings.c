#include <gtk/gtk.h>
#include <gdk_imlib.h>

#include "xpbm.h"
extern GtkWidget *settings_widg;

static GtkWidget *spinbutton1;
static  GtkWidget *spinbutton2;
static  GtkWidget *spinbutton3;

struct xpbm_settings settings = {
  1,         /* stack_offset_x */
  3,         /* stack_offset_y */
  15         /* autoscroll_marg */
};

struct xpbm_settings *pset = &settings;

void
settings_dialog()
{
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(spinbutton1), 
			    settings.stack_off_x);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(spinbutton2), 
			    settings.stack_off_y);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(spinbutton3), 
			    settings.autoscroll_marg);
  gtk_widget_show(settings_widg);
}

GtkWidget*
create_set_window2 (void)
{
  GtkWidget *set_window2;
  GtkWidget *vbox1;
  GtkWidget *table1;
  GtkWidget *set_label1;
  GtkWidget *set_label2;
  GtkWidget *set_label3;
  GtkObject *spinbutton1_adj;
  GtkObject *spinbutton2_adj;
  GtkObject *spinbutton3_adj;
  GtkWidget *hbuttonbox1;
  GtkWidget *set_button1;
  GtkWidget *set_button2;

  set_window2 = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_object_set_data (GTK_OBJECT (set_window2), "set_window2", set_window2);
  gtk_window_set_title (GTK_WINDOW (set_window2), "XPBM Settings");

  vbox1 = gtk_vbox_new (FALSE, 0);
  gtk_widget_ref (vbox1);
  gtk_object_set_data_full (GTK_OBJECT (set_window2), "vbox1", vbox1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (vbox1);
  gtk_container_add (GTK_CONTAINER (set_window2), vbox1);

  table1 = gtk_table_new (3, 2, FALSE);
  gtk_widget_ref (table1);
  gtk_object_set_data_full (GTK_OBJECT (set_window2), "table1", table1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (table1);
  gtk_box_pack_start (GTK_BOX (vbox1), table1, TRUE, TRUE, 0);

  set_label1 = gtk_label_new ("Stacking X Offset ");
  gtk_widget_ref (set_label1);
  gtk_object_set_data_full (GTK_OBJECT (set_window2), "set_label1", set_label1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (set_label1);
  gtk_table_attach (GTK_TABLE (table1), set_label1, 0, 1, 0, 1,
                    (GtkAttachOptions) (0),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (set_label1), GTK_JUSTIFY_RIGHT);




  set_label2 = gtk_label_new ("Stacking Y Offset");
  gtk_widget_ref (set_label2);
  gtk_object_set_data_full (GTK_OBJECT (set_window2), "set_label2", set_label2,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (set_label2);
  gtk_table_attach (GTK_TABLE (table1), set_label2, 0, 1, 1, 2,
                    (GtkAttachOptions) (0),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (set_label2), GTK_JUSTIFY_RIGHT);

  spinbutton1_adj = gtk_adjustment_new (1, 0, 100, 1, 10, 10);
  spinbutton1 = gtk_spin_button_new (GTK_ADJUSTMENT (spinbutton1_adj), 1, 0);
  gtk_widget_ref (spinbutton1);
  gtk_object_set_data_full (GTK_OBJECT (set_window2), "spinbutton1", spinbutton1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (spinbutton1);
  gtk_table_attach (GTK_TABLE (table1), spinbutton1, 1, 2, 0, 1,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  spinbutton2_adj = gtk_adjustment_new (1, 0, 100, 1, 10, 10);
  spinbutton2 = gtk_spin_button_new (GTK_ADJUSTMENT (spinbutton2_adj), 1, 0);
  gtk_widget_ref (spinbutton2);
  gtk_object_set_data_full (GTK_OBJECT (set_window2), "spinbutton2", spinbutton2,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (spinbutton2);
  gtk_table_attach (GTK_TABLE (table1), spinbutton2, 1, 2, 1, 2,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);



  set_label3 = gtk_label_new ("Autoscroll Margin Width");
  gtk_widget_ref (set_label3);
  gtk_object_set_data_full (GTK_OBJECT (set_window2), "set_label3", set_label3,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (set_label3);
  gtk_table_attach (GTK_TABLE (table1), set_label3, 0, 1, 2, 3,
                    (GtkAttachOptions) (0),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (set_label3), GTK_JUSTIFY_RIGHT);

  spinbutton3_adj = gtk_adjustment_new (1, 0, 100, 1, 10, 10);
  spinbutton3 = gtk_spin_button_new (GTK_ADJUSTMENT (spinbutton3_adj), 1, 0);
  gtk_widget_ref (spinbutton3);
  gtk_object_set_data_full (GTK_OBJECT (set_window2), "spinbutton3", spinbutton3,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (spinbutton3);
  gtk_table_attach (GTK_TABLE (table1), spinbutton3, 1, 2, 2, 3,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);







  hbuttonbox1 = gtk_hbutton_box_new ();
  gtk_widget_ref (hbuttonbox1);
  gtk_object_set_data_full (GTK_OBJECT (set_window2), "hbuttonbox1", hbuttonbox1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (hbuttonbox1);
  gtk_box_pack_start (GTK_BOX (vbox1), hbuttonbox1, FALSE, TRUE, 0);
  gtk_button_box_set_layout (GTK_BUTTON_BOX (hbuttonbox1), GTK_BUTTONBOX_SPREAD);

  set_button1 = gtk_button_new_with_label ("OK");
  gtk_widget_ref (set_button1);
  gtk_object_set_data_full (GTK_OBJECT (set_window2), "set_button1", set_button1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_signal_connect(GTK_OBJECT(set_button1), "clicked",
		     (GtkSignalFunc)accept_settings, NULL);
  gtk_widget_show (set_button1);
  gtk_container_add (GTK_CONTAINER (hbuttonbox1), set_button1);
  GTK_WIDGET_SET_FLAGS (set_button1, GTK_CAN_DEFAULT);

  set_button2 = gtk_button_new_with_label ("Cancel");
  gtk_widget_ref (set_button2);
  gtk_object_set_data_full (GTK_OBJECT (set_window2), "set_button2", set_button2,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_signal_connect(GTK_OBJECT(set_button2), "clicked",
		     (GtkSignalFunc)cancel_settings, NULL);
  gtk_widget_show (set_button2);
  gtk_container_add (GTK_CONTAINER (hbuttonbox1), set_button2);
  GTK_WIDGET_SET_FLAGS (set_button2, GTK_CAN_DEFAULT);

  return set_window2;
}

gint 
accept_settings(GtkWidget *w, gpointer d)
{
  settings.stack_off_x 
    = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spinbutton1));
  settings.stack_off_y 
    = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spinbutton2));
  settings.autoscroll_marg
    = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spinbutton3));
  gtk_widget_hide(settings_widg);
}

gint 
cancel_settings(GtkWidget *w, gpointer d)
{
  gtk_widget_hide(settings_widg);
}

