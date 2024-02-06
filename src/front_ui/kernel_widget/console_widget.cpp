#include "console_widget.h"


ConsoleWidget::ConsoleWidget(QWidget *parent) :
        parent_(parent){
    this->initSelfDefinedUI();

    // startPrompt();
}

ConsoleWidget::~ConsoleWidget() {
    delete control_;
    delete splitter_;
    delete pending_text_flush_interval_;
    delete menu_;
}

void ConsoleWidget::initSelfDefinedUI() {
    layout_ = new QStackedLayout();
    setLayout(layout_);
    layout_->setContentsMargins(0, 0, 0, 0);

    control_ = this->createControl();
    if (paging_ == PAGING::INSIDE) {
        layout_->addWidget(control_);
    } else {
        splitter_ = new QSplitter();
        if (paging_ == PAGING::H_SPLIT) {
            splitter_->setOrientation(Qt::Horizontal);
        } else {
            splitter_->setOrientation(Qt::Vertical);
        }
        splitter_->addWidget(control_);
        layout_->addWidget(splitter_);
    }

    // create the paging widget, if necessary
    page_control_ = createPageControl();
    if (splitter_ == nullptr) {
        layout_->addWidget(page_control_);
    } else {
        page_control_->hide();
        splitter_->addWidget(page_control_);
    }
    layout_->setCurrentIndex(0);

    // define text-cursor
    append_before_prompt_cursor_= control_->textCursor();
    prompt_cursor_ = control_->textCursor();

    pending_text_flush_interval_ = new QTimer(control_);
    pending_text_flush_interval_->setInterval(100);
    pending_text_flush_interval_->setSingleShot(true);
    connect(pending_text_flush_interval_, SIGNAL(timeout()), this, SLOT(flushPendingStream()));

    // reset font
    resetFont();

    act_print_ = new QAction("Print");
    act_print_->setEnabled(true);
    act_print_->setShortcut(QKeySequence::Print);
    act_print_->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    connect(act_print_, SIGNAL(triggered(bool)), this, SLOT(print()));
    this->addAction(act_print_);

    act_save_as_html_ = new QAction("Save as HTML/XML");
    act_save_as_html_->setShortcut(QKeySequence::Save);
    act_save_as_html_->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    connect(act_save_as_html_, SIGNAL(triggered(bool)), this, SLOT(exportHtml()));
    this->addAction(act_save_as_html_);

    act_select_all_ = new QAction("Select All");
    act_select_all_->setEnabled(true);
    act_select_all_->setShortcut(QKeySequence::SelectAll);
    act_select_all_->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    connect(act_select_all_, SIGNAL(triggered(bool)), this, SLOT(selectAllSmart()));
    this->addAction(act_select_all_);

    act_increase_font_size_ = new QAction("Bigger Font Size");
    act_increase_font_size_->setShortcut(QKeySequence::ZoomIn);
    act_increase_font_size_->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    act_increase_font_size_->setStatusTip("Increase the font size by one point");
    connect(act_increase_font_size_, SIGNAL(triggered(bool)), this, SLOT(increaseFontSize()));
    this->addAction(act_increase_font_size_);

    act_decrease_font_size_ = new QAction("Smaller Font Size");
    act_decrease_font_size_->setShortcut(QKeySequence::ZoomOut);
    act_decrease_font_size_->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    act_decrease_font_size_->setStatusTip("Decrease the font size by one point");
    connect(act_decrease_font_size_, SIGNAL(triggered(bool)), this, SLOT(decreaseFontSize()));
    this->addAction(act_decrease_font_size_);

    this->setAcceptDrops(true);
}


int ConsoleWidget::getFontWidth(const QFont & _font) {
    auto font_metrics = QFontMetrics(_font);
    auto font_width =  font_metrics.horizontalAdvance(" ");
    return font_width;
}

void ConsoleWidget::setFont(const QFont & _font) {
    int font_width = getFontWidth(_font);

    // control_->setTabStopWidth();
    control_->setTabStopDistance(tab_width_ * font_width);

    control_->document()->setDefaultFont(_font);
    page_control_->document()->setDefaultFont(_font);

    emit changeFont(_font);
}

