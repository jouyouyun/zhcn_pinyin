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

#ifndef __DBUS_DATABASE_H__
#define __DBUS_DATABASE_H__

#include "dbus_common.h"

#define DBUS_PINYIN_NAME    "com.deepin.zh_pinyin"
#define DBUS_PINYIN_PATH    "/com/deepin/zh_pinyin"
#define DBUS_PINYIN_INFACE  "com.deepin.zh_pinyin"

void dbus_pinyin();
char *dbus_get_pinyin(const char *cmd_buf);
void finalize_dbus_loop();

#endif
