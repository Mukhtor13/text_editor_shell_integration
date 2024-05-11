#ifndef INIT_MENUS_H_INCLUDED
#define INIT_MENUS_H_INCLUDED

#include <gtk/gtk.h>
#include "helpers.h"
#include "menu_objects.h"
#include "WindowDependecies.h"

void *create_schemes_modes_submenu(WindowDependecies *win_dep, GtkWidget **s_menu, GtkWidget **s_mi, MetaData *meta_data)
{
    GtkWidget *schemes_menu;
    GtkWidget *schemes_mi;
    GSList *group = NULL;

    GtkWidget *classic_mi = gtk_radio_menu_item_new_with_label(group, "Classic");
    group = gtk_radio_menu_item_get_group(GTK_RADIO_MENU_ITEM(classic_mi));
    GtkWidget *cobalt_mi = gtk_radio_menu_item_new_with_label(group, "Cobalt");
    group = gtk_radio_menu_item_get_group(GTK_RADIO_MENU_ITEM(cobalt_mi));
    GtkWidget *kate_mi = gtk_radio_menu_item_new_with_label(group, "Kate");
    group = gtk_radio_menu_item_get_group(GTK_RADIO_MENU_ITEM(kate_mi));
    GtkWidget *oblivion_mi = gtk_radio_menu_item_new_with_label(group, "Oblivion");
    group = gtk_radio_menu_item_get_group(GTK_RADIO_MENU_ITEM(oblivion_mi));
    GtkWidget *solarized_light_mi = gtk_radio_menu_item_new_with_label(group, "Solarized-Light");
    group = gtk_radio_menu_item_get_group(GTK_RADIO_MENU_ITEM(solarized_light_mi));
    GtkWidget *solarized_dark_mi = gtk_radio_menu_item_new_with_label(group, "Solarized-Dark");
    group = gtk_radio_menu_item_get_group(GTK_RADIO_MENU_ITEM(solarized_dark_mi));
    GtkWidget *tango_mi = gtk_radio_menu_item_new_with_label(group, "Tango");
    group = gtk_radio_menu_item_get_group(GTK_RADIO_MENU_ITEM(tango_mi));

    gtk_check_menu_item_set_active(oblivion_mi, TRUE);

    schemes_menu = gtk_menu_new();

    schemes_mi = gtk_menu_item_new_with_label("Themes");

    gtk_menu_item_set_submenu(GTK_MENU_ITEM(schemes_mi), schemes_menu);
    gtk_menu_shell_append(GTK_MENU_SHELL(schemes_menu), classic_mi);
    gtk_menu_shell_append(GTK_MENU_SHELL(schemes_menu), kate_mi);
    gtk_menu_shell_append(GTK_MENU_SHELL(schemes_menu), oblivion_mi);
    gtk_menu_shell_append(GTK_MENU_SHELL(schemes_menu), tango_mi);
    gtk_menu_shell_append(GTK_MENU_SHELL(schemes_menu), cobalt_mi);
    gtk_menu_shell_append(GTK_MENU_SHELL(schemes_menu), solarized_light_mi);
    gtk_menu_shell_append(GTK_MENU_SHELL(schemes_menu), solarized_dark_mi);

    g_signal_connect(G_OBJECT(classic_mi), "toggled", G_CALLBACK(scheme_settings_callback), meta_data);
    g_signal_connect(G_OBJECT(kate_mi), "toggled", G_CALLBACK(scheme_settings_callback), meta_data);
    g_signal_connect(G_OBJECT(oblivion_mi), "toggled", G_CALLBACK(scheme_settings_callback), meta_data);
    g_signal_connect(G_OBJECT(tango_mi), "toggled", G_CALLBACK(scheme_settings_callback), meta_data);
    g_signal_connect(G_OBJECT(cobalt_mi), "toggled", G_CALLBACK(scheme_settings_callback), meta_data);
    g_signal_connect(G_OBJECT(solarized_light_mi), "toggled", G_CALLBACK(scheme_settings_callback), meta_data);
    g_signal_connect(G_OBJECT(solarized_dark_mi), "toggled", G_CALLBACK(scheme_settings_callback), meta_data);

    *s_menu = schemes_menu;
    *s_mi = schemes_mi;
}

