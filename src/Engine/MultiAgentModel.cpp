#include <Engine/MultiAgentModel.h>

namespace OptiMA
{
    MultiAgentModel::MultiAgentModel() : agentTemplateNumber(0), pluginNumber(0), totalMaxNumber(0),
    schedulerSettingsAdded(false), estimatorAdded(false), timeCheck(false), numCheck(false),
    initialAgentsAdded(false), keepStats(false) { }

    void MultiAgentModel::AddSupervisor(int supervisorTemplateId, int subordinateTemplateId)
    {
        bool found1 = false;
        bool found2 = false;

        for(int id : agentCoreIds)
        {
            if(id == supervisorTemplateId)
            {
                found1 = true;
            }

            if(id == subordinateTemplateId)
            {
                found2 = true;
            }
        }

        if(!found1)
        {
            throw InvalidModelParameterException((char*)"Supervisor id does not exist");
        }

        if(!found2)
        {
            throw InvalidModelParameterException((char*)"Subordinate id does not exist");
        }

        relationships.push_back(make_pair(supervisorTemplateId, subordinateTemplateId));
        communications.push_back(make_pair(supervisorTemplateId, subordinateTemplateId));
    }

    void MultiAgentModel::AddCommunication(int templateId1, int templateId2)
    {
        bool found1 = false;
        bool found2 = false;

        for(int id : agentCoreIds)
        {
            if(id == templateId1)
            {
                found1 = true;
            }

            if(id == templateId2)
            {
                found2 = true;
            }
        }

        if(!(found1 && found2))
        {
            throw InvalidModelParameterException((char*)"One of the template ids does not exist");
        }

        communications.push_back(make_pair(templateId1, templateId2));
    }

    void MultiAgentModel::AddEstimator(Estimator* estimator)
    {
        if(estimatorAdded)
        {
            throw InvalidModelParameterException((char*)"An estimator object has already been added");
        }

        estimatorAdded = true;
        defaultEstimator = false;
        this->estimator = estimator;
    }    

    void MultiAgentModel::AllowPluginUse(int agentTemplateId, int pluginId)
    {
        bool found = false;

        for(int id : agentCoreIds)
        {
            if(id == agentTemplateId)
            {
                found = true;
                break;
            }
        }

        if(!found)
        {
            throw InvalidModelParameterException((char*)"Agent template id does not exist");
        }

        found = false;

        for(int id : pluginIds)
        {
            if(id == pluginId)
            {
                found = true;
                break;
            }
        }

        if(!found)
        {
            throw InvalidModelParameterException((char*)"Plugin id does not exist");
        }

        pluginAccesses.push_back(make_pair(agentTemplateId, pluginId));
    }

    void MultiAgentModel::AddHaltingAgent(int agentTemplateId)
    {
        bool found = false;

        for(int id : agentCoreIds)
        {
            if(agentTemplateId == id)
            {
                found = true;
                break;
            }
        }

        if(!found)
        {
            throw InvalidModelParameterException((char*)"Agent template id does not exist");
        }

        found = false;

        for(int id : haltingAgents)
        {
            if(agentTemplateId == id)
            {
                found == true;
                break;
            }
        }

        if(!found)
        {
            haltingAgents.push_back(agentTemplateId);
        }
    }

    void MultiAgentModel::AddSchedulerSettings(SchedulerSettings* settings)
    {
        schSettings = settings;
        schedulerSettingsAdded = true;
    }

    void MultiAgentModel::SetThreadNumber(int threadNumber)
    {
        this->threadNumber = threadNumber;
    }

    void MultiAgentModel::SetBatchSize(int batchSize)
    {
        numCheck = true;
        this->batchSize = batchSize;
    }

    void MultiAgentModel::SetTimeStep(double timeStep)
    {
        timeCheck = true;
        this->timeStep = timeStep;
    }

    void MultiAgentModel::KeepStatsFile(string statsFilePath)
    {
        keepStats = true;
        keepStatsFilePath = statsFilePath;
    }

    void MultiAgentModel::UseDefaultEstimator(string statsFilePath)
    {
        defaultEstimator = true;
        estimatorAdded = true;
        defaultEstimatorFilePath = statsFilePath;
    }

    MultiAgentModel::~MultiAgentModel()
    {

    }
}