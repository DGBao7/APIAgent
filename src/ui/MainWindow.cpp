#include "maf/ui/MainWindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      m_conversation(nullptr),
      m_history(nullptr),
      m_input_bar(nullptr)
{
    setWindowTitle("APIAgent");
    resize(1240, 820);
    setMinimumSize(860, 600);

    _setup_layout();
    setStyleSheet(ui::theme::_style_sheet());

    m_conversation->_append_ai_message(
        "Hỏi gì hỏi mau, hỏi ít thôi k hết token của t");

    connect(
        m_input_bar,
        &InputBar::message_submitted,
        this,
        &MainWindow::_handle_user_message
    );
}

void MainWindow::_setup_layout()
{
    QWidget *central_widget = new QWidget(this);
    QVBoxLayout *main_layout = new QVBoxLayout(central_widget);
    main_layout->setContentsMargins(0, 0, 0, 0);
    main_layout->setSpacing(0);
    setCentralWidget(central_widget);

    QWidget *top_bar = new QWidget(central_widget);
    top_bar->setObjectName("topBar");

    QHBoxLayout *top_layout = new QHBoxLayout(top_bar);
    top_layout->setContentsMargins(24, 16, 24, 16);
    top_layout->setSpacing(12);

    QLabel *app_title = new QLabel("✦ APIAgent", top_bar);
    app_title->setObjectName("appTitle");

    QLabel *app_subtitle = new QLabel("Multi-Agent workspace", top_bar);
    app_subtitle->setObjectName("appSubtitle");

    top_layout->addWidget(app_title);
    top_layout->addWidget(app_subtitle);
    top_layout->addStretch();

    QSplitter *main_splitter = new QSplitter(Qt::Horizontal, central_widget);
    main_splitter->setChildrenCollapsible(false);

    m_history = new History(main_splitter);

    QWidget *chat_panel = new QWidget(main_splitter);
    QVBoxLayout *chat_layout = new QVBoxLayout(chat_panel);
    chat_layout->setContentsMargins(0, 0, 0, 0);
    chat_layout->setSpacing(0);

    m_conversation = new Conversation(chat_panel);
    m_input_bar = new InputBar(chat_panel);

    chat_layout->addWidget(m_conversation, 1);
    chat_layout->addWidget(m_input_bar, 0);

    main_splitter->addWidget(m_history);
    main_splitter->addWidget(chat_panel);
    main_splitter->setStretchFactor(0, 0);
    main_splitter->setStretchFactor(1, 1);
    main_splitter->setSizes({270, 970});

    main_layout->addWidget(top_bar);
    main_layout->addWidget(main_splitter, 1);
}

void MainWindow::_handle_user_message(const QString &message)
{
    m_conversation->_append_user_message(message);
    emit user_message_submitted(message);
}

void MainWindow::_append_ai_message(const QString &message)
{
    m_conversation->_append_ai_message(message);
}

void MainWindow::_set_busy(bool busy)
{
    m_input_bar->setEnabled(!busy);
}

void MainWindow::_set_agent_statuses(
    const std::vector<
        std::shared_ptr<maf::agents::AgentStatus>
    > &statuses
)
{
    m_history->_set_agent_statuses(statuses);
}