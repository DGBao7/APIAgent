#include "AgentDashboard.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include <QTextBrowser>
#include <QTextEdit>
#include <QPushButton>
#include <QListWidget>
#include <QComboBox>
#include <QLabel>
#include <QScrollBar>
#include <QMessageBox>
#include <QTimer>
#include <QEnterEvent>
#include <QUuid>

// =====================================================================
// CLASS TÙY CHỈNH: GIAO DIỆN TỪNG DÒNG LỊCH SỬ
// =====================================================================
class HistoryItemWidget : public QWidget {
public:
    QPushButton *btnRemove;

    HistoryItemWidget(const QString &title, QWidget *parent = nullptr) : QWidget(parent) {
        QHBoxLayout *layout = new QHBoxLayout(this);
        layout->setContentsMargins(10, 8, 5, 8);
        layout->setSpacing(10);

        QLabel *lblIcon = new QLabel("💬", this);
        QLabel *lblTitle = new QLabel(title, this);
        lblTitle->setStyleSheet("color: #e3e3e3; font-size: 13px;");
        
        // Cho phép click xuyên qua Label để ListWidget nhận được sự kiện chọn dòng
        lblIcon->setAttribute(Qt::WA_TransparentForMouseEvents);
        lblTitle->setAttribute(Qt::WA_TransparentForMouseEvents);
        
        btnRemove = new QPushButton("✕", this);
        btnRemove->setFixedSize(22, 22);
        btnRemove->setCursor(Qt::PointingHandCursor);
        btnRemove->setStyleSheet(R"(
            QPushButton { background: transparent; color: #a8c7fa; border: none; font-weight: bold; border-radius: 11px; }
            QPushButton:hover { background-color: #444746; color: #f87171; }
        )");
        btnRemove->hide(); 

        layout->addWidget(lblIcon);
        layout->addWidget(lblTitle, 1); 
        layout->addWidget(btnRemove);
    }

protected:
    void enterEvent(QEnterEvent *event) override { btnRemove->show(); QWidget::enterEvent(event); }
    void leaveEvent(QEvent *event) override { btnRemove->hide(); QWidget::leaveEvent(event); }
};

// =====================================================================
// KHỞI TẠO DASHBOARD
// =====================================================================
AgentDashboard::AgentDashboard(QWidget *parent) : QMainWindow(parent) {
    setWindowTitle("Co-Stretch Agentic AI - Mission Control");
    resize(1200, 800);
    
    setupLayout();
    applyModernTheme();

    // Khởi tạo phiên trò chuyện đầu tiên khi mở app
    createNewSession();
}

void AgentDashboard::setupLayout() {
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // 0. TOP BAR
    QWidget *topBar = new QWidget;
    topBar->setObjectName("topBar");
    QHBoxLayout *topLayout = new QHBoxLayout(topBar);
    topLayout->setContentsMargins(15, 10, 15, 10);
    
    m_btnToggleSidebar = new QPushButton("☰");
    m_btnToggleSidebar->setObjectName("iconButton");
    m_btnToggleSidebar->setFixedSize(40, 40);
    
    QLabel *lblAppTitle = new QLabel("<b>Co-Stretch AI</b>");
    lblAppTitle->setStyleSheet("color: #e3e3e3; font-size: 18px; margin-left: 10px;");
    
    topLayout->addWidget(m_btnToggleSidebar);
    topLayout->addWidget(lblAppTitle);
    topLayout->addStretch();
    mainLayout->addWidget(topBar);

    m_mainSplitter = new QSplitter(Qt::Horizontal, this);
    
    // 1. CỘT TRÁI (SIDEBAR)
    m_sidebarWidget = new QWidget;
    m_sidebarWidget->setObjectName("sidebar");
    QVBoxLayout *sidebarLayout = new QVBoxLayout(m_sidebarWidget);
    sidebarLayout->setContentsMargins(20, 10, 20, 20);
    sidebarLayout->setSpacing(15);

    // Nút Cuộc trò chuyện mới
    m_btnNewChat = new QPushButton("+ Cuộc trò chuyện mới");
    m_btnNewChat->setObjectName("btnNewChat");
    m_btnNewChat->setCursor(Qt::PointingHandCursor);

    QLabel *lblHistory = new QLabel("Lịch sử trò chuyện:");
    lblHistory->setObjectName("sidebarHeader");
    m_historyList = new QListWidget;
    m_historyList->setObjectName("historyList");
    
    QLabel *lblStrategy = new QLabel("Chiến lược LLM (Strategy):");
    lblStrategy->setObjectName("sidebarHeader");
    m_strategyCombo = new QComboBox;
    m_strategyCombo->addItems({"Gemini 2.5 Pro", "OpenAI GPT-4o", "Local Ollama"});
    
    QLabel *lblAgents = new QLabel("Mạng lưới Agent (Co-Stretch):");
    lblAgents->setObjectName("sidebarHeader");
    m_agentList = new QListWidget;
    m_agentList->setObjectName("agentList");

    sidebarLayout->addWidget(m_btnNewChat);
    sidebarLayout->addWidget(lblHistory);
    sidebarLayout->addWidget(m_historyList, 4); 
    sidebarLayout->addWidget(lblStrategy);
    sidebarLayout->addWidget(m_strategyCombo);
    sidebarLayout->addWidget(lblAgents);
    sidebarLayout->addWidget(m_agentList, 3);
    
    // 2. CỘT PHẢI (CHAT AREA)
    QWidget *chatWidget = new QWidget;
    chatWidget->setObjectName("chatArea");
    QVBoxLayout *chatLayout = new QVBoxLayout(chatWidget);
    chatLayout->setContentsMargins(40, 0, 40, 20); 
    chatLayout->setSpacing(20);

    m_chatHistory = new QTextBrowser;
    m_chatHistory->setObjectName("chatHistory");
    m_chatHistory->setOpenExternalLinks(true);

    // Khu vực Input
    QWidget *inputContainer = new QWidget;
    inputContainer->setObjectName("inputContainer");
    QVBoxLayout *mainInputLayout = new QVBoxLayout(inputContainer);
    mainInputLayout->setContentsMargins(15, 10, 15, 10);
    mainInputLayout->setSpacing(10);

    m_filePreviewArea = new QWidget;
    m_filePreviewLayout = new QHBoxLayout(m_filePreviewArea);
    m_filePreviewLayout->setContentsMargins(0, 0, 0, 0);
    m_filePreviewLayout->setAlignment(Qt::AlignLeft);
    m_filePreviewArea->hide(); 

    QHBoxLayout *inputLayout = new QHBoxLayout;
    inputLayout->setContentsMargins(0, 0, 0, 0);
    inputLayout->setSpacing(12);

    m_btnAddFile = new QPushButton("+");
    m_btnAddFile->setObjectName("iconButton");
    m_btnAddFile->setFixedSize(40, 40);

    m_inputBox = new QTextEdit;
    m_inputBox->setObjectName("inputBox");
    m_inputBox->setPlaceholderText("Nhập yêu cầu phân tích dữ liệu hoặc sinh mã...");
    m_inputBox->setFixedHeight(40); 
    m_inputBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    connect(m_inputBox, &QTextEdit::textChanged, this, [this]() {
        int docHeight = m_inputBox->document()->size().height();
        int newHeight = qBound(40, docHeight + 12, 150);
        m_inputBox->setFixedHeight(newHeight);
    });

    m_btnMic = new QPushButton("🎤"); 
    m_btnMic->setObjectName("iconButton");
    m_btnMic->setFixedSize(40, 40);

    m_btnSend = new QPushButton("Gửi");
    m_btnSend->setObjectName("btnSend");
    m_btnSend->setFixedSize(60, 40);
    
    inputLayout->addWidget(m_btnAddFile);
    inputLayout->addWidget(m_inputBox);
    inputLayout->addWidget(m_btnMic);
    inputLayout->addWidget(m_btnSend);

    mainInputLayout->addWidget(m_filePreviewArea); 
    mainInputLayout->addLayout(inputLayout);       

    chatLayout->addWidget(m_chatHistory, 1);  
    chatLayout->addWidget(inputContainer, 0); 

    m_mainSplitter->addWidget(m_sidebarWidget);
    m_mainSplitter->addWidget(chatWidget);
    m_mainSplitter->setStretchFactor(0, 1); 
    m_mainSplitter->setStretchFactor(1, 5); 
    mainLayout->addWidget(m_mainSplitter);

    // KẾT NỐI SỰ KIỆN
    connect(m_btnToggleSidebar, &QPushButton::clicked, this, &AgentDashboard::onToggleSidebarClicked);
    connect(m_btnNewChat, &QPushButton::clicked, this, &AgentDashboard::onNewChatClicked);
    connect(m_historyList, &QListWidget::itemClicked, this, &AgentDashboard::onHistoryItemClicked);
    connect(m_btnSend, &QPushButton::clicked, this, &AgentDashboard::onSendButtonClicked);
    connect(m_btnAddFile, &QPushButton::clicked, this, &AgentDashboard::onAddFileClicked);
    connect(m_btnMic, &QPushButton::clicked, this, &AgentDashboard::onMicClicked);
}

// =====================================================================
// LOGIC QUẢN LÝ PHIÊN TRÒ CHUYỆN (SESSION MANAGEMENT)
// =====================================================================

void AgentDashboard::createNewSession() {
    // Tạo mã ID duy nhất cho phiên chat mới
    m_currentSessionId = QUuid::createUuid().toString();
    m_chatHistory->clear();
    
    // Khởi tạo bộ nhớ trống cho phiên này
    m_chatSessions[m_currentSessionId] = ""; 
    appendAgentMessage("System", "**Xin chào người dùng!** Tôi là hệ thống Co-Stretch Multi-Agent.\nHãy chọn chiến lược LLM ở cột bên trái và giao nhiệm vụ cho tôi.", "#9ca3af");
    
    // Bỏ chọn các item đang được highlight trong danh sách bên trái
    m_historyList->clearSelection();
}

void AgentDashboard::saveCurrentSession() {
    if (!m_currentSessionId.isEmpty()) {
        // Lưu toàn bộ giao diện HTML hiện tại vào QMap
        m_chatSessions[m_currentSessionId] = m_chatHistory->toHtml();
    }
}

void AgentDashboard::onNewChatClicked() {
    createNewSession();
}

void AgentDashboard::onHistoryItemClicked(QListWidgetItem *item) {
    QString clickedSessionId = item->data(Qt::UserRole).toString();
    
    if (m_currentSessionId != clickedSessionId) {
        // Lưu lại phiên hiện tại trước khi chuyển sang phiên cũ
        saveCurrentSession();
        
        // Đổi ID và nạp lại HTML của phiên cũ ra màn hình
        m_currentSessionId = clickedSessionId;
        m_chatHistory->setHtml(m_chatSessions[m_currentSessionId]);
    }
}

void AgentDashboard::addHistoryItem(const QString &sessionId, const QString &title) {
    QListWidgetItem *item = new QListWidgetItem(m_historyList);
    item->setSizeHint(QSize(m_historyList->width(), 40)); 
    item->setData(Qt::UserRole, sessionId); // Cất ID vào trong Item
    m_historyList->addItem(item);
    
    HistoryItemWidget *customWidget = new HistoryItemWidget(title, this);
    m_historyList->setItemWidget(item, customWidget);

    // Sự kiện khi bấm nút ✕: Xóa phiên khỏi RAM và khỏi danh sách
    connect(customWidget->btnRemove, &QPushButton::clicked, this, [this, item, sessionId]() {
        m_chatSessions.remove(sessionId);
        
        // Nếu xóa đúng phiên đang mở, tự động tạo phiên mới
        if (m_currentSessionId == sessionId) {
            createNewSession();
        }
        
        int row = m_historyList->row(item);
        m_historyList->takeItem(row);
        delete item;
    });
}

// =====================================================================
// CÁC SỰ KIỆN GIAO DIỆN KHÁC
// =====================================================================

void AgentDashboard::onToggleSidebarClicked() {
    m_sidebarWidget->setVisible(!m_sidebarWidget->isVisible());
}

void AgentDashboard::onSendButtonClicked() {
    QString prompt = m_inputBox->toPlainText().trimmed();
    if (prompt.isEmpty() && m_filePreviewArea->isHidden()) return;

    QLayoutItem *child;
    while ((child = m_filePreviewLayout->takeAt(0)) != nullptr) {
        delete child->widget();
        delete child;
    }
    m_filePreviewArea->hide();

    if (!prompt.isEmpty()) {
        appendUserMessage(prompt);
        m_inputBox->clear();
        
        // Kiểm tra xem đây có phải là tin nhắn ĐẦU TIÊN của phiên này không
        bool isNewSession = true;
        for (int i = 0; i < m_historyList->count(); ++i) {
            if (m_historyList->item(i)->data(Qt::UserRole).toString() == m_currentSessionId) {
                isNewSession = false; 
                break;
            }
        }
        
        // Nếu là tin nhắn đầu tiên, tạo một dòng tiêu đề bên cột trái
        if (isNewSession) {
            QString shortTitle = prompt.left(25) + (prompt.length() > 25 ? "..." : "");
            addHistoryItem(m_currentSessionId, shortTitle);
        }

        emit userSubmittedTask(prompt, m_strategyCombo->currentText());
    }
}

class FileThumbnailWidget : public QWidget {
public:
    QPushButton *btnRemove;
    FileThumbnailWidget(QWidget *parent = nullptr) : QWidget(parent) {
        setFixedSize(60, 60);
        QLabel *lblIcon = new QLabel("📷\nFile", this);
        lblIcon->setFixedSize(60, 60);
        lblIcon->setStyleSheet("background-color: #282a2c; border-radius: 8px; border: 1px solid #444746; color: #a8c7fa; font-weight: bold;");
        lblIcon->setAlignment(Qt::AlignCenter);

        btnRemove = new QPushButton("✕", this);
        btnRemove->setFixedSize(20, 20);
        btnRemove->move(45, -5); 
        btnRemove->setCursor(Qt::PointingHandCursor);
        btnRemove->setStyleSheet("QPushButton { background-color: #444746; color: #e3e3e3; border-radius: 10px; font-size: 12px; font-weight: bold; border: none; } QPushButton:hover { background-color: #f87171; color: white; }");
        btnRemove->hide(); 
    }
protected:
    void enterEvent(QEnterEvent *event) override { btnRemove->show(); QWidget::enterEvent(event); }
    void leaveEvent(QEvent *event) override { btnRemove->hide(); QWidget::leaveEvent(event); }
};

void AgentDashboard::onAddFileClicked() {
    if (m_filePreviewLayout->count() > 0) return; 
    FileThumbnailWidget *thumbnail = new FileThumbnailWidget(this);
    m_filePreviewLayout->addWidget(thumbnail);
    m_filePreviewArea->show(); 
    
    connect(thumbnail->btnRemove, &QPushButton::clicked, this, [this, thumbnail]() {
        m_filePreviewLayout->removeWidget(thumbnail);
        thumbnail->deleteLater();
        if (m_filePreviewLayout->count() == 0) m_filePreviewArea->hide();
    });
}

void AgentDashboard::onMicClicked() {
    if (m_btnMic->property("isListening").toBool()) return; 

    m_btnMic->setProperty("isListening", true);
    m_btnMic->setStyleSheet("background-color: #f87171; color: white; border: none; border-radius: 20px; font-weight: bold; font-size: 22px; padding-bottom: 3px;");
    
    QString oldPlaceholder = m_inputBox->placeholderText();
    m_inputBox->setPlaceholderText("Đang lắng nghe... (Hãy nói gì đó)");

    QTimer::singleShot(3000, this, [this, oldPlaceholder]() {
        m_btnMic->setProperty("isListening", false);
        m_btnMic->setStyleSheet(""); 
        m_inputBox->setPlaceholderText(oldPlaceholder);
        m_inputBox->setText("Dữ liệu chứng khoán 2026.");
    });
}

void AgentDashboard::appendUserMessage(const QString &content) {
    QString html = QString("<div style='margin-bottom: 20px; text-align: right;'><span style='background-color: #333538; color: #e3e3e3; padding: 12px 18px; border-radius: 18px; font-size: 15px; display: inline-block;'>%1</span></div><br>")
        .arg(content.toHtmlEscaped().replace("\n", "<br>"));
    m_chatHistory->append(html);
    saveCurrentSession(); // Cập nhật vào bộ nhớ RAM
}

void AgentDashboard::appendAgentMessage(const QString &agentName, const QString &markdownContent, const QString &hexColor) {
    QString header = QString("<div style='margin-top: 10px; margin-bottom: 5px;'><b style='color: %1; font-size: 14px;'>✦ %2</b></div>").arg(hexColor, agentName);
    m_chatHistory->append(header);
    m_chatHistory->append(markdownContent); 
    m_chatHistory->append("<br>");          
    saveCurrentSession(); // Cập nhật vào bộ nhớ RAM
}

void AgentDashboard::updateAgentStatus(const QString &agentId, const QString &displayName, const QString &state) {
    QListWidgetItem *targetItem = nullptr;
    for (int i = 0; i < m_agentList->count(); ++i) {
        if (m_agentList->item(i)->data(Qt::UserRole).toString() == agentId) {
            targetItem = m_agentList->item(i);
            break;
        }
    }
    QString displayText = QString("🤖 %1\nTrạng thái: %2").arg(displayName, state);
    if (targetItem) {
        targetItem->setText(displayText);
    } else {
        targetItem = new QListWidgetItem(displayText, m_agentList);
        targetItem->setData(Qt::UserRole, agentId);
    }
}

// =====================================================================
// BỘ QSS (CSS) TẠO GIAO DIỆN DARK MODE
// =====================================================================

void AgentDashboard::applyModernTheme() {
    setStyleSheet(R"(
        QMainWindow { background-color: #131314; }
        
        QWidget#topBar { background-color: #131314; }
        QWidget#sidebar { background-color: #1e1f22; border-right: 1px solid #333538; }
        
        QLabel#sidebarHeader { color: #a8c7fa; font-size: 13px; font-weight: bold; margin-top: 10px; }
        QComboBox { background-color: #131314; color: #e3e3e3; border: 1px solid #444746; border-radius: 8px; padding: 8px; font-size: 14px; }
        
        QPushButton#btnNewChat { background-color: #282a2c; color: #e3e3e3; border: 1px solid #444746; border-radius: 20px; padding: 10px; font-size: 14px; font-weight: bold; margin-bottom: 10px;}
        QPushButton#btnNewChat:hover { background-color: #333538; }

        QListWidget#historyList { background-color: transparent; border: none; outline: none; }
        QListWidget#historyList::item { background-color: transparent; border-radius: 8px; }
        QListWidget#historyList::item:hover { background-color: #282a2c; }
        QListWidget#historyList::item:selected { background-color: #333538; }

        QListWidget#agentList { background-color: transparent; border: none; color: #e3e3e3; font-size: 13px; }
        QListWidget#agentList::item { background-color: #282a2c; border-radius: 8px; padding: 10px; margin-bottom: 8px; }

        QWidget#chatArea { background-color: #131314; }
        QTextBrowser#chatHistory { background-color: transparent; border: none; color: #e3e3e3; font-family: 'Segoe UI', Arial, sans-serif; font-size: 15px; line-height: 1.5; }
        
        QWidget#inputContainer { background-color: #1e1f22; border-radius: 25px; }
        QTextEdit#inputBox { background-color: transparent; border: none; color: #e3e3e3; font-size: 15px; margin-top: 8px;}
        
        QPushButton#iconButton { background-color: transparent; color: #a8c7fa; border: none; border-radius: 20px; font-weight: bold; font-size: 22px; padding-bottom: 3px; }
        QPushButton#iconButton:hover { background-color: #282a2c; }
        QPushButton#iconButton:pressed { background-color: #444746; }

        QPushButton#btnSend { background-color: #a8c7fa; color: #041e49; border: none; border-radius: 20px; font-weight: bold; font-size: 14px; }
        QPushButton#btnSend:hover { background-color: #b5cffb; }
        QPushButton#btnSend:pressed { background-color: #8ab4f8; }
        
        QScrollBar:vertical { border: none; background: #131314; width: 10px; margin: 0px 0px 0px 0px; }
        QScrollBar::handle:vertical { background: #444746; min-height: 30px; border-radius: 5px; }
    )");
}