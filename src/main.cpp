#include <QApplication>
#include "MainWindow.h"
#include <QAbstractNativeEventFilter>
#include <QPalette>
#include <QSettings>

#ifdef Q_OS_WIN
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif

static const char *DARK_QSS = R"(
QWidget {
    background: #0b1018;
    color: #e9edf7;
    font-family: "Microsoft YaHei UI", "Segoe UI", sans-serif;
    font-size: 15px;
    letter-spacing: 0px;
}
QWidget#root {
    border: 1px solid #253044;
    border-radius: 8px;
}
QWidget#content {
    background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
        stop:0 #0d1420, stop:0.55 #0b111b, stop:1 #0a0f17);
}
QWidget#titleBar {
    background: #090e16;
    border-bottom: 1px solid #182233;
}
QLabel { background: transparent; }
QLabel#appIcon {
    border: 2px solid #6b65ff;
    border-radius: 6px;
    color: #8d86ff;
    font-size: 20px;
    font-weight: 700;
}
QLabel#titleLabel {
    color: #f5f7fb;
    font-size: 17px;
    font-weight: 700;
}
QPushButton#winMinBtn,
QPushButton#winMaxBtn,
QPushButton#winCloseBtn {
    background: transparent;
    border: none;
    color: #d7dce8;
    font-size: 17px;
    border-radius: 6px;
}
QPushButton#winMinBtn:hover,
QPushButton#winMaxBtn:hover {
    background: #1b2433;
}
QPushButton#winCloseBtn:hover {
    background: #dc3c43;
    color: #ffffff;
}
QFrame#stepCard {
    background: rgba(24, 33, 48, 210);
    border: 1px solid #263246;
    border-radius: 8px;
}
QLabel#stepBadge {
    background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
        stop:0 #6880ff, stop:1 #5537d9);
    color: #ffffff;
    border-radius: 15px;
    font-size: 18px;
    font-weight: 800;
}
QLabel#stepTitle {
    color: #f5f7fb;
    font-size: 20px;
    font-weight: 700;
}
QLabel#hintLabel,
QLabel#segmentCountLabel,
QLabel#statusLabel {
    color: #8d96aa;
    font-size: 14px;
}
QFrame#dropFrame {
    background: rgba(16, 24, 37, 120);
    border: 1px dashed #344055;
    border-radius: 8px;
}
QLabel#folderIcon {
    background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
        stop:0 #5266ff, stop:1 #7a43dc);
    border: 1px solid #8e8bff;
    border-radius: 8px;
    color: #dde2ff;
    font-size: 34px;
    font-weight: 800;
}
QLineEdit {
    background: #111927;
    border: 1px solid #2c374b;
    border-radius: 6px;
    padding: 8px 12px;
    color: #f1f4fb;
    selection-background-color: #5f69ff;
}
QLineEdit:focus {
    border-color: #6a73ff;
}
QLineEdit#inputPath {
    background: transparent;
    border: none;
    padding: 0;
    color: #e8ebf5;
    font-size: 16px;
}
QLineEdit#inputPath:focus {
    border: none;
}
QLineEdit#outputPath {
    min-height: 28px;
    padding-left: 16px;
}
QSpinBox#durationSpin {
    background: #111927;
    border: 1px solid #2c374b;
    border-radius: 6px;
    padding: 4px 36px 4px 14px;
    color: #f4f7ff;
    font-size: 17px;
}
QSpinBox#durationSpin::up-button,
QSpinBox#durationSpin::down-button {
    width: 30px;
    background: #172133;
    border-left: 1px solid #2c374b;
}
QSpinBox#durationSpin::up-button {
    border-top-right-radius: 6px;
}
QSpinBox#durationSpin::down-button {
    border-bottom-right-radius: 6px;
}
QSpinBox#durationSpin::up-arrow {
    image: none;
    border-left: 5px solid transparent;
    border-right: 5px solid transparent;
    border-bottom: 6px solid #b4bdcf;
    width: 0;
    height: 0;
}
QSpinBox#durationSpin::down-arrow {
    image: none;
    border-left: 5px solid transparent;
    border-right: 5px solid transparent;
    border-top: 6px solid #b4bdcf;
    width: 0;
    height: 0;
}
QLabel#unitLabel {
    color: #f0f3fb;
    font-size: 17px;
}
QPushButton {
    background: #192234;
    border: 1px solid #2e3a50;
    border-radius: 8px;
    color: #edf1fb;
    padding: 8px 18px;
    font-size: 16px;
    font-weight: 600;
}
QPushButton:hover {
    background: #202b40;
    border-color: #46536c;
}
QPushButton:pressed {
    background: #131b29;
}
QPushButton:disabled {
    color: #657086;
    background: #151d2b;
    border-color: #263246;
}
QPushButton#primaryBrowseBtn,
QPushButton#startBtn {
    background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
        stop:0 #566bff, stop:1 #743ee3);
    border: 1px solid #6d67ff;
    color: #ffffff;
}
QPushButton#primaryBrowseBtn:hover,
QPushButton#startBtn:hover {
    background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
        stop:0 #6377ff, stop:1 #8550ef);
}
QPushButton#secondaryBtn,
QPushButton#cancelBtn {
    background: #192235;
    border: 1px solid #2f3a50;
}
QFrame#actionBar {
    border-top: 1px solid #263246;
    background: transparent;
}
QLabel#settingsLabel {
    color: #d8deeb;
    font-size: 16px;
}
QStackedWidget#segmentsStack {
    background: transparent;
    border: none;
}
QFrame#emptyTableFrame {
    background: #0f1724;
    border: 1px solid #293548;
    border-radius: 8px;
}
QFrame#fakeHeader {
    background: transparent;
    border-bottom: 1px solid #293548;
    min-height: 48px;
}
QLabel#tableHeaderLabel {
    color: #dce2ee;
    font-size: 16px;
    font-weight: 600;
}
QWidget#emptyContent {
    background: transparent;
}
QLabel#emptyIcon {
    background: #151e2e;
    border: 1px solid #2d3850;
    border-radius: 34px;
    color: #8d91ff;
    font-size: 36px;
}
QLabel#emptyTitle {
    color: #f0f3fa;
    font-size: 17px;
    font-weight: 600;
}
QTableView#segmentTable {
    background: #0f1724;
    border: 1px solid #293548;
    border-radius: 8px;
    gridline-color: transparent;
    selection-background-color: #293d70;
    selection-color: #ffffff;
}
QTableView#segmentTable::item {
    padding: 8px 10px;
    border-bottom: 1px solid #1f2b3e;
}
QHeaderView::section {
    background: #0f1724;
    border: none;
    border-bottom: 1px solid #293548;
    padding: 13px 14px;
    color: #dce2ee;
    font-size: 16px;
    font-weight: 600;
}
QScrollBar:vertical {
    background: #0f1724;
    width: 9px;
}
QScrollBar::handle:vertical {
    background: #334056;
    border-radius: 4px;
}
QScrollBar::add-line:vertical,
QScrollBar::sub-line:vertical {
    height: 0;
}
)";

