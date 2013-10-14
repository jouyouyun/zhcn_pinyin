/**
 * Copyright (c) 2011 ~ 2013 Deepin, Inc.
 *               2011 ~ 2013 jouyouyun
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

#include "pinyin_trie.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

GHashTable* trie_table = NULL;
GHashTable* data_table = NULL;

void init_hash_table ()
{
    trie_table = g_hash_table_new_full (g_str_hash, g_str_equal, 
            (GDestroyNotify)g_free, (GDestroyNotify)remove_trie);
    data_table = g_hash_table_new_full (g_str_hash, g_str_equal, 
            (GDestroyNotify)g_free, (GDestroyNotify)remove_data);
}

gchar* create_pinyin_trie(const gchar* data)
{
    int i = 0;
    int err_flag = 0;
    pinyin_trie* root = NULL;
    gchar* pinyin = NULL;
    gchar* str_md5 = NULL;
    gchar** data_split = NULL;

    if ( !data ) {
        g_warning ("args error in create pinyin trie!\n");
        return NULL;
    }

    root = create_trie_node (' ');
    if ( !root ) {
        g_warning ("create trie failed!\n");
        return NULL;
    }

    fprintf (stdout, "\ntrie data: %s\n\n", data);
    str_md5 = g_compute_checksum_for_string (G_CHECKSUM_MD5, 
            data, -1);
    g_print ("str md5: %s\n", str_md5);

    data_split = g_strsplit (data, ";", -1);
    for (; data_split[i] != NULL; i++ ) {
        /*fprintf (stdout, "word: %s\n", data_split[i]);*/
        pinyin = get_pinyin (data_split[i]);
        if ( !pinyin ) {
            err_flag = 1;
            break;
        }

        insert_pinyin (pinyin, i, root);
        g_free (pinyin);
    }

    if ( err_flag ) {
        g_warning ("get pinyin failed!\n");
        g_strfreev (data_split);
        return NULL;
    }

    g_hash_table_insert (trie_table, g_strdup (str_md5), root);
    g_hash_table_insert (data_table, g_strdup (str_md5), data_split);
    return str_md5;
}

pinyin_trie* create_trie_node (char ch)
{
    pinyin_trie* node = g_new0 (pinyin_trie, 1);

    node->flag = 0;
    node->ch = ch;

    return node;
}

void insert_pinyin (const char* pinyin, int pos, pinyin_trie* root)
{
    int i = 0;
    int k = 0;
    int len = 0;
    pinyin_trie* cur_node = root;

    if ( !pinyin || !root ) {
        g_warning ("args error in insert trie!\n");
        return ;
    }

    for (; pinyin[i] != '\0'; i++ ) {
        k = ch_to_num (pinyin[i]);
        insert_char (pinyin[i], pos, cur_node);
        cur_node = cur_node->next[k];
    }
}

void insert_char (char ch, int pos, pinyin_trie* node)
{
    int i = 0;
    int len = 0;
    int k = ch_to_num (ch);
    int* value = NULL;

    if ( !node->next[k] ) {
        node->next[k] = create_trie_node (ch);
        node->next[k]->flag = 1;
    }
    node->next[k]->ret_pos.cnt += 1;
    len = node->next[k]->ret_pos.cnt;
    /*g_print ("$$$$$ value size: %d\n", len);*/
    
    value = g_new0 (int, len);
    memcpy (value, node->next[k]->ret_pos.pos, (len - 1) * sizeof (int));
    value[len - 1] = pos;

    g_free (node->next[k]->ret_pos.pos);
    node->next[k]->ret_pos.pos = value;
    value = NULL;
}

int ch_to_num (char ch)
{
    return (ch - 'a');
}

