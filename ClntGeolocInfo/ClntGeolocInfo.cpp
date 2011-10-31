#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include "CURLWrap.h"
#include "json/json.h"
#include <string>
#include <string.h>
#include <iostream>
#include <fstream>
#include <clocale>
#include <stdlib.h>
#include "Portable.h"
#include "DHCPConst.h"

using namespace std;

typedef struct
{
    GtkWidget *comboBox;
    GtkWidget *saveButton;
    GtkWidget *addressEntry, *cityEntry, *countryEntry;
} WIDGETS;

/*
 * Show dialog with information about app.
 */
void showInfo(GtkWidget *widget, gpointer window) {
    GtkWidget *dialog = gtk_about_dialog_new();
    
    gtk_about_dialog_set_name(GTK_ABOUT_DIALOG(dialog), "Geolocation");
    gtk_about_dialog_set_version(GTK_ABOUT_DIALOG(dialog), "0.1"); 
    gtk_about_dialog_set_copyright(GTK_ABOUT_DIALOG(dialog), 
        "(c) Michał Golon");
    gtk_about_dialog_set_comments(GTK_ABOUT_DIALOG(dialog), 
       "Getting geolocation information from DHCPv6 Dibbler client.");
    gtk_about_dialog_set_website(GTK_ABOUT_DIALOG(dialog), 
        "http://www.klub.com.pl");
    gtk_about_dialog_set_website_label(GTK_ABOUT_DIALOG(dialog), 
        "Dibbler Homepage"); 

    gtk_dialog_run(GTK_DIALOG (dialog));
    gtk_widget_destroy(dialog);
}

/*
 * Show dialog with error information.
 */
void showError() {
    GtkWidget *dialog = gtk_message_dialog_new(NULL,
        GTK_DIALOG_DESTROY_WITH_PARENT,
        GTK_MESSAGE_ERROR,
        GTK_BUTTONS_OK,
        "An error occured");
    
    gtk_window_set_title(GTK_WINDOW(dialog), "Error");
    gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(dialog),
        "There was some problem with calculation of geolocalization. Please try again.");
    
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

/*
 * Replace whitespaces with '+' -> for url which is send to Google.
 */
string replaceWhitespace(string url) {    
    for(unsigned int i = 0; i<url.length(); i++) {
        if(url[i] == ' ') {
            url[i] = '+';
        }
    }
    return url;
}

/*
 * Ask google about coordinates for specific address.
 * 
 * Google respons with JSON file, which is then parsed and necessary information
 * is taken out.
 * 
 * For asking Google, cURL library is used with C++ wrapper, which is based on cURLpp
 * project.
 * For parsing JSON file, JsonCpp library is used.
 */
