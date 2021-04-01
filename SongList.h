#ifndef SONGLIST_H
#define SONGLIST_H

#include <QObject>
#include <QList>
#include <QDir>
#include <QImage>
#include <QDebug>

struct SongItem {
    QString name = "";
    QString path = "";
    QString artist = "";
    qint64 duration = 0;
    QImage image = QImage(":/nomedia1.png");
};

class SongList : public QObject
{
    Q_OBJECT
public:
    explicit SongList(QObject *parent = nullptr);
    QList<SongItem> * items();
    bool setItemAt(int index, const SongItem & item);

signals:
    void preItemAppended();
    void postItemAppended();

    void preItemRemoved(int _index);
    void postItemRemoved(int _index);

    void preListClear();
    void postListClear();

    void dataChanged(int _index);

public slots:
    void appendItem(SongItem _item);
    void removeItemAt(int _index);
    void clear();
private:
    QList<SongItem> m_items;
};

#endif // SONGLIST_H
