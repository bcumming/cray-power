cray-power
==========

Quick and dirty (but very accurate) energy measurement templates for Cray XC-30.

These rely on registers that are updated at 10 Hz, so while they are very accurate over longer periods of time, you won't have much joy doing fine-grained measurements with them.

To do measurements on the sub-second scale, you would want to also check the freshness counters and take enough samples (number of samples will depend on how small the time intervals you want to measure are).

Note that these measurements are per-node, so if you have multiple threads/ranks on a node, they are all going to measure the same value, and some scaling will be required.
