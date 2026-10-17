#include <QApplication>
#include "AgentDashboard.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // Khởi tạo và hiển thị giao diện Mission Control
    AgentDashboard dashboard;
    dashboard.show();

    return app.exec();
}