bool getCoordinates(WIDGETS *widgets) {
    setlocale(LC_NUMERIC, "C");
    cout.precision(15);
    bool state;
    
    ofstream log(GEOLOCINFOLOG_FILE, ios::app);
    
    const gchar *addressEntry = gtk_entry_get_text(GTK_ENTRY(widgets->addressEntry));
    const gchar *cityEntry = gtk_entry_get_text(GTK_ENTRY(widgets->cityEntry));
    const gchar *countryEntry = gtk_entry_get_text(GTK_ENTRY(widgets->countryEntry));

    string url = "http://maps.googleapis.com/maps/api/geocode/json?address=";
    url += addressEntry;
    url += "+";
    url += cityEntry;
    url += "+";
    url += countryEntry;
    url += "&sensor=false";
    url = replaceWhitespace(url);
    //cout << url << endl;
    FILE * fp = fopen(JSONRESPONSE_FILE, "w");
    Wrapper::CURLWrap wrapper;
    wrapper.setOption(CURLOPT_URL, url.c_str());
    wrapper.setOption(CURLOPT_HEADER, 0);
    wrapper.setOption(CURLOPT_FOLLOWLOCATION, 1);
    wrapper.setOption(CURLOPT_WRITEFUNCTION, NULL);
    wrapper.setOption(CURLOPT_WRITEDATA, fp);
    wrapper.perform();
    fclose(fp);

    string buf;
    string line;
    ifstream in(JSONRESPONSE_FILE);
    
    while(getline(in,line)) {
        buf += line;
    }

    if (wrapper.isOK()) {
        log << time(NULL) << " Client: CURL responded properly.\n";
        if (!buf.empty()) {
            Json::Value root;
            Json::Reader reader;
            if (reader.parse(buf, root)) {
                log << time(NULL) << " Client: JSON responded properly.\n";
                const Json::Value status = root["status"];

                log << time(NULL) << " Client: JSON response status: '" << status.asString() << "'\n";
                if (status.asString() == "OK") {
                    const Json::Value results = root["results"];
 
                    for(unsigned int i = 0; i < results.size(); i++) {
                        const Json::Value geometry = results[i]["geometry"];
                        const Json::Value location = geometry["viewport"];
                        
                        const Json::Value latNorth = location["northeast"]["lat"];
                        const Json::Value lngEast = location["northeast"]["lng"];
                        
                        const Json::Value latSouth = location["southwest"]["lat"];
                        const Json::Value lngWest = location["southwest"]["lng"];
                        
                        string x1 = Json::valueToString(lngEast.asDouble());
                        string y1 = Json::valueToString(latNorth.asDouble());
                        
                        string x2 = Json::valueToString(lngWest.asDouble());
                        string y2 = Json::valueToString(latSouth.asDouble());
                        
                        string x3 = x1;
                        string y3 = y2;
                        
                        string x4 = x2;
                        string y4 = y1;
                        
                        ofstream config(CLNTGEOLOCINFOCFG_FILE, ios::out);
                        
                        int mode = gtk_combo_box_get_active(GTK_COMBO_BOX(widgets->comboBox));
                        if(mode == MODE_REMEMBER) {
                            config << ";remeber settings\n"<< MODE_REMEMBER << "\n";
                        } else if(mode == MODE_ASK) {
                            config << ";ask every time\n"<< MODE_ASK << "\n";
                        }
                        config << ";coordinates\n";
                        config << x1 << "\n";
                        config << y1 << "\n";
                        config << x2 << "\n";
                        config << y2 << "\n";
                        config << x3 << "\n";
                        config << y3 << "\n";
                        config << x4 << "\n";
                        config << y4 << "\n";

                        config.close();

                        state = true;
                    }
                } else {
                    log << time(NULL) << " Client: JSON returned no results.\n";
                    state = false;
                }
            } else {
                log << time(NULL) << " Client: JSON response error: " << reader.getFormatedErrorMessages() <<"\n";
                state = false;
            }
        } else {
            log << time(NULL) << " Client: CURL response is empty!\n";
            state = false;
        }
    } else {
        log << time(NULL) << " Client: CURL error! Error code: " << wrapper.getErrorCode() << "\n";
        state = false;
    }
    
    log.close();    
    wrapper.cleanup();
    
    return state;
}

/*
 * Show dialog when settings and coordinates are saved.
 * This is success!
 */
void showSaved() {
    GtkWidget *dialog =gtk_message_dialog_new(NULL,
            GTK_DIALOG_DESTROY_WITH_PARENT,
            GTK_MESSAGE_INFO,
            GTK_BUTTONS_OK,
            "Settings have been saved.", "title");
    
    gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(dialog), "Thank You!");
    gtk_window_set_title(GTK_WINDOW(dialog), "Settings saved");
    
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
    
    gtk_main_quit();
}

/*
 * Check if there are whitespaces in the beginig of address/city/country entry.
 */
gboolean isWhitespace(const char *entry) {   
    for(unsigned int i = 0; i<strlen(entry); i++) {
        if(entry[i] != ' ') {
            return false;
        }
    }
    return true;
}

/*
 * Check if settings can be saved.
 * Saved and send when:
 * - none of entries are empty,
 * - one of two modes (remember or ask again) is selected.
 * Saved when:
 * - don't ask again mode is selected.
 */
