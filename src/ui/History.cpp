#include "maf/ui/History.h"

History::History(QWidget *parent)
    : QWidget(parent),
      m_history_list(new QListWidget(this))
{
    setObjectName("historyPanel");
    setMinimumWidth(225);
    setMaximumWidth(310);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(16, 24, 16, 18);
    layout->setSpacing(12);

    QLabel *title = new QLabel("LỊCH SỬ TRÒ CHUYỆN", this);
    title->setObjectName("sectionTitle");

    QLabel *description = new QLabel("Các cuộc trò chuyện gần đây", this);
    description->setObjectName("mutedText");

    m_history_list->setObjectName("historyList");
    m_history_list->setSelectionMode(QAbstractItemView::SingleSelection);
    m_history_list->addItems({
        "Phân tích dữ liệu",
        "Viết chương trình C++",
        "Tìm hiểu APIAgent",
        "Lập kế hoạch dự án",
        "Cuộc trò chuyện mới"
    });

    QLabel *placeholder = new QLabel("Lịch sử hiện là giao diện mẫu.\nDữ liệu sẽ được kết nối sau.", this);
    placeholder->setObjectName("mutedText");
    placeholder->setWordWrap(true);

    layout->addWidget(title);
    layout->addWidget(description);
    layout->addWidget(m_history_list, 1);
    layout->addWidget(placeholder);
}
