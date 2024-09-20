/* sensors-info.c
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
#include "sensors-info.h"



//SensorsInfo GObject begin
struct _SensorsInfo{
  GObject parent_instance;
  const sensors_chip_name *chip_id;
  gchar* name;
  gdouble value;
  enum sensors_type sensors_type;
  guint subfeature;

};
G_DEFINE_TYPE(SensorsInfo,sensors_info,G_TYPE_OBJECT)

void sensors_info_update_value(SensorsInfo* info){
  sensors_get_value(info->chip_id,info->subfeature,&info->value);
  g_object_notify(G_OBJECT(info),"value");
}

static void sensors_info_determine_sensors_type (SensorsInfo* info,const sensors_feature* feature){
  sensors_feature_type sensors_type_org = feature->type;
  const sensors_subfeature* subfeature;
  switch (sensors_type_org){

    //case sensors is FAN
    case 0x01:
      info->sensors_type=1;
      subfeature =  sensors_get_subfeature(info->chip_id, feature, 0x01<<8);
      info->subfeature=subfeature->number;
    break;
        //case sensors is TEMP
    case 0x02:
      info->sensors_type=2;
      subfeature =  sensors_get_subfeature(info->chip_id, feature, 0x02<<8);
      info->subfeature=subfeature->number;
    break;
        //case sensors is POWER
    case 0x03:
      info->sensors_type=3;
      subfeature =  sensors_get_subfeature(info->chip_id, feature, 0x03<<8);
      info->subfeature=subfeature->number;
    break;
            //case sensors is ENERGY
    /*case 0x04:
      info->sensors_type=4;
      subfeature =  sensors_get_subfeature(info->chip_id, feature, 0x04<<8);
      info->subfeature=subfeature->number;
    break;*/
            //case sensors is CURRENT
    case 0x05:
      info->sensors_type=5;
      subfeature =  sensors_get_subfeature(info->chip_id, feature, 0x05<<8);
      info->subfeature=subfeature->number;
    break;
        //type not handled
    default:
      info->sensors_type=-1;
    break;
  }
}


SensorsInfo* sensors_info_new(const sensors_chip_name* chip_id,const sensors_feature* feature){
  SensorsInfo *info=g_object_new(SENSORS_TYPE_INFO,NULL);
  info->chip_id=chip_id;
  info->name=g_strdup(sensors_get_label(chip_id,feature));
  sensors_info_determine_sensors_type(info,feature);
  sensors_info_update_value(info);
  return info;
}

SensorsInfo* sensors_chip_new(const sensors_chip_name* chip_id){
  SensorsInfo *chip=g_object_new(SENSORS_TYPE_INFO,NULL);
  chip->chip_id=chip_id;
   char buffer[200];
   sensors_snprintf_chip_name(buffer,200,chip_id);
   chip->name=strdup(buffer);
   chip->sensors_type=0;
  return chip;
}

static void sensors_info_dispose (GObject *object){
  SensorsInfo* info=SENSORS_INFO(object);
  info->chip_id=NULL;
  G_OBJECT_CLASS (sensors_info_parent_class)->dispose(object);
}

static void sensors_info_finalize(GObject *object){
  SensorsInfo* info=SENSORS_INFO(object);
  g_free(info->name);
  G_OBJECT_CLASS (sensors_info_parent_class)->finalize(object);
}

static void sensors_info_init(SensorsInfo *info){
    info->value=-1000.0;
}

static void sensors_info_set_property(GObject* object, guint property_id, const GValue* value, GParamSpec* spec){
  SensorsInfo* info=SENSORS_INFO(object);
  if (property_id==1){
    info->value=g_value_get_double(value);
  }
  else{
    G_OBJECT_WARN_INVALID_PROPERTY_ID(object,property_id,spec);
  }
}

static void sensors_info_get_property(GObject* object, guint property_id, GValue* value, GParamSpec* spec){
  SensorsInfo* info=SENSORS_INFO(object);
  if (property_id==1){
    g_value_set_double(value,info->value);
  }
  else{
    G_OBJECT_WARN_INVALID_PROPERTY_ID(object,property_id,spec);
  }
}

static void sensors_info_class_init(SensorsInfoClass *class){
  GObjectClass *object_class = G_OBJECT_CLASS (class);
  object_class->set_property=sensors_info_set_property;
  object_class->get_property=sensors_info_get_property;
  GParamSpec* value_spec=g_param_spec_double("value","Value","value of sensors-info",-1000.0,10000.0,-1000.0,G_PARAM_READWRITE);
  g_object_class_install_property(object_class,1,value_spec);
  object_class->dispose=sensors_info_dispose;
  object_class->finalize =sensors_info_finalize;
}

const char* sensors_info_get_name(SensorsInfo *info){
  return info->name;
}


const char* sensors_info_get_value_string(SensorsInfo *info){
  const char* v_string;
  if (info->value==-1000){
    v_string="N/A";
  }
  else{
    enum sensors_type s_type=info->sensors_type;
      
      switch (s_type){
        case 0:
        v_string=NULL;
        break;
        //if FAN
        case 1:
          v_string=g_strdup_printf("%4.0f RPM",info->value);
        break;
        //if TEMP
        case 2:
          v_string=g_strdup_printf("%3.1f °C",info->value);
        break;
        //if POWER
        case 3:
          v_string=g_strdup_printf("%3.1f Watts",info->value);
        break;
        //if ENEREGY
      /* case 4:
          v_string=g_strdup_printf("%3.1f ?",info->value);
        break;*/
        //if CURRENT
        case 5:
          v_string=g_strdup_printf("%3.1f Volts",info->value);
        break;
        default:
        v_string="N/A";
        break;
      }
  }
  return v_string;
}

const sensors_chip_name* sensors_info_get_chip_id(SensorsInfo *info){
  return info->chip_id;
}

enum sensors_type sensors_info_get_info_type(SensorsInfo *info){
  return info->sensors_type;
}












