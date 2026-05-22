#include "SegmentModel.h"

SegmentModel::SegmentModel(QObject *parent) : QAbstractTableModel(parent) {}

QString SegmentModel::secsToHMS(int s) {
    return QString("%1:%2:%3")
        .arg(s / 3600, 2, 10, QChar('0'))
        .arg((s % 3600) / 60, 2, 10, QChar('0'))
        .arg(s % 60, 2, 10, QChar('0'));
}

int SegmentModel::hmsToSecs(const QString &hms) {
    auto p = hms.split(':');
    if (p.size() == 3) return p[0].toInt()*3600 + p[1].toInt()*60 + p[2].toInt();
    if (p.size() == 2) return p[0].toInt()*60 + p[1].toInt();
    return hms.toInt();
}

void SegmentModel::setDuration(const QString &duration) {
    m_totalSecs = hmsToSecs(duration);
    rebuild();
}

void SegmentModel::setSegmentDuration(int seconds) {
    m_segSecs = seconds;
    rebuild();
}

void SegmentModel::setStatus(int row, const QString &status) {
    if (row < 0 || row >= m_segments.size()) return;
    m_segments[row].status = status;
    auto idx = index(row, 5);
    emit dataChanged(idx, idx);
}

void SegmentModel::clearStatuses() {
    for (auto &s : m_segments) s.status = "-";
    if (!m_segments.isEmpty())
        emit dataChanged(index(0, 5), index(m_segments.size()-1, 5));
}

void SegmentModel::rebuild() {
    beginResetModel();
    m_segments.clear();
    if (m_segSecs > 0 && m_totalSecs > 0) {
        int start = 0, i = 1;
        while (start < m_totalSecs) {
            int end = qMin(start + m_segSecs, m_totalSecs);
            Segment seg;
            seg.start  = secsToHMS(start);
            seg.end    = secsToHMS(end);
            seg.name   = QString("part_%1").arg(i++);
            seg.status = "-";
            m_segments.append(seg);
            start = end;
        }
    }
    endResetModel();
}

int SegmentModel::rowCount(const QModelIndex &) const { return m_segments.size(); }

QVariant SegmentModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || (role != Qt::DisplayRole && role != Qt::EditRole)) return {};
    const auto &seg = m_segments[index.row()];
    switch (index.column()) {
        case 0: return index.row() + 1;
        case 1: return seg.start;
        case 2: return seg.end;
        case 3: return secsToHMS(hmsToSecs(seg.end) - hmsToSecs(seg.start));
        case 4: return seg.name;
        case 5: return seg.status;
    }
    return {};
}

QVariant SegmentModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role != Qt::DisplayRole || orientation != Qt::Horizontal) return {};
    static const char *headers[] = {"#", "开始时间", "结束时间", "时长", "名称", "状态"};
    return headers[section];
}

bool SegmentModel::setData(const QModelIndex &index, const QVariant &value, int role) {
    if (role != Qt::EditRole || index.column() != 4) return false;
    m_segments[index.row()].name = value.toString();
    emit dataChanged(index, index);
    return true;
}

Qt::ItemFlags SegmentModel::flags(const QModelIndex &index) const {
    auto f = QAbstractTableModel::flags(index);
    if (index.column() == 4) f |= Qt::ItemIsEditable;
    return f;
}