gchar* get_ret_via_keys (const gchar* keys, const gchar* str_md5)
{
    int i = 0;
    int k = 0;
    int len = 0;
    gchar* ret = NULL;
    gchar* tmp = NULL;
    gchar** data_array = NULL;
    struct pos_array* pos_buf = NULL;
    pinyin_trie* cur_trie = NULL;

    if ( !keys || !str_md5 ) {
        g_warning ("args error in get ret via keys!\n");
        return NULL;
    }

    /*print_flag ((gchar*)str_md5);*/
    g_print ("str md5: %s\n", str_md5);
    cur_trie = (pinyin_trie*)g_hash_table_lookup (trie_table, str_md5);
    /*g_print ("first char: %d\n", cur_trie->next[ch_to_num('w')]->flag);*/
    if ( !cur_trie ) {
        g_warning ("string md5 error in get pinyin trie!\n");
        return NULL;
    }

    data_array = (gchar**) g_hash_table_lookup (data_table, str_md5);
    if ( !data_array ) {
        g_warning ("string md5 error in get data array!\n");
        return NULL;
    }

    pos_buf = search_trie (keys, cur_trie);
    if ( !pos_buf ) {
        g_warning ("get pos array failed!\n");
        return NULL;
    }

    len = pos_buf->cnt;
    for (; i < len; i++ ) {
        k = pos_buf->pos[i];
        /*fprintf (stderr, "data %d: %s\n", k, data_array[k]);*/

        if ( i == 0 ) {
            ret = g_strdup (data_array[k]);
        } else {
            tmp = g_strdup_printf ("%s;%s", ret, data_array[k]);
            g_free (ret);
            ret = g_strdup (tmp);
            g_free (tmp);
        }
    }
    /*g_free (pos_buf);*/
    /*pos_buf = NULL;*/

    return ret;
}

struct pos_array* search_trie (const char* keys, pinyin_trie* root)
{
    int i = 0;
    int k = 0;
    int flag = 0;
    pinyin_trie* cur_node = root;
    struct pos_array* ret = NULL;

    if ( !keys || !root ) {
        g_warning ("args error in search trie!\n");
        return NULL;
    }

    for (; keys[i] != '\0'; i++ ) {
        k = ch_to_num (keys[i]);
        if ( !cur_node->next[k] ) {
            flag = 1;
            break;
        }

        cur_node = cur_node->next[k];
    }

    if ( (flag) || (cur_node->flag == 0) ) {
        g_warning ("No results!\n");
        return NULL;
    }

    g_print ("%s pos size: %d\n", keys, cur_node->ret_pos.cnt);
    /*ret = g_new0 (struct pos_array, sizeof (cur_node->ret_pos));*/
    /*memcpy (ret, &(cur_node->ret_pos), sizeof (cur_node->ret_pos));*/
    ret = &cur_node->ret_pos;

    return ret;
}

void destroy_trie (pinyin_trie* cur_node)
{
    int i = 0;

    if ( !cur_node ) {
        return ;
    }

    for (; i < EN_LEN; i++ ) {
        if ( !cur_node->next[i] ) {
            continue;
        }
        destroy_trie (cur_node->next[i]);
    }

    if ( cur_node->flag == 1 ) {
        cur_node->flag = 0;
        g_free (cur_node->ret_pos.pos);
        cur_node->ret_pos.pos = NULL;
    }

    g_free (cur_node);
    cur_node = NULL;

    return ;
}

void remove_trie (pinyin_trie* cur_trie)
{
    g_print ("remove trie\n");

    if ( !cur_trie ) {
        g_warning ("args error in remove trie!\n");
        return ;
    }

    destroy_trie (cur_trie);
    g_print ("remove trie over\n");

    return ;
}

void remove_data (gchar** data_array)
{
    g_print ("remove data\n");

    if ( !data_array ) {
        g_warning ("args error in remove data!\n");
        return ;
    }

    g_strfreev (data_array);
    g_print ("remove data over\n");

    return ;
}

void finalize_data_trie (const gchar* str_md5)
{
    gboolean is_ok;

    if ( !str_md5 ) {
        g_warning (" md5 is null in finalize trie!");
        return ;
    }

    g_print ("finalize trie md5: %s\n", str_md5);
    is_ok = g_hash_table_remove (trie_table, str_md5);
    if ( !is_ok ) {
        g_warning ("remove trie failed!\n");
    }
    is_ok = g_hash_table_remove (data_table, str_md5);
    if ( !is_ok ) {
        g_warning ("remove data failed!\n");
    }
}

void finalize_hash_table ()
{
    if ( !trie_table ) {
        g_hash_table_destroy (trie_table);
    }

    if ( !data_table ) {
        g_hash_table_destroy (data_table);
    }
}
