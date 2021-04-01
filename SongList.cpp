#include "SongList.h"

SongList::SongList(QObject *parent) : QObject(parent)
{

}

QList<SongItem> * SongList::items()
{
    return &m_items;
}

bool SongList::setItemAt(int index, const SongItem & item)
{
    if (index < 0 || index > m_items.size())
        return false;

    // checking raw memory won't work because of heap allocated objects such as QString,
    // also due to struct memory padding
    /*const SongItem & oldItem = m_items.at(index);
    if(memcmp(&item, &oldItem, sizeof(SongItem)) == 0)
        return false;*/

    m_items[index] = item;
    emit dataChanged(index);
    return true;
}

void SongList::appendItem(SongItem _item)
{
    emit preItemAppended();
    m_items.push_back(_item);
    emit postItemAppended();
}

void SongList::removeItemAt(int _index)
{
    if (_index < 0 || _index >= m_items.size())
        return;

    qDebug() << "removing " << _index;

    emit preItemRemoved(_index);
    m_items.removeAt(_index);
    emit postItemRemoved(_index);
}

void SongList::clear()
{
    emit preListClear();
    m_items.clear();
    emit postListClear();
}
