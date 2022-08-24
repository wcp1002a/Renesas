
# E0562320

客戶反應修改開始位址至 0xE000，則會出現 E0562320 錯誤

    E0562320:Section address overflowed out of range : ".const"

Address  | Section
---------|-----------
 0xE000  | .const
 &nbsp;  | .text
 &nbsp;  | .RLIB
 &nbsp;  | .SLIB
 &nbsp;  | .textf
 &nbsp;  | .constf
 &nbsp;  | .data
 &nbsp;  | .sdata
 0xFEA00 | .dataR
 &nbsp;  | .bss

但若是 0xD000，則沒問題

依據 [10.5.2 Fatal errors](http://tool-support.renesas.com/autoupdate/support/onlinehelp/csp/V4.01.00/CS+.chm/Compiler-CCRL.chm/Output/er_E_msglist.html)

E0562320 | [Message]       | Section address overflowed out of range : "section"
---------|-----------------|:---------------------------------------------
 &nbsp;  | [Explanation]   | The address of section exceeds the usable address range.
 &nbsp;  | [Action by User]|[V1.01 only] When a project is created in an integrated development environment, the dummy .const section which is to be allocated to the mirror area is generated in the startup routine that was created at project creation. Comment out the following lines in the startup routine.<br> .SECTION .constCONST <br> .L_section_const:

發生此錯誤是因為在啟動程式中(由CS+ IDE自動產生)，將分配給鏡像區域的虛擬 .const 部分。
當您分配的地址大於鏡像區域地址時，就會出現此錯誤。

客戶的晶片是R5F10BMG，鏡像區域是F2000H到F**DEFF**H, 如果起始地址大於 DEFFH，則出現 E0562320。

將 cstart.asm 中的兩行程式註解掉，則可避免此錯誤
```asm
    ;.SECTION .const, CONST
    ;.L_section_const:
```