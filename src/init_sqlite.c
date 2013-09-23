/**
 * Copyright (c) 2011 ~ 2012 Deepin, Inc.
 *               2011 ~ 2012 jouyouyun
 *
 * Author:      jouyouyun <jouyouwen717@gmail.com>
 * Maintainer:  jouyouyun <jouyouwen717@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 **/

#include "dbus_sqlite.h"
#include <sqlite3.h>

gint create_table (const gchar* table);
gint insert_record (const gchar* table, const gchar* value);
gint update_record (const gchar* table, const gchar* value);

gint init_data_sql (const gchar* data, const gchar* db_path)
{
    gchar** str_array =  g_strsplite (data, ",", -1);
}

gint add_value_count (const gchar* table, const gchar* value, 
        const gchar* db_path)
{
}

gint create_table (const gchar* table)
{
}

gint insert_record (const gchar* table, const gchar* value)
{
}

gint update_record (const gchar* table, const gchar* value)
{
}