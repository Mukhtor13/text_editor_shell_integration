#ifndef CALLBACKS_H
#define CALLBACKS_H

#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <libgen.h>
#include "WindowDependecies.h"
#include "menu_objects.h"
#include "init_source.h"
#include "helpers.h"
#include <gtksourceview-3.0/gtksourceview/gtksourcestyleschememanager.h>
#include <gtksourceview-3.0/gtksourceview/gtksourcestyleschemechooserbutton.h>

void save_file_callback(GtkMenuItem *menu_item, MetaData *meta_data);
void mark_set_callback(GtkTextBuffer *buffer, const GtkTextIter *new_location, GtkTextMark *mark, gpointer data);
void update_statusbar_language(GtkSourceBuffer *buffer, GtkStatusbar *statusbar);
void update_statusbar_col_lines(GtkSourceBuffer *buffer, GtkStatusbar *statusbar);
void update_unsaved_tab(GtkSourceBuffer *buffer, GtkWidget *tab);

void dummy_callback(GtkMenuItem *menu_item, MetaData *meta_data)
{
    g_printf("Not yet implemented\n");
}

void tab_width_menu_callback(GtkMenuItem *menu_item, MetaData *meta_data)
{
    gchar text[] = {"Tab width: "};
    gchar *label = gtk_menu_item_get_label(menu_item);
    gint width = label[strlen(text)] - '0';
    meta_data->tab_width = width;
    apply_tab_width(meta_data);
}

void new_file_callback(GtkMenuItem *menu_item, MetaData *meta_data)
{
    SourceObject *source_object;
    GtkWidget *box;
    GtkWidget *statusbar_col_lines;
    GtkWidget *statusbar_language;
    GtkWidget *statusbar_shell;
    GtkWidget *statusbar_font;
    GtkWidget *statusbar_tab_width;
    GtkWidget *tab_width_menu;
    GtkWidget *statusbox;
    GtkWidget *status_align;
    GtkWidget *buffer;
    GtkWidget *filename;
    GtkWidget *isSaved;
    GtkWidget *notebook = meta_data->notebook;

    source_object = create_source(meta_data);

    gtk_notebook_set_show_tabs(GTK_NOTEBOOK(notebook), TRUE);
    box = gtk_vbox_new(FALSE, 0);
    statusbox = gtk_hbox_new(FALSE, 100);

    gtk_notebook_set_show_tabs(GTK_NOTEBOOK(notebook), TRUE);
    gtk_box_pack_start(GTK_BOX(box), source_object->scrolled_window, TRUE, TRUE, 0);

    gtk_box_pack_start(GTK_BOX(box), source_object->terminal, FALSE, FALSE, 0);

    status_align = gtk_alignment_new(1, 0, 0, 0);
    statusbar_col_lines = gtk_statusbar_new();
    statusbar_language = gtk_statusbar_new();
    statusbar_shell = gtk_statusbar_new();
    statusbar_font = gtk_statusbar_new();
    statusbar_tab_width = gtk_statusbar_new();

    buffer = gtk_text_view_get_buffer(GTK_SOURCE_VIEW(source_object->textview));
    g_signal_connect(buffer, "changed", G_CALLBACK(update_statusbar_col_lines), statusbar_col_lines);
    g_signal_connect(buffer, "changed", G_CALLBACK(update_statusbar_language), statusbar_language);

    gtk_box_pack_start(GTK_BOX(statusbox), statusbar_language, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(statusbox), statusbar_shell, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(statusbox), statusbar_font, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(statusbox), statusbar_tab_width, FALSE, FALSE, 0);
    gtk_container_add(GTK_BOX(statusbox), status_align);
    gtk_box_pack_start(GTK_BOX(statusbox), statusbar_col_lines, FALSE, FALSE, 0);

    gchar *shell_msg = g_strdup_printf("Shell: %s", "Bourne-Again shell");
    gtk_statusbar_push(statusbar_shell, 0, shell_msg);
    g_free(shell_msg);

    gchar *font_msg = g_strdup_printf("Font: %s", pango_font_description_to_string(meta_data->font_desc));
    gtk_statusbar_push(statusbar_font, 0, font_msg);
    g_free(font_msg);

    gint tab_width = gtk_source_view_get_tab_width(GTK_SOURCE_VIEW(source_object->textview));
    gchar *tab_width_msg = g_strdup_printf("Tab width: %d", tab_width);
    gtk_statusbar_push(statusbar_tab_width, 0, tab_width_msg);
    g_free(tab_width_msg);

    g_signal_connect_object(buffer, "mark_set", G_CALLBACK(mark_set_callback), statusbar_col_lines, 0);
    gtk_box_pack_start(GTK_BOX(box), statusbox, FALSE, FALSE, 0);

    filename = gtk_label_new("Untitled");
    gtk_box_pack_start(GTK_BOX(box), filename, FALSE, FALSE, 0);

    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), box, source_object->tab_label);
    g_signal_emit_by_name(buffer, "changed");
    gtk_widget_show_all(GTK_WIDGET(notebook));

    gint curr_ind = gtk_notebook_get_n_pages(GTK_NOTEBOOK(notebook)) - 1;
    GtkWidget *curr_page = gtk_notebook_get_nth_page(GTK_NOTEBOOK(notebook), curr_ind);
    gtk_notebook_set_tab_reorderable(GTK_NOTEBOOK(notebook), curr_page, TRUE);

    GtkWidget *tab_name = gtk_notebook_get_tab_label(meta_data->notebook, curr_page);
    g_signal_connect(buffer, "changed", G_CALLBACK(update_unsaved_tab), tab_name);
}

