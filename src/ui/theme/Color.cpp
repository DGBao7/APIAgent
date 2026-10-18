#include "maf/ui/theme/Color.h"

namespace ui::theme
{
    QString _style_sheet()
    {
        return QString::fromUtf8(R"(
            QMainWindow {
                background-color: #0b100e;
                color: #e6eee9;
                font-family: "Segoe UI", Arial, sans-serif;
            }

            QWidget#topBar {
                background-color: #0b100e;
                border-bottom: 1px solid #1c2a23;
            }

            QLabel#appTitle {
                color: #e6eee9;
                font-size: 19px;
                font-weight: 700;
            }

            QLabel#appSubtitle {
                color: #80968a;
                font-size: 12px;
            }

            QWidget#historyPanel {
                background-color: #101813;
                border-right: 1px solid #1c2a23;
            }

            QLabel#sectionTitle {
                color: #d4e5da;
                font-size: 12px;
                font-weight: 700;
            }

            QLabel#mutedText {
                color: #718579;
                font-size: 12px;
            }

            QListWidget#historyList {
                background-color: transparent;
                border: none;
                outline: none;
                color: #c3d1c8;
                font-size: 13px;
            }

            QListWidget#historyList::item {
                padding: 11px 10px;
                border-radius: 8px;
                margin: 2px 0;
            }

            QListWidget#historyList::item:hover {
                background-color: #18251d;
            }

            QListWidget#historyList::item:selected {
                background-color: #20392b;
                color: #d8f3e2;
            }

            QTextBrowser#conversationView {
                background-color: #0b100e;
                border: none;
                padding: 14px;
                color: #e6eee9;
                font-size: 14px;
            }

            QWidget#inputPanel {
                background-color: #0b100e;
                border-top: 1px solid #1c2a23;
            }

            QLineEdit#inputField {
                background-color: #121d16;
                border: 1px solid #294333;
                border-radius: 17px;
                color: #e6eee9;
                padding: 12px 16px;
                font-size: 14px;
                selection-background-color: #276749;
            }

            QLineEdit#inputField:focus {
                border: 1px solid #34d399;
            }

            QLineEdit#inputField::placeholder {
                color: #718579;
            }

            QScrollBar:vertical {
                background: transparent;
                width: 9px;
                margin: 2px;
            }

            QScrollBar::handle:vertical {
                background: #2a4033;
                min-height: 28px;
                border-radius: 4px;
            }

            QScrollBar::handle:vertical:hover {
                background: #3b5946;
            }

            QScrollBar::add-line:vertical,
            QScrollBar::sub-line:vertical {
                height: 0;
            }
        )");
    }
}
