#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "maf/ui/Conversation.h"
#include "maf/ui/History.h"
#include "maf/ui/InputBar.h"
#include "maf/ui/theme/Color.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QSplitter>
#include <QVBoxLayout>
#include <QWidget>
#include <QMainWindow>
#include <QString>

class Conversation;
class History;
class InputBar;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

signals:
    void user_message_submitted(const QString &message);

private slots:
    void _handle_user_message(const QString &message);

private:
    void _setup_layout();

    Conversation *m_conversation;
    History *m_history;
    InputBar *m_input_bar;
};

#endif