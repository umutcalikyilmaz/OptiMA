#pragma once
#include "OptiMA/AgentModels/Agent.h"

namespace OptiMA
{
    template <class A>
    class AgentTemplate : public Agent
    {
    public:

        shared_ptr<Memory> operatePlugin(int pluginId, shared_ptr<Memory> input)
        {
            PluginInstance* pi = pmanager_->seizePlugin(pluginId, agentType_);
            auto res = pi->operate(input);
            pmanager_->releasePlugin(pi);
            return res;
        }

        shared_ptr<Memory> sendMessage(int agentId, shared_ptr<Message> msg)
        {
            shared_ptr<Memory> output = generateMemory();
            
            try
            {
                postmaster_->sendToId(currentTransaction_, agentId_, agentType_, agentId, msg);
                output->addTuple(true);
            }
            catch(const exception e)
            {
                output->addTuple(false, e.what());
            }
            
            return output;
        }

        shared_ptr<Memory> sendMessageToAll(int agentType, shared_ptr<Message> msg)
        {
            shared_ptr<Memory> output = generateMemory();
            
            try
            {
                postmaster_->sendToType(currentTransaction_, agentId_, agentType_, agentType, msg);
                output->addTuple(true);
            }
            catch(const exception e)
            {
                output->addTuple(false, e.what());
            }

            return output;
        }

        shared_ptr<Memory> getAgentInfoById(int agentId)
        {
            shared_ptr<Memory> output = generateMemory();

            try
            {
                output = amanager_->getAgentInfo(agentId_, agentType_, agentId);
            }
            catch(const exception& e)
            {
                output->addTuple(false, (string)e.what);
            }

            return output;
        }

        shared_ptr<Memory> getAgentInfoByType(int agentType)
        {
            shared_ptr<Memory> output = generateMemory();

            try
            {
                output = amanager_->getAgentInfos(agentId_, agentType_, agentType);
            }
            catch(const exception& e)
            {
                output->addTuple(false, (string)e.what());
            }
            
            return output;
        }

        shared_ptr<Memory> createAgent(int agentType)
        {
            shared_ptr<Memory> output = generateMemory();

            try
            {
                amanager_->requestCreateAgent(currentTransaction_, agentId_, agentType_, agentType);
                output->addTuple(true);
            }
            catch(const exception& e)
            {
                output->addTuple(false, (string)e.what());
            }

            return output;
        }

        shared_ptr<Memory> createAndStartAgent(int agentType)
        {
            shared_ptr<Memory> output = generateMemory();

            try
            {
                amanager_->requestCreateAndStartAgent(currentTransaction_, agentId_, agentType_, agentType);
                output->addTuple(true);
            }
            catch(const exception& e)
            {
                output->addTuple(false, (string)e.what());
            }

            return output;
        }

        shared_ptr<Memory> destroyAgent(int agentId)
        {
            shared_ptr<Memory> output = generateMemory();

            try
            {
                amanager_->requestDestroyAgent(currentTransaction_, agentId_, agentType_, agentId);
                output->addTuple(true);
            }
            catch(const exception& e)
            {
                output->addTuple(false, (string)e.what());
            }
            
            return output;
        }

        shared_ptr<Memory> startAgent(int agentId)
        {
            shared_ptr<Memory> output = generateMemory();

            try
            {
                amanager_->requestStartAgent(currentTransaction_, agentId_, agentType_, agentId);
                output->addTuple(true);
            }
            catch(const exception& e)
            {
                output->addTuple(false, (string)e.what());
            }            

            return output;
        }

        shared_ptr<Memory> stopAgent(int agentId)
        {
            shared_ptr<Memory> output = generateMemory();

            try
            {
                amanager_->requestStopAgent(currentTransaction_, agentId_, agentType_, agentId);
                output->addTuple(true);
            }
            catch(const exception& e)
            {
                output->addTuple(false, (string)e.what());
            }
            
            return output;
        }    

        queue<shared_ptr<Message>> checkMessages()
        {
            return postBox_->checkMessages();
        }

        void clearMemory() override
        {
            static_cast<A*>(this)->clearMemory();
        }

        virtual ~AgentTemplate() = default;
    };
}