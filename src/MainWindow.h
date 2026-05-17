#pragma once
#include <QMainWindow>
#include <QPoint>
#include "SegmentModel.h"
#include "FFmpegRunner.h"

class QLineEdit;
class QTableView;
class QPushButton;
class QLabel;
class QSpinBox;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

protected:
    void mousePressEvent(QMouseEvent *e) override;
    void mouseMoveEvent(QMouseEvent *e) override;
    void mouseReleaseEvent(QMouseEvent *e) override;
    void mouseDoubleClickEvent(QMouseEvent *e) override;
    void dragEnterEvent(QDragEnterEvent *e) override;
    void dropEvent(QDropEvent *e) override;

private slots:
    void browseInput();
    void browseOutput();
    void startSplit();
    void cancelSplit();
    void onDurationReady(const QString &duration);
    void onSegmentStarted(int row);
    void onSegmentDone(int row, bool ok);
    void onAllDone();

private:
    QLineEdit *m_inputPath;
    QSpinBox  *m_segDurEdit;
    QTableView *m_table;
    QLineEdit *m_outputPath;
    QPushButton *m_startBtn;
    QPushButton *m_cancelBtn;
    QLabel *m_statusLabel;
    QWidget *m_titleBar;

    SegmentModel *m_model;
    FFmpegRunner *m_runner;

    bool   m_dragging = false;
    QPoint m_dragPos;
};
