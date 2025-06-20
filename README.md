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

```
find_package(TxnSP REQUIRED)
find_package(OptiMA REQUIRED)
target_link_libraries(my_project OptiMA::optima)
```

The library is included in the project using the code below.

```
#include <OptiMA/OptiMA.h>
```

The framework also includes a benchmark module, FactoryFloor, that simulates the production process in a fully optimized manufacturing facility. To include the FactoryFloor benchmark, the following code is used.

```
#include <OptiMA/FactoryFloor.h>
```

### Model Creation
#### Creating Plugins
The user can create their own plugin classes to be used by the agents during execution. This is done by deriving the template class Plugin provided by OptiMA. When creating a plugin class, the operate function must be overrided. This function is called by an agent to use the plugin during execution.

```
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
In OptiMA, each agent has a specific role. These roles are defined by the user by creating agent templates. An agent template is created by overriding the AgentTemplate class. In these derived classes, the user can define member functions that can be invoked by a transaction during the execution. For a member function to be invokable, it has to have a specific output type, i.e std::shared_ptr<OptiMA::Memory>. 

```
class MyPlugin : public OptiMA::Plugin<MyPlugin>
{
public:
    std::shared_ptr<OptiMA::Memory> operate(std::shared_ptr<OptiMA::Memory> inputParameters) override
    {
        // contents of the operate function
    }
};
```

#### Creating Transactions
#### Creating Transaction Factory
#### Creating Estimator (Optional)
#### Creating Multi-Agent Model

### Model Execution
