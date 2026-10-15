
#include "maf/orchestration/Planner.h"

#include <algorithm>
#include <cctype>
#include <cstddef>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "maf/agents/Strategy.h"
#include "maf/core/Task.h"
#include "maf/orchestration/Memory.h"
#include "maf/providers/ILLMProvider.h"

namespace maf::orchestration {

namespace {

constexpr int kMaxAttempts = 3;                  
constexpr double kTemperature = 0.2;             
constexpr int kMaxTokens = 1024;
constexpr std::size_t kMaxSteps = 20;
constexpr std::size_t kMaxContextChars = 6000;   
constexpr const char* kFactAvailableTools = "available_tools";



bool isSpace(char c) { return std::isspace(static_cast<unsigned char>(c)) != 0; }
bool isDigit(char c) { return std::isdigit(static_cast<unsigned char>(c)) != 0; }

std::string trimCopy(const std::string& s) {
    std::size_t b = 0;
    std::size_t e = s.size();
    while (b < e && isSpace(s[b])) ++b;
    while (e > b && isSpace(s[e - 1])) --e;
    return s.substr(b, e - b);
}

std::string toLowerCopy(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return s;
}

std::vector<std::string> splitList(const std::string& text, char sep) {
    std::vector<std::string> out;
    std::size_t start = 0;
    while (start <= text.size()) {
        std::size_t end = text.find(sep, start);
        if (end == std::string::npos) end = text.size();
        std::string item = trimCopy(text.substr(start, end - start));
        if (!item.empty()) out.push_back(std::move(item));
        start = end + 1;
    }
    return out;
}

std::string joinList(const std::vector<std::string>& items, const std::string& sep) {
    std::string out;
    for (std::size_t i = 0; i < items.size(); ++i) {
        if (i > 0) out += sep;
        out += items[i];
    }
    return out;
}


std::string truncateTail(const std::string& text, std::size_t maxChars) {
    if (text.size() <= maxChars) return text;
    std::size_t start = text.size() - maxChars;
    while (start < text.size() &&
           (static_cast<unsigned char>(text[start]) & 0xC0) == 0x80) {
        ++start;  
    }
    return "...(truncated)...\n" + text.substr(start);
}


std::string stripListMarker(const std::string& input) {
    const std::string line = trimCopy(input);

    
    if (line.size() > 4 && toLowerCopy(line.substr(0, 4)) == "step") {
        std::size_t i = 4;
        while (i < line.size() && isSpace(line[i])) ++i;
        const std::size_t digitsStart = i;
        while (i < line.size() && isDigit(line[i])) ++i;
        if (i > digitsStart && i < line.size() &&
            (line[i] == ':' || line[i] == '.' || line[i] == ')')) {
            return trimCopy(line.substr(i + 1));
        }
    }

    
    std::size_t i = 0;
    while (i < line.size() && isDigit(line[i])) ++i;
    if (i > 0 && i < line.size() &&
        (line[i] == '.' || line[i] == ')' || line[i] == ':')) {
        return trimCopy(line.substr(i + 1));
    }

    
    if (line.size() > 1 && (line[0] == '-' || line[0] == '*' || line[0] == '+') &&
        isSpace(line[1])) {
        return trimCopy(line.substr(1));
    }
    return line;
}


std::string stripWrapping(const std::string& s) {
    static const char* const kWrap = " \t`'\"*[]<>()";
    const std::size_t b = s.find_first_not_of(kWrap);
    if (b == std::string::npos) return {};
    const std::size_t e = s.find_last_not_of(kWrap);
    return s.substr(b, e - b + 1);
}

struct ParseOutcome {
    std::vector<PlanStep> steps;
    std::vector<std::string> problems;  
};


ParseOutcome parsePlanText(const std::string& text,
                           const std::vector<std::string>& allowedTools) {
    ParseOutcome out;

    std::unordered_map<std::string, std::string> canonical;  
    for (const std::string& t : allowedTools) {
        canonical[toLowerCopy(t)] = t;
    }

    std::size_t pos = 0;
    while (pos <= text.size()) {
        std::size_t eol = text.find('\n', pos);
        if (eol == std::string::npos) eol = text.size();
        std::string line = trimCopy(text.substr(pos, eol - pos));  
        pos = eol + 1;

        if (line.empty() || line.compare(0, 3, "```") == 0) continue;

        line = stripListMarker(line);
        const std::size_t bar = line.find('|');
        if (bar == std::string::npos) continue;

        std::string tool = stripWrapping(line.substr(0, bar));
        const std::string instruction = trimCopy(line.substr(bar + 1));

        if (tool.empty() || instruction.empty() || tool.size() > 60) {
            out.problems.push_back(
                "A line does not follow the format '<tool_name> | <instruction>'.");
            continue;
        }

        if (!canonical.empty()) {
            const auto it = canonical.find(toLowerCopy(tool));
            if (it == canonical.end()) {
                out.problems.push_back("Unknown tool '" + tool + "'.");
                continue;
            }
            tool = it->second;  
        } else if (tool.find_first_of(" \t") != std::string::npos) {
            out.problems.push_back("Invalid tool name '" + tool + "'.");
            continue;
        }

        if (out.steps.size() >= kMaxSteps) {
            out.problems.push_back("Too many steps (maximum is " +
                                   std::to_string(kMaxSteps) + ").");
            break;
        }

        out.steps.push_back(
            PlanStep{static_cast<int>(out.steps.size()) + 1, std::move(tool), instruction});
    }
    return out;
}


std::string buildSystemPrompt(const std::vector<std::string>& allowedTools) {
    std::string p =
        "You are the Planner of a multi-agent system. Break the task into a short, "
        "ordered list of steps. Each step runs exactly ONE tool.\n\n"
        "Output rules (strict):\n"
        "- One step per line, exactly in this format: <tool_name> | <instruction>\n"
        "- No numbering, bullets, markdown, code fences or explanations.\n"
        "- Every instruction must be self-contained. To reuse the result of an earlier "
        "step write {{step_N_output}} (N starts at 1), or {{last_output}} for the "
        "previous step.\n"
        "- Use at most " + std::to_string(kMaxSteps) +
        " steps and prefer the smallest plan that solves the task.\n";

    if (allowedTools.empty()) {
        p += "- Choose short snake_case tool names that describe the action.\n";
    } else {
        p += "- Use ONLY these tools: " + joinList(allowedTools, ", ") + "\n";
    }
    return p;
}

std::string buildUserPrompt(const std::string& taskId, const std::string& description,
                            const std::string& context) {
    std::string p = "TASK (id: " + taskId + "):\n" + description + "\n";
    if (!context.empty()) {
        p += "\nCONTEXT:\n" + context + "\n";
    }
    p += "\nReturn the plan now.";
    return p;
}

std::string buildFeedback(const std::vector<std::string>& problems,
                          const std::vector<std::string>& allowedTools) {
    std::string p = "Your previous answer is not a valid plan:\n";
    for (const std::string& problem : problems) {
        p += "- " + problem + "\n";
    }
    p += "Reply again with ONLY lines in the format: <tool_name> | <instruction>.";
    if (!allowedTools.empty()) {
        p += "\nAllowed tools: " + joinList(allowedTools, ", ") + ".";
    }
    return p;
}

std::string describePlan(const std::string& taskId, const std::vector<PlanStep>& steps) {
    std::string out = "[planner] Plan for task " + taskId + ":";
    for (const PlanStep& s : steps) {
        out += "\n" + std::to_string(s.index) + ". " + s.toolName + " | " + s.instruction;
    }
    return out;
}

}  

std::vector<PlanStep> Planner::createPlan(
    const maf::core::Task& task,
    maf::agents::strategy::LLMStrategyContext& llmContext,
    Memory& memory
) const {
    const std::string description = trimCopy(task.getDesc());
    if (description.empty()) {
        throw std::invalid_argument("Planner::createPlan: task description is empty");
    }

    const auto provider = llmContext.getProvider();
    if (!provider) {
        throw std::runtime_error("Planner::createPlan: no LLM provider configured");
    }

    const std::vector<std::string> allowedTools =
        splitList(memory.getFact(kFactAvailableTools), ',');

   
    const std::string context = truncateTail(memory.buildContext(), kMaxContextChars);
    memory.setFact("current_task_id", task.getId());
    memory.setFact("current_task", description);

    std::vector<maf::providers::Message> messages;
    messages.push_back(maf::providers::Message{"system", buildSystemPrompt(allowedTools)});
    messages.push_back(maf::providers::Message{
        "user", buildUserPrompt(task.getId(), description, context)});

    std::string lastFailure = "unknown error";

    for (int attempt = 1; attempt <= kMaxAttempts; ++attempt) {
        const maf::providers::LLMResponse response =
            provider->_chat(messages, kTemperature, kMaxTokens);

        if (!response.ok) {
            lastFailure = "provider '" + provider->_name() + "' failed (HTTP " +
                          std::to_string(response.status_code) + "): " + response.error;
            continue;  
        }
        if (trimCopy(response.text).empty()) {
            lastFailure = "the LLM returned an empty answer";
            continue;
        }

        ParseOutcome parsed = parsePlanText(response.text, allowedTools);
        if (parsed.steps.empty() && parsed.problems.empty()) {
            parsed.problems.push_back("No step found in the answer.");
        }

        if (parsed.problems.empty()) {
            memory.addEntry("assistant", describePlan(task.getId(), parsed.steps));
            return std::move(parsed.steps);
        }

       
        lastFailure = "invalid plan (" + joinList(parsed.problems, " ") + ")";
        messages.push_back(maf::providers::Message{"assistant", response.text});
        messages.push_back(maf::providers::Message{
            "user", buildFeedback(parsed.problems, allowedTools)});
    }

    throw std::runtime_error("Planner::createPlan: could not create a plan after " +
                             std::to_string(kMaxAttempts) + " attempts: " + lastFailure);
}

}  