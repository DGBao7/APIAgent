#ifndef HISTORY_H
#define HISTORY_H

#include <QAbstractItemView>
#include <QLabel>
#include <QListWidget>
#include <QVBoxLayout>
#include <QWidget>

class QListWidget;

class History : public QWidget
{
    Q_OBJECT

public:
    explicit History(QWidget *parent = nullptr);

private:
    QListWidget *m_history_list;
};

#endif