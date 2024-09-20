/* sensors-view.c
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


#include <sensors/sensors.h>
#include <gtk/gtk.h>

#include "sensors-view.h"
#include "sensors-info.h"



//SensorsInfo GObject begin
struct _SensorsView{
  GObject parent_instance;
  GtkColumnView* column_view;
  GtkTreeListModel* tree;
  gboolean is_monitoring;
  GThread* monitoring;

};
G_DEFINE_TYPE(SensorsView,sensors_view,G_TYPE_OBJECT)

static void sensors_view_dispose (GObject *object){
  SensorsView* view=SENSORS_VIEW(object);
  if (view->is_monitoring){
    sensors_view_stop_monitoring(view);
  }
  sensors_cleanup();
  g_object_unref(view->column_view);
  g_object_unref(view->tree);
  g_object_unref(view->monitoring);
  G_OBJECT_CLASS (sensors_view_parent_class)->dispose(object);
}

static void sensors_view_finalize(GObject *object){
  SensorsView* view=SENSORS_VIEW(object);
  g_free(&view->is_monitoring);
  G_OBJECT_CLASS (sensors_view_parent_class)->finalize(object);
}

static void sensors_view_init(SensorsView *view){


}
static void sensors_view_class_init(SensorsViewClass *class){
  GObjectClass *object_class = G_OBJECT_CLASS (class);
  object_class->dispose=sensors_view_dispose;
  object_class->finalize =sensors_view_finalize;
}

static void update_tree_list_model(gpointer data){
  GtkTreeListModel* tree=data;
  int i=0;
  GtkTreeListRow* row=gtk_tree_list_model_get_row(tree,i);
  while (row!=NULL){
    GObject* item=gtk_tree_list_row_get_item(row);
    sensors_info_update_value(SENSORS_INFO(item));
    g_object_unref(item);
    g_object_unref(row);
    i++;
    row=gtk_tree_list_model_get_row(tree,i);
  }
}

static gpointer update_data_sensors_view(gpointer data){
  SensorsView* view=data;
  while (view->is_monitoring){
    g_idle_add(G_SOURCE_FUNC(update_tree_list_model),view->tree);
    g_usleep(100000);
  }
  return NULL;
}

bool sensors_view_start_monitoring(SensorsView* view){
  if (!view->is_monitoring){
    view->is_monitoring=true;
    view->monitoring=g_thread_new("monitoring",update_data_sensors_view,view);
    return true;
  }
  else{
    return false;
  }
}
bool sensors_view_stop_monitoring(SensorsView* view){
  if (view->is_monitoring){
    view->is_monitoring=false;
    g_thread_join(view->monitoring);
  return true;
  }
  else{
    return false;
  }
}

GtkColumnView* sensors_view_get_column_view(SensorsView* view){
  return view->column_view;
}

static void populate_gstore_chips(GListStore *store){
  int nr=0;
  while (nr>=0){
    const sensors_chip_name* chip;
    chip=sensors_get_detected_chips(NULL,&nr);//sensors_get_detected_chips increments nr during iteration
    if (chip==NULL){
      nr=-1;
    }
    else{
      g_list_store_append(store,sensors_chip_new(chip));
    }
  }
}

static GListModel* populate_gstore_sensors(gpointer item, gpointer user_data){
  SensorsInfo* chip=item;
  GListStore* sensors_store = g_list_store_new(G_TYPE_OBJECT);
  if (item==NULL || sensors_info_get_info_type(chip)!=0){
    return NULL;
  }
  else{
    const sensors_chip_name* chip_id=sensors_info_get_chip_id(chip);
    int nr=0; 
    while (nr>=0){
        const sensors_feature* feature;
        feature = sensors_get_features(chip_id,&nr);//sensors_get_features increments nr during iteration
        if (feature!=NULL){
          SensorsInfo* info = sensors_info_new(chip_id,feature);
           g_list_store_append(sensors_store,G_OBJECT(info));
           g_object_unref(info);
        }
        else{nr=-1;}         
        }
  }
  return G_LIST_MODEL(sensors_store);
} 

//setup how col data is display
static void setup_name(GtkBuilderListItemFactory *factory, GObject *listitem){
   GtkWidget *expander =gtk_tree_expander_new();
   GtkWidget *label=gtk_label_new(NULL);
   gtk_tree_expander_set_child(GTK_TREE_EXPANDER(expander),label);
    gtk_list_item_set_child(GTK_LIST_ITEM(listitem),expander);
}

static void setup_col(GtkBuilderListItemFactory *factory, GObject *listitem){
   GtkWidget* label =gtk_label_new(NULL);
    gtk_list_item_set_child(GTK_LIST_ITEM(listitem),label);
}

//bind data to factory
static void bind_name(GtkSignalListItemFactory *factory, GObject *listitem){
    GtkTreeExpander *expander = GTK_TREE_EXPANDER(gtk_list_item_get_child(GTK_LIST_ITEM(listitem)));
    GtkTreeListRow* row = gtk_list_item_get_item(GTK_LIST_ITEM(listitem));
    GObject* chip=gtk_tree_list_row_get_item(row);
    gtk_tree_expander_set_list_row(GTK_TREE_EXPANDER(expander),row);
    GtkWidget *label=gtk_tree_expander_get_child(expander);
    const char *string =sensors_info_get_name(SENSORS_INFO(chip));
    gtk_label_set_label(GTK_LABEL(label), string);
    //g_object_unref(string);
}
static gboolean from_value_to_char(GBinding* binding, const GValue* info_gvalue,GValue* label_gvalue, gpointer data){
  SensorsInfo* info=SENSORS_INFO(data);
    if (sensors_info_get_info_type(info)>0){
     const char* value=sensors_info_get_value_string(info);
     g_value_set_string(label_gvalue,value);
    }
    return true;
}

static void bind_value(GtkSignalListItemFactory *factory, GObject *listitem){
  GtkWidget* label=gtk_list_item_get_child(GTK_LIST_ITEM(listitem));
  GtkWidget* row=gtk_list_item_get_item(GTK_LIST_ITEM(listitem));
  SensorsInfo* info=gtk_tree_list_row_get_item(GTK_TREE_LIST_ROW(row));
  g_object_bind_property_full(info,"value",label,"label",G_BINDING_SYNC_CREATE,from_value_to_char,NULL,info,NULL);
  
}

static void create_columview_sensors_data(SensorsView* view){
	GListStore *chip_store = g_list_store_new(G_TYPE_OBJECT);
	populate_gstore_chips(chip_store);
	GListModel *list_chip =  G_LIST_MODEL(chip_store);  
  GtkTreeListModel* tree_list= gtk_tree_list_model_new(list_chip,false,true,populate_gstore_sensors,NULL,NULL);
  GtkNoSelection *selectmodel= gtk_no_selection_new(G_LIST_MODEL(tree_list));
  GtkWidget *cview = gtk_column_view_new(GTK_SELECTION_MODEL(selectmodel));
  gtk_column_view_set_show_row_separators(GTK_COLUMN_VIEW(cview),true);

  //col 1 -> Name
  GtkListItemFactory *fact1 = gtk_signal_list_item_factory_new();
  g_signal_connect(fact1,"setup",G_CALLBACK (setup_name),NULL);
  g_signal_connect(fact1,"bind",G_CALLBACK (bind_name),NULL);
  GtkColumnViewColumn *column = gtk_column_view_column_new("Name", fact1);
  gtk_column_view_append_column (GTK_COLUMN_VIEW (cview), column);
  gtk_column_view_column_set_expand(column,true);
  gtk_column_view_column_set_resizable(column,true);
  //col 2 -> Value
  GtkListItemFactory *fact2 = gtk_signal_list_item_factory_new();
  g_signal_connect(fact2,"setup",G_CALLBACK (setup_col),NULL);
  g_signal_connect(fact2,"bind",G_CALLBACK (bind_value),NULL);
  GtkColumnViewColumn *column2 = gtk_column_view_column_new("Value", fact2);
  gtk_column_view_append_column (GTK_COLUMN_VIEW (cview), column2);
  gtk_column_view_column_set_expand(column2,true);
  gtk_column_view_column_set_resizable(column2,true);
  view->column_view=GTK_COLUMN_VIEW(cview);
  view->tree=tree_list;

}

SensorsView* sensors_view_new(void){
  SensorsView *view=g_object_new(SENSORS_TYPE_VIEW,NULL);
  int init =  sensors_init(NULL);
  if (init==0){
   create_columview_sensors_data(view);
  }
  else{
    view=NULL;
    fprintf(stderr, "Failed to initialize sensors properly\n");
  }
  return view;
}