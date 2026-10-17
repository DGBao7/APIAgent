#ifndef STRATEGY_H
#define STRATEGY_H

#include <cstddef>
#include <memory>
#include <string>
#include <vector>

#include <algorithm>
#include <chrono>
#include <cctype>
#include <stdexcept>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <iostream>
#include <nlohmann/json.hpp>

#include "maf/providers/ILLMProvider.h"
#include "maf/core/Task.h"

namespace maf::agents::strategy 
{
    class LLMStrategyContext 
    {
    private:
        std::vector<std::shared_ptr<maf::providers::ILLMProvider>> providers_;
        std::vector<std::string> originalOrder_;
        std::size_t activeIndex_ = 0;
        int maxTokens_;
        int defaultMaxTokens_;
        int maxRetries_;
        int maxTokenLimit_;

    public:
        explicit LLMStrategyContext(
            std::vector<std::shared_ptr<maf::providers::ILLMProvider>> providers,
            int maxTokens = 2048,
            int maxRetries = 3,
            int maxTokenLimit = 4096
        );

        explicit LLMStrategyContext(
            const std::vector<maf::providers::ILLMProvider*>& providers,
            int maxTokens = 2048,
            int maxRetries = 3,
            int maxTokenLimit = 4096
        );

        std::shared_ptr<maf::providers::ILLMProvider> getProvider() const;
        bool switchToNextProvider();
        int getMaxTokens() const;
        int getDefaultMaxTokens() const;
        int getMaxRetries() const;
        int getMaxTokenLimit() const;
        void sleepForBackoff(int attempt) const;

        std::vector<std::string> getProviderNames() const;
        std::vector<std::string> getDefaultProviderNames() const;

        void applySelection(const std::vector<std::string>& ranking, int maxTokens);
    };

    struct StrategyDecision {
        std::string selectedProvider;
        std::vector<std::string> providerRanking;
        int maxTokens = 0;
        bool isSuccess = false;
        bool usedFallback = false;
        std::string errorMessage;
    };

    class Strategy {
    public:
        virtual ~Strategy() = default;
        virtual StrategyDecision decide(
            const maf::core::Task& task,
            LLMStrategyContext& context
        ) = 0;
    };

    class DefaultSelectionStrategy final : public Strategy {
    public:
        StrategyDecision decide(
            const maf::core::Task& task,
            LLMStrategyContext& context
        ) override;
    };

    class LLMSelectionStrategy final : public Strategy {
    private:
        maf::providers::ILLMProvider& bootstrapProvider_; // non-owning
        DefaultSelectionStrategy fallback_;

    public:
        explicit LLMSelectionStrategy(maf::providers::ILLMProvider& bootstrapProvider);

        StrategyDecision decide(
            const maf::core::Task& task,
            LLMStrategyContext& context
        ) override;
    };

    std::vector<std::shared_ptr<maf::providers::ILLMProvider>> borrowProviders(
        const std::vector<maf::providers::ILLMProvider*> providers
    );
}

#endif
