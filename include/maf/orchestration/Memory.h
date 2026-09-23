#pragma once

#include <string>
#include <unordered_map>
#include <vector>

namespace maf::orchestration {

struct MemoryEntry {
    std::string role;    
    std::string content;
};


class Memory {
public:
    void addEntry(const std::string& role, const std::string& content);
    const std::vector<MemoryEntry>& getHistory() const noexcept;

    void setFact(const std::string& key, const std::string& value);
    std::string getFact(const std::string& key) const;
    bool hasFact(const std::string& key) const;

    
    std::string buildContext() const;

    void clear();

private:
    std::vector<MemoryEntry> history_;
    std::unordered_map<std::string, std::string> facts_;
};

}
