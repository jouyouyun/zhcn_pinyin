#include <stdio.h>
#include <stdlib.h>
#include <dbus/dbus-glib.h>

#define DMUSIC_NAME		"com.deepin.zh_pinyin"
#define DMUSIC_PATH		"/com/deepin/zh_pinyin"
#define DMUSIC_INFACE	"com.deepin.zh_pinyin"

void test()
{
	DBusGConnection *connection;
	GError *error;
	DBusGProxy *proxy;
	gchar *ret_num = NULL;

	error = NULL;
	connection = dbus_g_bus_get (DBUS_BUS_SESSION, &error);
    if (connection == NULL) {
		g_printerr ("Failed to open connection to bus: %s\n",
                  error->message);
		g_error_free (error);
		exit (1);
    }
	proxy = dbus_g_proxy_new_for_name (connection,
                                     DMUSIC_NAME,
                                     DMUSIC_PATH,
                                     DMUSIC_INFACE);

	error = NULL;
	if (!dbus_g_proxy_call (proxy, "dbus_get_pinyin", &error, 
						G_TYPE_STRING, "火狐", 
  						G_TYPE_INVALID,
						G_TYPE_STRING, &ret_num, G_TYPE_INVALID)) {
		if ((error->domain == DBUS_GERROR) && 
				(error->code == DBUS_GERROR_REMOTE_EXCEPTION)) {
			g_printerr ("Caught remote method exception %s: %s",
						dbus_g_error_get_name (error), error->message);
		} else {
			g_printerr ("Error: %s\n", error->message);
			g_printerr ("Error: %s\n", error->message);
		}
		
		g_error_free (error);
		exit (1);
	}
	
	if ( ret_num == NULL ) {
		g_printerr("return false\n");
	} else {
		g_printerr("ret : %s\n", ret_num);
	}

	g_object_unref (proxy);

	return ;
}

int main()
{
	test();
	
	return 0;
}
