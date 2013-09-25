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
#include <stdio.h>

gint create_table (const gchar* value, sqlite3* db);
void insert_record (const gchar* table, const gchar* value, sqlite3* db);
static gint  cb_get_count (void *data, int col_num, 
        char** col_value, char** col_name);
static int cb_get_ret (void *data, int col_num, 
        char** col_value, char** col_name);

gint init_data_sql (const gchar* data, const gchar* db_path)
{
    gchar** str_array = NULL;
    sqlite3* db = NULL;
    int ret = 0;
    int i = 0;

    if ( !data || !db_path ) {
        g_warning ("init data sql args error!\n");
        return -1;
    }

    if ( sqlite3_open (db_path, &db) != SQLITE_OK ) {
        g_warning ("Open sqlite db \'%s\' failed!\n", db_path);
        return -3;
    }

    str_array =  g_strsplit (data, ",", -1);
    while ( *(str_array + i) != NULL ) {
        ret = create_table (*(str_array + i), db);
        if ( ret == -1 ) {
            break;
        }
        i++;
    }

    if ( ret == -1 ) {
        g_strfreev(str_array);
        sqlite3_close (db);
        return -4;
    }

    g_strfreev(str_array);
    sqlite3_close (db);
    return 0;
}

gint create_table (const gchar* value, sqlite3* db)
{
    gchar* pinyin = NULL;
    gchar* query = NULL;
    int flag = 0;
    int len = 0;

    if ( !value ) {
        g_warning ("args error in create table!\n");
        return -1;
    }

    pinyin = get_pinyin (value);
    if ( !pinyin ) {
        g_warning ("get pinyin failed!\n");
        return -1;
    }

    len = strlen (pinyin);
    while ( len > 1 ) {
        query = g_strdup_printf ("create table if not exists "
                "%s(value text, count int(10))", pinyin);
        /*fprintf (stderr, "sqlite sql: %s\n", query);*/
        if ( sqlite3_exec (db, query, NULL, NULL, NULL) != SQLITE_OK ) {
            g_warning ("Create table \'%s\' failed!\n", pinyin);
            g_free (query);
            flag = 1;
            break;
        }
        insert_record (pinyin, value, db);

        g_free (query);
        len -= 1;
        *(pinyin + len) = '\0';
    }

    if ( flag ) {
        return -1;
    }

    return 0;
}

void insert_record (const gchar* table, const gchar* value, sqlite3* db)
{
    gchar* query = NULL;

    query = g_strdup_printf ("insert into %s values('%s', \'0\')", 
            table, value);
    fprintf (stderr, "insert sql: %s\n", query);
    if ( sqlite3_exec (db, query, NULL, NULL, NULL) != SQLITE_OK ) {
        g_warning ("Insert record failed!\n");
        return ;
    }

    return ;
}

static gchar* str_ret = NULL;

gchar* get_ret_via_key (const gchar* key, const gchar* db_path)
{
    sqlite3* db = NULL;
    gchar* query = NULL;
    /*gchar* str_ret = NULL;*/

    if ( !key || !db_path ) {
        g_warning ("Get ret failed in search table!\n");
        return NULL;
    }

    if ( sqlite3_open (db_path, &db) != SQLITE_OK ) {
        g_warning ("Open db failed: %s\n", db_path);
        return NULL;
    }

    if ( str_ret ) {
        fprintf (stderr, "free ret string!\n");
        g_free (str_ret);
        str_ret = NULL;
    }
    query = g_strdup_printf ("select value from %s", key);
    fprintf (stderr, "select query: %s\n", query);
    if ( sqlite3_exec (db, query, cb_get_ret, 
                NULL, NULL) != SQLITE_OK ) {
        g_warning ("get ret failed in search table!\n");
        sqlite3_close (db);
        g_free (query);
        g_warning ("free query!\n");
        return NULL;
    }

    sqlite3_close (db);
    g_free (query);
    fprintf (stderr, "select ret: %s\n", str_ret);
    return str_ret;
}

static int cb_get_ret (void *data, int col_num, 
        char** col_value, char** col_name)
{
    int i = 0;
    /*gchar* buf = (gchar*)data;*/
    gchar* buf = NULL;
    gchar* tmp = NULL;

    fprintf (stderr, "col num: %d\n", col_num);
    if ( col_num < 1 ) {
        g_warning ("No results!\n");
        return -1;
    }

    for (; i < col_num; i++ ) {
        fprintf (stderr, "select value: %d %s\n", i, col_value[i]);
        if ( i == 0 ) {
            buf = g_strdup (col_value[i]);
        } else {
            tmp = g_strdup_printf ("%s%s", buf, col_value[i]);
            g_free (buf);
            buf = g_strdup (tmp);
            g_free (tmp);
        }
        /*buf = g_strdup (col_value[i]);*/
        /*fprintf (stderr, "select buf: %s\n", buf);*/
    }

    fprintf (stderr, "select buf: %s\n", buf);
    if ( !str_ret ) {
        str_ret = g_strdup (buf);
    } else {
        tmp = g_strdup_printf ("%s,%s", str_ret, buf);
        g_free (str_ret);
        str_ret = g_strdup (tmp);
        g_free (tmp);
    }
    g_free (buf);

    return 0;
}

gint add_value_count (const gchar* table, const gchar* value, 
        const gchar* db_path)
{
    sqlite3* db = NULL;
    gchar* query = NULL;
    gchar cnt[10];
    int count = 0;

    if ( !table || !value ) {
        g_warning ("args error in add value count!\n");
        return -1;
    }

    if ( sqlite3_open (db_path, &db) != SQLITE_OK ) {
        g_warning ("open sql db \'%s\' failed!\n", db_path);
        return -1;
    }

    query = g_strdup_printf ("select count from %s where value=\'%s\'", 
            table, value);
    memset (cnt, 0, 10);
    if ( sqlite3_exec (db, query, cb_get_count, cnt, NULL) != SQLITE_OK ) {
        g_warning ("Get count failed!\n");
        sqlite3_close (db);
        g_free (query);
        return -1;
    }
    g_free (query);

    g_print ("cnt : %s\n", cnt);
    count = atoi(cnt) + 1;
    query = g_strdup_printf ("update %s set count=\'%d\' where value=\'%s\'", 
            table, count, value);
    if ( sqlite3_exec (db, query, NULL, NULL, NULL) != SQLITE_OK ) {
        g_warning ("Update \'%s\' count failed!\n", value);
        sqlite3_close (db);
        g_free (query);
        return -1;
    }

    g_free (query);
    sqlite3_close (db);
    return 0;
}

static gint  cb_get_count (void *data, int col_num, 
        char** col_value, char** col_name)
{
    int i = 0;
    gchar* tmp = (gchar*)data;

    for ( ; i < col_num; i++ ) {
        g_print ("%s = %s\n", col_name[i], 
                col_value[i] ? col_value[i] : 0);
        strcpy (tmp, col_value[i]);
    }

    return 0;
}
