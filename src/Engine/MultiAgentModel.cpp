#include "OptiMA/Engine/MultiAgentModel.h"

namespace OptiMA
{
    MultiAgentModel::MultiAgentModel() : agentTemplateNumber_(0), pluginNumber_(0), totalMaxNumber_(0),
    schedulerSettingsAdded_(false), estimatorAdded_(false), numCheck_(false), initialAgentsAdded_(false),
    keepStats_(false), tfactorySet_(false) { }

    void MultiAgentModel::addSupervisor(int supervisorTemplateId, int subordinateTemplateId)
    {
        bool found1 = false;
        bool found2 = false;

        for(int id : agentCoreIds_)
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
            throw InvalidModelParameterException("Supervisor id does not exist");
        }

        if(!found2)
        {
            throw InvalidModelParameterException("Subordinate id does not exist");
        }

        relationships_.push_back(make_pair(supervisorTemplateId, subordinateTemplateId));
        communications_.push_back(make_pair(supervisorTemplateId, subordinateTemplateId));
    }

    void MultiAgentModel::addCommunication(int templateId1, int templateId2)
    {
        bool found1 = false;
        bool found2 = false;

        for(int id : agentCoreIds_)
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
            throw InvalidModelParameterException("One of the template ids does not exist");
        }

        communications_.push_back(make_pair(templateId1, templateId2));
    }

    void MultiAgentModel::setEstimator(Estimator* estimator)
    {
        if(estimatorAdded_)
        {
            throw InvalidModelParameterException("An estimator object has already been added");
        }

        estimatorAdded_ = true;
        defaultEstimator_ = false;
        this->estimator_ = estimator;
    }

    void MultiAgentModel::allowPluginUse(int agentTemplateId, int pluginId)
    {
        bool found = false;

        for(int id : agentCoreIds_)
        {
            if(id == agentTemplateId)
            {
                found = true;
                break;
            }
        }

        if(!found)
        {
            throw InvalidModelParameterException("Agent template id does not exist");
        }

        found = false;

        for(int id : pluginIds_)
        {
            if(id == pluginId)
            {
                found = true;
                break;
            }
        }

        if(!found)
        {
            throw InvalidModelParameterException("Plugin id does not exist");
        }

        pluginAccesses_.push_back(make_pair(agentTemplateId, pluginId));
    }

    void MultiAgentModel::setSchedulerSettings(SchedulerSettings* settings)
    {
        schSettings_ = settings;
        schedulerSettingsAdded_ = true;
    }

    void MultiAgentModel::setThreadNumber(int threadNumber)
    {
        threadNumber_ = threadNumber;
    }

    void MultiAgentModel::setTransactionFactory(TransactionFactory* tfactory)
    {
        tfactory_ = tfactory;
        tfactorySet_ = true;
    }

    void MultiAgentModel::setBatchSize(int batchSize)
    {
        numCheck_ = true;
        batchSize_ = batchSize;
    }

    void MultiAgentModel::setTrigger()
    {
        trigger_ = true;
    }

    void MultiAgentModel::keepStatsFile(string statsFilePath)
    {
        keepStats_ = true;
        keepStatsFilePath_ = statsFilePath;
    }

    void MultiAgentModel::useDefaultEstimator(string statsFilePath)
    {
        defaultEstimator_ = true;
        estimatorAdded_ = true;
        defaultEstimatorFilePath_ = statsFilePath;
    }

    MultiAgentModel::~MultiAgentModel()
    {

    }
}