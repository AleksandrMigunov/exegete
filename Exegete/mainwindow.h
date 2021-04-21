/*
Copyright (C) 2021 Aleksandr Migunov

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSettings>
#include <QCloseEvent>
#include <QPrinter>
#include <QTextCodec>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void readSettings();
    void writeSettings();
    void closeEvent(QCloseEvent *event);

    QString language;

private slots:
    void on_textEdit_List_selectionChanged();

    void on_textEdit_Original_selectionChanged();

    void on_textEdit_TargetText_selectionChanged();

    void on_textEdit_BaseText_selectionChanged();

    void on_textEdit_Additional1_selectionChanged();

    void on_textEdit_Additional2_selectionChanged();

    void on_actionOpen_original_triggered();

    void on_actionOpen_list_triggered();

    void on_actionOpen_target_text_triggered();

    void on_actionOpen_base_text_triggered();

    void on_actionClose_original_triggered();

    void on_actionClose_list_triggered();

    void on_actionClose_target_text_triggered();

    void on_actionClose_base_text_triggered();

    void on_actionNew_list_triggered();

    void on_actionSave_list_triggered();

    void on_actionSave_list_as_triggered();

    void on_actionPrint_original_triggered();

    void on_actionPrint_list_triggered();

    void on_actionPrint_target_text_triggered();

    void on_actionPrint_base_text_triggered();

    void paintPreviewOriginal(QPrinter *printer);

    void on_actionPrint_preview_original_triggered();

    void paintPreviewList(QPrinter *printer);

    void on_actionPrint_preview_list_triggered();

    void paintPreviewTargetText(QPrinter *printer);

    void on_actionPrint_preview_target_text_triggered();

    void paintPreviewBaseText(QPrinter *printer);

    void on_actionPrint_preview_base_text_triggered();

    void on_actionConvert_original_to_PDF_triggered();

    void on_actionConvert_list_to_PDF_triggered();

    void on_actionConvert_target_text_to_PDF_triggered();

    void on_actionConvert_base_text_to_PDF_triggered();

    void on_actionConvert_original_to_ODT_triggered();

    void on_actionConvert_list_to_ODT_triggered();

    void on_actionConvert_target_text_to_ODT_triggered();

    void on_actionConvert_base_text_to_ODT_triggered();

    void on_actionExit_triggered();

    void on_actionUndo_triggered();

    void on_actionRedo_triggered();

    void on_actionCut_triggered();

    void on_actionCopy_from_original_triggered();

    void on_actionCopy_from_list_triggered();

    void on_actionCopy_from_target_text_triggered();

    void on_actionCopy_from_base_text_triggered();

    void on_actionPaste_triggered();

    void on_actionCopy_and_paste_from_original_triggered();

    void on_actionCopy_and_paste_from_list_triggered();

    void on_actionCopy_and_paste_from_target_text_triggered();

    void on_actionCopy_and_paste_from_base_text_triggered();

    void on_actionDelete_triggered();

    void on_actionSelect_all_from_original_triggered();

    void on_actionSelect_all_from_list_triggered();

    void on_actionSelect_all_from_target_text_triggered();

    void on_actionSelect_all_from_base_text_triggered();

    void on_actionFind_in_original_triggered();

    void on_actionFind_in_list_triggered();

    void on_actionFind_in_target_text_triggered();

    void on_actionFind_in_base_text_triggered();

    void on_actionFind_and_replace_triggered();

    void on_actionFind_and_replace_all_triggered();

    void on_actionZoom_in_all_windows_triggered();

    void on_actionZoom_in_original_triggered();

    void on_actionZoom_in_list_triggered();

    void on_actionZoom_in_target_text_triggered();

    void on_actionZoom_in_base_text_triggered();

    void on_actionZoom_out_all_windows_triggered();

    void on_actionZoom_out_original_triggered();

    void on_actionZoom_out_list_triggered();

    void on_actionZoom_out_target_text_triggered();

    void on_actionZoom_out_base_text_triggered();

    void on_actionFont_of_original_triggered();

    void on_actionFont_of_list_triggered();

    void on_actionFont_of_target_text_triggered();

    void on_actionFont_of_base_text_triggered();

    void on_actionText_color_of_original_triggered();

    void on_actionText_color_of_list_triggered();

    void on_actionText_color_of_target_text_triggered();

    void on_actionText_color_of_base_text_triggered();

    void on_actionBackground_color_of_original_triggered();

    void on_actionBackground_color_of_list_triggered();

    void on_actionBackground_color_of_target_text_triggered();

    void on_actionBackground_color_of_base_text_triggered();

    void on_actionPalette_color_of_all_windows_triggered();

    void on_actionPalette_color_of_original_triggered();

    void on_actionPalette_color_of_list_triggered();

    void on_actionPalette_color_of_target_text_triggered();

    void on_actionPalette_color_of_base_text_triggered();

    void on_actionFormatted_target_text_triggered();

    void on_actionUnformatted_target_text_triggered();

    void on_actionFormatted_base_text_triggered();

    void on_actionUnformatted_base_text_triggered();

    void on_actionFormatted_original_triggered();

    void on_actionUnformatted_original_triggered();

    void on_actionEncoding_of_original_triggered();

    void on_actionEncoding_of_list_triggered();

    void on_actionEncoding_of_target_text_triggered();

    void on_actionEncoding_of_base_text_triggered();

    void on_actionInsert_book_title_triggered();

    void on_actionInsert_chapter_number_triggered();

    void on_actionInsert_chapters_numbers_triggered();

    void on_actionInsert_verse_number_triggered();

    void on_actionInsert_verses_numbers_triggered();

    void on_actionInsert_question_number_triggered();

    void on_actionInsert_time_of_work_start_triggered();

    void on_actionInsert_time_of_work_end_triggered();

    void on_actionInsert_list_title_triggered();

    void on_actionEnglish_triggered();

    void on_actionRussian_triggered();

    void on_actionInsert_time_automatically_triggered();

    void on_actionInsert_title_automatically_triggered();

    void on_actionLoad_files_automatically_triggered();

    void on_actionOriginalUSFM_HTML_triggered();

    void on_actionTarget_textUSFM_HTML_triggered();

    void on_actionBase_textUSFM_HTML_triggered();

    void on_actionEncoding_for_USFM_HTML_triggered();

    void on_actionAbout_triggered();

    void on_actionTextBold_triggered();

    void on_actionTextItalic_triggered();

    void on_actionTextUnderline_triggered();

    void on_actionAlignLeft_triggered();

    void on_actionAlignRight_triggered();

    void on_actionAlignCenter_triggered();

    void on_actionAlignJustify_triggered();

    void on_textEdit_Original_customContextMenuRequested();

    void on_textEdit_List_customContextMenuRequested();

    void on_textEdit_TargetText_customContextMenuRequested();

    void on_textEdit_BaseText_customContextMenuRequested();

    void on_actionFormatted_Transcelerator_triggered();

    void on_actionWord_is_missing_triggered();

    void on_actionWords_are_missing_triggered();

    void on_actionWord_should_be_replaced_triggered();

    void on_actionWords_should_be_replaced_triggered();

    void on_actionIn_original_triggered();

    void on_actionIn_Chechen_triggered();

    void on_actionIn_Avar_triggered();

    void on_actionIn_Adyghe_triggered();

    void on_actionIn_Kabardian_triggered();

    void on_actionIn_CARS_triggered();

    void on_actionIn_CRV_triggered();

    void on_actionIn_CASS_triggered();

    void on_actionIn_RBT_Kulakov_triggered();

    void on_actionOpen_additional_text_1_triggered();

    void on_actionOpen_additional_text_2_triggered();

    void on_actionClose_additional_text_1_triggered();

    void on_actionClose_additional_text_2_triggered();

    void on_actionPrint_additional_text_1_triggered();

    void on_actionPrint_additional_text_2_triggered();

    void paintPreviewAdditional1(QPrinter *printer);
    void on_actionPrint_preview_additional_text_1_triggered();

    void paintPreviewAdditional2(QPrinter *printer);
    void on_actionPrint_preview_additional_text_2_triggered();

    void on_actionConvert_additional_text_1_to_PDF_triggered();

    void on_actionConvert_additional_text_2_to_PDF_triggered();

    void on_actionConvert_additional_text_1_to_ODT_triggered();

    void on_actionConvert_additional_text_2_to_ODT_triggered();

    void on_actionCopy_from_additional_text_1_triggered();

    void on_actionCopy_from_additional_text_2_triggered();

    void on_actionCopy_and_paste_from_additional_text_1_triggered();

    void on_actionCopy_and_paste_from_additional_text_2_triggered();

    void on_actionSelect_all_from_additional_text_1_triggered();

    void on_actionSelect_all_from_additional_text_2_triggered();

    void on_actionFind_in_additional_text_1_triggered();

    void on_actionFind_in_additional_text_2_triggered();

    void on_actionZoom_in_additional_text_1_triggered();

    void on_actionZoom_in_additional_text_2_triggered();

    void on_actionZoom_out_additional_text_1_triggered();

    void on_actionZoom_out_additional_text_2_triggered();

    void on_actionFont_of_additional_text_1_triggered();

    void on_actionFont_of_additional_text_2_triggered();

    void on_actionText_color_of_additional_text_1_triggered();

    void on_actionText_color_of_additional_text_2_triggered();

    void on_actionBackground_color_of_additional_text_1_triggered();

    void on_actionBackground_color_of_additional_text_2_triggered();

    void on_actionPalette_color_of_additional_text_1_triggered();

    void on_actionPalette_color_of_additional_text_2_triggered();

    void on_actionFormatted_additional_text_1_triggered();

    void on_actionUnformatted_additional_text_1_triggered();

    void on_actionFormatted_additional_text_2_triggered();

    void on_actionUnformatted_additional_text_2_triggered();

    void on_actionEncoding_of_additional_text_1_triggered();

    void on_actionEncoding_of_additional_text_2_triggered();

    void on_actionAdditional_text_1_USFM_HTML_triggered();

    void on_actionAdditional_text_2_USFM_HTML_triggered();

    void on_textEdit_Additional1_customContextMenuRequested();

    void on_textEdit_Additional2_customContextMenuRequested();

    void on_actionIn_NRT_Biblica_triggered();

    void on_actionIn_Synodal_triggered();

    void on_actionIs_the_pronoun_reference_clear_triggered();

    void on_actionWould_it_be_better_to_turn_it_to_a_pronoun_triggered();

    void on_actionIs_this_sentence_too_complicated_or_long_triggered();

    void on_actionWould_it_be_better_to_break_up_this_long_complicated_sentence_triggered();

    void on_actionWould_it_be_better_to_restructure_the_sentence_triggered();

    void on_actionHow_does_this_verse_connect_to_the_preceding_text_triggered();

    void on_actionIt_would_be_more_accurate_to_translate_triggered();

    void on_actionTranslation_has_added_word_s_triggered();

    void on_actionHow_is_this_figurative_expression_understood_triggered();

    void on_actionOpen_Paratext_file_for_editing_triggered();

    void on_actionSave_Paratext_file_triggered();

    void on_actionSave_Paratext_file_as_triggered();

    void on_actionClose_Paratext_file_triggered();

    void on_actionFormatted_Paratext_file_triggered();

    void on_actionUnformatted_Paratext_file_triggered();

    void on_actionShow_hide_file_paths_and_names_triggered();

    void on_actionAbout_Qt_triggered();

private:
    Ui::MainWindow *ui;
    QString FilenameList;
    QString FilenameOriginal;
    QString FilenameTargetText;
    QString FilenameBaseText;
    QString FilenameAdditional1;
    QString FilenameAdditional2;
    QString TextList;

    QString textEditName;

    QString file_list;
    QString file_original;
    QString file_target_text;
    QString file_base_text;
    QString file_additional_1;
    QString file_additional_2;

    bool TextBold;
    bool TextItalic;
    bool TextUnderline;
    bool TimeAuto;
    bool TitleAuto;
    bool FilesNotLoadAuto;

    int ZoomList;
    int ZoomOriginal;
    int ZoomTargetText;
    int ZoomBaseText;
    int ZoomAdditional1;
    int ZoomAdditional2;

    QString EncodingList;
    QString EncodingOriginal;
    QString EncodingTargetText;
    QString EncodingBaseText;
    QString EncodingAdditional1;
    QString EncodingAdditional2;
    QString EncodingHTML;

    bool ParatextFileEditing;
    QString ParatextFileName;

    void textEdit_focused(QString textEditName);

    void set_zoom_at_start();
    void close_list();

    void open_files_at_start();

    void open_list(QString file_list);
    void open_original(QString file_original);
    void open_target_text(QString file_target_text);
    void open_base_text(QString file_base_text);
    void open_additional_1(QString file_additional_1);
    void open_additional_2(QString file_additional_2);

    void formatted(QString document_name);
    void unformatted(QString document_name);
    void convert_to_PDF(QString writing, QString document_name);
    void convert_to_ODT(QString writing, QString document_name);
    void search_word(QString document_name);
    void change_encoding(QString document_name);

    void USFM_to_HTML(QString document_name);

    void hideLabelDirs();
    void showLabelDirs();
    bool showDirs;

protected:
    QSettings *m_settings;
    QVariant list;
    QVariant original;
    QVariant target_text;
    QVariant base_text;
    QVariant additional_1;
    QVariant additional_2;
    QVariant lang;
    QVariant time_auto;
    QVariant title_auto;
    QVariant files_not_load_auto;
    QVariant zoom_list;
    QVariant zoom_original;
    QVariant zoom_target_text;
    QVariant zoom_base_text;
    QVariant zoom_additional_1;
    QVariant zoom_additional_2;
};

#endif // MAINWINDOW_H