void *create_highlights_modes_submenu(WindowDependecies *win_dep, GtkWidget **h_menu, GtkWidget **h_mi, MetaData *meta_data)
{
    GtkWidget *highlighting_menu;
    GtkWidget *highlighting_mi;
    GtkWidget *c_mi = gtk_menu_item_new_with_label("C");
    GtkWidget *cpp_mi = gtk_menu_item_new_with_label("C++");
    GtkWidget *python3_mi = gtk_menu_item_new_with_label("Python");
    highlighting_menu = gtk_menu_new();

    highlighting_mi = gtk_menu_item_new_with_label("Highlighting modes");

    gtk_menu_item_set_submenu(GTK_MENU_ITEM(highlighting_mi), highlighting_menu);
    gtk_menu_shell_append(GTK_MENU_SHELL(highlighting_menu), c_mi);
    gtk_menu_shell_append(GTK_MENU_SHELL(highlighting_menu), cpp_mi);
    gtk_menu_shell_append(GTK_MENU_SHELL(highlighting_menu), python3_mi);
    g_signal_connect(G_OBJECT(c_mi), "activate", G_CALLBACK(highlighting_settings_callback), meta_data);
    g_signal_connect(G_OBJECT(cpp_mi), "activate", G_CALLBACK(highlighting_settings_callback), meta_data);
    g_signal_connect(G_OBJECT(python3_mi), "activate", G_CALLBACK(highlighting_settings_callback), meta_data);
    *h_menu = highlighting_menu;
    *h_mi = highlighting_mi;
}

void *create_settings_submenu(WindowDependecies *win_dep, GtkWidget **s_menu, GtkWidget **s_mi, MetaData *meta_data)
{
    GtkWidget *settings_menu;
    GtkWidget *settings_mi;

    GtkWidget *highlighting_mi;
    GtkWidget *schemes_mi;

    GtkWidget *schemes_menu;
    GtkWidget *highlighting_menu;

    GtkWidget *accel_group = win_dep->accel_group;

    settings_menu = gtk_menu_new();

    settings_mi = gtk_menu_item_new_with_label("Settings");

    create_highlights_modes_submenu(win_dep, &highlighting_menu, &highlighting_mi, meta_data);
    create_schemes_modes_submenu(win_dep, &schemes_menu, &schemes_mi, meta_data);

    gtk_menu_item_set_submenu(GTK_MENU_ITEM(settings_mi), settings_menu);
    gtk_menu_shell_append(GTK_MENU_SHELL(settings_menu), schemes_mi);
    gtk_menu_shell_append(GTK_MENU_SHELL(settings_menu), highlighting_mi);

    gtk_widget_add_accelerator(GTK_MENU_ITEM(highlighting_mi), "activate", GTK_ACCEL_GROUP(accel_group), GDK_KEY_h, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);

    *s_menu = settings_menu;
    *s_mi = settings_mi;
}

