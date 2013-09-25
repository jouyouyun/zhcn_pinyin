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
#include <string.h>
#include <stdlib.h>

#define ZHPY_PATH   "/usr/share/dde/data/zh_py.db"
#define PY_BUFFER   128

gchar* select_zhpy (const gchar* zhcn, sqlite3* db);
static gint cb_search_zhpy (void* data, int col_num, 
        char** col_value, char** col_name);

static int multi_py = 0;
static int err_flag = 0;

gchar* get_pinyin (const gchar* zhcn)
{
    sqlite3* db = NULL;
    char buf[4];
    gchar* data = NULL;
    gchar* pinyin = NULL;
    gchar* py_tmp = NULL;
    const gchar* tmp = zhcn;

    if ( sqlite3_open (ZHPY_PATH, &db) != SQLITE_OK ) {
        g_warning ("Open db failed: %s\n", ZHPY_PATH);
        return NULL;
    }

    while ( *tmp != '\0' ) {
        memset (buf, 0, 4 * sizeof(char));
        memcpy (buf, tmp, 3);
        tmp += 3;

        data = select_zhpy (buf, db);
        if ( data == NULL ) {
            err_flag = 1;
            break;
        }

        if ( !pinyin ) {
            pinyin = g_strdup (data);
        } else {
            py_tmp = g_strdup_printf ("%s%s", pinyin, data);
            g_free (pinyin);
            pinyin = g_strdup (py_tmp);
            g_free (py_tmp);
        }
        g_free (data);

        if ( multi_py ) {
            break;
        }
    }

    if ( err_flag ) {
        return NULL;
    }

    if ( multi_py ) {
        multi_py = 0;
        g_free (pinyin);

        data = select_zhpy (zhcn, db);
        if ( data == NULL ) {
            return NULL;;
        }
        pinyin = g_strdup (data);
        g_free (data);
    }

    return pinyin;
}

gchar* select_zhpy (const gchar* zhcn, sqlite3* db)
{
    gchar* query = NULL;
    gchar* data = NULL;

    query = g_strdup_printf ("select ZHpy from zh_py where ZHcn=\'%s\'", 
            zhcn);
    data = g_new0 (char, PY_BUFFER);
    if ( sqlite3_exec (db, query, cb_search_zhpy, 
                data, NULL) != SQLITE_OK ) {
        g_warning ("select zhpy failed!\n");
        g_free (query);
        g_free (data);
        return NULL;
    }

    g_print ("select data: %s\n", data);
    g_free (query);
    return data;
}

static gint cb_search_zhpy (void* data, int col_num, 
        char** col_value, char** col_name)
{
    int i = 0;
    gchar* buf = (gchar*)data;

    for ( ; i < col_num; i++ ) {
        g_print ("%s = %s\n", col_name[i], 
                col_value[i] ? col_value[i] : "NULL");
        if ( i == 0 ) {
            strcpy (buf, col_value[i]);
        } else {
            multi_py = 1;
            break;
        }
    }

    return 0;
}
