#include "maf/agents/Strategy.h"

namespace maf::agents::strategy {
    constexpr int kMinOutputTokens = 128;
    constexpr int kSelectionResponseTokens = 512;
    constexpr int kSchemaAttempts = 2;

    struct RankedSelection {
        std::vector<std::string> ranking;
        int maxTokens;
    };

    // check if ""
    bool hasContent(const std::string& s) {
        return std::any_of(s.begin(), s.end(), [](unsigned char c) 
        {
            return std::isspace(c) == 0;
        });
    }

    // ktra danh sách providers
    void verifyProviders(
        const std::vector<std::shared_ptr<maf::providers::ILLMProvider>> providers
    ) 
    {
        if (providers.empty()) 
        {
            // std::cout << "Strategy: provider list cannot be empty." << "\n";
            // return;
            throw std::invalid_argument("agents/strategy: provider list empty.");
        }

        std::unordered_set<std::string> seen;

        for (const auto &provider : providers) 
        {
            if (!provider) 
            {
                // std::cout << "Strategy: a provider is null." << "\n";
                // return;
                throw std::invalid_argument("agents/strategy: a provider is null.");
            }

            const std::string name = provider -> _name();

            if (!hasContent(name) || !seen.insert(name).second) 
            {
                // std::cout << "Strategy: provider names must be nonempty and unique." << "\n";
                // return;
                throw std::invalid_argument("agents/strategy: provider names empty and unique.");
            }
        }
    }

    // chuyển đổi giữa hai kiểu dữ liệu
    std::vector<std::shared_ptr<maf::providers::ILLMProvider>> borrowProviders(
        const std::vector<maf::providers::ILLMProvider*> providers
    ) 
    {
        std::vector<std::shared_ptr<maf::providers::ILLMProvider>> result;
        result.reserve(providers.size());

        for (auto* ptr : providers) 
        {
            if (!ptr) 
            {   
                // std::cout << "Strategy: a provider is null." << "\n";
                throw std::invalid_argument("agents/strategy: provider null.");
            }
            result.emplace_back(ptr, [](maf::providers::ILLMProvider*) {});
        }

        return result;
    }

    StrategyDecision currentDecision(const LLMStrategyContext& context) 
    {
        StrategyDecision decision;

        decision.providerRanking = context.getProviderNames();
        decision.maxTokens = context.getMaxTokens();
        const auto selected = context.getProvider();

        if (selected) 
        {
            decision.selectedProvider = selected->_name();
            decision.isSuccess = true;
        }

        return decision;
    }

    RankedSelection parseSelection(const std::string& text) 
    {
        const nlohmann::json value = nlohmann::json::parse(text);
        if (!value.is_object() || value.size() != 2 ||
            !value.contains("providerRanking") || !value.contains("maxTokens")) {
            throw std::runtime_error("agents/strategy: error");
        }
        if (!value.at("providerRanking").is_array() ||
            !value.at("maxTokens").is_number_integer()) {
            throw std::runtime_error("agents/strategy: error JSON");
        }
        RankedSelection selection;
        for (const auto& item : value.at("providerRanking")) {
            if (!item.is_string()) {
                throw std::runtime_error("agents/strategy: providerRanking error.");
            }
            selection.ranking.push_back(item.get<std::string>());
        }
        selection.maxTokens = value.at("maxTokens").get<int>();
        if (selection.maxTokens <= 0) {
            throw std::runtime_error("agents/strategy: maxtokens negative.");
        }
        return selection;
    }

    std::shared_ptr<maf::providers::ILLMProvider>
        LLMStrategyContext::getProvider() const 
    {
        if (activeIndex_ >= providers_.size()) return nullptr;
        return providers_[activeIndex_];
    }

    bool LLMStrategyContext::switchToNextProvider() 
    {
        if (activeIndex_ + 1 >= providers_.size()) return false;
        ++activeIndex_;
        return true;
    }

    int LLMStrategyContext::getMaxTokens() const { return maxTokens_; }
    int LLMStrategyContext::getDefaultMaxTokens() const { return defaultMaxTokens_; }
    int LLMStrategyContext::getMaxRetries() const { return maxRetries_; }
    int LLMStrategyContext::getMaxTokenLimit() const { return maxTokenLimit_; }

    LLMStrategyContext::LLMStrategyContext(
        std::vector<std::shared_ptr<maf::providers::ILLMProvider>> providers,
        int maxTokens,
        int maxRetries,
        int maxTokenLimit
    )
    {
        this -> providers_ = std::move(providers);
        this -> maxTokens_ = maxTokens;
        this -> defaultMaxTokens_ = maxTokens;
        this -> maxRetries_ = maxRetries;
        this -> maxTokenLimit_ = maxTokenLimit;

        verifyProviders(providers_);

        if (this -> maxTokenLimit_ < kMinOutputTokens ||
            this -> maxTokens_ <= 0 ||
            this -> maxRetries_ <= 0)
        {
            throw std::invalid_argument("agents/strategy: error token");
        }

        this -> maxTokens_ = std::clamp(maxTokens_, kMinOutputTokens, maxTokenLimit_);
        this -> defaultMaxTokens_ = maxTokens_;
        this -> originalOrder_ = getProviderNames();
    }

