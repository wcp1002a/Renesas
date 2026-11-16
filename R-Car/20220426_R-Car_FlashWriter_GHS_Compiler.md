20220426_R-Car_FlashWriter_GHS_Compiler
=======================================
`20261116 update`

Reference "Section 4. How to build the Flash writer" of below

    C:\Renesas\rcar-xos\v2.17.0\docs\sw\boot\flash_writer\RENESAS_RCV3HV3U_ICUMXA_Flash_Writer_Application_Note_v1.17a.pdf

Install GHS 
----------------------------------------
[GHS Package for Renesas CPA User](http://www.adac.co.jp/support/ind/relcpa/index.html)

* ID:`relcpauser`
* PW:`relcpa3799`

Enable GHS network license
----------------------------------------
Follow instructions of [GHS internet license](https://jira-gasg.renesas.eu/confluence/pages/viewpage.action?spaceKey=TPS&title=Integrity+Installation+Guide)

* `Utilities` ➜ `License Administrator`
* License my MULTI products (recommended for end users)
* Specify the network location of License Managers

Input below information
```sh
    @duesvn2.ad.ree.renesas.com
```
__Note__: 需要連VPN才可使用歐洲的license

Help -> License Info.

Execute cygwin64 Terminal
Add Path
```sh
export PATH=$PATH:"/cygdrive/c/ghs/V800.V2015.1.7/comp_201517/"
```
make


#### Make
清除
```sh
    make clean
```
加上 cxrh850.exe 的目錄
```sh
    export PATH=$PATH:"/cygdrive/c/ghs/V800.V2015.1.7/comp_201517/"
```


 