QFont ConsoleWidget::getFont(const QString& _family, const QString& _name){
    QFont font = QFont(_family);
    auto font_info = QFontInfo(font);

    if (_name!="" || font_info.family() != _family){
        font = QFont(_name);
    }

    return font;
}

void ConsoleWidget::resetFont() {
    cur_font_ = getFont(font_family_, font_name_);
    cur_font_.setPointSize(size_font_);
    cur_font_.setStyleHint(QFont::TypeWriter);

    setFont(cur_font_);
}

// Reimplemented to suggest a size that is 80 characters wide and 25 lines high.
QSize ConsoleWidget::sizeHint() {
    // auto font_metrics = QFontMetrics(cur_font_);
    // auto margin = (control_->frameWidth() + control_->document()->documentMargin())*2;

    return QSize();
}

void ConsoleWidget::decreaseFontSize(){

}

void ConsoleWidget::increaseFontSize(){

}

void ConsoleWidget::selectDocument() {

}

void ConsoleWidget::exportHtml() {

}

void ConsoleWidget::print() {

}

QPlainTextEdit *ConsoleWidget::createPageControl() {
    QPlainTextEdit *_page_control{nullptr};
    if (page_control_ == nullptr) {
        _page_control = new QPlainTextEdit();
    } else {
        _page_control = page_control_;
    }

    _page_control->installEventFilter(this);
    _page_control->viewport()->installEventFilter(this);
    _page_control->setReadOnly(true);
    _page_control->setUndoRedoEnabled(false);

    if (scrollbar_visibility_) {
        _page_control->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    } else {
        _page_control->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    }

    return _page_control;
}

QPlainTextEdit *ConsoleWidget::createControl() {
    QPlainTextEdit *control{nullptr};
    if (control_ == nullptr) {
        control = new QPlainTextEdit();
    } else {
        control = control_;
    }

    control->setFrameShape(QFrame::Box);
    control->setFrameShadow(QFrame::Plain);

    control->setAcceptDrops(false);
    control->installEventFilter(this);
    control->viewport()->installEventFilter(this);
    control->setAttribute(Qt::WA_InputMethodEnabled, true);
    control->setContextMenuPolicy(Qt::CustomContextMenu);
    control->setReadOnly(true);
    control->setUndoRedoEnabled(false);

    if (scrollbar_visibility_) {
        control->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    } else {
        control->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    }

    connect(control, SIGNAL(cursorPositionChanged()), this, SLOT(changeCursorPosition()));
    connect(control, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(requestCustomContextMenu(const QPoint &)));
    connect(control, SIGNAL(copyAvailable(bool)), this, SLOT(copyAvailable(bool)));
    connect(control, SIGNAL(redoAvailable(bool)), this, SLOT(redoAvailable(bool)));
    connect(control, SIGNAL(undoAvailable(bool)), this, SLOT(undoAvailable(bool)));
    connect(control->document()->documentLayout(), SIGNAL(documentSizeChanged(const QSizeF &)), this,
            SLOT(adjustScrollbar(const QSizeF &)));

    return control;
}

int ConsoleWidget::getLinesCounter(){
    return control_->document()->lineCount();
}

void ConsoleWidget::changeCursorPosition() {
    QTextCursor cursor = control_->textCursor();
    QTextLayout * layout = cursor.block().layout();

    int cur_pos = cursor.position() - cursor.block().position();

    // text_line record the true line-number
    int cur_line_number = layout->lineForTextPosition(cur_pos).lineNumber() + cursor.block().firstLineNumber();

    emit changeTextCursor(cur_line_number, cur_pos);
    // if (text_line == getLinesCounter()){
    //     control_->setReadOnly(false);
    // }else{
    //     control_->setReadOnly(true);
    // }
}

bool ConsoleWidget::canCut() {
    return true;
}

bool ConsoleWidget::canCopy() {
    return  control_->textCursor().hasSelection();
}