void *create_file_menu(WindowDependecies *win_dep, MenuBar *menubar_obj, MetaData *meta_data)
{
    GtkWidget *file_menu;
    GtkWidget *file_mi;
    GtkWidget *new_mi;
    GtkWidget *open_mi;
    GtkWidget *close_mi;
    GtkWidget *sep1;
    GtkWidget *save_mi;
    GtkWidget *save_as_mi;
    GtkWidget *sep2;
    GtkWidget *settings_mi;
    GtkWidget *sep3;
    GtkWidget *quit_mi;

    GtkWidget *settings_menu;

    GtkWidget *accel_group = win_dep->accel_group;

    file_menu = gtk_menu_new();

    file_mi = gtk_menu_item_new_with_mnemonic("_File");
    new_mi = gtk_menu_item_new_with_label("New");
    open_mi = gtk_menu_item_new_with_label("Open");
    close_mi = gtk_menu_item_new_with_label("Close");

    sep1 = gtk_separator_menu_item_new();

    save_mi = gtk_menu_item_new_with_label("Save");
    save_as_mi = gtk_menu_item_new_with_label("Save as");

    sep2 = gtk_separator_menu_item_new();

    create_settings_submenu(win_dep, &settings_menu, &settings_mi, meta_data);

    sep3 = gtk_separator_menu_item_new();

    quit_mi = gtk_menu_item_new_with_label("Quit");

    gtk_menu_item_set_submenu(GTK_MENU_ITEM(file_mi), file_menu);
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), new_mi);
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), open_mi);
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), close_mi);
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), sep1);
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), save_mi);
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), save_as_mi);
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), sep2);
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), settings_mi);
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), sep3);
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), quit_mi);

    g_signal_connect(G_OBJECT(new_mi), "activate", G_CALLBACK(new_file_callback), meta_data);
    g_signal_connect(G_OBJECT(open_mi), "activate", G_CALLBACK(open_file_callback), meta_data);
    g_signal_connect(G_OBJECT(close_mi), "activate", G_CALLBACK(close_file_callback), meta_data);
    g_signal_connect(G_OBJECT(save_mi), "activate", G_CALLBACK(save_file_callback), meta_data);
    g_signal_connect(G_OBJECT(save_as_mi), "activate", G_CALLBACK(save_as_file_callback), meta_data);

    g_signal_connect(G_OBJECT(quit_mi), "activate", G_CALLBACK(gtk_main_quit), NULL);

    gtk_widget_add_accelerator(GTK_MENU_ITEM(new_mi), "activate", GTK_ACCEL_GROUP(accel_group), GDK_KEY_n, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator(GTK_MENU_ITEM(open_mi), "activate", GTK_ACCEL_GROUP(accel_group), GDK_KEY_o, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator(GTK_MENU_ITEM(close_mi), "activate", GTK_ACCEL_GROUP(accel_group), GDK_KEY_w, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator(GTK_MENU_ITEM(save_mi), "activate", GTK_ACCEL_GROUP(accel_group), GDK_KEY_s, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator(GTK_MENU_ITEM(quit_mi), "activate", GTK_ACCEL_GROUP(accel_group), GDK_KEY_q, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);

    menubar_obj->file_menu->file_menu = file_menu;
    menubar_obj->file_menu->file_mi = file_mi;
    menubar_obj->file_menu->new_mi = new_mi;
    menubar_obj->file_menu->open_mi = open_mi;
    menubar_obj->file_menu->close_mi = close_mi;
    menubar_obj->file_menu->sep1 = sep1;
    menubar_obj->file_menu->save_as_mi = save_as_mi;
    menubar_obj->file_menu->sep2 = sep2;
    menubar_obj->file_menu->settings_mi = settings_mi;
    menubar_obj->file_menu->sep3 = sep3;
    menubar_obj->file_menu->quit_mi = quit_mi;
}

void *create_edit_menu(WindowDependecies *win_dep, MenuBar *menubar_obj, MetaData *meta_data)
{
    GtkWidget *edit_menu;
    GtkWidget *edit_mi;
    GtkWidget *undo_mi;
    GtkWidget *redo_mi;
    GtkWidget *sep1;
    GtkWidget *cut_mi;
    GtkWidget *copy_mi;
    GtkWidget *paste_mi;
    GtkWidget *sep2;
    GtkWidget *comment_mi;
    GtkWidget *sep3;

    GtkWidget *accel_group = win_dep->accel_group;

    edit_menu = gtk_menu_new();

    edit_mi = gtk_menu_item_new_with_mnemonic("_Edit");
    undo_mi = gtk_menu_item_new_with_label("Undo");
    redo_mi = gtk_menu_item_new_with_label("Redo");

    sep1 = gtk_separator_menu_item_new();

    cut_mi = gtk_menu_item_new_with_label("Cut");
    copy_mi = gtk_menu_item_new_with_label("Copy");
    paste_mi = gtk_menu_item_new_with_label("Paste");

    sep2 = gtk_separator_menu_item_new();

    comment_mi = gtk_menu_item_new_with_label("Comment");

    sep3 = gtk_separator_menu_item_new();

    gtk_menu_item_set_submenu(GTK_MENU_ITEM(edit_mi), edit_menu);
    gtk_menu_shell_append(GTK_MENU_SHELL(edit_menu), undo_mi);
    gtk_menu_shell_append(GTK_MENU_SHELL(edit_menu), redo_mi);
    gtk_menu_shell_append(GTK_MENU_SHELL(edit_menu), sep1);
    gtk_menu_shell_append(GTK_MENU_SHELL(edit_menu), cut_mi);
    gtk_menu_shell_append(GTK_MENU_SHELL(edit_menu), copy_mi);
    gtk_menu_shell_append(GTK_MENU_SHELL(edit_menu), paste_mi);
    gtk_menu_shell_append(GTK_MENU_SHELL(edit_menu), sep2);
    gtk_menu_shell_append(GTK_MENU_SHELL(edit_menu), comment_mi);
    gtk_menu_shell_append(GTK_MENU_SHELL(edit_menu), sep3);

    g_signal_connect(G_OBJECT(undo_mi), "activate", G_CALLBACK(undo_callback), meta_data);
    g_signal_connect(G_OBJECT(redo_mi), "activate", G_CALLBACK(redo_callback), meta_data);
    g_signal_connect(G_OBJECT(cut_mi), "activate", G_CALLBACK(cut_callback), meta_data);
    g_signal_connect(G_OBJECT(copy_mi), "activate", G_CALLBACK(copy_callback), meta_data);
    g_signal_connect(G_OBJECT(paste_mi), "activate", G_CALLBACK(paste_callback), meta_data);
    g_signal_connect(G_OBJECT(comment_mi), "activate", G_CALLBACK(comment_callback), meta_data);

    gtk_widget_add_accelerator(GTK_MENU_ITEM(undo_mi), "activate", GTK_ACCEL_GROUP(accel_group), GDK_KEY_z, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator(GTK_MENU_ITEM(cut_mi), "activate", GTK_ACCEL_GROUP(accel_group), GDK_KEY_x, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator(GTK_MENU_ITEM(copy_mi), "activate", GTK_ACCEL_GROUP(accel_group), GDK_KEY_c, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator(GTK_MENU_ITEM(paste_mi), "activate", GTK_ACCEL_GROUP(accel_group), GDK_KEY_v, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);

    menubar_obj->edit_menu->edit_menu = edit_menu;
    menubar_obj->edit_menu->edit_mi = edit_mi;
    menubar_obj->edit_menu->undo_mi = undo_mi;
    menubar_obj->edit_menu->redo_mi = redo_mi;
    menubar_obj->edit_menu->sep1 = sep1;
    menubar_obj->edit_menu->cut_mi = cut_mi;
    menubar_obj->edit_menu->copy_mi = copy_mi;
    menubar_obj->edit_menu->paste_mi = paste_mi;
    menubar_obj->edit_menu->sep2 = sep2;
    menubar_obj->edit_menu->comment_mi = comment_mi;
    menubar_obj->edit_menu->sep3 = sep3;
}

void create_tab_width_menu(MetaData *meta_data, GtkWidget **twh_mi)
{
    GtkWidget *tab_width_menu;
    GtkWidget *tab_width_head_mi;
    GSList *group = NULL;
    GtkWidget *tab_width_mis[MAX_TAB_WIDTH];

    for (int i = 0; i < MAX_TAB_WIDTH; ++i)
    {
        gchar *label = g_strdup_printf("Tab width: %d", i + 1);
        tab_width_mis[i] = gtk_radio_menu_item_new_with_label(group, label);
        group = gtk_radio_menu_item_get_group(GTK_RADIO_MENU_ITEM(tab_width_mis[i]));
        g_free(label);
    }
    gtk_check_menu_item_set_active(tab_width_mis[meta_data->tab_width - 1], TRUE);
    tab_width_menu = gtk_menu_new();

    tab_width_head_mi = gtk_menu_item_new_with_label("Tab width");
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(tab_width_head_mi), tab_width_menu);
    for (int i = 0; i < MAX_TAB_WIDTH; ++i)
    {
        gtk_menu_shell_append(GTK_MENU_SHELL(tab_width_menu), tab_width_mis[i]);
        g_signal_connect(G_OBJECT(tab_width_mis[i]), "toggled", G_CALLBACK(tab_width_menu_callback), meta_data);
    }
    *twh_mi = tab_width_head_mi;
}

void *create_format_menu(WindowDependecies *win_dep, MenuBar *menubar_obj, MetaData *meta_data)
{
    GList *group = NULL;
    GtkWidget *format_menu;
    GtkWidget *format_mi;
    GtkWidget *auto_indent_mi;
    GtkWidget *highlight_line_mi;
    GtkWidget *line_numbers_mi;
    GtkWidget *wrap_words_mi;
    GtkWidget *tab_width_head_mi;

    GtkWidget *accel_group = win_dep->accel_group;

    format_menu = gtk_menu_new();

    format_mi = gtk_menu_item_new_with_mnemonic("F_ormat");
    auto_indent_mi = gtk_check_menu_item_new_with_label("Enable Auto-Indent");
    gtk_check_menu_item_set_active(auto_indent_mi, TRUE);
    highlight_line_mi = gtk_check_menu_item_new_with_label("Highlight current line");
    gtk_check_menu_item_set_active(highlight_line_mi, TRUE);
    line_numbers_mi = gtk_check_menu_item_new_with_label("Display line numbers");
    gtk_check_menu_item_set_active(line_numbers_mi, TRUE);
    wrap_words_mi = gtk_check_menu_item_new_with_label("Text wrapping");
    create_tab_width_menu(meta_data, &tab_width_head_mi);

    gtk_menu_item_set_submenu(GTK_MENU_ITEM(format_mi), format_menu);
    gtk_menu_shell_append(GTK_MENU_SHELL(format_menu), auto_indent_mi);
    gtk_menu_shell_append(GTK_MENU_SHELL(format_menu), line_numbers_mi);
    gtk_menu_shell_append(GTK_MENU_SHELL(format_menu), highlight_line_mi);
    gtk_menu_shell_append(GTK_MENU_SHELL(format_menu), wrap_words_mi);
    gtk_menu_shell_append(GTK_MENU_SHELL(format_menu), tab_width_head_mi);

    g_signal_connect(G_OBJECT(auto_indent_mi), "toggled", G_CALLBACK(format_callback), meta_data);
    g_signal_connect(G_OBJECT(line_numbers_mi), "toggled", G_CALLBACK(format_callback), meta_data);
    g_signal_connect(G_OBJECT(highlight_line_mi), "toggled", G_CALLBACK(format_callback), meta_data);
    g_signal_connect(G_OBJECT(wrap_words_mi), "toggled", G_CALLBACK(format_callback), meta_data);
    menubar_obj->format_menu->format_menu = format_menu;
    menubar_obj->format_menu->format_mi = format_mi;
    menubar_obj->format_menu->auto_indent_mi = auto_indent_mi;
    menubar_obj->format_menu->line_numbers_mi = line_numbers_mi;
    menubar_obj->format_menu->wrap_words_mi = wrap_words_mi;
}

void *create_search_menu(WindowDependecies *win_dep, MenuBar *menubar_obj, MetaData *meta_data)
{
    GtkWidget *search_menu;
    GtkWidget *search_mi;
    GtkWidget *find_mi;
    GtkWidget *goto_mi;

    GtkWidget *accel_group = win_dep->accel_group;

    search_menu = gtk_menu_new();

    search_mi = gtk_menu_item_new_with_mnemonic("_Search");
    find_mi = gtk_menu_item_new_with_label("Find");
    goto_mi = gtk_menu_item_new_with_label("Go to line");

    gtk_menu_item_set_submenu(GTK_MENU_ITEM(search_mi), search_menu);
    gtk_menu_shell_append(GTK_MENU_SHELL(search_menu), find_mi);
    gtk_menu_shell_append(GTK_MENU_SHELL(search_menu), goto_mi);

    g_signal_connect(G_OBJECT(find_mi), "activate", G_CALLBACK(find_callback), meta_data);
    g_signal_connect(G_OBJECT(goto_mi), "activate", G_CALLBACK(goto_callback), meta_data);

    gtk_widget_add_accelerator(GTK_MENU_ITEM(find_mi), "activate", GTK_ACCEL_GROUP(accel_group), GDK_KEY_f, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator(GTK_MENU_ITEM(goto_mi), "activate", GTK_ACCEL_GROUP(accel_group), GDK_KEY_g, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);

    menubar_obj->search_menu->search_menu = search_menu;
    menubar_obj->search_menu->search_mi = search_mi;
    menubar_obj->search_menu->find_mi = find_mi;
    menubar_obj->search_menu->goto_mi = goto_mi;
}

void *create_build_menu(WindowDependecies *win_dep, MenuBar *menubar_obj, MetaData *meta_data)
{
    GtkWidget *build_menu;
    GtkWidget *build_mi;
    GtkWidget *compile_mi;
    GtkWidget *run_mi;
    GtkWidget *compile_and_run_mi;

    GtkWidget *accel_group = win_dep->accel_group;

    build_menu = gtk_menu_new();

    build_mi = gtk_menu_item_new_with_mnemonic("_Build");
    compile_mi = gtk_menu_item_new_with_label("Compile");
    run_mi = gtk_menu_item_new_with_label("Run");
    compile_and_run_mi = gtk_menu_item_new_with_label("Compile and run");

    gtk_menu_item_set_submenu(GTK_MENU_ITEM(build_mi), build_menu);
    gtk_menu_shell_append(GTK_MENU_SHELL(build_menu), compile_mi);
    gtk_menu_shell_append(GTK_MENU_SHELL(build_menu), run_mi);
    gtk_menu_shell_append(GTK_MENU_SHELL(build_menu), compile_and_run_mi);

    g_signal_connect(G_OBJECT(compile_mi), "activate", G_CALLBACK(compile_callback), meta_data);
    g_signal_connect(G_OBJECT(run_mi), "activate", G_CALLBACK(run_callback), meta_data);
    g_signal_connect(G_OBJECT(compile_and_run_mi), "activate", G_CALLBACK(compile_and_run_callback), meta_data);

    gtk_widget_add_accelerator(GTK_MENU_ITEM(compile_mi), "activate", GTK_ACCEL_GROUP(accel_group), GDK_KEY_F9, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator(GTK_MENU_ITEM(run_mi), "activate", GTK_ACCEL_GROUP(accel_group), GDK_KEY_F10, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator(GTK_MENU_ITEM(compile_and_run_mi), "activate", GTK_ACCEL_GROUP(accel_group), GDK_KEY_F11, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);

    menubar_obj->build_menu->build_menu = build_menu;
    menubar_obj->build_menu->build_mi = build_mi;
    menubar_obj->build_menu->compile_mi = compile_mi;
    menubar_obj->build_menu->run_mi = run_mi;
    menubar_obj->build_menu->compile_and_run_mi = compile_and_run_mi;
}

GtkWidget *create_menubar(WindowDependecies *win_dep, MenuBar *menubar_obj, MetaData *meta_data)
{
    GtkWidget *menubar;

    menubar = gtk_menu_bar_new();
    create_file_menu(win_dep, menubar_obj, meta_data);
    create_edit_menu(win_dep, menubar_obj, meta_data);
    create_format_menu(win_dep, menubar_obj, meta_data);
    create_search_menu(win_dep, menubar_obj, meta_data);
    create_build_menu(win_dep, menubar_obj, meta_data);

    gtk_menu_shell_append(GTK_MENU_SHELL(menubar), menubar_obj->file_menu->file_mi);
    gtk_menu_shell_append(GTK_MENU_SHELL(menubar), menubar_obj->edit_menu->edit_mi);
    gtk_menu_shell_append(GTK_MENU_SHELL(menubar), menubar_obj->format_menu->format_mi);
    gtk_menu_shell_append(GTK_MENU_SHELL(menubar), menubar_obj->search_menu->search_mi);
    gtk_menu_shell_append(GTK_MENU_SHELL(menubar), menubar_obj->build_menu->build_mi);

    return menubar;
}

#endif // INIT_MENUS_H_INCLUDED
