#pragma once
#include <QObject>
#include <QProcess>
#include <QVector>

struct Segment {
    QString start, end, name, status;
};

class FFmpegRunner : public QObject {
    Q_OBJECT
public:
    explicit FFmpegRunner(QObject *parent = nullptr);
    void probe(const QString &inputFile);
    void start(const QVector<Segment> &segments, const QString &inputFile, const QString &outputDir);
    void cancel();

signals:
    void durationReady(const QString &duration);
    void segmentStarted(int row);
    void segmentDone(int row, bool ok);
    void allDone();

private slots:
    void onProcessFinished(int exitCode, QProcess::ExitStatus status);

private:
    void runNext();
    QString ffmpegPath() const;

    QProcess *m_process;
    QVector<Segment> m_segments;
    QString m_inputFile, m_outputDir;
    int m_current = -1;
    bool m_probing = false;
};