bool ConsoleWidget::canPaste() {
    auto cursor = control_->textCursor();
    // return( cursor.hasSelection() && )

    return true;
}

void ConsoleWidget::cut() {
    this->copy();
    if (canCut()){
        control_->textCursor().removeSelectedText();
    }
}

void ConsoleWidget::copy() {
    control_->copy();
}

void ConsoleWidget::paste() {
    ;
}

QMenu *ConsoleWidget::makeContextMenu(const QPoint &_pos) {
    QMenu *menu = new QMenu(this);

    act_cut_ = menu->addAction("Cut");
    act_cut_->setEnabled(canCut());
    act_cut_->setShortcut(QKeySequence::Cut);
    connect(act_cut_, SIGNAL(triggered(bool)), this, SLOT(cut()));

    act_copy_ = menu->addAction("Copy");
    act_copy_->setEnabled(canCopy());
    act_copy_->setShortcut(QKeySequence::Copy);
    connect(act_copy_, SIGNAL(triggered(bool)), this, SLOT(copy()));

    act_paste_ = menu->addAction("Paste");
    act_paste_->setEnabled(canPaste());
    act_paste_->setShortcut(QKeySequence::Paste);
    connect(act_paste_, SIGNAL(triggered(bool)), this, SLOT(paste()));

    menu->addSeparator();
    menu->addAction(act_select_all_);
    menu->addSeparator();
    menu->addAction(act_export_);
    menu->addAction(act_print_);

    return menu;
}

void ConsoleWidget::requestCustomContextMenu(const QPoint &pos) {
    menu_ = makeContextMenu(pos);
    menu_->exec(control_->mapFromGlobal(pos));
}

void ConsoleWidget::copyAvailable(bool) {

}

void ConsoleWidget::redoAvailable(bool) {

}

void ConsoleWidget::undoAvailable(bool) {

}

// Expands the vertical scrollbar beyond the range set by Qt.
void ConsoleWidget::adjustScrollbar(const QSizeF &) {
    QTextDocument *document = control_->document();
    QScrollBar *scroll_bar = control_->verticalScrollBar();
    auto height_viewport = control_->viewport()->height();

    auto maximum = std::max(0, document->lineCount() - 1);
    auto step = height_viewport / control_->fontMetrics().lineSpacing();

    auto diff = maximum - scroll_bar->maximum();
    scroll_bar->setRange(0, round(maximum));
    scroll_bar->setPageStep(step);

    if ((diff < 0) && (document->blockCount() == document->maximumBlockCount())) {
        scroll_bar->setValue(round(scroll_bar->value()) + diff);
    }
}

void ConsoleWidget::flushPendingStream() {

}

QTextCursor ConsoleWidget::getEndCursor() {
    QTextCursor cursor_end=control_->textCursor();
    cursor_end.movePosition(QTextCursor::End);

    return cursor_end;
}

void ConsoleWidget::showPrompt(const QString& _prompt="", bool _is_html, bool _is_newline, bool _is_separator) {
    flushPendingStream();
    QTextCursor cursor_end = getEndCursor();

    bool move_forward = false;
    if (cursor_end.position()!=0){
        move_forward = true;
        if (cursor_end.position()>0){
            append_before_prompt_cursor_.setPosition(cursor_end.position()-1);
        }
    }

    if (_is_newline && cursor_end.position() > 0){
        cursor_end.movePosition(QTextCursor::Left, QTextCursor::KeepAnchor);

        if (cursor_end.selection().toPlainText()!="\n"){
            QTextBlockFormat new_prompt_format{};
            // new_prompt_format.setBackground(QColor("#5888d0"));
            appendBlock(new_prompt_format);
        }
    }

    if (_is_separator){
        appendPlainText(prompt_sep_);
    }

    if(_prompt!=""){
        appendPlainText(_prompt);
        // if (prompt_html_==""){
        //     appendPlainText(_prompt);
        // } else{
        //     appendPlainText(prompt_html_);
        // }
    } else{
        if (_is_html){
            prompt_ = appendHtmlFetchingPlainText(_prompt);
            prompt_html_ = _prompt;
        }else{
            appendPlainText(_prompt);
            prompt_=_prompt;
            prompt_html_ = "";
        }
    }

    flushPendingStream();
    if (getEndPos()>0){
        prompt_cursor_.setPosition(getEndPos()-1);
    }

    if(move_forward){
        append_before_prompt_cursor_.setPosition(append_before_prompt_cursor_.position() + 1);
    }
    startPrompt();
}

