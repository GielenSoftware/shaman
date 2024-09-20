/* sensors-view.h
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


#define SENSORS_TYPE_VIEW (sensors_view_get_type())
G_DECLARE_FINAL_TYPE(SensorsView,sensors_view,SENSORS,VIEW,GObject)

SensorsView* sensors_view_new(void);
GtkColumnView* sensors_view_get_column_view(SensorsView* view);
bool sensors_view_start_monitoring(SensorsView* view);
bool sensors_view_stop_monitoring(SensorsView* view);


