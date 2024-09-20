/* sensors-info.h
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

#pragma once

#include <gtk/gtk.h>


#define SENSORS_TYPE_INFO (sensors_info_get_type())
G_DECLARE_FINAL_TYPE(SensorsInfo,sensors_info,SENSORS,INFO,GObject)

 enum sensors_type{CHIP=0,FAN=1,TEMP=2,POWER=3,ENERGY=4,CURRENT=5};

SensorsInfo* sensors_info_new(const sensors_chip_name* chip_id,const sensors_feature* feature);
SensorsInfo* sensors_chip_new(const sensors_chip_name* chip_id);
const char* sensors_info_get_name(SensorsInfo *info);
const char* sensors_info_get_value_string(SensorsInfo *info);
const sensors_chip_name* sensors_info_get_chip_id(SensorsInfo *info);
enum sensors_type sensors_info_get_info_type(SensorsInfo *info);
void sensors_info_update_value(SensorsInfo* info);