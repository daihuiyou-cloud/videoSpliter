#include <QApplication>
#include "MainWindow.h"

static const char *QSS = R"(
QWidget { background: #1e2228; color: #d0d4dc; font-size: 15px; }
QFrame#toolbarFrame { background: #2a2f38; border: none; border-bottom: 1px solid #3d4450; }
QLineEdit {
    background: #1a1e24; border: 1px solid #3d4450; border-radius: 3px;
    padding: 4px 6px; color: #d0d4dc;
}
QLineEdit:focus { border-color: #5a8dee; }
QLabel { background: transparent; }
QPushButton {
    background: #3d4450; border: 1px solid #555e6e; border-radius: 3px;
    padding: 5px 14px; color: #d0d4dc;
}
QPushButton:hover { background: #4a5260; }
QPushButton:pressed { background: #2e333d; }
QPushButton:disabled { color: #555e6e; }
QPushButton#startBtn { background: #2d6a4f; border-color: #3a8a65; }
QPushButton#startBtn:hover { background: #38845f; }
QPushButton#cancelBtn { background: #7a2020; border-color: #9a3030; }
QPushButton#cancelBtn:hover { background: #922828; }
QTableView {
    background: #1a1e24; border: none; gridline-color: #2a2f38;
    selection-background-color: #2d4a6e;
}
QHeaderView::section {
    background: #2a2f38; border: none; border-right: 1px solid #3d4450;
    border-bottom: 1px solid #3d4450; padding: 5px 8px; color: #a0a8b8;
}
QScrollBar:vertical { background: #1e2228; width: 8px; }
QScrollBar::handle:vertical { background: #3d4450; border-radius: 4px; }
QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0; }
QWidget#titleBar { background: #161a20; }
QLabel#titleLabel { font-size: 13px; color: #a0a8b8; }
QPushButton#winMinBtn, QPushButton#winMaxBtn { background: transparent; border: none; color: #a0a8b8; font-size: 13px; }
QPushButton#winMinBtn:hover, QPushButton#winMaxBtn:hover { background: #3d4450; border: none; }
QPushButton#winCloseBtn { background: transparent; border: none; color: #a0a8b8; font-size: 13px; }
QPushButton#winCloseBtn:hover { background: #c0392b; border: none; color: #fff; }
)";

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    app.setStyleSheet(QSS);
    MainWindow w;
    w.setWindowFlags(Qt::FramelessWindowHint | Qt::Window);
    w.show();
    return app.exec();
}