void open_file_callback(GtkMenuItem *menu_item, MetaData *meta_data)
{
    GtkWidget *dialog;
    GtkWidget *save_dialog, *error_dialog;
    gint current_page;
    gint id;
    gint offset;
    gchar *filename;
    gchar *contents;
    GtkWidget *scrolled_win;
    GtkWidget *view;
    GtkTextBuffer *buffer;
    GtkWidget *tab_name;
    GtkTextIter start, end;
    GtkTextMark *mark;
    GtkWidget *prompt_label;
    GtkWidget *content_area;

    new_file_callback(NULL, meta_data);

    current_page = gtk_notebook_get_n_pages(meta_data->notebook) - 1;
    scrolled_win = gtk_notebook_get_nth_page(meta_data->notebook, current_page);
    view = get_last_page_view(meta_data);

    if (view != NULL)
    {
        dialog = gtk_file_chooser_dialog_new("Open A File", NULL, GTK_FILE_CHOOSER_ACTION_OPEN, GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT, GTK_STOCK_CANCEL, GTK_RESPONSE_REJECT, NULL);

        id = gtk_dialog_run(GTK_DIALOG(dialog));

        tab_name = gtk_notebook_get_tab_label(meta_data->notebook, scrolled_win);

        switch (id)
        {
        case GTK_RESPONSE_ACCEPT:
            filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));

            buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(view));
            gtk_text_buffer_get_end_iter(buffer, &end);
            offset = gtk_text_iter_get_offset(&end);
            if (g_file_test(filename, G_FILE_TEST_EXISTS))
            {
                g_file_get_contents(filename, &contents, NULL, NULL);

                gtk_source_buffer_begin_not_undoable_action(buffer);
                mark = gtk_text_buffer_get_insert(buffer);
                gtk_text_buffer_get_iter_at_mark(buffer, &start, mark);

                gtk_text_buffer_set_text(buffer, contents, -1);

                gtk_label_set_text(GTK_LABEL(tab_name), basename(filename));
                set_page_filename(meta_data, gtk_notebook_get_n_pages(meta_data->notebook) - 1, filename);
                switch_to_last_page(meta_data);
                GtkSourceLanguage *lang = gtk_source_language_manager_guess_language(meta_data->lang_manager, filename, NULL);
                if (lang != NULL)
                    meta_data->lang = lang;
                apply_language(meta_data);

                gtk_source_buffer_end_not_undoable_action(buffer);
            }
            else
            {
                error_dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL,
                                                      GTK_MESSAGE_ERROR,
                                                      GTK_BUTTONS_OK, NULL);

                gtk_dialog_run(GTK_DIALOG(error_dialog));
                gtk_widget_destroy(error_dialog);
            }
            break;

        case GTK_RESPONSE_DELETE_EVENT:
        case GTK_RESPONSE_REJECT:
            close_file(meta_data, gtk_notebook_get_n_pages(meta_data->notebook) - 1);
            break;
        }
        gtk_widget_destroy(dialog);
    }
}