QString ConsoleWidget::appendHtmlFetchingPlainText(const QString &) {
    return "";
}

void ConsoleWidget::startPromptHook() {

}

void ConsoleWidget::startPrompt() {
    control_->document()->setMaximumBlockCount(0);
    control_->setUndoRedoEnabled(true);

    control_->setReadOnly(false);
    control_->setAttribute(Qt::WA_InputMethodEnabled, true);

    if (!is_reading_){
        is_executing_ = false;
    }

    startPromptHook();
    if (input_buffer_pending_!=""){
        input_buffer_ = input_buffer_pending_;
        input_buffer_pending_="";
    }

    control_->moveCursor(QTextCursor::End);
}

int ConsoleWidget::getEndPos(){
    return getEndCursor().position();
}

void ConsoleWidget::insertPlainText(QTextCursor _cursor, const QString & text, bool is_flush) {
    auto buffer_size = control_->document()->maximumBlockCount();

    _cursor.beginEditBlock();
    _cursor.insertText(text);
    _cursor.endEditBlock();
}

// append plain text, processing ANSI codes if enabled.
void ConsoleWidget::appendPlainText(const QString& text,  bool is_before_prompt) {
    PF_insertPlainText pf_insert = &ConsoleWidget::insertPlainText;
    appendCustom(pf_insert, text, is_before_prompt);
}

void ConsoleWidget::appendCustom(PF_insertPlainText pf, const QString & input, bool is_before_prompt) {
    QTextCursor cursor = control_->textCursor();
    if (is_before_prompt && ( is_reading_ || (!is_executing_) )){
        flushPendingStream();
        // cursor.insert_mode=true;
        if (append_before_prompt_pos_>=0) {
            cursor.setPosition(append_before_prompt_pos_);
        }
    } else{
        if (pf!=&ConsoleWidget::insertPlainText){
            flushPendingStream();
        }
        cursor.movePosition(QTextCursor::End);
    }

    (this->*pf)(cursor, input, false);
}

void ConsoleWidget::appendCustom(PF_insertBlock pf, QTextBlockFormat input, bool is_before_prompt) {
    QTextCursor cursor = control_->textCursor();
    if (is_before_prompt && ( is_reading_ || (!is_executing_) )){
        flushPendingStream();
        // cursor.insert_mode=true;
        if (append_before_prompt_pos_>=0){
            cursor.setPosition(append_before_prompt_pos_);
        }
    } else{
        flushPendingStream();
        cursor.movePosition(QTextCursor::End);
    }

    (this->*pf)(cursor, input);
}

void ConsoleWidget::insertBlock(QTextCursor _cursor, QTextBlockFormat _format){
    if (_format.isEmpty()){
        _format = QTextBlockFormat();
    }
    _cursor.insertBlock(_format);
}

void ConsoleWidget::appendBlock(QTextBlockFormat _format, bool is_before_prompt) {
    PF_insertBlock pf_insert = &ConsoleWidget::insertBlock;
    appendCustom(pf_insert, _format, is_before_prompt);
}

void ConsoleWidget::clear(bool keep_input = true) {
    if (is_executing_){
        control_->clear();
    } else{
        QString input_buffer{};
        if (keep_input){
            input_buffer= input_buffer_;
        }
        control_->clear();
        showPrompt();
        if (keep_input){
            input_buffer_ = input_buffer;
        }
    }
}

