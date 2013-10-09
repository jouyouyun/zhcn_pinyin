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

#ifndef __DBUS_PINYIN_H__
#define __DBUS_PINYIN_H__

#include <glib.h>

#define DBUS_PYSQL_NAME     "com.search.pysql"
#define DBUS_PYSQL_PATH     "/com/search/pysql"
#define DBUS_PYSQL_INFACE   "com.search.pysql"

gchar* get_pinyin(const gchar* zhcn);
gint init_data_sql (const gchar* data, const gchar* db_path);
gchar* get_ret_via_key (const gchar* key, const gchar* db_path);
gint add_value_count (const gchar* table, const gchar* value, 
        const gchar* db_path);

/* trie */
void init_hash_table ();
gchar* create_pinyin_trie (const gchar* data);
gchar* get_ret_via_keys (const gchar* keys, const gchar* str_md5);
void finalize_data_trie (const gchar* str_md5);
void finalize_hash_table ();

#endif
