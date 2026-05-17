#include "MainWindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>
#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>
#include <QPushButton>
#include <QTableView>
#include <QHeaderView>
#include <QFileDialog>
#include <QMessageBox>
#include <QMouseEvent>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QDesktopServices>
#include <QUrl>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setWindowTitle("视频分割工具");
    resize(1000, 660);

    m_model  = new SegmentModel(this);
    m_runner = new FFmpegRunner(this);

    connect(m_runner, &FFmpegRunner::durationReady,  this, &MainWindow::onDurationReady);
    connect(m_runner, &FFmpegRunner::segmentStarted, this, &MainWindow::onSegmentStarted);
    connect(m_runner, &FFmpegRunner::segmentDone,    this, &MainWindow::onSegmentDone);
    connect(m_runner, &FFmpegRunner::allDone,        this, &MainWindow::onAllDone);

    // ── title bar ─────────────────────────────────────────────────────────────
    m_titleBar = new QWidget;
    m_titleBar->setObjectName("titleBar");
    m_titleBar->setFixedHeight(42);
    auto *titleLayout = new QHBoxLayout(m_titleBar);
    titleLayout->setContentsMargins(12, 0, 8, 0);
    auto *titleLabel = new QLabel("视频分割工具");
    titleLabel->setObjectName("titleLabel");
    auto *minBtn   = new QPushButton("─");
    auto *maxBtn   = new QPushButton("□");
    auto *closeBtn = new QPushButton("✕");
    minBtn->setObjectName("winMinBtn");
    maxBtn->setObjectName("winMaxBtn");
    closeBtn->setObjectName("winCloseBtn");
    for (auto *b : {minBtn, maxBtn, closeBtn}) b->setFixedSize(46, 34);
    titleLayout->addWidget(titleLabel);
    titleLayout->addStretch();
    titleLayout->addWidget(minBtn);
    titleLayout->addWidget(maxBtn);
    titleLayout->addWidget(closeBtn);
    connect(minBtn,   &QPushButton::clicked, this, &QWidget::showMinimized);
    connect(maxBtn,   &QPushButton::clicked, [this]{ isMaximized() ? showNormal() : showMaximized(); });
    connect(closeBtn, &QPushButton::clicked, this, &QWidget::close);

    // ── toolbar ───────────────────────────────────────────────────────────────
    auto *topFrame = new QFrame;
    topFrame->setObjectName("toolbarFrame");
    auto *topLayout = new QHBoxLayout(topFrame);
    topLayout->setContentsMargins(8, 8, 8, 8);
    m_inputPath = new QLineEdit;
    m_inputPath->setPlaceholderText("选择输入视频文件...");
    m_inputPath->setReadOnly(true);
    auto *browseInputBtn = new QPushButton("浏览");
    topLayout->addWidget(new QLabel("输入:"));
    topLayout->addWidget(m_inputPath, 1);
    topLayout->addWidget(browseInputBtn);
    connect(browseInputBtn, &QPushButton::clicked, this, &MainWindow::browseInput);

    // ── segment duration bar ──────────────────────────────────────────────────
    auto *durFrame = new QFrame;
    durFrame->setObjectName("toolbarFrame");
    auto *durLayout = new QHBoxLayout(durFrame);
    durLayout->setContentsMargins(8, 6, 8, 6);
    m_segDurEdit = new QSpinBox;
    m_segDurEdit->setRange(1, 86400);
    m_segDurEdit->setValue(60);
    m_segDurEdit->setSuffix(" s");
    m_segDurEdit->setFixedWidth(90);
    durLayout->addWidget(new QLabel("片段时长:"));
    durLayout->addWidget(m_segDurEdit);
    durLayout->addStretch();
    connect(m_segDurEdit, QOverload<int>::of(&QSpinBox::valueChanged),
            m_model, &SegmentModel::setSegmentDuration);

    // ── table ─────────────────────────────────────────────────────────────────
    m_table = new QTableView;
    m_table->setModel(m_model);
    m_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    m_table->horizontalHeader()->setSectionResizeMode(4, QHeaderView::ResizeToContents);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setEditTriggers(QAbstractItemView::DoubleClicked);
    m_table->verticalHeader()->hide();

    // ── bottom bar ────────────────────────────────────────────────────────────
    auto *botFrame = new QFrame;
    botFrame->setObjectName("toolbarFrame");
    auto *botLayout = new QHBoxLayout(botFrame);
    botLayout->setContentsMargins(8, 8, 8, 8);
    m_outputPath = new QLineEdit;
    m_outputPath->setPlaceholderText("选择输出文件夹...");
    m_outputPath->setReadOnly(true);
    auto *browseOutBtn = new QPushButton("浏览");
    m_startBtn  = new QPushButton("开始");
    m_cancelBtn = new QPushButton("取消");
    m_statusLabel = new QLabel;
    m_startBtn->setObjectName("startBtn");
    m_cancelBtn->setObjectName("cancelBtn");
    m_cancelBtn->setEnabled(false);
    botLayout->addWidget(new QLabel("输出:"));
    botLayout->addWidget(m_outputPath, 1);
    botLayout->addWidget(browseOutBtn);
    botLayout->addSpacing(16);
    botLayout->addWidget(m_statusLabel);
    botLayout->addWidget(m_startBtn);
    botLayout->addWidget(m_cancelBtn);
    connect(browseOutBtn, &QPushButton::clicked, this, &MainWindow::browseOutput);
    connect(m_startBtn,   &QPushButton::clicked, this, &MainWindow::startSplit);
    connect(m_cancelBtn,  &QPushButton::clicked, this, &MainWindow::cancelSplit);

    // ── assemble ──────────────────────────────────────────────────────────────
    auto *central = new QWidget;
    auto *vbox = new QVBoxLayout(central);
    vbox->setContentsMargins(0, 0, 0, 0);
    vbox->setSpacing(0);
    vbox->addWidget(m_titleBar);
    vbox->addWidget(topFrame);
    vbox->addWidget(durFrame);
    vbox->addWidget(m_table, 1);
    vbox->addWidget(botFrame);
    setCentralWidget(central);
    setAcceptDrops(true);
}

