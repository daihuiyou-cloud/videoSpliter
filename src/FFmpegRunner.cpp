#include "FFmpegRunner.h"
#include <QCoreApplication>
#include <QRegularExpression>

FFmpegRunner::FFmpegRunner(QObject *parent) : QObject(parent), m_process(new QProcess(this)) {
    connect(m_process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &FFmpegRunner::onProcessFinished);
}

QString FFmpegRunner::ffmpegPath() const {
    return QCoreApplication::applicationDirPath() + "/ffmpeg.exe";
}

void FFmpegRunner::probe(const QString &inputFile) {
    m_probing = true;
    m_process->setProgram(ffmpegPath());
    m_process->setArguments({"-i", inputFile});
    m_process->start();
}

void FFmpegRunner::start(const QVector<Segment> &segments, const QString &inputFile, const QString &outputDir) {
    m_segments = segments;
    m_inputFile = inputFile;
    m_outputDir = outputDir;
    m_current = -1;
    runNext();
}

void FFmpegRunner::cancel() {
    m_current = m_segments.size(); // stop queue
    if (m_process->state() != QProcess::NotRunning)
        m_process->kill();
}

void FFmpegRunner::runNext() {
    ++m_current;
    if (m_current >= m_segments.size()) {
        emit allDone();
        return;
    }
    const Segment &seg = m_segments[m_current];
    QStringList args;
    args << "-y" << "-ss" << seg.start << "-to" << seg.end
         << "-i" << m_inputFile << "-c" << "copy"
         << "-avoid_negative_ts" << "make_zero"
         << m_outputDir + "/" + seg.name + ".mp4";
    m_process->setProgram(ffmpegPath());
    m_process->setArguments(args);
    emit segmentStarted(m_current);
    m_process->start();
}

void FFmpegRunner::onProcessFinished(int exitCode, QProcess::ExitStatus) {
    if (m_probing) {
        m_probing = false;
        QString output = m_process->readAllStandardError();
        QRegularExpression re(R"(Duration:\s*(\d{2}:\d{2}:\d{2}\.\d+))");
        auto m = re.match(output);
        if (m.hasMatch())
            emit durationReady(m.captured(1).left(8)); // HH:MM:SS
        return;
    }
    emit segmentDone(m_current, exitCode == 0);
    if (m_current < m_segments.size() - 1)
        runNext();
    else
        emit allDone();
}