void canSave(GtkWidget *widget, WIDGETS *structData) {
    WIDGETS *widgets;
    widgets = structData;
        
    gboolean isAddress = false, isCity = false, isCountry = false;
    gint option;
       
    const gchar *addressEntry = gtk_entry_get_text(GTK_ENTRY(widgets->addressEntry));
    const gchar *cityEntry = gtk_entry_get_text(GTK_ENTRY(widgets->cityEntry));
    const gchar *countryEntry = gtk_entry_get_text(GTK_ENTRY(widgets->countryEntry));
        
    option = gtk_combo_box_get_active(GTK_COMBO_BOX(widgets->comboBox));
    
    if(strlen(addressEntry) > 0 && !isWhitespace(addressEntry)) {
        isAddress = true;
    }
    
    if(strlen(cityEntry) > 0 && !isWhitespace(cityEntry)) {
        isCity = true;
    }
    
    if(strlen(countryEntry) > 0 && !isWhitespace(countryEntry)) {
        isCountry = true;
    }
    
    if((option == 1 || option == 2) && isAddress && isCity && isCountry) {
        gtk_button_set_label(GTK_BUTTON(widgets->saveButton), "Save & Send");
        gtk_widget_set_sensitive(GTK_WIDGET(widgets->saveButton), true);
        gtk_widget_set_sensitive(GTK_WIDGET(widgets->addressEntry), true);
        gtk_widget_set_sensitive(GTK_WIDGET(widgets->cityEntry), true);
        gtk_widget_set_sensitive(GTK_WIDGET(widgets->countryEntry), true);
    } else if(option == 3) {
        gtk_button_set_label(GTK_BUTTON(widgets->saveButton), "Save");
        gtk_widget_set_sensitive(GTK_WIDGET(widgets->saveButton), true);
        gtk_widget_set_sensitive(GTK_WIDGET(widgets->addressEntry), false);
        gtk_widget_set_sensitive(GTK_WIDGET(widgets->cityEntry), false);
        gtk_widget_set_sensitive(GTK_WIDGET(widgets->countryEntry), false);
    } else {
        gtk_button_set_label(GTK_BUTTON(widgets->saveButton), "Save & Send");
        gtk_widget_set_sensitive(GTK_WIDGET(widgets->saveButton), false); 
        gtk_widget_set_sensitive(GTK_WIDGET(widgets->addressEntry), true);
        gtk_widget_set_sensitive(GTK_WIDGET(widgets->cityEntry), true);
        gtk_widget_set_sensitive(GTK_WIDGET(widgets->countryEntry), true);
    }
}

/*
 * Save action.
 */
void save(GtkWidget *widget, WIDGETS *structData) {
    WIDGETS *widgets;
    widgets = structData;
    
    bool state;
    
    const gchar *saveLabel = gtk_button_get_label(GTK_BUTTON(widgets->saveButton));
    const gchar *saveAndSendLabel = "Save & Send";
    
    if(strcmp(saveLabel, saveAndSendLabel) == 0) {
        state = getCoordinates(widgets);
        if(state) {
            showSaved();
        } else {
            showError();
        }
    } else {
        ofstream config(CLNTGEOLOCINFOCFG_FILE, ios::out);
        config << ";don't ask again\n"<< MODE_NEVER << "\n";
        config.close();
        showSaved();
        //showError();
    }
}

/*
 * Main function: creating a window and needed widgets.
 */