void close_file_callback(GtkMenuItem *menu_item, MetaData *meta_data)
{
    gint current_page;

    current_page = gtk_notebook_get_current_page(meta_data->notebook);
    close_file(meta_data, current_page);
}

void save_file_callback(GtkMenuItem *menu_item, MetaData *meta_data)
{
    GtkWidget *dialog;
    GtkWidget *textview;
    GtkTextBuffer *buffer;
    GtkWidget *scrolled_win;
    gint current_page;
    gint response;
    GtkWidget *tab_name;
    GtkTextIter start, end;
    gchar *pathname;
    gchar *contents;

    current_page = gtk_notebook_get_current_page(meta_data->notebook);
    scrolled_win = gtk_notebook_get_nth_page(meta_data->notebook, current_page);
    textview = get_current_page_view(meta_data);
    tab_name = gtk_notebook_get_tab_label(meta_data->notebook, scrolled_win);

    gchar *filename = (char *)malloc(sizeof(char) * 255);
    get_page_filename(meta_data, current_page, filename);

    if (strcmp(basename(filename), "Untitled") == 0)
    {
        dialog = gtk_file_chooser_dialog_new("Save File", NULL, GTK_FILE_CHOOSER_ACTION_SAVE, GTK_STOCK_SAVE, GTK_RESPONSE_APPLY, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, NULL);
        response = gtk_dialog_run(GTK_DIALOG(dialog));
        switch (response)
        {
        case GTK_RESPONSE_APPLY:
            pathname = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
            buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview));
            gtk_text_buffer_get_bounds(buffer, &start, &end);
            contents = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);
            g_file_set_contents(pathname, contents, -1, NULL);
            set_page_filename(meta_data, current_page, pathname);
            gtk_label_set_text(GTK_LABEL(tab_name), basename(pathname));
            break;
        case GTK_RESPONSE_CANCEL:
            break;
        }
        gtk_widget_destroy(dialog);
    }
    else
    {
        pathname = (char *)malloc(sizeof(char) * 255);
        get_page_filename(meta_data, current_page, pathname);

        buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview));
        gtk_text_buffer_get_bounds(buffer, &start, &end);
        contents = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);

        g_file_set_contents(pathname, contents, -1, NULL);
        gtk_label_set_text(GTK_LABEL(tab_name), basename(pathname));
    }
    free(filename);
}

void save_as_file_callback(GtkMenuItem *menu_item, MetaData *meta_data)
{
    GtkWidget *dialog;
    GtkWidget *textview;
    GtkTextBuffer *buffer;
    GtkWidget *scrolled_win;
    gint current_page;
    gint response;
    GtkWidget *tab_label;
    GtkTextIter start, end;
    gchar *filename;
    gchar *contents;

    current_page = gtk_notebook_get_current_page(meta_data->notebook);
    scrolled_win = gtk_notebook_get_nth_page(meta_data->notebook, current_page);
    textview = get_current_page_view(meta_data);
    tab_label = gtk_notebook_get_tab_label(meta_data->notebook, scrolled_win);

    dialog = gtk_file_chooser_dialog_new("Save File", NULL, GTK_FILE_CHOOSER_ACTION_SAVE, GTK_STOCK_SAVE, GTK_RESPONSE_APPLY, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, NULL);
    response = gtk_dialog_run(GTK_DIALOG(dialog));
    switch (response)
    {
    case GTK_RESPONSE_APPLY:
        filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview));
        gtk_text_buffer_get_bounds(buffer, &start, &end);
        contents = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);
        g_file_set_contents(filename, contents, -1, NULL);
        break;
    case GTK_RESPONSE_CANCEL:
        break;
    }
    gtk_widget_destroy(dialog);
}

