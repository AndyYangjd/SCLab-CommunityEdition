#ifndef SCLAB_CONSOLE_WIDGET_H
#define SCLAB_CONSOLE_WIDGET_H

#include <QDebug>
#include <QMimeData>
#include <QEvent>
#include <QKeyEvent>
#include <QWidget>
#include <QSplitter>
#include <QStackedLayout>
#include <QPlainTextEdit>
#include <QTextBlock>
#include <QScrollBar>
#include <QTextDocument>
#include <QTextDocumentFragment>
#include <QFontMetrics>
#include <QTimer>
#include <QMenu>
#include <QApplication>


QT_BEGIN_NAMESPACE
namespace Ui { class ConsoleWidget; }
QT_END_NAMESPACE

/**
 *  ConsoleWidget ia one part of Console view.
 *  1. an abstract base class
 *  2. maintain a prompt and editing region
 *  3. provide the traditional Unix-style console keyboard shortcuts
 *  4. perform tab completion
 *  5. handle ANSI escape codes
 *
 */
class ConsoleWidget : public QWidget {
Q_OBJECT

public:
    explicit ConsoleWidget(QWidget *parent = nullptr);

    ~ConsoleWidget() override;

    // Console-widget public interface
    bool canCut();
    bool canCopy();
    bool canPaste();
    void clear(bool);

    void copyAnchor(const QTextCursor);

    void handleIsCompleteReply(const QString& msg){};
    void triggerIsCompleteCallback(bool is_complete=false, QString indent=""){};
    void registerIsCompleteCallback(const QString& source, const QString& callback){};
    void execute(QString source="", bool is_hidden=false, bool is_interactive=false);
    void doExecute(const QString& source, bool is_complete=false, QString indent="");

    void finalizeInputRequest();
    QString getInputBuffer(bool is_force=false);
    void setInputBuffer(const QString& text);
    static QFont getFont(const QString&, const QString&);
    static int getFontWidth(const QFont&);
    void setFont(const QFont&);
    void resetFont();
    void setCompleteWidget(){};
    void openAnchor(){};

    void promptToTop(void){};
    void redo(){control_->redo();}
    void undo(){control_->undo();}
    void changeFontSize(const int delta){};
    int getTabWidth(){return tab_width_;}
    void setTabWidth(const int tab_space){tab_width_ = tab_space;}

    // Console-widget abstract interface
    virtual void _is_complete(){};
    virtual void _execute(QString source, bool is_hidden){;};
    virtual void _prompt_started_hook(){};
    virtual void _prompt_finished_hook(){};
    virtual bool _up_pressed(){return true;}
    virtual bool _down_pressed(){return true;}
    virtual bool _tab_pressed(){return true;}

protected:
    using PF_insertPlainText = void(ConsoleWidget::*)(QTextCursor, const QString&, bool);
    using PF_insertBlock = void(ConsoleWidget::*)(QTextCursor, QTextBlockFormat);

    /**
     *  PLAIN:      show the available completion as a text list  below the editing area.
     *  DROP_LIST:  show the completion in a drop down list navigable by the arrow keys, and
     *              from which you can select completion by pressing Return.
     *  NCURSES:    Show the completion as a text list which is navigable by `tab` and arrow keys.
     *
     */
    enum GUI_COMPLETE{PLAIN, DROP_LIST, NCURSES};
    GUI_COMPLETE gui_completion_{GUI_COMPLETE::NCURSES};

    /**
     *  INSIDE: The widget pages like a traditional terminal.
     *  H_SPLIT: When paging is requested, the widget is split horizontally. The top
     *           pane contains the console, and the bottom pane contains the paged text.
     *  V_SPLIT: Similar to H_SPLIT, except that a vertical splitter is used.
     */
    enum PAGING{INSIDE, H_SPLIT, V_SPLIT};
    PAGING paging_{PAGING::INSIDE};

    bool handle_ansi_escape_code_{true};
    int buffer_size_{500};
    bool execute_on_complete_input_{false};
    bool scrollbar_visibility_{false};

    QString font_family_{"JetBrains Mono"};
    QString font_name_{"JetBrains Mono Light"};
    int size_font_{12u};
    QFont cur_font_{};

    unsigned int width_console_{81};
    unsigned int height_console_{25};

    unsigned long complete_msg_id{0u};
    float complete_timeout_{0.1f};
    float complete_max_time{2.0f};

    // QStackedLayout* layout_{nullptr};
    QPlainTextEdit* control_{nullptr};
    QPlainTextEdit* page_control_{nullptr};
    QSplitter* splitter_{nullptr};

    // define cursor
    QTextCursor append_before_prompt_cursor_{};
    QTextCursor prompt_cursor_{};
    int append_before_prompt_pos_{0};
    QString continuation_prompt_{">"};
    QString continuation_prompt_html_{""};
    bool is_executing_{false};
    bool filter_resize_{false};
    QString input_buffer_executing_{""};
    QString input_buffer_pending_{""};
    QString prompt_{""};
    QString prompt_html_{""};
    QString prompt_sep_{""};
    int getPromptPos();
    bool is_reading_{false};
    int tab_width_{4};
    QStringList pending_insert_text{};