static const char *LIGHT_QSS = R"(
QWidget {
    background: #f3f6fb;
    color: #182131;
    font-family: "Microsoft YaHei UI", "Segoe UI", sans-serif;
    font-size: 15px;
    letter-spacing: 0px;
}
QWidget#root {
    border: 1px solid #d4dceb;
    border-radius: 8px;
}
QWidget#content {
    background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
        stop:0 #f7f9fd, stop:0.55 #eef3fa, stop:1 #eaf0f8);
}
QWidget#titleBar {
    background: #ffffff;
    border-bottom: 1px solid #dce4f1;
}
QLabel { background: transparent; }
QLabel#appIcon {
    border: 2px solid #605bff;
    border-radius: 6px;
    color: #5e58f2;
    font-size: 20px;
    font-weight: 700;
}
QLabel#titleLabel {
    color: #121826;
    font-size: 17px;
    font-weight: 700;
}
QPushButton#winMinBtn,
QPushButton#winMaxBtn,
QPushButton#winCloseBtn {
    background: transparent;
    border: none;
    color: #405069;
    font-size: 17px;
    border-radius: 6px;
}
QPushButton#winMinBtn:hover,
QPushButton#winMaxBtn:hover {
    background: #e8edf6;
}
QPushButton#winCloseBtn:hover {
    background: #dc3c43;
    color: #ffffff;
}
QFrame#stepCard {
    background: rgba(255, 255, 255, 232);
    border: 1px solid #d7dfed;
    border-radius: 8px;
}
QLabel#stepBadge {
    background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
        stop:0 #667cff, stop:1 #6045e8);
    color: #ffffff;
    border-radius: 15px;
    font-size: 18px;
    font-weight: 800;
}
QLabel#stepTitle {
    color: #121826;
    font-size: 20px;
    font-weight: 700;
}
QLabel#hintLabel,
QLabel#segmentCountLabel,
QLabel#statusLabel {
    color: #697589;
    font-size: 14px;
}
QFrame#dropFrame {
    background: rgba(247, 250, 255, 210);
    border: 1px dashed #b9c5d8;
    border-radius: 8px;
}
QLabel#folderIcon {
    background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
        stop:0 #6578ff, stop:1 #7947e6);
    border: 1px solid #928dff;
    border-radius: 8px;
    color: #ffffff;
    font-size: 34px;
    font-weight: 800;
}
QLineEdit {
    background: #ffffff;
    border: 1px solid #cdd7e7;
    border-radius: 6px;
    padding: 8px 12px;
    color: #172033;
    selection-background-color: #6578ff;
}
QLineEdit:focus {
    border-color: #6372f2;
}
QLineEdit#inputPath {
    background: transparent;
    border: none;
    padding: 0;
    color: #1a2435;
    font-size: 16px;
}
QLineEdit#inputPath:focus {
    border: none;
}
QLineEdit#outputPath {
    min-height: 28px;
    padding-left: 16px;
}
QSpinBox#durationSpin {
    background: #ffffff;
    border: 1px solid #cdd7e7;
    border-radius: 6px;
    padding: 4px 36px 4px 14px;
    color: #172033;
    font-size: 17px;
}
QSpinBox#durationSpin::up-button,
QSpinBox#durationSpin::down-button {
    width: 30px;
    background: #eef3fa;
    border-left: 1px solid #cdd7e7;
}
QSpinBox#durationSpin::up-button {
    border-top-right-radius: 6px;
}
QSpinBox#durationSpin::down-button {
    border-bottom-right-radius: 6px;
}
QSpinBox#durationSpin::up-arrow {
    image: none;
    border-left: 5px solid transparent;
    border-right: 5px solid transparent;
    border-bottom: 6px solid #647089;
    width: 0;
    height: 0;
}
QSpinBox#durationSpin::down-arrow {
    image: none;
    border-left: 5px solid transparent;
    border-right: 5px solid transparent;
    border-top: 6px solid #647089;
    width: 0;
    height: 0;
}
QLabel#unitLabel {
    color: #172033;
    font-size: 17px;
}
QPushButton {
    background: #ffffff;
    border: 1px solid #cbd5e6;
    border-radius: 8px;
    color: #1c2638;
    padding: 8px 18px;
    font-size: 16px;
    font-weight: 600;
}
QPushButton:hover {
    background: #f1f5fb;
    border-color: #aebbd0;
}
QPushButton:pressed {
    background: #e7edf6;
}
QPushButton:disabled {
    color: #97a1b3;
    background: #eef2f8;
    border-color: #d8e0ec;
}
QPushButton#primaryBrowseBtn,
QPushButton#startBtn {
    background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
        stop:0 #586eff, stop:1 #7440e5);
    border: 1px solid #6a65f8;
    color: #ffffff;
}
QPushButton#primaryBrowseBtn:hover,
QPushButton#startBtn:hover {
    background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
        stop:0 #667aff, stop:1 #8350ef);
}
QPushButton#secondaryBtn,
QPushButton#cancelBtn {
    background: #ffffff;
    border: 1px solid #cbd5e6;
}
QFrame#actionBar {
    border-top: 1px solid #d7dfed;
    background: transparent;
}
QLabel#settingsLabel {
    color: #344258;
    font-size: 16px;
}
QStackedWidget#segmentsStack {
    background: transparent;
    border: none;
}
QFrame#emptyTableFrame {
    background: #ffffff;
    border: 1px solid #d7dfed;
    border-radius: 8px;
}
QFrame#fakeHeader {
    background: transparent;
    border-bottom: 1px solid #d7dfed;
    min-height: 48px;
}
QLabel#tableHeaderLabel {
    color: #263248;
    font-size: 16px;
    font-weight: 600;
}
QWidget#emptyContent {
    background: transparent;
}
QLabel#emptyIcon {
    background: #f1f4fb;
    border: 1px solid #d3dced;
    border-radius: 34px;
    color: #6569e8;
    font-size: 36px;
}
QLabel#emptyTitle {
    color: #172033;
    font-size: 17px;
    font-weight: 600;
}
QTableView#segmentTable {
    background: #ffffff;
    border: 1px solid #d7dfed;
    border-radius: 8px;
    gridline-color: transparent;
    selection-background-color: #dfe7ff;
    selection-color: #152033;
}
QTableView#segmentTable::item {
    padding: 8px 10px;
    border-bottom: 1px solid #edf1f7;
}
QHeaderView::section {
    background: #ffffff;
    border: none;
    border-bottom: 1px solid #d7dfed;
    padding: 13px 14px;
    color: #263248;
    font-size: 16px;
    font-weight: 600;
}
QScrollBar:vertical {
    background: #ffffff;
    width: 9px;
}
QScrollBar::handle:vertical {
    background: #c0cadb;
    border-radius: 4px;
}
QScrollBar::add-line:vertical,
QScrollBar::sub-line:vertical {
    height: 0;
}
)";

