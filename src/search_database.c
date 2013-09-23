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

#include "dbus_database.h"
#include <sqlite3.h>

#define ZH_LEN 4
#define EN_LEN 7
#define EXEC_BUF    80
#define READ_LINE   256
#define DATABASE_PATH   "/usr/share/dde/data/zh_py.db"

static int search_zhcn_callback(void *data, int col_num, 
                            char **col_value, char **col_name);
static int flag = 0;

char *dbus_get_pinyin(const char *cmd_buf)
{
    char tmp[4];
    char cmd[READ_LINE];
    char buf[EXEC_BUF];
    sqlite3 *db = NULL;
    char *err_msg = NULL;
    char sql[READ_LINE];
    char *pinyin = NULL;

    /* 打开内存数据库 */
    if ( SQLITE_OK != sqlite3_open(DATABASE_PATH, &db) ) {
        sys_err("search_table sqlite3_open failed...\n");
    }
    
    memset(cmd, 0, READ_LINE);
    
    while ( *cmd_buf != '\0' ) {
        memset(tmp, 0, 4);
        memset(buf, 0, EXEC_BUF);
        memset(sql, 0, READ_LINE);
        
        memcpy(tmp, cmd_buf, 3);
        sys_says("tmp : %s\n", tmp);
        sprintf( sql, "select ZHpy from zh_py where ZHcn=\'%s\'", tmp );
        /* 查询， 并调用回调函数 */
        if ( SQLITE_OK != sqlite3_exec( db, sql, search_zhcn_callback, 
                                buf, &err_msg ) ) {
            sys_err("search_table sqlite3_exec failed...\n");           
        }
        if ( flag ) {
            flag = 0;
            break;
        }
        sys_says("buf : %s\n", buf);
        if ( buf[0] != '\0' )
            strcat(cmd, buf);
        sys_says("cmd_buf : %s\n", cmd_buf);
        cmd_buf += 3;
    }
    
    if ( SQLITE_OK != sqlite3_exec( db, sql, search_zhcn_callback, 
                            buf, &err_msg ) ) {
        sys_err("search_table sqlite3_exec failed...\n");           
    }

    if ( SQLITE_OK != sqlite3_close(db) ) {
        sys_err("search_table sqlite3_close failed...\n");  
    }
    
    sys_says("cmd : %s\n", cmd);
    if ( cmd[0] != '\0' ) {
        pinyin = (char*)calloc(1, strlen(cmd) + 1);
        if ( pinyin == NULL ) {
            return NULL;
        }
        memcpy(pinyin, cmd, strlen(cmd));
    }
    
    return pinyin;
}

static int search_zhcn_callback(void *data, int col_num, 
                            char **col_value, char **col_name)
{
    int i;
    char *buf = (char*)data;

    for ( i = 0; i < col_num; i++ ) {
        printf( "%s = %s \n", col_name[i], col_value[i] ? col_value[i] : "NULL" );
        if ( strcmp("ZHpy", col_name[i]) == 0 ) {
            //memset(buf, 0, EXEC_BUF);
            if ( i == 0 ) {
                strcpy(buf, col_value[i])aggcha round sqlite3_prepare_v2(), sqlite3_step(), and sqlite3_finalize(), tha)))* tmp = g_strdup_printf (",%s", col_value[i]);
                strcat(buf, tmp);
                g_free (tmp);char* tmp = g_strdup_printf (",%s", col_value[i]);
                strcat(buf, tmp);
                g_free (tmp);
            } else {
                flag = 1;
                break;
            }
            /*sys_says("buf : %s\n", buf);*/
        }
    }
    sys_says("buf : %s\n", buf);

    return 0;
}