void zoom_in_callback(GtkMenuItem *menu_item, MetaData *meta_data)
{
    int curr_size = pango_font_description_get_size(meta_data->font_desc);
    if (curr_size < 18 * PANGO_SCALE)
    {
        curr_size += PANGO_SCALE;
    }
    else if (curr_size < 28 * PANGO_SCALE)
    {
        curr_size += 2 * PANGO_SCALE;
    }
    else if (curr_size < 40 * PANGO_SCALE)
    {
        curr_size += 4 * PANGO_SCALE;
    }
    else if (curr_size < 72 * PANGO_SCALE)
    {
        curr_size += 8 * PANGO_SCALE;
    }
    pango_font_description_set_size(meta_data->font_desc, curr_size);

    apply_font(meta_data);
}

void zoom_out_callback(GtkMenuItem *menu_item, MetaData *meta_data)
{
    int curr_size = pango_font_description_get_size(meta_data->font_desc);
    if (curr_size <= 18 * PANGO_SCALE)
    {
        curr_size -= PANGO_SCALE;
    }
    else if (curr_size <= 28 * PANGO_SCALE)
    {
        curr_size -= 2 * PANGO_SCALE;
    }
    else if (curr_size <= 40 * PANGO_SCALE)
    {
        curr_size -= 4 * PANGO_SCALE;
    }
    else if (curr_size <= 72 * PANGO_SCALE)
    {
        curr_size -= 8 * PANGO_SCALE;
    }
    pango_font_description_set_size(meta_data->font_desc, curr_size);

    apply_font(meta_data);
}

void highlighting_settings_callback(GtkMenuItem *menu_item, MetaData *meta_data)
{
    const char *lang_code;
    const char *label;
    label = gtk_menu_item_get_label(menu_item);

    if (strcmp(label, "C") == 0)
    {
        lang_code = "c";
    }
    if (strcmp(label, "C++") == 0)
    {
        lang_code = "cpp";
    }
    if (strcmp(label, "Python") == 0)
    {
        lang_code = "python3";
    }

    g_printf("%s\n", lang_code);
    meta_data->lang = gtk_source_language_manager_get_language(GTK_SOURCE_LANGUAGE_MANAGER(meta_data->lang_manager), lang_code);
    apply_language(meta_data);
}

void scheme_settings_callback(GtkMenuItem *menu_item, MetaData *meta_data)
{
    const char *scheme_code;
    const char *label;
    label = gtk_menu_item_get_label(menu_item);

    if (strcmp(label, "Classic") == 0)
    {
        scheme_code = "classic";
    }
    if (strcmp(label, "Cobalt") == 0)
    {
        scheme_code = "cobalt";
    }
    if (strcmp(label, "Kate") == 0)
    {
        scheme_code = "kate";
    }
    if (strcmp(label, "Oblivion") == 0)
    {
        scheme_code = "oblivion";
    }
    if (strcmp(label, "Solarized-Light") == 0)
    {
        scheme_code = "solarized-light";
    }
    if (strcmp(label, "Solarized-Dark") == 0)
    {
        scheme_code = "solarized-dark";
    }
    if (strcmp(label, "Tango") == 0)
    {
        scheme_code = "tango";
    }

    g_printf("%s\n", scheme_code);
    meta_data->scheme = gtk_source_style_scheme_manager_get_scheme(meta_data->scheme_manager, scheme_code);
    apply_scheme(meta_data);
}

void undo_callback(GtkMenuItem *menu_item, MetaData *meta_data)
{
    GtkTextBuffer *buffer;
    GtkClipboard *clipboard;

    clipboard = gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);

    buffer = get_current_page_buffer(meta_data);

    gtk_source_buffer_undo(buffer);
}

void redo_callback(GtkMenuItem *menu_item, MetaData *meta_data)
{
    GtkTextBuffer *buffer;
    GtkClipboard *clipboard;

    clipboard = gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);

    buffer = get_current_page_buffer(meta_data);

    gtk_source_buffer_redo(buffer);
}