void ConsoleWidget::setContinuationPrompt(const QString & _prompt, bool is_html= false) {
    if (is_html){
        continuation_prompt_html_ = _prompt;
    } else{
        continuation_prompt_ = _prompt;
        continuation_prompt_html_ = "";
    }
}

QTextCursor ConsoleWidget::getCursor() {
    return control_->textCursor();
}

void ConsoleWidget::finishPrompt() {
    control_->setReadOnly(true);
    finishPromptHook();
}

void ConsoleWidget::finishPromptHook() {

}


// set the widget to a non-reading state.
void ConsoleWidget::finalizeInputRequest() {
    // Must set _reading to False before calling _prompt_finished
    is_reading_= false;
    finishPrompt();

    // There is no prompt now, so before_prompt_position is eof
    append_before_prompt_cursor_.setPosition(getEndCursor().position());

    // The maximum block count is only in effect during execution.
    // This ensures that _prompt_pos does not become invalid due to text truncation.
    control_->document()->setMaximumBlockCount(buffer_size_);

    // Setting a positive maximum block count will automatically
    // disable the undo/redo history, but just to be safe:
    control_->setUndoRedoEnabled(false);
}

bool ConsoleWidget::eventFilterPageKeypress(QKeyEvent * key_event) {
    return true;
}

QTextCursor ConsoleWidget::getPromptCursor() {
    auto cursor = control_->textCursor();
    cursor.setPosition(getPromptPos());

    return cursor;
}

int ConsoleWidget::getPromptPos() {
    return (std::min(prompt_cursor_.position()+1, getEndPos()));
}

// return the next position in buffer
int ConsoleWidget::movePositionInBuffer(const int pos) {
    auto cursor = control_->textCursor();
    cursor.setPosition(pos);
    auto line = cursor.blockNumber();
    // qDebug() << "Line-Num: " << line;
    auto prompt_line = getPromptCursor().blockNumber();
    // qDebug() << "Prompt-Num: " << prompt_line;

    if (line == prompt_line){
        return ( pos>getPromptPos()? pos : getPromptPos());
    }

    if (line>prompt_line){
        cursor.movePosition(QTextCursor::StartOfBlock);

        auto prompt_pos = cursor.position() + continuation_prompt_.length();
        return (pos > prompt_pos? pos : prompt_pos);
    }

    return getPromptPos();
}

// return whether the specified position is inside the editing region.
bool ConsoleWidget::inBuffer(const int pos) {
    return (pos == movePositionInBuffer(pos));
}