static bool systemUsesDarkTheme() {
#ifdef Q_OS_WIN
    QSettings settings(
        "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
        QSettings::NativeFormat);
    return settings.value("AppsUseLightTheme", 1).toInt() == 0;
#else
    return qApp->palette().color(QPalette::Window).lightness() < 128;
#endif
}

static void applySystemTheme(QApplication &app) {
    app.setStyleSheet(systemUsesDarkTheme() ? DARK_QSS : LIGHT_QSS);
}

class ThemeEventFilter : public QAbstractNativeEventFilter {
public:
    explicit ThemeEventFilter(QApplication &app)
        : m_app(app), m_dark(systemUsesDarkTheme()) {}

    bool nativeEventFilter(const QByteArray &eventType, void *message, long *result) override {
        Q_UNUSED(result);
#ifdef Q_OS_WIN
        if (eventType == "windows_generic_MSG" || eventType == "windows_dispatcher_MSG") {
            const MSG *msg = static_cast<MSG *>(message);
            if (msg->message == WM_SETTINGCHANGE || msg->message == WM_THEMECHANGED) {
                const bool dark = systemUsesDarkTheme();
                if (dark != m_dark) {
                    m_dark = dark;
                    m_app.setStyleSheet(dark ? DARK_QSS : LIGHT_QSS);
                }
            }
        }
#else
        Q_UNUSED(eventType);
        Q_UNUSED(message);
#endif
        return false;
    }

private:
    QApplication &m_app;
    bool m_dark;
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    applySystemTheme(app);
    ThemeEventFilter themeFilter(app);
    app.installNativeEventFilter(&themeFilter);

    MainWindow w;
    w.setWindowFlags(Qt::FramelessWindowHint | Qt::Window);
    w.show();
    return app.exec();
}