void cut_callback(GtkMenuItem *menu_item, MetaData *meta_data)
{
    GtkTextBuffer *buffer;
    GtkClipboard *clipboard;

    clipboard = gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);

    buffer = get_current_page_buffer(meta_data);

    gtk_text_buffer_cut_clipboard(buffer, clipboard, TRUE);
}

void copy_callback(GtkMenuItem *menu_item, MetaData *meta_data)
{
    GtkTextBuffer *buffer;
    GtkClipboard *clipboard;

    clipboard = gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);

    buffer = get_current_page_buffer(meta_data);

    gtk_text_buffer_copy_clipboard(buffer, clipboard);
}

void paste_callback(GtkMenuItem *menu_item, MetaData *meta_data)
{
    GtkTextBuffer *buffer;
    GtkClipboard *clipboard;

    clipboard = gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);

    buffer = get_current_page_buffer(meta_data);

    gtk_text_buffer_paste_clipboard(buffer, clipboard, NULL, TRUE);
}

void comment_callback(GtkMenuItem *menu_item, MetaData *meta_data)
{
    GtkSourceBuffer *buffer;
    GtkClipboard *clipboard;
    GtkTextMark *selection;
    GtkTextIter begI, endI;
    gchar *comment;

    comment = (gchar *)malloc(sizeof(gchar) * 30);
    calc_lang_comment(meta_data, comment);
    buffer = get_current_page_buffer(meta_data);

    GtkTextMark *begM = gtk_text_buffer_get_mark(buffer, "insert");
    GtkTextMark *endM = gtk_text_buffer_get_mark(buffer, "selection_bound");

    clipboard = gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);

    gtk_text_buffer_get_iter_at_mark(buffer, &begI, begM);
    gtk_text_buffer_get_iter_at_mark(buffer, &endI, endM);

    if (gtk_text_iter_get_line(&begI) > gtk_text_iter_get_line(&endI))
    {
        GtkTextIter temp = begI;
        begI = endI;
        endI = temp;

        GtkTextMark *tM = begM;
        begM = endM;
        endM = tM;
    }

    while (gtk_text_iter_get_line(&begI) != gtk_text_iter_get_line(&endI))
    {
        gtk_text_buffer_insert(buffer, &begI, comment, strlen(comment));
        gtk_text_iter_forward_line(&begI);

        gtk_text_buffer_get_iter_at_mark(buffer, &begI, begM);
        gtk_text_buffer_get_iter_at_mark(buffer, &endI, endM);
        gtk_text_iter_forward_line(&begI);
        gtk_text_buffer_move_mark(buffer, begM, &begI);
    }
    gtk_text_buffer_insert(buffer, &begI, comment, strlen(comment));
}

void format_callback(GtkMenuItem *item, MetaData *meta_data)
{
    if (strcmp(gtk_menu_item_get_label(item), "Enable Auto-Indent") == 0)
    {
        meta_data->auto_indent = !meta_data->auto_indent;
    }
    if (strcmp(gtk_menu_item_get_label(item), "Highlight current line") == 0)
    {
        meta_data->highlight_line = !meta_data->highlight_line;
    }
    if (strcmp(gtk_menu_item_get_label(item), "Display line numbers") == 0)
    {
        meta_data->line_numbers = !meta_data->line_numbers;
    }
    if (strcmp(gtk_menu_item_get_label(item), "Text wrapping") == 0)
    {
        if (meta_data->wrap_mode == GTK_WRAP_NONE)
            meta_data->wrap_mode = GTK_WRAP_WORD_CHAR;
        else
            meta_data->wrap_mode = GTK_WRAP_NONE;
    }
    apply_format_prefs(meta_data);
}

void switch_case_sense_callback(GtkCheckButton *case_sense_btn, MetaData *meta_data)
{
    meta_data->case_sense_option = gtk_toggle_button_get_active((GtkToggleButton *)case_sense_btn);
}

void cancel_file_dlg_callback(GtkWidget *widget, MetaData *meta_data)
{
    gtk_window_close(meta_data->dial_data->dialog);
}