bool ConsoleWidget::eventFilterConsoleKeypress(QKeyEvent * key_event){
    bool is_intercepted = false;

    auto cursor = control_->textCursor();
    auto position = cursor.position();
    auto key = key_event->key();

    bool is_ctrl_down = bool(controlKeyDown(key_event->modifiers()));
    bool is_alt_down = bool(key_event->modifiers() & Qt::AltModifier);
    bool is_shift_down = bool(key_event->modifiers() & Qt::ShiftModifier);

    if (key == Qt::Key_Return || key==Qt::Key_Enter){
        is_intercepted = true;

        cancelComplete();

        if (inBuffer(position)){
            if (is_reading_){
                appendPlainText("\n");
                is_reading_ = false;
                // if (is_reading_callback){
                //
                // }
            }
            else if (!is_executing_){
                cursor.movePosition(QTextCursor::End, QTextCursor::KeepAnchor);
                auto str_after_strip = cursor.selectedText().simplified();
                bool is_at_end = (str_after_strip.length()==0);
                bool is_single_line = ( getEndCursor().blockNumber()==getPromptCursor().blockNumber());

                if ((is_at_end || is_shift_down || is_single_line) && (!is_ctrl_down)){
                    execute("", false, (!is_shift_down));
                } else{ // Do this inside an edit block for clean undo/redo
                    ;
                }
            }
        }

    } else if (is_ctrl_down){
        if (key==Qt::Key_A){
            selectAllSmart();
        }else{
            ;
        }
    } else if (is_alt_down){
        ;
    } else{
        if (key==Qt::Key_Backspace){ // Backspace
            auto line = cursor.blockNumber();
            auto col = cursor.columnNumber();
            if ((!is_reading_)&&(col==continuation_prompt_.length())&&(line>getPromptCursor().blockNumber())){
                cursor.beginEditBlock();
                cursor.movePosition(QTextCursor::StartOfBlock, QTextCursor::KeepAnchor);
                cursor.removeSelectedText();
                cursor.deletePreviousChar();
                cursor.endEditBlock();
                is_intercepted= true;
            } else{
                auto anchor = cursor.anchor();
                if (anchor==position){
                    is_intercepted= !inBuffer(position-1);
                } else{
                    is_intercepted=!inBuffer(std::min(anchor, position));
                }
            }
        } else if (key==Qt::Key_Delete){ // Delete
            if ((!is_reading_) && (inBuffer(position)) && cursor.atBlockEnd() && (!cursor.hasSelection())){
                cursor.movePosition(QTextCursor::NextBlock, QTextCursor::KeepAnchor);
                cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, continuation_prompt_.length());

                cursor.removeSelectedText();
                is_intercepted = true;
            }else{
                auto anchor = cursor.anchor();
                is_intercepted = ((!inBuffer(anchor)) || (!inBuffer(position)));
            }
        } else if (key==Qt::Key_Escape) {// Esc
            quitKeyboard();
            is_intercepted = true;
        } else if (key==Qt::Key_Up && (!is_shift_down)){
            ;
        } else if (key==Qt::Key_Down && (!is_shift_down)){
            ;
        } else if (key==Qt::Key_Tab){
            if (!is_reading_){
                if(_tab_pressed()){
                    _indent(false);
                }
                is_intercepted=true;
            }
        } else if (key==Qt::Key_Backtab){
            _indent(true);
            is_intercepted=true;
        } else if (key==Qt::Key_Left && (!is_shift_down)){
            ;
        } else if (key==Qt::Key_Right && (!is_shift_down)){
            ;
        } else if (key==Qt::Key_Home){
            ;
        }else{
            auto anchor = cursor.anchor();
            is_intercepted = ((!inBuffer(anchor)) || (!inBuffer(position)));
        }
    }

    if (!is_intercepted){
        if (key_event->matches(QKeySequence::Copy)){
            copy();
            is_intercepted = true;
        } else if (key_event->matches(QKeySequence::Cut)){
            cut();
            is_intercepted = true;
        } else if (key_event->matches(QKeySequence::Paste)){
            paste();
            is_intercepted = true;
        }
    }

    return is_intercepted;
}

bool ConsoleWidget::controlKeyDown(Qt::KeyboardModifiers modifiers){
    return bool(modifiers & Qt::ControlModifier);
}

bool ConsoleWidget::eventFilter(QObject *obj, QEvent *_event) {
    auto type_event = _event->type();

    // keyboard press event
    if (type_event == QEvent::KeyPress){
        auto *key_event = dynamic_cast<QKeyEvent *>(_event);
        auto key = key_event->key();

        bool is_controlDown = controlKeyDown(key_event->modifiers());
        if (obj == control_){
            return eventFilterConsoleKeypress(key_event);
        } else if (obj == page_control_){
            return eventFilterPageKeypress(key_event);
        }
    } else if (type_event == QEvent::Resize && (!filter_resize_)){
        ;
    } else if (type_event == QEvent::MouseMove){
        _event = dynamic_cast<QMouseEvent*>(_event);
        // auto anchor = control_->anchorAt(_event->pos());
        // auto pos = _event->globalPos();
        // QtWidgets::Qt
        ;
    }

    return parent_->eventFilter(obj, _event);
}

void ConsoleWidget::insertContinuationPrompt(QTextCursor cursor, QString indent) {
    insertPlainText(cursor, continuation_prompt_);
    if (indent!=""){
        cursor.insertText(indent);
    }
}

