# Environment Setup
## Install e2_studio
* unzip e2_studio+SC_20210427.zip
* Execute e2studio_installer-2021-07.R20210426-1052.exe
* Follow the instruction from section '6. How to install this product' of Readme_e_RX140_alpha_for_e2_studio_20210427.pdf
* Unzip FITModules.zip
* Copy FITModules folder to C:\Users\<user_name>\.eclipse\com.renesas.platform_download 

## Create Project
* Execute e2 studio
* File -> New -> Renesas C/C++ Project -> Renesas RX
* Select Renesas CC-RX C/C++ Executable Project
* Input project name, select Next
* Target Device: RX100 -> RX140 -> RX140 - 64pin -> R5F51403AxFM
* Configurations: check `Create Hardware Debug Configuration` and select `E2 Lite(RX)`, select Next
* check `Use Smart Configurator`, select Next
* Select Finish

## Configuration
* Open <project_name>.scfg
* Select tab `Components`
* remove the r_bsp components
* Add `Board Support Packages. (r_bsp)` manually
* Add components you want
* Select tab `Pins` to fix pin conflict problem

## Build & Debug
* Press icon `Hammer` to build the project
* Select icon `Bug` -> `Debug Configurations`
* Left pannel switch to Renesas GDB HardwareDebug -> <project_name>_HardwareDebug
* Right pannel switch to tab `Debugger`
* Confirm the Debug hardware is `E2 Lite (RX)`
* Switch Debugger sub tab to `Connection Settings`
* Check the Power settings
* Press `Debug`