void forward_file_dlg_callback(GtkWidget *widget, MetaData *meta_data)
{
    FindDialog *dial_data = meta_data->dial_data;
    add_find_entry(meta_data, dial_data);
    gchar *search_text;
    GtkTextSearchFlags flags;
    GtkSourceView *text_view;
    GtkSourceBuffer *buffer;
    GtkTextMark *endM;
    GtkTextIter sel_iter, begI, endI;

    if (!meta_data->case_sense_option)
        flags = GTK_TEXT_SEARCH_CASE_INSENSITIVE;
    search_text = gtk_combo_box_text_get_active_text(dial_data->text_entry);
    text_view = get_current_page_view(meta_data);
    buffer = gtk_text_view_get_buffer(text_view);

    endM = gtk_text_buffer_get_mark(buffer, "selection_bound");
    gtk_text_buffer_get_iter_at_mark(buffer, &sel_iter, endM);

    if (gtk_text_iter_forward_search(&sel_iter, search_text, flags, &begI, &endI, NULL))
    {
        gtk_text_buffer_select_range(buffer, &begI, &endI);
        GtkTextMark *new_pos = gtk_text_buffer_get_mark(buffer, "selection_bound");
        gtk_text_view_scroll_to_mark(text_view, new_pos, 0.0, 1, 0.0, 0.5);
    }
    else
    {
        if (gtk_text_iter_get_offset(&sel_iter) == 0)
        {
            gchar *msg = g_strdup_printf("Not found:\"%s\"", search_text);
            display_error_message(meta_data, msg);
            g_free(msg);
            return;
        }
        GtkTextIter home;
        gtk_text_buffer_get_iter_at_offset(buffer, &home, 0);
        gtk_text_buffer_place_cursor(buffer, &home);
        forward_file_dlg_callback(NULL, meta_data);
    }
}

void backward_file_dlg_callback(GtkWidget *widget, MetaData *meta_data)
{
    FindDialog *dial_data = meta_data->dial_data;
    add_find_entry(meta_data, dial_data);
    gchar *search_text;
    GtkTextSearchFlags flags;
    GtkSourceView *text_view;
    GtkSourceBuffer *buffer;
    GtkTextMark *begM;
    GtkTextIter ins_iter, begI, endI;

    if (!meta_data->case_sense_option)
        flags = GTK_TEXT_SEARCH_CASE_INSENSITIVE;
    search_text = gtk_combo_box_text_get_active_text(dial_data->text_entry);
    text_view = get_current_page_view(meta_data);
    buffer = gtk_text_view_get_buffer(text_view);

    begM = gtk_text_buffer_get_mark(buffer, "insert");
    gtk_text_buffer_get_iter_at_mark(buffer, &ins_iter, begM);

    if (gtk_text_iter_backward_search(&ins_iter, search_text, flags, &begI, &endI, NULL))
    {
        gtk_text_buffer_select_range(buffer, &begI, &endI);
        GtkTextMark *new_pos = gtk_text_buffer_get_mark(buffer, "insert");
        gtk_text_view_scroll_to_mark(text_view, new_pos, 0.0, 1, 0.0, 0.5);
    }
    else
    {
        GtkTextIter end;
        gtk_text_buffer_get_end_iter(buffer, &end);
        if (gtk_text_iter_get_offset(&ins_iter) == gtk_text_iter_get_offset(&end))
        {
            gchar *msg = g_strdup_printf("Not found:\"%s\"", search_text);
            display_error_message(meta_data, msg);
            g_free(msg);
            return;
        }
        gtk_text_buffer_place_cursor(buffer, &end);
        backward_file_dlg_callback(NULL, meta_data);
    }
}