void ConsoleWidget::doExecute(const QString &source, bool is_complete, QString indent) {
    if (is_complete){
        appendPlainText("\n");
        input_buffer_executing_ = input_buffer_;
        is_executing_ = true;

        finalizeInputRequest();
        _execute(source, false);

        showPrompt(">> ");
    } else{
        auto cursor = getEndCursor();
        cursor.beginEditBlock();
        cursor.insertText("\n");
        insertContinuationPrompt(cursor, indent);
        cursor.endEditBlock();
        control_->moveCursor(QTextCursor::End);
    }
}


// The text that the user has entered at the current prompt
QString ConsoleWidget::getInputBuffer(bool is_force) {
    if (is_executing_ && (!is_force)){
        return input_buffer_executing_;
    } else{
        auto cursor = getEndCursor();
        cursor.setPosition(getPromptPos(), QTextCursor::KeepAnchor);
        auto input_buffer = cursor.selection().toPlainText();

        return input_buffer;
    }
}

// If not define source, then source will be input_buffer;
void ConsoleWidget::execute(QString source, bool is_hidden, bool is_interactive) {
    if (source==""){
        input_buffer_ = getInputBuffer();
        source = input_buffer_;
    } else if (!is_hidden){
        input_buffer_=source;
    }

    if (is_hidden){
        _execute(source, is_hidden);
    } else if (is_interactive && execute_on_complete_input_){
        ;
    } else{
        doExecute(source, true, "");
    }
}

// Displays text using the pager if it exceeds the height of the viewport.
void ConsoleWidget::_page(QString text, bool is_html) {
    ;
}

void ConsoleWidget::fillTemporaryBuffer(QTextCursor cursor, QString text, bool is_html) {
    auto cur_pos = control_->textCursor().position();

    cursor.beginEditBlock();
    appendPlainText("\n");
    cursor.endEditBlock();
    _page(text, is_html);
    cursor.setPosition(cur_pos);
    control_->moveCursor(QTextCursor::End);
    control_->setTextCursor(cursor);

    is_temp_buffer_filled= true;
}

// Clears the "temporary text" buffer, i.e. all the text following the prompt region.
void ConsoleWidget::clearTemporaryBuffer() {
    auto cursor = getPromptCursor();

}

void ConsoleWidget::quitKeyboard() {
    if (is_temp_buffer_filled){
        cancelComplete();
        clearTemporaryBuffer();
    } else{
        input_buffer_="";
    }
}

// Indent/Dedent current line or current text selection.
void ConsoleWidget::_indent(bool is_de_indent) {
    ;
}

void ConsoleWidget::copyAnchor(const QTextCursor cursor) {
    ;
}

// Select text in current prompt-cell
void ConsoleWidget::selectAllSmart() {
    auto cursor = getCursor();
    cursor.setPosition(getPromptCursor().position());
    cursor.setPosition(getEndPos(), QTextCursor::KeepAnchor);
    control_->setTextCursor(cursor);
}

void ConsoleWidget::dragEnterEvent(QDragEnterEvent *_event) {
    if (_event->mimeData()->hasUrls()){
        _event->setDropAction(Qt::LinkAction);
        _event->accept();
    } else if (_event->mimeData()->hasText()){
        _event->setDropAction(Qt::CopyAction);
        _event->accept();
    }
}

void ConsoleWidget::dragMoveEvent(QDragEnterEvent *_event) {
    if (_event->mimeData()->hasUrls()){
        ;
    } else if (_event->mimeData()->hasText()){
        auto cursor = control_->cursorForPosition(_event->pos());
        if (inBuffer(cursor.position())){
            _event->setDropAction(Qt::CopyAction);
            control_->setTextCursor(cursor);
        } else{
            _event->setDropAction(Qt::IgnoreAction);
        }
        _event->accept();
    }
}

void ConsoleWidget::dropEvent(QDragEnterEvent *_event) {
    if (_event->mimeData()->hasUrls()){
        ;
    } else if (_event->mimeData()->hasText()){
        auto cursor = control_->cursorForPosition(_event->pos());
        if (inBuffer(cursor.position())){
            auto text = _event->mimeData()->text();
            insertPlainTextIntoBuffer(cursor, text);
        }
    }
}