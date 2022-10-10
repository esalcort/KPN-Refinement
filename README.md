# KPN-Refinement
A Kahn process architecture along with an operating system API and a bus model for refinement of KPN [1] models in SystemC [2].

# Content
* **HWBus.h**: Pin-accurate and transaction-level bus models in SystemC.
* **kpn_queue.h**: Queue model for SystemC
* **os_api.h**: Interface for operating system models in SystemC
* **kpn-arch/**
    - **- kpn-arch.cpp**: A sample KPN architecture
* **perf_estimation/**
    - **- gemm_nn.c**: An dummy matrix multiply code with variable input sizes M, N, and K
    - **- sample.csv**: A set of profiling results after running gemm_nn with different inputs
    - **- estimation_models.ipynb**: A jupyter notebook with examples to train ML models that predict data in sample.csv


# References
[1] Gilles, K. A. H. N. "The semantics of a simple language for parallel programming." Information processing 74 (1974): 471-475.  
[2] IEEE Standard SystemC Reference Manual (IEEE 16666-2011), https://standards.ieee.org/standard/1666-2011.html


# Contact
If you have a question or suggestion about this repository or a problem running examples, you can contact Susy at esalcort@utexas.edu.
