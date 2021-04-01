#include "SongModel.h"

SongModel::SongModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

int SongModel::rowCount(const QModelIndex &parent) const
{
    // For list models only the root node (an invalid parent) should return the list's size. For all
    // other (valid) parents, rowCount() should return 0 so that it does not become a tree model.
    if (parent.isValid() || !m_list)
        return 0;

    return m_list->items()->size();
}

SongList *SongModel::list() const
{
    return m_list;
}

void SongModel::setList(SongList *_list)
{
    beginResetModel();

    if(m_list) {
        //        m_list->disconnect(this);
    }

    m_list = _list;

    if(m_list) {
        connect(m_list, &SongList::preItemAppended, this, [=](){
            const int _idx = m_list->items()->size();
            beginInsertRows(QModelIndex(), _idx, _idx);
        });
        connect(m_list, &SongList::postItemAppended, this, [=](){
            endInsertRows();
        });
        connect(m_list, &SongList::preItemRemoved, this, [=](int _idx){
            beginRemoveRows(QModelIndex(), _idx, _idx);
        });
        connect(m_list, &SongList::postItemRemoved, this, [=](int _idx){
            endRemoveRows();
        });
        connect(m_list, &SongList::preListClear, this, [=](){
            beginRemoveRows(QModelIndex(), 0, rowCount() - 1);
        });
        connect(m_list, &SongList::postListClear, this, [=](){
            endRemoveRows();
        });
        connect(m_list, &SongList::dataChanged, this, [=](int row){
            emit dataChanged(index(row, 0), index(row, 0));
        });
    }

    endResetModel();
}

QVariant SongModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || !m_list)
        return QVariant();

    const SongItem & item = m_list->items()->at(index.row());
    switch (role) {
    case NameRole:
        return item.name;
    case DurationRole:
        return item.duration;
    case ArtistRole:
        return item.artist;
    case ThumbnailRole:
        return item.image;
    default:
        break;
    }
    return QVariant();
}

QHash<int, QByteArray> SongModel::roleNames() const
{
    QHash<int, QByteArray> names;
    names[NameRole] = "Name";
    names[DurationRole] = "Duration";
    names[ArtistRole] = "Artist";
    names[ThumbnailRole] = "Thumbnail";
    return names;
}
