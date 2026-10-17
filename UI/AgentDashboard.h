#pragma once

#include <QMainWindow>
#include <QString>
#include <QMap>

class QTextBrowser;
class QTextEdit;
class QPushButton;
class QListWidget;
class QComboBox;
class QSplitter;
class QWidget;
class QHBoxLayout;
class QListWidgetItem;

class AgentDashboard : public QMainWindow {
    Q_OBJECT

private:
    QSplitter *m_mainSplitter;
    QPushButton *m_btnToggleSidebar;
    
    QWidget *m_sidebarWidget; 
    QPushButton *m_btnNewChat;  // Nút "Cuộc trò chuyện mới"
    QListWidget *m_historyList; 
    QListWidget *m_agentList; 
    QComboBox *m_strategyCombo;

    QTextBrowser *m_chatHistory;
    QTextEdit *m_inputBox;
    QPushButton *m_btnSend;
    QPushButton *m_btnAddFile;
    QPushButton *m_btnMic;

    QWidget *m_filePreviewArea;
    QHBoxLayout *m_filePreviewLayout;

    // QUẢN LÝ DỮ LIỆU LỊCH SỬ TRÒ CHUYỆN
    QMap<QString, QString> m_chatSessions; // Ánh xạ: ID Cuộc trò chuyện -> Nội dung HTML
    QString m_currentSessionId;            // ID của cuộc trò chuyện hiện tại đang mở

    void setupLayout();
    void applyModernTheme();
    void addHistoryItem(const QString &sessionId, const QString &title);
    
    // Hàm xử lý phiên
    void createNewSession();
    void saveCurrentSession();

public:
    explicit AgentDashboard(QWidget *parent = nullptr);

public slots:
    void appendAgentMessage(const QString &agentName, const QString &markdownContent, const QString &hexColor = "#38bdf8");
    void updateAgentStatus(const QString &agentId, const QString &displayName, const QString &state);
    void appendUserMessage(const QString &content);

signals:
    void userSubmittedTask(const QString &taskPrompt, const QString &selectedStrategy);

private slots:
    void onSendButtonClicked();
    void onAddFileClicked();
    void onMicClicked();
    void onToggleSidebarClicked();
    void onHistoryItemClicked(QListWidgetItem *item); // Xử lý khi nhấn vào lịch sử cũ
    void onNewChatClicked();                          // Xử lý khi nhấn tạo Chat mới
};