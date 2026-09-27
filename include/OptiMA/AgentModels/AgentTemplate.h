#pragma once
#include "OptiMA/AgentModels/Agent.h"

namespace OptiMA
{
    template <class A>
    class AgentTemplate : public Agent
    {
    public:

        virtual ~AgentTemplate() = default;

    protected:

        AgentTemplate() { }    

        std::shared_ptr<Memory> operatePlugin(int pluginId, std::shared_ptr<Memory> input)
        {
            PluginInstance* pi = pmanager_->seizePlugin(typename PluginManager::AgentKey {}, pluginId, agentType_);
            auto res = pi->operate(typename PluginInstance::AgentKey {}, input);
            pmanager_->releasePlugin(typename PluginManager::AgentKey {}, pi);
            return res;
        }

        std::shared_ptr<Memory> sendMessage(int agentId, std::shared_ptr<Message> msg)
        {
            std::shared_ptr<Memory> output = generateMemory();
            
            try
            {
                postmaster_->sendToId(typename Postmaster::AgentKey {}, currentTransaction_, agentId_, agentType_,
                    agentId, msg);
                output->addTuple(true);
            }
            catch(const std::exception e)
            {
                output->addTuple(false, e.what());
            }
            
            return output;
        }

        std::shared_ptr<Memory> sendMessageToAll(int agentType, std::shared_ptr<Message> msg)
        {
            std::shared_ptr<Memory> output = generateMemory();
            
            try
            {
                postmaster_->sendToType(typename Postmaster::AgentKey {}, currentTransaction_, agentId_,
                    agentType_, agentType, msg);
                output->addTuple(true);
            }
            catch(const std::exception e)
            {
                output->addTuple(false, e.what());
            }

            return output;
        }

        std::shared_ptr<Memory> getAgentInfoById(int agentId)
        {
            std::shared_ptr<Memory> output = generateMemory();

            try
            {
                output = amanager_->getAgentInfo(typename IAgentManager::AgentKey {}, agentId_, agentType_, agentId);
            }
            catch(const std::exception& e)
            {
                output->addTuple(false, (std::string)e.what);
            }

            return output;
        }

        std::shared_ptr<Memory> getAgentInfoByType(int agentType)
        {
            std::shared_ptr<Memory> output = generateMemory();

            try
            {
                output = amanager_->getAgentInfos(typename IAgentManager::AgentKey {}, agentId_, agentType_, agentType);
            }
            catch(const std::exception& e)
            {
                output->addTuple(false, (std::string)e.what());
            }
            
            return output;
        }

        std::shared_ptr<Memory> createAgent(int agentType)
        {
            std::shared_ptr<Memory> output = generateMemory();

            try
            {
                amanager_->requestCreateAgent(typename IAgentManager::AgentKey {}, currentTransaction_, agentId_, agentType_, agentType);
                output->addTuple(true);
            }
            catch(const std::exception& e)
            {
                output->addTuple(false, (std::string)e.what());
            }

            return output;
        }

        std::shared_ptr<Memory> createAndStartAgent(int agentType)
        {
            std::shared_ptr<Memory> output = generateMemory();

            try
            {
                amanager_->requestCreateAndStartAgent(typename IAgentManager::AgentKey {}, currentTransaction_, agentId_, agentType_, agentType);
                output->addTuple(true);
            }
            catch(const std::exception& e)
            {
                output->addTuple(false, (std::string)e.what());
            }

            return output;
        }

        std::shared_ptr<Memory> destroyAgent(int agentId)
        {
            std::shared_ptr<Memory> output = generateMemory();

            try
            {
                amanager_->requestDestroyAgent(typename IAgentManager::AgentKey {}, currentTransaction_, agentId_, agentType_, agentId);
                output->addTuple(true);
            }
            catch(const std::exception& e)
            {
                output->addTuple(false, (std::string)e.what());
            }
            
            return output;
        }

        std::shared_ptr<Memory> startAgent(int agentId)
        {
            std::shared_ptr<Memory> output = generateMemory();

            try
            {
                amanager_->requestStartAgent(typename IAgentManager::AgentKey {}, currentTransaction_, agentId_, agentType_, agentId);
                output->addTuple(true);
            }
            catch(const std::exception& e)
            {
                output->addTuple(false, (std::string)e.what());
            }            

            return output;
        }

        std::shared_ptr<Memory> stopAgent(int agentId)
        {
            std::shared_ptr<Memory> output = generateMemory();

            try
            {
                amanager_->requestStopAgent(typename IAgentManager::AgentKey {}, currentTransaction_, agentId_, agentType_, agentId);
                output->addTuple(true);
            }
            catch(const std::exception& e)
            {
                output->addTuple(false, (std::string)e.what());
            }
            
            return output;
        }    

        std::queue<std::shared_ptr<Message>> checkMessages()
        {
            return postBox_->checkMessages(typename PostBox::AgentKey {});
        }

        void clearMemory() override
        {
            static_cast<A*>(this)->clearMemory();
        }
    };
}