int main (int argc, char *argv[]) {
    /*
     * main containers
     */
    GtkWidget *window;
    GtkWidget *mainVbox;
    
    /*
     * menu
     */
    GtkWidget *bar;
    GtkWidget *menuFile;
    GtkWidget *menuInfo;
    GtkWidget *file;
    GtkWidget *help;
    GtkWidget *close;
    GtkWidget *infos;
    GtkAccelGroup *accelGroup;
    
    /*
     * tabs
     */
    GtkWidget *table;
    GtkWidget *notebook;
    
    /*
     * content
     */
    GtkWidget *contentVbox;
    GtkWidget *contentHbox;
    GtkWidget *geolocLabel;
    GtkWidget *infoLabel;
    GtkWidget *infoContainer;

    /*
     * frames, entries + box
     */
    GtkWidget *geolocBox;
    GtkWidget *addressFrame;
    GtkWidget *cityFrame;
    GtkWidget *countryFrame;
    
    /*
     * buttons + box
     */
    GtkWidget *buttonBox;
    GtkWidget *cancelButton;
    
    /*
     * align boxes
     */
    GtkWidget *alignGeolocBox;
    GtkWidget *alignComboBox;
    GtkWidget *alignButtonBox;
    
    /*
     * information about purpose
     */
    GtkWidget *information;
    
    /*
     * tips for buttons
     */
    GtkTooltips *saveTip;
    GtkTooltips *cancelTip;
    
    /*
     * labels
     */
    const char addressFrameLabel[] = "Address";
    const char cityFrameLabel[] = "City";
    const char countryFrameLabel[] = "Country";
    
    /*
     * labels for notebook's tab
     */
    const char geolocLabelChar[] = "Geolocation";
    const char infoLabelChar[] = "    Info    ";
    
    const char info[] = "Main purpose of this program is to gain information about \
geographical location of Internet user.\n\nThis information will be used only by \
authorized entities such as emergency services, internet providers etc.";

    WIDGETS *widgets;
    widgets = g_new(WIDGETS, 1);

    gtk_init(&argc, &argv);
 
    // BASIC SETTINGS
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_default_size(GTK_WINDOW(window), 350, 350);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    gtk_window_set_title(GTK_WINDOW(window), "Geolocation");
    gtk_window_set_resizable(GTK_WINDOW(window), FALSE);
    
    mainVbox = gtk_vbox_new(FALSE, 0);
    gtk_container_add(GTK_CONTAINER(window), mainVbox);
    
    // CREATING MENU
    bar = gtk_menu_bar_new();
    menuFile = gtk_menu_new();
    menuInfo = gtk_menu_new();
    accelGroup = gtk_accel_group_new();
    gtk_window_add_accel_group(GTK_WINDOW(window), accelGroup);
      
    file = gtk_menu_item_new_with_mnemonic("_File");
    close = gtk_image_menu_item_new_from_stock(GTK_STOCK_QUIT, NULL);
    help = gtk_menu_item_new_with_mnemonic("_Help");
    infos = gtk_image_menu_item_new_from_stock(GTK_STOCK_INFO, NULL);
 
    gtk_image_menu_item_set_always_show_image(GTK_IMAGE_MENU_ITEM(close), true);
    gtk_image_menu_item_set_always_show_image(GTK_IMAGE_MENU_ITEM(infos), true);
    gtk_widget_add_accelerator(close, "activate", accelGroup, GDK_q, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator(infos, "activate", accelGroup, GDK_i, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
 
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(file), menuFile);
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(help), menuInfo);
    gtk_menu_shell_append(GTK_MENU_SHELL(menuFile), close);
    gtk_menu_shell_append(GTK_MENU_SHELL(menuInfo), infos);
    gtk_menu_shell_append(GTK_MENU_SHELL(bar), file);
    gtk_menu_shell_append(GTK_MENU_SHELL(bar), help);
    gtk_box_pack_start(GTK_BOX(mainVbox), bar, FALSE, FALSE, 0);
    
    // CREATING TABLE/NOTEBOOK
    table = gtk_table_new(2, 2, FALSE);
    gtk_box_pack_start(GTK_BOX(mainVbox), table, TRUE, TRUE, 0);
    
    notebook = gtk_notebook_new();
    gtk_notebook_set_tab_pos(GTK_NOTEBOOK(notebook), GTK_POS_TOP);
    gtk_table_attach_defaults(GTK_TABLE(table), notebook, 0, 2, 0, 1);
    
    // CREATING CONTENT - GEOLOCATION
    contentVbox = gtk_vbox_new(FALSE, 5);
    contentHbox = gtk_hbox_new(FALSE, 5);

    // box with address information
    geolocBox = gtk_vbox_new(FALSE, 5);
    // frames for information
    addressFrame = gtk_frame_new(addressFrameLabel);
    cityFrame = gtk_frame_new(cityFrameLabel);
    countryFrame = gtk_frame_new(countryFrameLabel);
    // entry fields
    widgets->addressEntry = gtk_entry_new();
    widgets->cityEntry = gtk_entry_new();
    widgets->countryEntry = gtk_entry_new();
    // set sizes of entry fields
    gtk_widget_set_size_request(widgets->addressEntry, 230, 25);
    gtk_widget_set_size_request(widgets->cityEntry, 230, 25);
    gtk_widget_set_size_request(widgets->countryEntry, 230, 25);
    
    // create align box
    alignGeolocBox = gtk_alignment_new(0, 1, 0, 0);
    gtk_alignment_set_padding(GTK_ALIGNMENT(alignGeolocBox), 10, 0, 10, 0);
    // adding entries for frames
    gtk_container_add(GTK_CONTAINER(addressFrame), widgets->addressEntry);
    gtk_container_add(GTK_CONTAINER(cityFrame), widgets->cityEntry);
    gtk_container_add(GTK_CONTAINER(countryFrame), widgets->countryEntry);
    // adding frames to box with frames and entries
    gtk_box_pack_start(GTK_BOX(geolocBox), addressFrame, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(geolocBox), cityFrame, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(geolocBox), countryFrame, FALSE, FALSE, 0);
    // add box with frames and entries to align box
    gtk_container_add(GTK_CONTAINER(alignGeolocBox), geolocBox);
    
        // create align box for buttons and combobox
    alignComboBox = gtk_alignment_new(0, 1, 0, 0);
    gtk_alignment_set_padding(GTK_ALIGNMENT(alignComboBox), 5, 30, 10, 0);
    alignButtonBox = gtk_alignment_new(1, 1, 0, 0);
    gtk_alignment_set_padding(GTK_ALIGNMENT(alignButtonBox), 0, 10, 50, 10);
    
    // creating combobox
    widgets->comboBox = gtk_combo_box_new_text();

    gtk_combo_box_append_text(GTK_COMBO_BOX(widgets->comboBox), "--Choose--");
    gtk_combo_box_append_text(GTK_COMBO_BOX(widgets->comboBox), "Remember geolocation");
    gtk_combo_box_append_text(GTK_COMBO_BOX(widgets->comboBox), "Ask everytime");
    gtk_combo_box_append_text(GTK_COMBO_BOX(widgets->comboBox), "Do not ask again");
    gtk_combo_box_set_active(GTK_COMBO_BOX(widgets->comboBox), 0);

    //creating button box and buttons
    buttonBox = gtk_vbox_new(FALSE, 10);
    widgets->saveButton = gtk_button_new_with_label("Save & Send");
    cancelButton = gtk_button_new_with_label("Cancel");
    gtk_widget_set_size_request(widgets->saveButton, 120, 30);
    gtk_widget_set_size_request(cancelButton, 120, 30);
    //adding tip to buttons
    saveTip = gtk_tooltips_new();
    cancelTip = gtk_tooltips_new();
    gtk_tooltips_set_tip(saveTip, widgets->saveButton, "Save settings & send info.", NULL);
    gtk_tooltips_set_tip(cancelTip, cancelButton, "Close the program.", NULL);
    gtk_widget_set_sensitive(widgets->saveButton, false);
    
    // adding buttons to button box
    gtk_box_pack_start(GTK_BOX(buttonBox), widgets->saveButton, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(buttonBox), cancelButton, FALSE, FALSE, 0);
    
    // adding combobox and buttonbox sections to align boxes
    gtk_container_add(GTK_CONTAINER(alignComboBox), widgets->comboBox);
    gtk_container_add(GTK_CONTAINER(alignButtonBox), buttonBox);
    
    // adding align boxes to content box
    gtk_box_pack_start(GTK_BOX(contentHbox), alignComboBox, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(contentHbox), alignButtonBox, TRUE, TRUE, 0);

    // final adding
    gtk_box_pack_start(GTK_BOX(contentVbox), alignGeolocBox, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(contentVbox), contentHbox, TRUE, TRUE, 0);

    // CREATING CONTENT - INFO
    infoContainer = gtk_fixed_new();
    information = gtk_label_new(info);
    gtk_label_set_line_wrap(GTK_LABEL(information), TRUE);
    gtk_label_set_justify(GTK_LABEL(information), GTK_JUSTIFY_CENTER);
    gtk_container_set_border_width(GTK_CONTAINER(infoContainer), 20);
    gtk_container_add(GTK_CONTAINER(infoContainer), information);
    
    // adding tabs
    geolocLabel = gtk_label_new(geolocLabelChar);
    gtk_notebook_prepend_page(GTK_NOTEBOOK(notebook), contentVbox, geolocLabel);
    infoLabel = gtk_label_new(infoLabelChar);
    gtk_notebook_prepend_page(GTK_NOTEBOOK(notebook), infoContainer, infoLabel);
    
    // SIGNALS
    g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(G_OBJECT(close), "activate", G_CALLBACK(gtk_main_quit), NULL);  
    g_signal_connect(G_OBJECT(cancelButton), "clicked", G_CALLBACK(gtk_main_quit), NULL);
    
    g_signal_connect(G_OBJECT(infos), "activate", G_CALLBACK(showInfo), (gpointer)window);
 
    g_signal_connect(G_OBJECT(widgets->comboBox), "changed", G_CALLBACK(canSave), widgets);
    g_signal_connect(G_OBJECT(widgets->addressEntry), "changed", G_CALLBACK(canSave), widgets);
    g_signal_connect(G_OBJECT(widgets->cityEntry), "changed", G_CALLBACK(canSave), widgets);
    g_signal_connect(G_OBJECT(widgets->countryEntry), "changed", G_CALLBACK(canSave), widgets);
    
    g_signal_connect(G_OBJECT(widgets->saveButton), "clicked", G_CALLBACK(save), widgets);
    
    gtk_widget_show_all(window);
 
    gtk_main();
    
    return 0;
}