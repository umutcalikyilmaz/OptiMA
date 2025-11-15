# OptiMA
THe optimum multi agent framework (OptiMA) is  a framework for developing transaction-based multi-agent systems, and efficiently executing them on a limited hardware. The user can create a system by inserting the agent roles, plugins and engine components to the framework. OptiMA also has the option to use transaction scheduling during execution to ensure an improvement on the throughput of the system.

## Citation
OptiMA is developed as a part of a study on the transaction scheduling problem and its use in transaction-based optimizing multi-agent systems. The results of the study is compiled in a research paper, which is pending approval for publication. The information for citation will be provided after the publication.

## Installation
OptiMA is designed for Debian and currently it is not compatible with Windows or MacOS. It uses the TxnSP software library (https://github.com/umutcalikyilmaz/TxnSP) for transaction scheduling. The user is not required ot install TxnSP manually, it is installed automatically during the installation of OptiMA. For standard installation, the following bash instructions should be executed in the project's root folder.

```bash
mkdir build
cd build
cmake ..
sudo make install
```

Mixed-integer programming solver module (MIPSolver) is not included in the standard installation and without it, the user cannot select to use mixed integer programming for transaction scheduling. To install with MIPSolver, first the SCIP Optimization Suite should be installed from https://www.scipopt.org/download.php?fname=scipoptsuite-8.0.2.tgz. Then the following bash insructions should be executed in the TxnSP project's root folder.


```bash
mkdir build
cd build
cmake -DENABLE_MIP=ON ..
sudo make install
```

## Usage
### Importing the Library
After installing OptiMA, it can be imported to a project by adding the following lines in the CMakeLists.txt file.

```cmake
find_package(TxnSP REQUIRED)
find_package(OptiMA REQUIRED)
target_link_libraries(my_project OptiMA::optima)
```

The library is included in the project using the code below.

```c++
#include <OptiMA/OptiMA.h>
```

The framework also includes a benchmark module, FactoryFloor, that simulates the production process in a fully optimized manufacturing facility. To include the FactoryFloor benchmark, the following code is used.

```c++
#include <OptiMA/FactoryFloor.h>
```

### Memory Class

OptiMA includes shared classes that are used by various modules of the framework. These are used for storing and transfering multimodal information. One of these classes is `OptiMA::Memory`. This class is able to store multiple `std::tuple` objects, each of which can have a different signature. Due to the flexibility it provides, `OptiMA::Memory` is used as the return type and parameter of many built-in and custom functions in OptiMA. These objects are managed using `std::shared_ptr` instead of being copied or using raw pointers for performance and safety issues. The code block below shows an example of creating a `OptiMA::Memory`, inserting and retrieveing two `std::tuple` objects into it.

```c++
// constructs a OptiMA::Memory and returns the std::shared_ptr to this object
std::shared_ptr<OptiMA::Memory> memory = OptiMA::generateMemory();     

// A std::tuple<int, double, bool> is created and inserted into memory
memory->addTuple(
    intValue,        //an example variable of integer type
    doubleValue,     //an example variable of double type
    boolValue        //an example variable of boolean type
);

// A std::tuple<std::string, double> is created and inserted into memory
memory->addTuple(
    stringValue,        //an example variable of std::string type
    doubleValue,     //an example variable of double type
);

// the first tuple is retrieved
std::tuple<int, double, bool> firstTuple = memory->getTuple<int, double, bool>(0);

// the second tuple is retrieved   
std::tuple<std::string, double> secondTuple = memory->getTuple<std::string, double>(1);
```



### Model Creation
#### Creating Plugins
The user can create their own plugin classes to be used by the agents during execution. This is done by deriving the template class Plugin provided by OptiMA. When creating a plugin class, the operate function must be overrided. This function is called by an agent to use the plugin during execution.

```c++
class MyPlugin : public OptiMA::Plugin<MyPlugin>
{
public:
    std::shared_ptr<OptiMA::Memory> operate(std::shared_ptr<OptiMA::Memory> inputParameters) override
    {
        // contents of the operate function
    }
};
```
#### Creating Agent Templates
In OptiMA, each agent has a specific role. These roles are defined by the user by creating agent templates. An agent role is created by overriding the `OptiMA::AgentTemplate` class. In these derived classes, the user can define member functions that can be invoked by a transaction during the execution. For a member function to be callable by a transaction, it has to have a return type of `std::shared_ptr<OptiMA::Memory>`, but there is no restirictions on the signature of the input parameters. An exemple agent template class is given below.

```c++
class MyAgentTemplate : public OptiMA::AgentTemplate<MyAgentTemplate>
{
public:
    std::shared_ptr<OptiMA::Memory> callableFunction1()
    {
        // contents of the callableFunction1
    }

    std::shared_ptr<OptiMA::Memory> callableFunction2(int input)
    {
        // contents of the callableFunction2
    }
};
```

#### Creating Transactions

The user is supposed to create the classes of transactions that are going to be used to create transaction objects during model executions. A transaction class is created by deriving the `OptiMA::Transaction` class. This class does not have a default constructor, so a user-defined transaction class is required to call one of the two constructors of `OptiMA::Transaction`. The purpose of this structure is to force the user to provide the necessary information when creating a transaction class, such as the type of the transaction, subtype of the transaction and the set of plugins to be used during the execution of the transaction.

When creating a transaction class, the `OptiMA::Transaction::procedure` function is required to be overrided, which defines the operation to be run during the execution of the transaction. The user also has the option to override the `OptiMA::Transaction::commitProcedure` and `OptiMA::Transaction::rollbackProcedure` functions that are executed when the transaction is commited and rolled back respectively. Two example transaction classes are shown below.

```c++
// a transaction class 
class MyTransaction : public OptiMA::Transaction
{
    // First constructor for the OptiMA::Transaction class is used
    MyTransaction(int transactionType, int transactionSubType, std::set<int> pluginSet) : OptiMA::Transaction(
        transactionType,        // type of the transaction (used by framework components during execution)
        transactionSubType,     // subtype of the transaction (used by framework components during execution)
        pluginSet,              // set of plugins used during execution (used for the locking process during execution)
    )
    {
        // contents of the constructor
    }

    // Second constructor for the OptiMA::Transaction class is used
    MyTransaction(std::vector<OptiMA::Agent*> agents, int transactionType, int transactionSubType, std::set<int> pluginSet) : OptiMA::Transaction(
        agents,                // vector of seized agents that can be used by the transaction
        transactionType,       // type of the transaction (used by framework components during execution)
        transactionSubType,    // subtype of the transaction (used by framework components during execution)
        pluginSet,             // set of plugins used during execution (used for the locking process during execution)
    )
    {
        // contents of the constructor
    }

    shared_ptr<Memory> procedure() override {
        // contents of the procedure function
    }

    void commitProcedure() {
        // contents of the commitProcedure function
    }

    void rollbackProcedure() {
        // contents of the rollbackProcedure function
    }
};
```
#### Creating Transaction Factory
Transaction factory is a component of the framework that creates new transactions after execution of a transaction, depending on its results. The inner workings of this module are required to be provided by the user, since they heavily depend on the design of a specific multi-agent system. A model-specific transaction factory class is derived from the `OptiMA::TransactionFactory` class. Two functions of the base class, `OptiMA::TransactionFactory::generateInitialTransactions` and `OptiMA::TransactionFactory::generateTransactions` must be overrided by the user. The first one is called in the beginning of the model execution to create the initial transactions, and the second is called after each transaction execution to create new transactions depending on the result.

```c++

class MyTransactionFactory : public OptiMA::TransactionFactory {

    std::vector<unique_ptr<OptiMA::ITransaction>> generateInitialTransactions() override {
        // contents of the generateInitialTransactions function
    }

    std::vector<unique_ptr<OptiMA::ITransaction>> generateTransactions(std::unique_ptr<OptiMA::ITransaction> txn, shared_ptr<OptiMA::TransactionResult> result) override {
        // contents of the generateTransactions function
    }    
};
```

#### Creating Estimator (Optional)
Estimator module is used to estimate the lengths of the transactions during model execution. The estimated lengths are used for the schedule optimization process. OptiMA includes a default estimator module that keeps the statistics for different transaction types and subtypes, and uses them for estimation. To do this, first the model should be executed without optimization to keep statistics, which can then used for an optimized execution.

The user is allowed to implement their own estimator, if they find performance of the default estimator insufficient. This is done by deriving a custom class from the `OptiMA::Estimator` class. The derived class is required to override the `estimateLength` function as shown below.

```c++

class MyEstimator : public OptiMA::Estimator {

    double estimateLength(const ITransaction& txn) override {
        // contents of the estimateLength function
    }   
};
```

#### Creating Multi-Agent Model
After the completion of the steps below, the latest step is to create a `OptiMA::MultiAgentModel` object and modifying it to represent the system to be executed. In this process, the user defined agent roles, plugins, transactions, transaction factory (and optinally estimator) are inserted into the model. The user is also expected to define the constraints of the models and set additional parameters related to model execution. Creation and modification of a `OptiMA::MultiAgentModel` object is demonstrated in the following code block.

```c++

OptiMA::MultiAgentModel model;

// Adding the custom plugins
model.addPlugin<MyPlugin1>(
    pluginId1,                       // int: A unique id that is used for reference the plugin during execution
    OptiMA::PluginType::SHAREABLE    // Marks that this transaction is shareable and does not require locking
);

model.addPlugin<MyPlugin2>(
    pluginId2,                            // int: A unique id that is used for reference during execution
    OptiMA::PluginType::NONSHAREABLE      // Marks that this transaction is nonshareable and require locking
);

// Adding the custom agent roles
model.addAgentTemplate<MyAgentTemplate>(
    roleId,            // int: A unique id that is used for reference the agent role during execution
    initialNumber,     // int: The initial number of agents having this role
    maximumNumber,     // int: The maximum number of agents having this role
    startingAgent      // bool: The boolean value showing if the agents of this role will be started at the beginning of the model execution
);

// Adding supervisor-subordinate relationship
model.addSupervisor(
    supervisorAgentRole,     // int: Id of the supervisor agent role
    subordinateAgentRole     // int: Id of the subordinate agent role
);

// Giving authorization for communication (the two agents roles are allowed to communicate with each other)
model.addCommunication(
    agentRole1,        // int: Id of the first agent role
    agentRole2         // int: Id of the second agent role
);

// Giving authorization to use a plugin to an agent role
model->allowPluginUse(
    agentRole,        // int: Id of the agent role with the authorization to use the plugin
    pluginId          // int: Id of the plugin that can be used by the indicated agent role
);

// Creating an object of a custom transaction factory class and assigning it to the model
MyTransactionFactory transactionFactory;
model.setTransactionFactory(&transactionFactory);

// Creating an object of a custom estimator class and assigning it to the model (optional)
// MyEstimator estimator;
// model->setEstimator(&estimator);

// Recording statistics for default estimator (when this option is selected, default estimator cannot be used)
// model->keepStatsFile(statsFilePath);

// Using default estimator (it uses the stats file created in another run of the model)
model.useDefaultEstimator(
    statsFilePath    // std::string: Path to the file containing the transaction statistics
);

model.setThreadNumber(
    threadNumber        // int: Number of threads to be used in execution
);

// Creating a OptiMA::SchedulerSettings object and inserting it to the model
OptiMA::SchedulerSettings schSettings;

// bool: Set true to enable schedule optimization during execution
schSettings.optimized = optimized;

// int: Size of the transaction batch in an optimized execution
schSettings.batchSize = batchSize;

// bool: Set true to enable thread trigger which starts scheduling when a thread is idle without checking batch size
schSettings.trigger = trigger;

// TxnSP::SolverType: Type of the optimizer used in an optimized execution
schSettings.optimizationMethod = optimization method

// TxnSP::SolutionType: If DPSolver is selected for optimization, this setting determines the solution type (exact or approximate)
schSettings.DP_SolutionType = DP_SolutionType;

// double: If SASolver is selected for optimization, this setting determines the maximum temperature value
schSettings.SA_MaxTemperature = SA_MaxTemperature;

// TxnSP::TemperatureEvolution: If SASolver is selected for optimization, this setting determines the temperature decrement type (linear, exponential or slow)
schSettings.SA_DecrementType ) SA_DecrementType;

// double: If SASolver is selected for optimization, this setting determines the decrement parameter
schSettings.SA_DecrementParameter = SA_DecrementParameter;

// Setting the scheduler settings of the model
model.setSchedulerSettings(&schSettings);
```

### Model Execution
After creating and modifying a model, the user can create an `OptiMA::Driver` object and execute the created model. The code below shows how to create a driver, execute a model and get the results of the execution.

```c++
OptiMA::Driver drv;
driver.startModel(model);
std::shared_ptr<OptiMA::Memory> result = drv.getOutputParameters();

```
