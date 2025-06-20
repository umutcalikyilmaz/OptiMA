# OptiMA
OptiMA is a framework for developing transaction-based multi-agent systems, and efficiently executing them on a limited hardware. The user can create a system by inserting the agent roles, plugins and engine components to the framework. OptiMA also has the option to use transaction scheduling during execution to ensure an improvement on the throughput of the system.

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