void MainWindow::mouseDoubleClickEvent(QMouseEvent *e) {
    if (m_titleBar->geometry().contains(e->pos()))
        isMaximized() ? showNormal() : showMaximized();
}

void MainWindow::dragEnterEvent(QDragEnterEvent *e) {
    if (e->mimeData()->hasUrls()) e->acceptProposedAction();
}

void MainWindow::dropEvent(QDropEvent *e) {
    const auto urls = e->mimeData()->urls();
    if (urls.isEmpty()) return;
    QString f = urls.first().toLocalFile();
    m_inputPath->setText(f);
    m_model->setDuration("00:00:00");
    m_runner->probe(f);
}

void MainWindow::mousePressEvent(QMouseEvent *e) {
    if (e->button() == Qt::LeftButton && m_titleBar->geometry().contains(e->pos())) {
        m_dragging = true;
        m_dragPos = e->globalPos() - frameGeometry().topLeft();
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *e) {
    if (m_dragging && (e->buttons() & Qt::LeftButton))
        move(e->globalPos() - m_dragPos);
}

void MainWindow::mouseReleaseEvent(QMouseEvent *) {
    m_dragging = false;
}

void MainWindow::browseInput() {
    QString f = QFileDialog::getOpenFileName(this, "打开视频", {},
        "Video Files (*.mp4 *.mkv *.avi *.mov *.wmv *.flv *.ts *.m4v);;All Files (*)");
    if (f.isEmpty()) return;
    m_inputPath->setText(f);
    m_model->setDuration("00:00:00");
    m_runner->probe(f);
}

void MainWindow::browseOutput() {
    QString d = QFileDialog::getExistingDirectory(this, "选择输出文件夹");
    if (!d.isEmpty()) m_outputPath->setText(d);
}

void MainWindow::startSplit() {
    if (m_inputPath->text().isEmpty()) { QMessageBox::warning(this, "", "请选择输入文件。"); return; }
    if (m_outputPath->text().isEmpty()) { QMessageBox::warning(this, "", "请选择输出文件夹。"); return; }
    if (m_model->rowCount() == 0) { QMessageBox::warning(this, "", "没有片段，请先加载视频。"); return; }
    m_model->clearStatuses();
    m_startBtn->setEnabled(false);
    m_cancelBtn->setEnabled(true);
    m_statusLabel->setText("处理中...");
    m_runner->start(m_model->segments(), m_inputPath->text(), m_outputPath->text());
}

void MainWindow::cancelSplit() {
    m_runner->cancel();
    m_startBtn->setEnabled(true);
    m_cancelBtn->setEnabled(false);
    m_statusLabel->setText("已取消");
}

void MainWindow::onDurationReady(const QString &duration) {
    m_model->setDuration(duration);
    m_model->setSegmentDuration(m_segDurEdit->value());
}

void MainWindow::onSegmentStarted(int row) {
    m_model->setStatus(row, "处理中");
    m_table->scrollTo(m_model->index(row, 0));
}

void MainWindow::onSegmentDone(int row, bool ok) {
    m_model->setStatus(row, ok ? "完成" : "错误");
}

void MainWindow::onAllDone() {
    m_startBtn->setEnabled(true);
    m_cancelBtn->setEnabled(false);
    m_statusLabel->setText("完成");
    QDesktopServices::openUrl(QUrl::fromLocalFile(m_outputPath->text()));
}