    QTimer* pending_text_flush_interval_{nullptr};
    QMenu* menu_{nullptr};

    QAction* act_print_{nullptr};
    QAction* act_save_as_html_{nullptr};
    QAction* act_cut_{nullptr};
    QAction* act_copy_{nullptr};
    QAction* act_paste_{nullptr};
    QAction* act_select_all_{nullptr};
    QAction* act_export_{nullptr};
    QAction* act_increase_font_size_{nullptr};
    QAction* act_decrease_font_size_{nullptr};

    QString input_buffer_{};
    bool is_temp_buffer_filled{true};

    // self-defined functions
    void initSelfDefinedUI();

    // drag and drop support
    bool eventFilter(QObject* obj, QEvent* e);

    // QWidget interface
    QSize sizeHint();

    // console-widget protected interface
    void appendCustom(PF_insertBlock , QTextBlockFormat, bool is_before_prompt=false);
    void appendCustom(PF_insertPlainText, const QString&, bool is_before_prompt=false);
    void appendBlock(QTextBlockFormat _format, bool _is_before_prompt=false);
    void appendHtml(){};
    QString appendHtmlFetchingPlainText(const QString&);
    void appendPlainText(const QString&, bool is_before_prompt= false);
    void cancelComplete(){};
    void clearTemporaryBuffer();
    void completeWithItems(){};
    void fillTemporaryBuffer(QTextCursor, QString text, bool is_html=false);
    QMenu* makeContextMenu(const QPoint&);
    bool controlKeyDown(Qt::KeyboardModifiers modifiers);
    QPlainTextEdit* createControl();
    QPlainTextEdit* createPageControl();
    bool eventFilterConsoleKeypress(QKeyEvent * );
    bool eventFilterPageKeypress(QKeyEvent * );
    void onFlushPendingStreamTimer(){};

    void formatAsColumns(){};
    void getBlockPlainText(){};
    QTextCursor getCursor();
    QTextCursor getEndCursor();
    int getEndPos();
    QTextCursor getLineStartCursor(){};
    int getLineStartPos(){return getLineStartCursor().position();}
    QTextCursor getLineEndCursor(){};
    int getLineEndPos(){return getLineEndCursor().position();}
    void getInputBufferCursorColumn(){};
    void getInputBufferCursorLine(){};
    void getInputBufferCursorPos(){};
    void getInputBufferCursorPrompt(){};
    void getLastLines(const QString& text, int num_lines, bool is_return_count=false){};
    void getLastLinesFromList(const QStringList& text_list, int num_lines){};
    void getLeadingSpaces(){};
    QTextCursor getPromptCursor();
    QTextCursor getSelectionCursor(){};
    QTextCursor getWordStartCursor(const int pos){};
    QTextCursor getWordEndCursor(const int pos);
    void _indent(bool is_de_indent=true);
    void setContinuationPrompt(const QString&, bool);
    void insertBlock(QTextCursor, QTextBlockFormat);
    void insertHtml(){};
    void insertHtmlFetchingPlainText(){};
    void viewportAtEnd(){};
    void scrollToEnd(){};
    void insertPlainText(QTextCursor, const QString&, bool is_flush= false);
    void insertPlainTextIntoBuffer(QTextCursor cursor, const QString& text){};
    bool inBuffer(const int pos);
    int movePositionInBuffer(const int pos);
    void keepCursorInBuffer(){};
    void quitKeyboard();
    void _page(QString text, bool is_html=false);
    void getPaging(){};
    void finishPrompt();
    void startPrompt();
    void readline(){};
    void setContinuationPrompt(){};
    void insertContinuationPrompt(QTextCursor, QString indent="");
    void setCursor(){};
    void setTopCursor(){};
    void showPrompt(const QString&, bool _is_html= false, bool _is_newline= true, bool _is_separator=true);

    void appendBeforePromptPos(){};
    void promptPos(){};
    int getLinesCounter();

    // property
private:
    QWidget* parent_{};
    QStackedLayout* layout_{nullptr};
    void startPromptHook();
    void finishPromptHook();
    void dragEnterEvent(QDragEnterEvent *);
    void dragMoveEvent(QDragEnterEvent *);
    void dropEvent(QDragEnterEvent *);
signals:
    void changeFont(QFont);
    void changeTextCursor(int, int);

protected slots:
    void changeCursorPosition();
    void copyAvailable(bool);
    void redoAvailable(bool);
    void undoAvailable(bool);
    void requestCustomContextMenu(const QPoint &);
    void adjustScrollbar(const QSizeF &);

    void selectDocument();
    void increaseFontSize();
    void decreaseFontSize();
    void flushPendingStream();
    void print();
    void exportHtml();
    void selectAllSmart();
    void cut();
    void copy();
    void paste();
};


#endif //SCLAB_CONSOLE_WIDGET_H
