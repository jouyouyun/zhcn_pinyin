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

#ifndef __PINYIN_TRIE_H__
#define __PINYIN_TRIE_H__

#include "dbus_pinyin.h"

#define EN_LEN  26

struct pos_array {
    int cnt;   //存储的数据个数
    int* pos;  //数据数组
};

struct _pinyin_trie {
    char flag;  //是否有数据
    char ch;    //当前字母
    struct pos_array ret_pos;
    struct _pinyin_trie* next[EN_LEN];
};
typedef struct _pinyin_trie pinyin_trie;

//gchar* create_pinyin_trie (gchar* data);
pinyin_trie* create_trie_node (char ch);
int ch_to_num (char ch);
void insert_pinyin (const char* pinyin, int pos, pinyin_trie* root);
void insert_char (char ch, int pos, pinyin_trie* node);
struct pos_array* search_trie (const char* keys, pinyin_trie* root);
void destroy_trie (pinyin_trie* cur_node);

#endif
