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
#include <QStackedWidget>
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
    resize(1180, 810);
    setMinimumSize(980, 680);

    m_model  = new SegmentModel(this);
    m_runner = new FFmpegRunner(this);

    connect(m_runner, &FFmpegRunner::durationReady,  this, &MainWindow::onDurationReady);
    connect(m_runner, &FFmpegRunner::segmentStarted, this, &MainWindow::onSegmentStarted);
    connect(m_runner, &FFmpegRunner::segmentDone,    this, &MainWindow::onSegmentDone);
    connect(m_runner, &FFmpegRunner::allDone,        this, &MainWindow::onAllDone);
    connect(m_model, &QAbstractItemModel::modelReset, this, [this]{ updateSegmentSummary(); });

    m_titleBar = new QWidget;
    m_titleBar->setObjectName("titleBar");
    m_titleBar->setFixedHeight(58);
    auto *titleLayout = new QHBoxLayout(m_titleBar);
    titleLayout->setContentsMargins(24, 0, 18, 0);
    titleLayout->setSpacing(12);
    auto *appIcon = new QLabel("◇");
    appIcon->setObjectName("appIcon");
    appIcon->setAlignment(Qt::AlignCenter);
    appIcon->setFixedSize(28, 28);
    auto *titleLabel = new QLabel("视频分割工具");
    titleLabel->setObjectName("titleLabel");
    auto *minBtn   = new QPushButton("─");
    auto *maxBtn   = new QPushButton("□");
    auto *closeBtn = new QPushButton("✕");
    minBtn->setObjectName("winMinBtn");
    maxBtn->setObjectName("winMaxBtn");
    closeBtn->setObjectName("winCloseBtn");
    for (auto *b : {minBtn, maxBtn, closeBtn}) b->setFixedSize(46, 34);
    titleLayout->addWidget(appIcon);
    titleLayout->addWidget(titleLabel);
    titleLayout->addStretch();
    titleLayout->addWidget(minBtn);
    titleLayout->addWidget(maxBtn);
    titleLayout->addWidget(closeBtn);
    connect(minBtn,   &QPushButton::clicked, this, &QWidget::showMinimized);
    connect(maxBtn,   &QPushButton::clicked, [this]{ isMaximized() ? showNormal() : showMaximized(); });
    connect(closeBtn, &QPushButton::clicked, this, &QWidget::close);

    auto makeStepBadge = [](const QString &number) {
        auto *badge = new QLabel(number);
        badge->setObjectName("stepBadge");
        badge->setAlignment(Qt::AlignCenter);
        badge->setFixedSize(30, 30);
        return badge;
    };
    auto makeStepTitle = [](const QString &text) {
        auto *title = new QLabel(text);
        title->setObjectName("stepTitle");
        return title;
    };
    auto makeSection = [&](const QString &number, const QString &title, QVBoxLayout **body) {
        auto *frame = new QFrame;
        frame->setObjectName("stepCard");
        auto *layout = new QVBoxLayout(frame);
        layout->setContentsMargins(20, 18, 20, 12);
        layout->setSpacing(12);
        auto *head = new QHBoxLayout;
        head->setContentsMargins(0, 0, 0, 0);
        head->setSpacing(14);
        head->addWidget(makeStepBadge(number));
        head->addWidget(makeStepTitle(title));
        head->addStretch();
        layout->addLayout(head);
        *body = new QVBoxLayout;
        (*body)->setContentsMargins(44, 0, 0, 0);
        (*body)->setSpacing(10);
        layout->addLayout(*body);
        return frame;
    };

    QVBoxLayout *inputBody = nullptr;
    auto *inputFrame = makeSection("1", "选择输入文件", &inputBody);
    auto *dropFrame = new QFrame;
    dropFrame->setObjectName("dropFrame");
    dropFrame->setMinimumHeight(92);
    auto *dropLayout = new QHBoxLayout(dropFrame);
    dropLayout->setContentsMargins(20, 14, 18, 14);
    dropLayout->setSpacing(18);
    auto *folderIcon = new QLabel("▱");
    folderIcon->setObjectName("folderIcon");
    folderIcon->setAlignment(Qt::AlignCenter);
    folderIcon->setFixedSize(64, 54);
    auto *inputTextLayout = new QVBoxLayout;
    inputTextLayout->setSpacing(8);
    m_inputPath = new QLineEdit;
    m_inputPath->setObjectName("inputPath");
    m_inputPath->setPlaceholderText("点击选择或拖拽视频文件到此处");
    m_inputPath->setReadOnly(true);
    auto *supportLabel = new QLabel("支持常见的视频格式（MP4、MOV、AVI、MKV 等）");
    supportLabel->setObjectName("hintLabel");
    inputTextLayout->addWidget(m_inputPath);
    inputTextLayout->addWidget(supportLabel);
    auto *browseInputBtn = new QPushButton("浏览文件");
    browseInputBtn->setObjectName("primaryBrowseBtn");
    browseInputBtn->setFixedSize(126, 48);
    dropLayout->addWidget(folderIcon);
    dropLayout->addLayout(inputTextLayout, 1);
    dropLayout->addWidget(browseInputBtn);
    inputBody->addWidget(dropFrame);
    connect(browseInputBtn, &QPushButton::clicked, this, &MainWindow::browseInput);

    QVBoxLayout *durationBody = nullptr;
    auto *durFrame = makeSection("2", "片段时长", &durationBody);
    auto *durLayout = new QHBoxLayout;
    durLayout->setContentsMargins(0, 0, 0, 0);
    durLayout->setSpacing(16);
    m_segDurEdit = new QSpinBox;
    m_segDurEdit->setObjectName("durationSpin");
    m_segDurEdit->setRange(1, 86400);
    m_segDurEdit->setValue(60);
    m_segDurEdit->setPrefix("每个片段时长   ");
    m_segDurEdit->setFixedSize(250, 40);
    durLayout->addWidget(m_segDurEdit);
    auto *secLabel = new QLabel("秒 (s)");
    secLabel->setObjectName("unitLabel");
    durLayout->addWidget(secLabel);
    durLayout->addStretch();
    auto *recommendLabel = new QLabel("ⓘ  建议值：10 - 1800 秒");
    recommendLabel->setObjectName("hintLabel");
    durLayout->addWidget(recommendLabel);
    durationBody->addLayout(durLayout);
    auto *durationHint = new QLabel("设置每个输出片段的时长");
    durationHint->setObjectName("hintLabel");
    durationBody->addWidget(durationHint);
    connect(m_segDurEdit, QOverload<int>::of(&QSpinBox::valueChanged),
            m_model, &SegmentModel::setSegmentDuration);

    QVBoxLayout *segmentsBody = nullptr;
    auto *segmentsFrame = makeSection("3", "分割片段列表", &segmentsBody);
    auto *segmentsHead = static_cast<QHBoxLayout*>(segmentsFrame->layout()->itemAt(0)->layout());
    m_segmentCountLabel = new QLabel("共 0 个片段");
    m_segmentCountLabel->setObjectName("segmentCountLabel");
    segmentsHead->addWidget(m_segmentCountLabel);

    m_table = new QTableView;
    m_table->setObjectName("segmentTable");
    m_table->setModel(m_model);
    m_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    m_table->horizontalHeader()->setSectionResizeMode(5, QHeaderView::ResizeToContents);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setEditTriggers(QAbstractItemView::DoubleClicked);
    m_table->setAlternatingRowColors(false);
    m_table->setShowGrid(false);
    m_table->verticalHeader()->hide();
    m_table->setMinimumHeight(200);

    auto *emptyFrame = new QFrame;
    emptyFrame->setObjectName("emptyTableFrame");
    auto *emptyLayout = new QVBoxLayout(emptyFrame);
    emptyLayout->setContentsMargins(0, 0, 0, 0);
    emptyLayout->setSpacing(0);
    auto *fakeHeader = new QFrame;
    fakeHeader->setObjectName("fakeHeader");
    auto *fakeHeaderLayout = new QHBoxLayout(fakeHeader);
    fakeHeaderLayout->setContentsMargins(28, 0, 28, 0);
    fakeHeaderLayout->setSpacing(0);
    for (const auto &text : {"#", "开始时间", "结束时间", "时长", "名称", "状态"}) {
        auto *label = new QLabel(text);
        label->setObjectName("tableHeaderLabel");
        fakeHeaderLayout->addWidget(label, text == QString("#") ? 1 : 3);
    }
    auto *emptyContent = new QWidget;
    emptyContent->setObjectName("emptyContent");
    auto *emptyContentLayout = new QVBoxLayout(emptyContent);
    emptyContentLayout->setAlignment(Qt::AlignCenter);
    emptyContentLayout->setSpacing(10);
    auto *emptyIcon = new QLabel("▤");
    emptyIcon->setObjectName("emptyIcon");
    emptyIcon->setAlignment(Qt::AlignCenter);
    emptyIcon->setFixedSize(68, 68);
    auto *emptyTitle = new QLabel("暂无分割任务");
    emptyTitle->setObjectName("emptyTitle");
    emptyTitle->setAlignment(Qt::AlignCenter);
    auto *emptyHint = new QLabel("设置片段时长后，点击“开始”生成分割任务");
    emptyHint->setObjectName("hintLabel");
    emptyHint->setAlignment(Qt::AlignCenter);
    emptyContentLayout->addWidget(emptyIcon, 0, Qt::AlignHCenter);
    emptyContentLayout->addWidget(emptyTitle);
    emptyContentLayout->addWidget(emptyHint);
    emptyLayout->addWidget(fakeHeader);
    emptyLayout->addWidget(emptyContent, 1);

    m_segmentsStack = new QStackedWidget;
    m_segmentsStack->setObjectName("segmentsStack");
    m_segmentsStack->addWidget(emptyFrame);
    m_segmentsStack->addWidget(m_table);
    m_segmentsStack->setMinimumHeight(210);
    segmentsBody->addWidget(m_segmentsStack);

    QVBoxLayout *outputBody = nullptr;
    auto *outputFrame = makeSection("4", "输出文件夹", &outputBody);
    auto *outputRow = new QHBoxLayout;
    outputRow->setContentsMargins(0, 0, 0, 0);
    outputRow->setSpacing(10);
    m_outputPath = new QLineEdit;
    m_outputPath->setObjectName("outputPath");
    m_outputPath->setPlaceholderText("选择输出文件夹...");
    m_outputPath->setReadOnly(true);
    auto *browseOutBtn = new QPushButton("浏览");
    browseOutBtn->setObjectName("secondaryBtn");
    browseOutBtn->setFixedSize(132, 38);
    outputRow->addWidget(m_outputPath, 1);
    outputRow->addWidget(browseOutBtn);
    outputBody->addLayout(outputRow);

    auto *actionBar = new QFrame;
    actionBar->setObjectName("actionBar");
    auto *actionLayout = new QHBoxLayout(actionBar);
    actionLayout->setContentsMargins(0, 12, 0, 0);
    actionLayout->setSpacing(12);
    auto *settingsLabel = new QLabel("⚙  设置");
    settingsLabel->setObjectName("settingsLabel");
    m_startBtn  = new QPushButton("开始");
    m_cancelBtn = new QPushButton("取消");
    m_statusLabel = new QLabel;
    m_statusLabel->setObjectName("statusLabel");
    m_startBtn->setObjectName("startBtn");
    m_cancelBtn->setObjectName("cancelBtn");
    m_startBtn->setText("▶  开始");
    m_startBtn->setFixedSize(154, 46);
    m_cancelBtn->setFixedSize(132, 46);
    m_cancelBtn->setEnabled(false);
    actionLayout->addWidget(settingsLabel);
    actionLayout->addWidget(m_statusLabel);
    actionLayout->addStretch();
    actionLayout->addWidget(m_cancelBtn);
    actionLayout->addWidget(m_startBtn);
    outputBody->addWidget(actionBar);
    connect(browseOutBtn, &QPushButton::clicked, this, &MainWindow::browseOutput);
    connect(m_startBtn,   &QPushButton::clicked, this, &MainWindow::startSplit);
    connect(m_cancelBtn,  &QPushButton::clicked, this, &MainWindow::cancelSplit);

    auto *central = new QWidget;
    central->setObjectName("root");
    auto *vbox = new QVBoxLayout(central);
    vbox->setContentsMargins(0, 0, 0, 0);
    vbox->setSpacing(0);
    vbox->addWidget(m_titleBar);
    auto *content = new QWidget;
    content->setObjectName("content");
    auto *contentLayout = new QVBoxLayout(content);
    contentLayout->setContentsMargins(16, 8, 16, 12);
    contentLayout->setSpacing(12);
    contentLayout->addWidget(inputFrame);
    contentLayout->addWidget(durFrame);
    contentLayout->addWidget(segmentsFrame, 1);
    contentLayout->addWidget(outputFrame);
    vbox->addWidget(content, 1);
    setCentralWidget(central);
    setAcceptDrops(true);
    updateSegmentSummary();
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

void MainWindow::updateSegmentSummary() {
    const int count = m_model->rowCount();
    m_segmentCountLabel->setText(QString("共 %1 个片段").arg(count));
    m_segmentsStack->setCurrentIndex(count > 0 ? 1 : 0);
}