void find_callback(GtkMenuItem *menu_item, MetaData *meta_data)
{
    FindDialog *dial_data = (FindDialog *)malloc(sizeof(FindDialog));
    create_find_dlg(meta_data, dial_data);

    GtkSourceBuffer *buffer = get_current_page_buffer(meta_data);
    GtkTextIter home;
    gtk_text_buffer_get_iter_at_offset(buffer, &home, 0);
    gtk_text_buffer_place_cursor(buffer, &home);

    g_signal_connect(dial_data->dialog, "destroy",
                     G_CALLBACK(cancel_file_dlg_callback), meta_data);

    g_signal_connect(dial_data->case_sense_chk, "toggled",
                     G_CALLBACK(switch_case_sense_callback), meta_data);

    g_signal_connect(dial_data->backward_btn, "clicked",
                     G_CALLBACK(backward_file_dlg_callback), meta_data);

    g_signal_connect(dial_data->forward_btn, "clicked",
                     G_CALLBACK(forward_file_dlg_callback), meta_data);

    g_signal_connect(dial_data->cancel_btn, "clicked",
                     G_CALLBACK(cancel_file_dlg_callback), meta_data);

    gtk_widget_show(dial_data->dialog);
}

void goto_callback(GtkMenuItem *menu_item, MetaData *meta_data)
{
    GtkWidget *label;
    gchar *label_text;
    GtkWidget *entry;
    GtkWidget *separator;
    guint dest_line;

    GtkWidget *buffer = get_current_page_buffer(meta_data);
    gint lines = gtk_text_buffer_get_line_count(buffer);

    label_text = g_strdup_printf("Line (1 - %d): ", lines);
    label = gtk_label_new(label_text);
    gtk_widget_set_halign(label, GTK_ALIGN_START);

    entry = gtk_entry_new();
    separator = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);

    GtkDialogFlags flags = GTK_DIALOG_DESTROY_WITH_PARENT | GTK_DIALOG_MODAL;
    GtkWidget *goto_dialog = gtk_dialog_new_with_buttons("Go to Line", meta_data->window,
                                                         flags,
                                                         GTK_STOCK_OK,
                                                         GTK_RESPONSE_OK,
                                                         GTK_STOCK_CANCEL,
                                                         GTK_RESPONSE_REJECT,
                                                         NULL);
    GtkWidget *content_area = gtk_dialog_get_content_area(goto_dialog);
    gtk_box_set_spacing(GTK_BOX(content_area), 8);
    gtk_box_pack_start(GTK_BOX(content_area), label, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(content_area), entry, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(content_area), separator, TRUE, TRUE, 0);
    gtk_widget_show_all(goto_dialog);

    gint response = gtk_dialog_run(goto_dialog);
    GtkTextIter iter;
    GtkTextMark *last_pos;
    GtkWidget *text_view = get_current_page_view(meta_data);
    switch (response)
    {
    case GTK_RESPONSE_OK:
        dest_line = strtoul(gtk_entry_get_text(entry), NULL, 0);
        if (dest_line > 0 && dest_line <= lines)
        {
            gtk_text_buffer_get_iter_at_line(buffer, &iter, dest_line - 1);
            gtk_text_buffer_place_cursor(buffer, &iter);
            last_pos = gtk_text_buffer_create_mark(buffer, "last_pos", &iter, FALSE);
            gtk_text_view_scroll_to_mark(text_view, last_pos, 0.0, 1, 0.0, 0.5);
            gtk_text_buffer_delete_mark(buffer, last_pos);
        }
        break;
    case GTK_RESPONSE_REJECT:
        break;
    }

    gtk_widget_destroy(goto_dialog);
    g_free(label_text);
}

gboolean compile_callback(GObject *menu_item, MetaData *meta_data)
{
    GtkWidget *terminal;
    GtkWidget *box;
    GList *box_children;
    gchar *filename;
    gchar *command;
    gint curr_page;
    gboolean success = TRUE;

    curr_page = gtk_notebook_get_current_page(meta_data->notebook);
    box = gtk_notebook_get_nth_page(meta_data->notebook, curr_page);
    box_children = gtk_container_get_children(GTK_CONTAINER(box));
    box_children = box_children->next;
    terminal = box_children->data;

    filename = (gchar *)malloc(sizeof(gchar) * 255);
    command = (gchar *)malloc(sizeof(gchar) * 4096);
    get_page_filename(meta_data, curr_page, filename);

    const gchar *tab_name = gtk_notebook_get_tab_label_text(meta_data->notebook, box);

    if (strcmp(filename, "Untitled") == 0 || tab_name[0] == '*')
    {
        gchar *msg = g_strdup("File is not saved");
        display_error_message(meta_data, msg);
        success = FALSE;
        g_free(msg);
    }
    else
    {
        calc_compile_command(meta_data, command, filename);

        vte_terminal_feed_child(terminal, command, strlen(command));
    }
    free(filename);
    free(command);
    return success;
}

