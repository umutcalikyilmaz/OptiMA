# OptiMA
OptiMA is a framework for designing and executing transaction-based multi-agent systems. The framework applies strict locking procedures to ensure a high level of isolation and consistency. It uses transaction schedule optimization to mitigate the performance drawbacks that might be caused by these strict procedures.

## Citation
OptiMA is developed as part of a study on the transaction scheduling problem and its use in transaction-based optimizing multi-agent systems. The study is presented in the paper given below.

[OptiMA: A Transaction-Based Framework with Throughput Optimization for Very Complex Multi-Agent Systems](https://arxiv.org/pdf/2511.03761)

### Version Used in the Paper
The experiments reported in the paper were conducted using **OptiMA v1.0**. The exact version of the source code used for those experiments is archived here:

**[OptiMA v1.0](https://github.com/umutcalikyilmaz/OptiMA/tree/v1.0)**

## Installation
OptiMA requires a compiler with `C++20` support. It is designed for Debian-based systems and is currently not compatible with Windows or macOS. It uses the **[TxnSP Software Library v1.1](https://github.com/umutcalikyilmaz/TxnSP/tree/v1.1)** for transaction scheduling, which is fetched automatically from GitHub during CMake configuration and built as part of OptiMA. For a standard installation, execute the following commands in the project's root directory:

```bash
mkdir build
cd build
cmake ..
sudo make install
```

The `MIPSolver` module, which is a part of **TxnSP**, is not included in the standard installation. To install the OptiMA library with `MIPSolver`, the **[SCIP Optimization Suite](https://www.scipopt.org/download.php?fname=scipoptsuite-8.0.2.tgz)** must first be installed. Then, execute the following commands in the project's root directory:

```bash
mkdir build
cd build
cmake -DENABLE_MIP=ON ..
sudo make install
```

## Usage
### Importing the Library
After installing OptiMA, it can be imported into a project by adding the following lines in the CMakeLists.txt file.

```cmake
find_package(OptiMA REQUIRED)
target_link_libraries(my_project OptiMA::optima)
```

The library is included in the project using the code below.

```c++
#include <OptiMA/OptiMA.h>
```

The framework also includes a benchmark module, FactoryFloor, that simulates the production process in a fully automated manufacturing facility. Use the following code to include the FactoryFloor benchmark.

```c++
#include <OptiMA/FactoryFloor.h>
```

### Memory Class

OptiMA includes the shared class `Memory` that is used by various modules of the framework for storing and transferring multimodal information. A `Memory` instance is able to store multiple `std::tuple` objects, each of which can have a different signature. Due to the flexibility it provides, `Memory` is used as the return type and parameter of many built-in and custom functions in OptiMA. The code block below shows an example of creating a `Memory` instance, and inserting and retrieving two `std::tuple` objects into it.

```c++
// constructs a Memory object and returns std::shared_ptr<Memory> type
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
    doubleValue         //an example variable of double type
);

// the first tuple is retrieved
std::tuple<int, double, bool> firstTuple = memory->getTuple<int, double, bool>(0);

// the second tuple is retrieved   
std::tuple<std::string, double> secondTuple = memory->getTuple<std::string, double>(1);
```

### Model Design
The process of designing a model in OptiMA includes the following steps:
- Designing agent templates to determine the capabilities of different agent types,
- Designing plugins that are used by agents as additional tools,
- Defining the relationships between different agent types and plugins using model constraints,
- Designing transaction templates which are used to generate transactions with intended functionalities at runtime,
- Customizing several modules to control execution-level details of the model.

Please refer to the associated paper for a more detailed understanding of the general logic and principles of model design in OptiMA. In the following, the necessary steps for designing a system using the OptiMA framework are given.

#### Creating Plugins
To create plugin classes, the template class `Plugin` must be derived. When creating a plugin class, the operate function must be overridden. This function is called by an agent to use the plugin during execution.

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
In OptiMA, each agent has a specific role. These roles are defined by the user by creating agent templates. An agent role is created by deriving a class from the `AgentTemplate` class. In these derived classes, the user can define member functions that can be invoked by transactions during the execution. For a member function to be callable by a transaction, it must have a return type of `std::shared_ptr<OptiMA::Memory>`, but there are no restrictions on the signature of the input parameters. An example agent template class is given below.

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

Every process in OptiMA is enclosed in a transaction for high-level isolation and consistency. A transaction can perform complex operations that include actions from multiple agents. At runtime, transactions are generated from transaction templates included in the model design. This can be done by creating a class derived from the `Transaction` class. `Transaction` does not have a default constructor, so a user-defined transaction class is required to call one of the two constructors of `OptiMA::Transaction`. The purpose of this structure is to force the user to provide the necessary information when creating a transaction class, such as the type of the transaction, subtype of the transaction and the set of plugins to be used during the execution of the transaction.

When creating a transaction class, the `Transaction::procedure` function is also required to be overridden, which defines the operation to be run during the execution of the transaction. The user also has the option to override the `Transaction::commitProcedure` and `Transaction::rollbackProcedure` functions, which are executed when the transaction is committed and rolled back respectively. An example transaction class using the two available constructors is shown below.

```c++
// a transaction class 
class MyTransaction : public OptiMA::Transaction
{
public:

    // First constructor for the OptiMA::Transaction class is used
    MyTransaction(int transactionType, int transactionSubType, std::set<int> pluginSet)
        : OptiMA::Transaction(
            transactionType,        // type of the transaction (used by framework components during execution)
            transactionSubType,     // subtype of the transaction (used by framework components during execution)
            pluginSet               // set of plugins used during execution (used for the locking process during execution)
          )
    {
        // contents of the constructor
    }

    // Second constructor for the OptiMA::Transaction class is used
    MyTransaction(std::vector<OptiMA::Agent*> agents, int transactionType, int transactionSubType, std::set<int> pluginSet)
        : OptiMA::Transaction(
            agents,                // std::vector of seized agents that can be used by the transaction
            transactionType,       // type of the transaction (used by framework components during execution)
            transactionSubType,    // subtype of the transaction (used by framework components during execution)
            pluginSet              // set of plugins used during execution (used for the locking process during execution)
          )
    {
        // contents of the constructor
    }

    std::shared_ptr<OptiMA::Memory> procedure() override {
        // contents of the procedure function
    }

    void commitProcedure() override
    {
        // contents of the commitProcedure function
    }

    void rollbackProcedure() override
    {
        // contents of the rollbackProcedure function
    }
};
```
#### Creating Transaction Factory
Transaction factory is a component of the framework that creates new transactions after execution of a transaction, depending on its results. The inner workings of this module are required to be provided by the user, since they heavily depend on the design of a specific multi-agent system. A model-specific transaction factory class is derived from the `TransactionFactory` class. Two functions of the base class, `TransactionFactory::generateInitialTransactions` and `TransactionFactory::generateTransactions` must be overridden by the user. The first one is called in the beginning of the model execution to create the initial transactions, and the second is called after each transaction execution to create new transactions depending on the result.

```c++

class MyTransactionFactory : public OptiMA::TransactionFactory
{
public:

    std::vector<std::unique_ptr<OptiMA::ITransaction>> generateInitialTransactions() override
    {
        // contents of the generateInitialTransactions function
    }

    std::vector<std::unique_ptr<OptiMA::ITransaction>> generateTransactions(std::unique_ptr<OptiMA::ITransaction> txn, std::shared_ptr<OptiMA::TransactionResult> result) override
    {
        // contents of the generateTransactions function
    }    
};
```

#### Creating Estimator (Optional)
Estimator module is used to estimate the lengths of the transactions during model execution. The estimated lengths are used for the schedule optimization process. OptiMA includes a default estimator module that keeps the statistics for different transaction types and subtypes, and uses them for estimation. To do this, first the model should be executed without optimization to keep statistics, which can then be used for an optimized execution.

The user is allowed to implement their own estimator, if they find performance of the default estimator insufficient. This is done by deriving a custom class from the `Estimator` class. The derived class is required to override the `estimateLength` function as shown below.

```c++

class MyEstimator : public OptiMA::Estimator
{
public:

    double estimateLength(const OptiMA::ITransaction& txn) override
    {
        // contents of the estimateLength function
    }   
};
```

#### Creating Multi-Agent Model
The final step of model design is to create a `MultiAgentModel` object and modify it to represent the system to be executed. In this process, the user-defined agent roles, plugins, transactions, transaction factory (and optionally estimator) are inserted into the model. The user is also expected to define the constraints of the models and set additional parameters related to model execution. Creation and modification of a `MultiAgentModel` object is demonstrated in the following code block.

```c++

OptiMA::MultiAgentModel model;

// Adding the custom plugins
model.addPlugin<MyPlugin1>(
    pluginId1,                       // int: A unique id that is used for reference the plugin during execution
    OptiMA::PluginType::SHAREABLE    // Marks that this plugin is shareable and does not require locking
);

model.addPlugin<MyPlugin2>(
    pluginId2,                            // int: A unique id that is used for reference during execution
    OptiMA::PluginType::NONSHAREABLE      // Marks that this plugin is non-shareable and requires locking
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
model.allowPluginUse(
    agentRole,        // int: Id of the agent role with the authorization to use the plugin
    pluginId          // int: Id of the plugin that can be used by the indicated agent role
);

// Creating an object of a custom transaction factory class and assigning a pointer to it to the model
MyTransactionFactory transactionFactory;
model.setTransactionFactory(&transactionFactory);

// Creating an object of a custom estimator class and assigning it to the model (optional)
// MyEstimator estimator;
// model.setEstimator(&estimator);

// Recording statistics for default estimator (when this option is not selected, default estimator cannot be used)
// model.keepStatsFile(statsFilePath);

// Using default estimator (it uses the stats file created in another run of the model)
model.useDefaultEstimator(
    statsFilePath    // std::string: Path to the file containing the transaction statistics
);

// Setting the number of threads to be used in execution
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
schSettings.optimizationMethod = optimizationMethod;

// TxnSP::SolutionType: If DPSolver is selected for optimization, this setting determines the solution type (exact or approximate)
schSettings.DP_SolutionType = DP_SolutionType;

// double: If SASolver is selected for optimization, this setting determines the maximum temperature value
schSettings.SA_MaxTemperature = SA_MaxTemperature;

// TxnSP::TemperatureEvolution: If SASolver is selected for optimization, this setting determines the temperature decrement type (linear, exponential or slow)
schSettings.SA_DecrementType = SA_DecrementType;

// double: If SASolver is selected for optimization, this setting determines the decrement parameter
schSettings.SA_DecrementParameter = SA_DecrementParameter;

// Setting the scheduler settings of the model
model.setSchedulerSettings(&schSettings);
```

### Model Execution
After creating and modifying a model, the user can create an `OptiMA::Driver` object and execute the created model. The code below shows how to create a driver, execute a model and get the results of the execution.

```c++
OptiMA::Driver driver;
driver.startModel(model);
std::shared_ptr<OptiMA::Memory> result = driver.getOutputParameters();
```
The result can only be obtained after the execution is terminated by an authorized agent.
