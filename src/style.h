#pragma once
#include <QString>

inline QString appStyleSheet() {
    return R"(
/* ════════════════════════════════════════════════════
   PRAVOSLAVNI KALENDAR — Dark Glass Theme
   ════════════════════════════════════════════════════ */

QMainWindow, QDialog {
    background-color: #0a0a1e;
    color: #e0e0f0;
    font-family: "Segoe UI", "Arial Unicode MS", Arial, sans-serif;
    font-size: 13px;
}

QWidget {
    color: #e0e0f0;
    font-family: "Segoe UI", "Arial Unicode MS", Arial, sans-serif;
}

/* ── Scrollbars ── */
QScrollBar:vertical {
    background: rgba(255,255,255,0.03);
    width: 8px;
    border-radius: 4px;
}
QScrollBar::handle:vertical {
    background: rgba(255,255,255,0.15);
    border-radius: 4px;
    min-height: 24px;
}
QScrollBar::handle:vertical:hover { background: rgba(255,255,255,0.28); }
QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0; }

QScrollBar:horizontal {
    background: rgba(255,255,255,0.03);
    height: 8px;
    border-radius: 4px;
}
QScrollBar::handle:horizontal {
    background: rgba(255,255,255,0.15);
    border-radius: 4px;
}
QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal { width: 0; }

/* ── Tooltip ── */
QToolTip {
    background: #1a1a3e;
    color: #e0e0f0;
    border: 1px solid rgba(255,255,255,0.12);
    border-radius: 6px;
    padding: 4px 8px;
    font-size: 12px;
}

/* ── Labels ── */
QLabel { background: transparent; }

/* ── Line Edit ── */
QLineEdit, QTextEdit, QPlainTextEdit {
    background: rgba(255,255,255,0.06);
    border: 1px solid rgba(255,255,255,0.12);
    border-radius: 8px;
    padding: 6px 10px;
    color: #e0e0f0;
    selection-background-color: #533483;
}
QLineEdit:focus, QTextEdit:focus, QPlainTextEdit:focus {
    border: 1px solid #533483;
    background: rgba(83,52,131,0.15);
}

/* ── ComboBox ── */
QComboBox {
    background: rgba(255,255,255,0.07);
    border: 1px solid rgba(255,255,255,0.12);
    border-radius: 8px;
    padding: 5px 10px;
    color: #e0e0f0;
    min-width: 80px;
}
QComboBox:hover { border-color: #533483; }
QComboBox::drop-down {
    subcontrol-origin: padding;
    subcontrol-position: right center;
    width: 22px;
    border: none;
}
QComboBox QAbstractItemView {
    background: #12122e;
    border: 1px solid rgba(255,255,255,0.12);
    border-radius: 8px;
    selection-background-color: #533483;
    color: #e0e0f0;
    padding: 4px;
}

/* ── CheckBox ── */
QCheckBox { spacing: 8px; }
QCheckBox::indicator {
    width: 16px; height: 16px;
    border: 1px solid rgba(255,255,255,0.2);
    border-radius: 4px;
    background: rgba(255,255,255,0.05);
}
QCheckBox::indicator:checked {
    background: #533483;
    border-color: #7b5ea7;
    image: none;
}

/* ── SpinBox ── */
QSpinBox, QTimeEdit, QDateEdit {
    background: rgba(255,255,255,0.07);
    border: 1px solid rgba(255,255,255,0.12);
    border-radius: 8px;
    padding: 5px 10px;
    color: #e0e0f0;
}
QSpinBox:focus, QTimeEdit:focus, QDateEdit:focus { border-color: #533483; }
QSpinBox::up-button, QSpinBox::down-button,
QTimeEdit::up-button, QTimeEdit::down-button {
    background: rgba(255,255,255,0.08);
    border: none;
    width: 18px;
    border-radius: 4px;
}

/* ── GroupBox ── */
QGroupBox {
    border: 1px solid rgba(255,255,255,0.1);
    border-radius: 10px;
    margin-top: 16px;
    padding: 12px 8px 8px 8px;
    font-weight: 600;
    color: #aaaacc;
    font-size: 12px;
}
QGroupBox::title {
    subcontrol-origin: margin;
    subcontrol-position: top left;
    padding: 0 8px;
    color: #aaaacc;
}

/* ── Message Box ── */
QMessageBox {
    background: #12122e;
    color: #e0e0f0;
}
QMessageBox QPushButton {
    background: rgba(83,52,131,0.5);
    border: 1px solid #533483;
    border-radius: 8px;
    color: #e0e0f0;
    padding: 6px 18px;
    min-width: 70px;
}
QMessageBox QPushButton:hover { background: rgba(83,52,131,0.8); }

/* ── Splitter ── */
QSplitter::handle { background: rgba(255,255,255,0.06); }
QSplitter::handle:horizontal { width: 1px; }
QSplitter::handle:vertical   { height: 1px; }
)";
}
