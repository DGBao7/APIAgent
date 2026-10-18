#ifndef INPUTBAR_H
#define INPUTBAR_H

#include <QWidget>
#include <QString>
#include <QHBoxLayout>
#include <QLineEdit>

class QLineEdit;

class InputBar : public QWidget
{
    Q_OBJECT

public:
    explicit InputBar(QWidget *parent = nullptr);

signals:
    void message_submitted(const QString &message);

private slots:
    void _submit_message();

private:
    QLineEdit *m_input_field;
};

#endif