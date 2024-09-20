/* main.c
 *
 * Copyright 2024 Julien Gielen
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */


#include <stdlib.h>
#include <gtk/gtk.h>


#include "sensors-view.h"



static void start_monitoring(GSimpleAction *action, GVariant *parameter,gpointer view){
  bool has_started=sensors_view_start_monitoring(SENSORS_VIEW(view));
  if(has_started){
    printf("Monitoring has started\n");
  }
}

static void stop_monitoring(GSimpleAction *action, GVariant *parameter,gpointer view){
  bool has_stopped=sensors_view_stop_monitoring(SENSORS_VIEW(view));
  if (has_stopped){
    printf("Monitoring has stopped\n");
  }
}


static void show_about(GSimpleAction *action,GVariant *paramater, gpointer app){
   GtkWindow* window = gtk_application_get_active_window (GTK_APPLICATION (app));
   const char* authors[] = {"Julien Gielen", NULL};
  gtk_show_about_dialog(window,"program-name","Shaman","authors",authors,"copyright","© 2024 Julien Gielen","version","0.1","license-type",GTK_LICENSE_GPL_3_0,NULL);
}

static void app_quit(GSimpleAction *action, GVariant *parameter,gpointer app){
  g_application_quit(G_APPLICATION(app));
}

static void setup_actions(GApplication *app, SensorsView* view){

  //actions related to app
  GActionEntry actions_app[]={{"about",show_about,NULL,NULL,NULL},
                          {"quit",app_quit,NULL,NULL,NULL}};
  g_action_map_add_action_entries (G_ACTION_MAP (app),actions_app,G_N_ELEMENTS(actions_app),app); 
    const char* quit_shorcut[2]={"<Ctrl>q",NULL};
 gtk_application_set_accels_for_action(GTK_APPLICATION(app),"app.quit",quit_shorcut);
  //actions related to sensors view  
  GActionEntry actions_cv[] = {{"start-monitoring",start_monitoring,NULL,NULL,NULL},
                                {"stop-monitoring",stop_monitoring,NULL,NULL,NULL}};
  g_action_map_add_action_entries (G_ACTION_MAP (app),actions_cv,G_N_ELEMENTS(actions_cv),view);
  const char* start_monitoring_shorcut[2]={"<Ctrl>m",NULL};
  gtk_application_set_accels_for_action(GTK_APPLICATION(app),"app.start-monitoring",start_monitoring_shorcut);
  const char* stop_monitoring_shorcut[2]={"<Ctrl>P",NULL};
  gtk_application_set_accels_for_action(GTK_APPLICATION(app),"app.stop-monitoring",stop_monitoring_shorcut);
 }

static void activate(GtkApplication* app,gpointer user_data){

  GtkBuilder* builder=gtk_builder_new_from_file("./views/shaman-window.ui");
  GObject* window=gtk_builder_get_object(builder,"shaman-window");
  gtk_window_set_application(GTK_WINDOW(window),app);
  GObject* scrolled_window=gtk_builder_get_object(builder,"scrolled-window");
  GObject* menu=gtk_builder_get_object(builder,"primary-menu");
  gtk_application_set_menubar(app,G_MENU_MODEL(menu));
  SensorsView* view=sensors_view_new();
  GtkColumnView* cview=sensors_view_get_column_view(view);
  gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolled_window),GTK_WIDGET(cview));
  setup_actions(G_APPLICATION(app),view);
  gtk_window_present (GTK_WINDOW(window));
  g_object_unref(builder);

}


int main(int argc, char *argv[]){

	GtkApplication *app;

  app = gtk_application_new ("org.gielensoftware.shaman", G_APPLICATION_DEFAULT_FLAGS);
  g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
  int status;
  status = g_application_run (G_APPLICATION (app), argc, argv);
  g_object_unref (app);
  return status;
 	
}
