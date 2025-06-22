#pragma once
#include "OptiMA/AgentModels/Agent.h"
#include "OptiMA/AgentModels/AgentFactory.h"
#include "OptiMA/Engine/Estimator.h"
#include "OptiMA/Engine/SchedulerSettings.h"
#include "OptiMA/Engine/TransactionFactory.h"
#include "OptiMA/PluginModels/PluginInstanceFactory.h"
#include "OptiMA/Shared/Exceptions.h"

namespace OptiMA
{
    class MultiAgentModel
    {
    private:

        TransactionFactory* tfactory_;
        Estimator* estimator_;
        SchedulerSettings* schSettings_;
        vector<IAgentFactory*> agentFactories_;
        vector<IPluginInstanceFactory*> instanceFactories_;
        vector<int> agentCoreIds_;
        vector<int> pluginIds_;
        vector<int> initialNumbers_;
        vector<int> maximumNumbers_;
        vector<PluginType> pluginTypes_;
        vector<pair<int,int>> relationships_;
        vector<pair<int,int>> communications_;
        vector<pair<int,int>> pluginAccesses_;
        vector<int> initialAgents_;
        string keepStatsFilePath_;
        string defaultEstimatorFilePath_;
        int agentTemplateNumber_;
        int pluginNumber_;
        int totalMaxNumber_;
        int threadNumber_;
        int batchSize_;
        bool schedulerSettingsAdded_;
        bool estimatorAdded_;
        bool numCheck_;
        bool trigger_;
        bool initialAgentsAdded_;
        bool keepStats_;
        bool defaultEstimator_;
        bool tfactorySet_;
        
    public:

        MultiAgentModel();

        template <typename A, typename = std::enable_if_t<std::is_base_of<AgentTemplate<A>, A>::value>>
        void addAgentTemplate(int templateId, int initialNumber, int maxNumber, bool startInTheBeginning)
        {
            for(int id : agentCoreIds_)
            {
                if(id == templateId)
                {
                    throw InvalidModelParameterException((char*)"Duplicate template id");
                }
            }

            agentCoreIds_.push_back(templateId);
            agentFactories_.push_back(new AgentFactory<A>());
            initialNumbers_.push_back(initialNumber);
            maximumNumbers_.push_back(maxNumber);
            totalMaxNumber_ += maxNumber;
            agentTemplateNumber_++;

            if(startInTheBeginning)
            {
                initialAgents_.push_back(templateId);
                initialAgentsAdded_ = true;
            }
        }

        void addSupervisor(int supervisorTemplateId, int subordinateTemplateId);

        void addCommunication(int templateId1, int templateId2);

        template <typename P, typename = std::enable_if_t<std::is_base_of<Plugin<P>, P>::value>>
        void addPlugin(int pluginId, PluginType type)
        {
            for(int id : pluginIds_)
            {
                if(id == pluginId)
                {
                    throw InvalidModelParameterException((char*)"Plugin id already exist");
                }            
            }

            pluginIds_.push_back(pluginId);
            pluginTypes_.push_back(type);
            instanceFactories_.push_back(new PluginInstanceFactory<P>());
            pluginNumber_++;
        }

        void allowPluginUse(int agentTemplateId, int pluginId);

        void setEstimator(Estimator* estimator);

        void setSchedulerSettings(SchedulerSettings* settings);

        void setTransactionFactory(TransactionFactory* tfactory);

        void setThreadNumber(int threadNumber);

        void setBatchSize(int batchSize);

        void setTrigger();

        void keepStatsFile(string statsFilePath);

        void useDefaultEstimator(string statsFilePath);

        ~MultiAgentModel();

        friend class Driver;
    };
}