    void LLMStrategyContext::sleepForBackoff(int attempt) const {
        const int exponent = std::clamp(attempt, 0, 5);
        std::this_thread::sleep_for(std::chrono::seconds(1 << exponent));
    }

    std::vector<std::string> LLMStrategyContext::getProviderNames() const {
        std::vector<std::string> names;
        names.reserve(providers_.size());
        for (const auto& provider : providers_) names.push_back(provider->_name());
        return names;
    }

    std::vector<std::string> LLMStrategyContext::getDefaultProviderNames() const {
        return originalOrder_;
    }

    void LLMStrategyContext::applySelection(
        const std::vector<std::string>& ranking, int maxTokens
    ) {
        if (maxTokens <= 0 || ranking.size() != providers_.size()) {
            throw std::invalid_argument("agents/strategy: error ranking");
        }
        std::unordered_map<std::string, std::shared_ptr<maf::providers::ILLMProvider>> byName;
        for (const auto& provider : providers_) byName.emplace(provider->_name(), provider);

        std::vector<std::shared_ptr<maf::providers::ILLMProvider>> reordered;
        reordered.reserve(ranking.size());
        for (const auto& name : ranking) {
            const auto iter = byName.find(name);
            if (iter == byName.end()) {
                throw std::invalid_argument("agents/strategy: duplicate: " + name);
            }
            reordered.push_back(iter->second);
            byName.erase(iter);
        }
        providers_ = std::move(reordered);
        activeIndex_ = 0;
        maxTokens_ = std::clamp(maxTokens, kMinOutputTokens, maxTokenLimit_);
    }

    StrategyDecision DefaultSelectionStrategy::decide(
        const maf::core::Task& task, LLMStrategyContext& context
    ) {
        StrategyDecision decision;
        if (!hasContent(task.getDesc())) {
            decision.errorMessage = "agents/strategy: desc empty.";
            return decision;
        }
        try {
            context.applySelection(context.getDefaultProviderNames(), context.getDefaultMaxTokens());
            return currentDecision(context);
        } catch (const std::exception& e) {
            decision.errorMessage = e.what();
            return decision;
        }
    }

    LLMSelectionStrategy::LLMSelectionStrategy(maf::providers::ILLMProvider& bootstrapProvider)
        : bootstrapProvider_(bootstrapProvider) {}

    StrategyDecision LLMSelectionStrategy::decide(
        const maf::core::Task& task, LLMStrategyContext& context
    ) 
    {
    StrategyDecision failure;
    if (!hasContent(task.getDesc())) {
        failure.errorMessage = "agents/strategy: desc empty.";
        return failure;
    }

    const auto names = context.getDefaultProviderNames();
    std::vector<maf::providers::Message> messages = {
        {"system",
         "You select a provider for a task, NOT an execution plan. "
         "Return ONLY a JSON object with exactly providerRanking (array of names) "
         "and maxTokens (positive integer, proposed OUTPUT token limit). "
         "Use each supplied provider name exactly once, with exact spelling. "
         "You know provider NAMES only, not actual models, costs, quota or availability. "
         "Do not invent performance facts; when there is insufficient evidence, "
         "retain the original order. The host application validates limits."},
        {"user", nlohmann::json({
            {"task", task.getDesc()},
            {"availableProviders", names},
            {"maximumAllowedOutputTokens", context.getMaxTokenLimit()}
        }).dump()}
    };

    std::string lastError;
    for (int attempt = 0; attempt < kSchemaAttempts; ++attempt) {
        try {
            const auto response = bootstrapProvider_._chat(
                messages, 0.1, kSelectionResponseTokens
            );
            if (!response.ok) {
                lastError = "Bootstrap provider failed: " + response.error;
                break;
            }
            try {
                const RankedSelection selection = parseSelection(response.text);
                context.applySelection(selection.ranking, selection.maxTokens);
                return currentDecision(context);
            } catch (const std::exception& e) {
                lastError = e.what();
                if (attempt + 1 < kSchemaAttempts) {
                    messages.push_back({"assistant", response.text});
                    messages.push_back({"user",
                        "Invalid answer. Retry with ONLY JSON containing providerRanking "
                        "(all supplied names exactly once) and maxTokens (positive integer)."});
                }
            }
        } catch (const std::exception& e) {
            lastError = e.what();
            break;
        }
    }
    StrategyDecision fallbackDecision = fallback_.decide(task, context);
    fallbackDecision.usedFallback = fallbackDecision.isSuccess;
    fallbackDecision.errorMessage = lastError;
    return fallbackDecision;
}
} 