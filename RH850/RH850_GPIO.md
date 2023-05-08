RH850 GPIO
==========

* Section 2  Pin Function
* Pn_m: n 為 group, m 為腳位

Test Firmware

    C:\Users\a5134211\OneDrive - Renesas Electronics Corporation\work\0_Delta\General\40_Tool\EmbarkE2_Software\RH850_EMBARK_4.20_RepackwithApps\Embark_RH850_Software

### main functin
```
#define P8_11_H (PORT.P8 |= 0x0800)
#define P8_11_L (PORT.P8 &= ~0x0800)
```

##### PSFSC

```c
void R_PORT_Unlock(port_lock_t port_unlock)
{
    uint32 pwe;

    /* read PWE setting and update */
    pwe  = PORT[0].PWE.WORD;
    pwe |= port_unlock;

    /* unlock writes to PWE */
    PORT[0].PKCPROT.WORD = PORT_MASK_UNLOCK;

    /* store new setting in PWE register */
    PORT[0].PWE.WORD = pwe;

    /* re-lock PWE register */
    PORT[0].PKCPROT.WORD = PORT_MASK_LOCK;
}
```

##### PKCPORT
```
Port Keycode Protection register <PORT0_base> + 2F40 H  
B31-1 用來控制是否可以寫入PWEE

B0: PWEE
0: 不允許寫入 PWE
1: 允許寫入 PWE

A5A5 A500: 不允許寫入PWE
A5A5 A501: 允許寫入 PWE
```

##### PWE
```
Port Write enable register <PORT0_base> + 2F44 H
0: 不可寫入
1: 可以寫入


main(void)
└─ R_PRJ_SwInit()
```