void run_callback(GtkMenuItem *menu_item, MetaData *meta_data)
{
    GtkWidget *terminal;
    GtkWidget *box;
    GList *box_children;
    gchar *filename;
    gchar *command;
    gint curr_page;

    printf("%d\n", meta_data->notebook);

    curr_page = gtk_notebook_get_current_page(meta_data->notebook);
    box = gtk_notebook_get_nth_page(meta_data->notebook, curr_page);
    box_children = gtk_container_get_children(GTK_CONTAINER(box));
    box_children = box_children->next;
    terminal = box_children->data;

    filename = (gchar *)malloc(sizeof(gchar) * 255);
    command = (gchar *)malloc(sizeof(gchar) * 4096);
    get_page_filename(meta_data, curr_page, filename);

    if (strcmp(filename, "Untitled") == 0)
    {
        GtkDialogFlags flags = GTK_DIALOG_MODAL;
        GtkWidget *dialog = gtk_message_dialog_new(meta_data->window,
                                                   flags,
                                                   GTK_MESSAGE_ERROR,
                                                   GTK_BUTTONS_CLOSE,
                                                   "File is not saved:");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
    }
    else
    {
        calc_run_command(meta_data, command, filename);

        gtk_widget_grab_focus(GTK_WIDGET(terminal));
        printf("%s\n", command);
        vte_terminal_feed_child(terminal, command, strlen(command));
    }
    free(filename);
    free(command);
}

void compile_and_run_callback(GtkMenuItem *menu_item, MetaData *meta_data)
{
    if (compile_callback(menu_item, meta_data))
    {
        run_callback(menu_item, meta_data);
    }
}

void update_statusbar_col_lines(GtkSourceBuffer *buffer, GtkStatusbar *statusbar)
{
    gchar *posititon_msg;
    gint row, col, pos;
    GtkTextIter iter;

    gtk_statusbar_pop(statusbar, 0);

    gtk_text_buffer_get_iter_at_mark(buffer, &iter, gtk_text_buffer_get_insert(buffer));

    row = gtk_text_iter_get_line(&iter);
    col = gtk_text_iter_get_line_offset(&iter);
    pos = gtk_text_iter_get_offset(&iter);

    posititon_msg = g_strdup_printf("Line %d,   Col %d,   Pos %d", row + 1, col + 1, pos + 1);

    gtk_statusbar_push(statusbar, 0, posititon_msg);

    g_free(posititon_msg);
}

void update_statusbar_language(GtkSourceBuffer *buffer, GtkStatusbar *statusbar)
{
    gchar *language_msg;
    GtkSourceLanguage *lang = gtk_source_buffer_get_language(buffer);

    gtk_statusbar_pop(statusbar, 0);

    language_msg = g_strdup_printf("Language: %s", gtk_source_language_get_name(lang));

    gtk_statusbar_push(statusbar, 0, language_msg);

    g_free(language_msg);
}

void update_unsaved_tab(GtkSourceBuffer *buffer, GtkWidget *tab)
{
    const gchar *tab_name = gtk_label_get_text(GTK_LABEL(tab));
    if (tab_name[0] != '*')
    {
        const gchar *language_msg = g_strdup_printf("*%s", tab_name);
        gtk_label_set_text(GTK_LABEL(tab), language_msg);
        g_free(language_msg);
    }
}

void mark_set_callback(GtkTextBuffer *buffer, const GtkTextIter *new_location, GtkTextMark *mark, gpointer data)
{
    update_statusbar_col_lines(buffer, GTK_STATUSBAR(data));
}
void search_bar_callback(GtkWidget *widget, GdkEvent *event, gpointer user_data)
{
    GtkSearchBar *bar = GTK_SEARCH_BAR(user_data);
    return gtk_search_bar_handle_event(bar, event);
}

#endif
