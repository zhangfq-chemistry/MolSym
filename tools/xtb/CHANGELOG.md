## xtb version 6.2.2

Maintenance, bugfix and hotfix release.

- configuration scripts are back in xtb
- pkg-config file is now available
- DFTB+ genFormat is supported as geometry input
- additional check to prevent calculations on invalid geometry input
- RF-Optimizer will not read hessian file by default anymore (now really)
- GFN0-xTB can now be correctly requested from the $gfn group
- Thermodynamic functions will not attempted to be calculated for T=0K anymore
- Bugfix: periodic GFN0-xTB used wrong i-i' D4 dispersion interactions

This release is API compatible to version 6.2.1
