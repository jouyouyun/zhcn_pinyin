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

#include "dbus_pinyin.h"
#include <gio/gio.h>

//internal functions
static gboolean _retry_registration (gpointer user_data);
static void _on_bus_acquired (GDBusConnection * connection, const gchar * name, gpointer user_data);
static void _on_name_acquired (GDBusConnection * connection, const gchar * name, gpointer user_data);
static void _on_name_lost (GDBusConnection * connection, const gchar * name, gpointer user_data);
static void _bus_method_call (GDBusConnection * connection, const gchar * sender,
                             const gchar * object_path, const gchar * interface,
                             const gchar * method, GVariant * params,
                             GDBusMethodInvocation * invocation, gpointer user_data);
static gboolean do_exit(gpointer user_data);

GMainLoop *loop = NULL;

static guint lock_service_owner_id;
static guint lock_service_reg_id;        //used for unregister an object path
static guint retry_reg_timeout_id;   //timer used for retrying dbus name registration.
GDBusConnection* lock_connection;

static GDBusNodeInfo *      node_info = NULL;
static GDBusInterfaceInfo *interface_info = NULL;
static GDBusInterfaceVTable interface_table = {
    method_call:   _bus_method_call,
    get_property:   NULL, // No properties 
    set_property:   NULL  // No properties 
};

const char *_lock_dbus_iface_xml = 
"<?xml version=\"1.0\"?>\n"
"<node>\n"
"   <interface name=\""DBUS_PYSQL_INFACE"\">\n"
"       <method name=\"get_pinyin\">\n"
"           <arg name=\"zhcn\" type=\"s\" direction=\"in\">\n"
"           </arg>\n"
"           <arg name=\"pinyin\" type=\"s\" direction=\"out\">\n"
"           </arg>\n"
"       </method>\n"
"       <method name=\"finalize_dbus_loop\">\n"
"       </method>\n"
"   </interface>\n"
"</node>\n"
;

void dbus_pinyin()
{
    loop = g_main_loop_new(NULL, FALSE);
    GError *error = NULL;
    
    node_info = g_dbus_node_info_new_for_xml (_lock_dbus_iface_xml, &error);
    if ( error != NULL ) {
        g_critical("Unable to parse interface xml: %s\n", error->message);
        g_error_free(error);
    }

    interface_info = g_dbus_node_info_lookup_interface(node_info, 
            DBUS_PYSQL_INFACE );
    if ( interface_info == NULL ) {
        g_critical("Unable to find interface '"DBUS_PYSQL_INFACE"'");
    }

    lock_service_owner_id = 0;
    lock_service_reg_id = 0;
    retry_reg_timeout_id = 0;
    _retry_registration(NULL);

    /*g_timeout_add_seconds( 15, do_exit, NULL );*/
    g_main_loop_run(loop);
    g_main_loop_unref(loop);

    return ;
}

void finalize_dbus_loop()
{
    g_main_loop_quit(loop);
}

static gboolean
do_exit(gpointer user_data)
{
    g_main_loop_quit(loop);

    return FALSE;
}

static gboolean
_retry_registration ( gpointer user_data )
{
    lock_service_owner_id = g_bus_own_name( G_BUS_TYPE_SESSION, 
                DBUS_PYSQL_NAME, 
                G_BUS_NAME_OWNER_FLAGS_NONE, 
                lock_service_reg_id ? NULL : _on_bus_acquired,
                _on_name_acquired,
                _on_name_lost, 
                NULL, 
                NULL);
    return 0;
}

static void
_on_bus_acquired ( GDBusConnection *connection, 
        const gchar *name, 
        gpointer user_data )
{
    g_debug(" on bus acquired ...\n");

    lock_connection = connection;

    //register object
    GError *error = NULL;
    lock_service_reg_id = g_dbus_connection_register_object( connection, 
            DBUS_PYSQL_PATH, 
            interface_info, 
            &interface_table, 
            user_data, 
            NULL, 
            &error );

    if ( error != NULL ) {
        g_critical ( "Unable to register object to the dbus: %s\n", 
                error->message );
        g_error_free(error);
        g_bus_unown_name(lock_service_owner_id);
        lock_service_owner_id = 0;
        retry_reg_timeout_id = g_timeout_add_seconds(1, 
                _retry_registration, NULL );
        return;
    }

    return;
}

static void
_on_name_acquired ( GDBusConnection *connection, 
        const gchar* name, 
        gpointer user_data )
{
    g_debug ( "Dbus name acquired ... \n" );
}

static void
_on_name_lost ( GDBusConnection *connection, 
        const gchar *name, 
        gpointer user_data )
{
    if ( connection == NULL ) {
        g_critical ( "Unable to get a connection to DBus...\n" );
    } else {
        g_critical ( "Unable to claim the name %s\n", DBUS_PYSQL_NAME );
    }

    lock_service_owner_id = 0;
}

static void
_bus_method_call (GDBusConnection * connection,
                 const gchar * sender, const gchar * object_path, const gchar * interface,
                 const gchar * method, GVariant * params,
                 GDBusMethodInvocation * invocation, gpointer user_data)
{
    g_debug ("bus_method_call");

    GVariant * retval = NULL;
    GError * error = NULL;

    if (g_strcmp0 (method, "get_pinyin") == 0) {
        const gchar *buf = NULL;
        gchar* pinyin = NULL;
        
        g_variant_get (params, "(s)", &buf);
        pinyin = get_pinyin (buf);
        retval = g_variant_new("(s)", pinyin ? pinyin : "");
    } else if (g_strcmp0 (method, "finalize_dbus_loop") == 0) {
        finalize_dbus_loop();
    } else {
        g_warning ("Calling method '%s' on lock and it's unknown", method);
    }

    if (error != NULL) {
        g_dbus_method_invocation_return_dbus_error (invocation,
                "com.deepin.dde.lock.Error",
                error->message);
        g_error_free (error);

    } else {
        g_dbus_method_invocation_return_value (invocation, retval);
    }
}
