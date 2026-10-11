
#include "maf/orchestration/Memory.h"

#include <map>
#include <sstream>
#include <string>
#include <vector>

namespace maf::orchestration {

void Memory::addEntry(const std::string& role, const std::string& content) {
    history_.push_back(MemoryEntry{role, content});
}

const std::vector<MemoryEntry>& Memory::getHistory() const noexcept {
    return history_;
}

void Memory::setFact(const std::string& key, const std::string& value) {
    facts_[key] = value;  
}

std::string Memory::getFact(const std::string& key) const {
    auto it = facts_.find(key);
    if (it != facts_.end()) {
        return it->second;
    }
    return "";
}

bool Memory::hasFact(const std::string& key) const {
    return facts_.find(key) != facts_.end();
}


std::string Memory::buildContext() const {
    std::ostringstream oss;

    if (!facts_.empty()) {
        const std::map<std::string, std::string> sortedFacts(facts_.begin(), facts_.end());
        oss << "=== Key Facts ===\n";
        for (const auto& [key, value] : sortedFacts) {
            oss << key << ": " << value << "\n";
        }
        oss << "\n";
    }

    if (!history_.empty()) {
        oss << "=== Conversation History ===\n";
        for (const auto& entry : history_) {
            oss << entry.role << ": " << entry.content << "\n";
        }
    }

    return oss.str();
}

void Memory::clear() {
    history_.clear();
    facts_.clear();
}

}  