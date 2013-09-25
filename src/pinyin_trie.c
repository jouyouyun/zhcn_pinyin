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

#include "pinyin_trie.h"

/*GArray* trie_array = NULL;*/
GHashTable* trie_table = NULL;
GHashTable* data_table = NULL;

void init_hash_table ()
{
    /*trie_array = g_array_new (TRUE, FALSE, sizeof (pinyin_trie*));*/
    trie_table = g_hash_table_new (g_direct_hash, g_direct_equal);
    data_table = g_hash_table_new (g_direct_hash, g_direct_equal);
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
    /*trie_array = g_array_append_val (trie_array, root);*/

    str_md5 = g_compute_checksum_for_string (G_CHECKSUM_MD5, 
            data, -1);
    g_hash_table_insert (trie_table, str_md5, root);

    data_split = g_strsplit (data, ",", -1);
    g_hash_table_insert (data_table, str_md5, g_strdupv (data_split));
    for (; data_split[i] != NULL; i++ ) {
        pinyin = get_pinyin (data_split[i]);
        if ( !pinyin ) {
            err_flag = 1;
            break;
        }

        insert_pinyin (pinyin, i, root);
        g_free (pinyin);
    }
    root = NULL;

    if ( err_flag ) {
        g_warning ("get pinyin failed!\n");
        g_strfreev (data_split);
        return NULL;
    }

    g_strfreev (data_split);
    return str_md5;
}

pinyin_trie* create_trie_node (char ch)
{
    pinyin_trie* node = g_new0 (pinyin_trie, sizeof (pinyin_trie));

    node->flag = 0;
    node->ch = ch;

    return node;
}

void insert_pinyin (const char* pinyin, int pos, pinyin_trie* root)
{
    int i = 0;
    int k = 0;
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
    int k = ch_to_num (ch);

    node->next[k] = create_trie_node (ch);
    node->next[k]->flag = 1;
    node->next[k]->ret_pos.cnt += 1;
    node->next[k]->ret_pos.pos = g_new0 (int, sizeof(int));
    *(node->next[k]->ret_pos.pos) = pos;
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

    cur_trie = (pinyin_trie*)g_hash_table_lookup (trie_table, str_md5);
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

        if ( i == 0 ) {
            ret = g_strdup (data_array[k]);
        } else {
            tmp = g_strdup_printf ("%s,%s", ret, data_array[k]);
            g_free (ret);
            ret = g_strdup (tmp);
            g_free (tmp);
        }
    }

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

    /*ret = g_new0 ((struct pos_array), sizeof (struct pos_array));*/
    /*memcpy (ret, cur_node->ret_pos, sizeof (cur_node->ret_pos));*/
    *ret = cur_node->ret_pos;

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
        g_free (cur_node->ret_pos.pos);
        cur_node->ret_pos.pos = NULL;
    }

    g_free (cur_node);
    cur_node = NULL;

    return ;
}

void remove_trie (const gchar* str_md5)
{
    int i = 0;
    pinyin_trie* cur_trie = NULL;
    gchar** data_array = NULL;

    if ( !str_md5 ) {
        g_warning ("args error in remove trie!\n");
        return ;
    }

    cur_trie = (pinyin_trie*)g_hash_table_lookup (trie_table, str_md5);
    destroy_trie (cur_trie);

    data_array = (gchar**)g_hash_table_lookup (data_table, str_md5);
    for ( ; data_array[i] != NULL; i++ ) {
        g_free (data_array[i]);
        data_array[i] = NULL;
    }
    g_strfreev (data_array);

    g_hash_table_remove (trie_table, str_md5);
    g_hash_table_remove (data_table, str_md5);

    return ;
}
