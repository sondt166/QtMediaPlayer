#ifndef MUSICMODEL_H
#define MUSICMODEL_H

#include <QAbstractListModel>
#include <QDebug>
#include <QVariant>

#include "SongList.h"

class SongModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(SongList * list READ list WRITE setList)
public:
    enum Roles {
        NameRole = Qt::UserRole,
        DurationRole,
        ArtistRole,
        ThumbnailRole
    };
    explicit SongModel(QObject *parent = nullptr);

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    SongList * list() const;
    void setList(SongList *_list);

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    QHash<int, QByteArray> roleNames() const override;
private:
    SongList * m_list;
};

#endif // MUSICMODEL_H
