#pragma once
#include <AgentModels/Agent.h>
#include <AgentModels/AgentCoreFactory.h>
#include <Engine/Estimator.h>
#include <Engine/SchedulerSettings.h>
#include <PluginModels/PluginInstanceFactory.h>
#include <Shared/Exceptions.h>

namespace OptiMA
{
    class MultiAgentModel
    {
        Estimator* estimator;
        SchedulerSettings* schSettings;
        vector<IAgentCoreFactory*> coreFactories;
        vector<IPluginInstanceFactory*> instanceFactories;
        vector<int> agentCoreIds;
        vector<int> pluginIds;
        vector<int> initialNumbers;
        vector<int> maximumNumbers;
        vector<PluginType> pluginTypes;
        vector<pair<int,int>> relationships;
        vector<pair<int,int>> communications;
        vector<pair<int,int>> pluginAccesses;
        vector<int> haltingAgents;
        vector<int> initialAgents;
        map<int,shared_ptr<Memory>> initialParameters;
        string keepStatsFilePath;
        string defaultEstimatorFilePath;
        double timeStep;
        int agentTemplateNumber;
        int pluginNumber;
        int totalMaxNumber;
        int threadNumber;
        int batchSize;
        bool schedulerSettingsAdded;
        bool estimatorAdded;
        bool timeCheck;
        bool numCheck;
        bool initialAgentsAdded;
        bool keepStats;
        bool defaultEstimator;
        
    public:

        MultiAgentModel();

        template <typename A, typename = std::enable_if_t<std::is_base_of<AgentTemplate<A>, A>::value>>
        void AddAgentTemplate(int templateId, int initialNumber, int maxNumber, bool startInTheBeginning)
        {
            for(int id : agentCoreIds)
            {
                if(id == templateId)
                {
                    throw InvalidModelParameterException((char*)"Duplicate template id");
                }
            }

            agentCoreIds.push_back(templateId);
            coreFactories.push_back(new AgentCoreFactory<A>());
            initialNumbers.push_back(initialNumber);
            maximumNumbers.push_back(maxNumber);
            totalMaxNumber += maxNumber;
            agentTemplateNumber++;

            if(startInTheBeginning)
            {
                initialAgents.push_back(templateId);
                initialParameters[templateId] = nullptr;
                initialAgentsAdded = true;
            }
        }

        template <typename A, typename = std::enable_if_t<std::is_base_of<AgentTemplate<A>, A>::value>>
        void AddAgentTemplate(int templateId, int initialNumber, int maxNumber, bool startInTheBeginning,
        shared_ptr<Memory>& initialParameters)
        {
            for(int id : agentCoreIds)
            {
                if(id == templateId)
                {
                    throw InvalidModelParameterException((char*)"Duplicate template id");
                }
            }

            agentCoreIds.push_back(templateId);
            coreFactories.push_back(new AgentCoreFactory<A>());
            initialNumbers.push_back(initialNumber);
            maximumNumbers.push_back(maxNumber);
            totalMaxNumber += maxNumber;
            agentTemplateNumber++;

            if(startInTheBeginning)
            {
                initialAgents.push_back(templateId);
                this->initialParameters[templateId] = initialParameters;
                initialAgentsAdded = true;
            }
        }

        void AddSupervisor(int supervisorTemplateId, int subordinateTemplateId);

        void AddCommunication(int templateId1, int templateId2);

        template <typename P, typename = std::enable_if_t<std::is_base_of<Plugin<P>, P>::value>>
        void AddPlugin(int pluginId, PluginType type)
        {
            for(int id : pluginIds)
            {
                if(id == pluginId)
                {
                    throw InvalidModelParameterException((char*)"Plugin id already exist");
                }            
            }

            pluginIds.push_back(pluginId);
            pluginTypes.push_back(type);
            instanceFactories.push_back(new PluginInstanceFactory<P>());
            pluginNumber++;
        }

        void AllowPluginUse(int agentTemplateId, int pluginId);

        void AddEstimator(Estimator* estimator);

        void AddHaltingAgent(int agentTemplateId);

        void AddSchedulerSettings(SchedulerSettings* settings);

        void SetThreadNumber(int threadNumber);

        void SetBatchSize(int batchSize);

        void SetTimeStep(double timeStep);

        void KeepStatsFile(string statsFilePath);

        void UseDefaultEstimator(string statsFilePath);

        ~MultiAgentModel();

        friend class Driver;
    };
}