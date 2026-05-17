#pragma once
#include <QAbstractTableModel>
#include <QVector>
#include "FFmpegRunner.h"

class SegmentModel : public QAbstractTableModel {
    Q_OBJECT
public:
    explicit SegmentModel(QObject *parent = nullptr);

    void setDuration(const QString &duration);
    void setSegmentDuration(int seconds);
    void setStatus(int row, const QString &status);
    void clearStatuses();
    QVector<Segment> segments() const { return m_segments; }

    int rowCount(const QModelIndex & = {}) const override;
    int columnCount(const QModelIndex & = {}) const override { return 5; }
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

private:
    void rebuild();
    static QString secsToHMS(int s);
    static int hmsToSecs(const QString &hms);

    int m_segSecs = 0;
    int m_totalSecs = 0;
    QVector<Segment> m_segments;
};
