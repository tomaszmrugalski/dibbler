#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include "CURLWrap.h"
#include "json/json.h"
#include <string>
#include <string.h>
#include <iostream>
#include <fstream>
#include <clocale>
#include <stdlib.h>
#include "Portable.h"

using namespace std;

/*
 * Show dialog for saving file.
 */
void saveDialog(GtkWidget *widget, GtkWidget *map) {
    GtkWidget *dialog = gtk_file_chooser_dialog_new("Save File",
        NULL,
     	GTK_FILE_CHOOSER_ACTION_SAVE,
     	GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
     	GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT,
        NULL);

    gtk_file_chooser_set_do_overwrite_confirmation(GTK_FILE_CHOOSER(dialog), TRUE);
    gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(dialog), "Untitled document");
     
    if(gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT)
    {
         char *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
         GdkPixbuf *pixbuf = gtk_image_get_pixbuf(GTK_IMAGE(map));
         GError *error = NULL;

         gdk_pixbuf_save(pixbuf, filename, "png", &error, "compression", "9", NULL);
         g_free(filename);
    }
     
    gtk_widget_destroy(dialog);
}

/*
 * Get static map from Google.
 * 
 * For asking Google, cURL library is used with C++ wrapper, which is based on cURLpp
 * project.
 * Downloaded static map is saved as .png file.
 */
void getStaticMap(string * arguments) {
    setlocale(LC_NUMERIC, "C");
    cout.precision(15);
    
    ofstream log(GEOLOCINFOLOG_FILE, ios::app);
    
    string url = "http://maps.googleapis.com/maps/api/staticmap?size=512x512&zoom=14";
    url += "&path=color:0x0000ff|weight:3|fillcolor:0x0033FF|";
            url += arguments[1]+","+arguments[0];
            url += "|";
            url += arguments[5]+","+arguments[4];
            url += "|";
            url += arguments[3]+","+arguments[2];
            url += "|";
            url += arguments[7]+","+arguments[6];
            url += "|";
            url += arguments[1]+","+arguments[0];
            url += "&sensor=false";
            cout << url << endl;
    
    FILE * fp = fopen(REQGEOLOCINFOMAP_FILE, "w");
    Wrapper::CURLWrap wrapper;
    wrapper.setOption(CURLOPT_URL, url.c_str());
    wrapper.setOption(CURLOPT_HEADER, 0);
    wrapper.setOption(CURLOPT_FOLLOWLOCATION, 1);
    wrapper.setOption(CURLOPT_WRITEFUNCTION, NULL);
    wrapper.setOption(CURLOPT_WRITEDATA, fp);
    wrapper.perform();
    fclose(fp);

    if (wrapper.isOK()) {
        log << time(NULL) <<" Requestor: CURL responded properly.\n";
    } else {
        log << time(NULL) <<" Requestor: CURL error! Error code: " << wrapper.getErrorCode() << "\n";
    }
    
    log.close();
    wrapper.cleanup();
}

/*
 * Main function: create a window, needed widgets and display map.
 */
int main (int argc, char *argv[]) {
    
    // get coordinates
    string arguments[8];
    if(argc == 9) {
        arguments[0] = argv[1];
        arguments[1] = argv[2];
        arguments[2] = argv[3];
        arguments[3] = argv[4];
        arguments[4] = argv[5];
        arguments[5] = argv[6];
        arguments[6] = argv[7];
        arguments[7] = argv[8];  
    } else {
        cout << "Error: too few arguments!" << endl;
        return 0;
    }

    /*
     * main containers
     */
    GtkWidget *window;
    GtkWidget *mainVbox;
    GtkWidget *map;
    
    /*
     * menu
     */
    GtkWidget *bar;
    GtkWidget *menuFile;
    GtkWidget *file;
    GtkWidget *close;
    GtkWidget *save;
    GtkAccelGroup *accelGroup;

    gtk_init(&argc, &argv);
    
    // GET MAP
    getStaticMap(arguments);
 
    // BASIC SETTINGS
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_default_size(GTK_WINDOW(window), 350, 350);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    gtk_window_set_title(GTK_WINDOW(window), "Geolocation - Map");
    gtk_window_set_resizable(GTK_WINDOW(window), FALSE);
    
    mainVbox = gtk_vbox_new(FALSE, 0);
    gtk_container_add(GTK_CONTAINER(window), mainVbox);
    
    // CREATING MENU
    bar = gtk_menu_bar_new();
    menuFile = gtk_menu_new();
    accelGroup = gtk_accel_group_new();
    gtk_window_add_accel_group(GTK_WINDOW(window), accelGroup);
      
    file = gtk_menu_item_new_with_mnemonic("_File");
    close = gtk_image_menu_item_new_from_stock(GTK_STOCK_QUIT, NULL);
    save = gtk_image_menu_item_new_from_stock(GTK_STOCK_SAVE, NULL);
 
    gtk_image_menu_item_set_always_show_image(GTK_IMAGE_MENU_ITEM(close), true);
    gtk_widget_add_accelerator(close, "activate", accelGroup, GDK_q, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    
    gtk_image_menu_item_set_always_show_image(GTK_IMAGE_MENU_ITEM(save), true);
    gtk_widget_add_accelerator(save, "activate", accelGroup, GDK_s, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
 
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(file), menuFile);
    gtk_menu_shell_append(GTK_MENU_SHELL(menuFile), save);
    gtk_menu_shell_append(GTK_MENU_SHELL(menuFile), close);
    gtk_menu_shell_append(GTK_MENU_SHELL(bar), file);
    gtk_box_pack_start(GTK_BOX(mainVbox), bar, FALSE, FALSE, 0);
    
    // GENERATING MAP
    map = gtk_image_new_from_file(REQGEOLOCINFOMAP_FILE);
    gtk_container_add(GTK_CONTAINER(mainVbox), map);
        
    // SIGNALS
    g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(G_OBJECT(close), "activate", G_CALLBACK(gtk_main_quit), NULL);  
    g_signal_connect(G_OBJECT(save), "activate", G_CALLBACK(saveDialog), map);  
    
    gtk_widget_show_all(window);
 
    gtk_main();
    
    return 0;
}