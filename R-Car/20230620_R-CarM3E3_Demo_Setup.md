20230620_R-CarM3E3_Demo_Setup
=============================
[R-Car eLinux](https://elinux.org/R-Car/Boards/Yocto-Gen3/v3.21.0#Loading_kernel_and_rootfs_via_eMMC.2FSD_card)

Guideline to setup
------------------
#### Prepare eMMC card
Use fdisk to prepare 1 partition on eMMC card

    #fdisk /dev/mmcblk0
    #n
    #p
    #1
    #enter
    #enter
    #w

Use mkfs.ext4 to format the partition (或使用 ubuntu 的 Disk 工具來 Format)

    #mkfs.ext4 /dev/mmcblk0p1

Mount the ext4 partition to a folder

    #mount /dev/mmcblk0p1 /mnt

Extract the tarball root file system to the SD card

    #cd /mnt
    #tar vxf rootfs_QtPoC_fastboot.tar.gz
    #sync
    #cd ~
    #umount /mnt

Clear uBoot settings

    env default -a

### Mode (Jumper)
Jumper設定參考以下文件

    R-Car_Technical_document_for_Internal\R-Car Gen3\Gen3_Gen3e_Common\Design_Guide\02_BOARD\07_Ebisu\all\4D(512MB_x4)\RTP0RC77990SEB0020S_cd_1.03\manual(English)\e_Ebisu-4D_suman_rev101_0719.pdf

Table 2.1.11 Selection of Boot Device (MD4, MD3, MD2, and MD1)

    HyperFlash switch Settings:
    SW3 ON
    SW31 ON
    SW1 All Off
    SW2 All Off
    SW10 ON OFF OFF ON ON ON OFF ON

 PIN |   1  |   2  |   3  |   4  |   5  |   6  |   7  |  8   |
-----|------|------|------|------|------|------|------|------|
  MD |  MD7 | MD6  | MD19 | MD17 |  MD4 |  MD3 |  MD2 |  MD1 |
  SW | ON(0)|OFF(1)|OFF(1)| ON(0)| ON(0)| ON(0)|OFF(1)| ON(0)|

HyperFlash ROM boot at 150 MHz (300 Mbps) using DMA, Booting is from the HyperFlash memory (U86) on the Ebisu-4D)board. 

##### SCIF download mode

由 HyperFlash -> SCIF download mode

    SW10[5:8] = OFF OFF OFF OFF
切回 HyperFlash, 由 HyperFlash -> SCIF download mode

    SW10[5:8] = ON ON OFF ON

Demonstration Setup
-------------------
### 1. E3 210408_PoC_DISCOM_DOC_2D_3D
設置，顯示由 Dual LVDS (CN18及CN38)輸出，Jumper設置如下：

    SW17: ON
    SW48: ON
    SW44: All FF
    SW45: ON
    SW47: OFF
    
IPL

    Hyper-AutoOrSelectTransfer_UltraSpeed_E3.ttl

rootfs 檔案

    sudo tar xvf rfs_docdis_mod_cluster.tar.gz

rootfs存放於SD插槽

    setenv bootargs 'rw root=/dev/mmcblk1p1 rootfstype=ext4 rootwait'
    setenv bootcmd 'ext4load mmc 0:1 0x48080000 /boot/Image;ext4load mmc 0:1 0x48000000 /boot/r8a77990-ebisu-4d_cr7_rproc.dtb;booti 0x48080000 - 0x48000000'
    saveenv

rootfs存放於TF插槽

    setenv bootargs 'rw root=/dev/mmcblk2p1 rootwait'
    setenv bootcmd 'ext4load mmc 1:1 0x48080000 boot/Image;ext4load mmc 1:1 0x48000000 boot/r8a77990-ebisu-4d_cr7_rproc.dtb;booti 0x48080000 - 0x48000000'
    


### 2. E3 20230203_PoC_CR7_Demo
IPL

    Hyper-AutoOrSelectTransfer_UltraSpeed.ttl

rootfs 檔案

    sudo tar xvf rootfs_QtPoC_fastboot.tar.gz

目前似乎只能放在eMMC (dual_rendering-E3-safertos.srec測試 ok)

    setenv bootargs 'rw root=/dev/mmcblk0p1 rootwait'

    setenv bootcmd 'ext4load mmc 2:1 0x48080000 /boot/Image;ext4load mmc 2:1 0x48000000 /boot/r8a77990-ebisu-4d_cr7_rproc.dtb;booti 0x48080000 - 0x48000000'

TF (eMMC有儲存狀況下)

    setenv bootargs 'rw root=/dev/mmcblk0p1 rootwait'
    setenv bootcmd 'ext4load mmc 1:1 0x48080000 /boot/Image;ext4load mmc 1:1 0x48000000 /boot/r8a77990-ebisu-4d_cr7_rproc.dtb;booti 0x48080000 - 0x48000000'
    saveenv

需注意，若已燒錄 safertos時，在解壓縮 rootfs 至 eMMC 時，常會有錯誤造成中斷。

### 3. ebisu_4D_AndroidAuto_Cable_PoC_20230224 Setup
IPL

    Hyper-AutoOrSelectTransfer_UltraSpeed_E3_4D_CR7.ttl

Image: Follow img/230207_guideline_fastboot_android.txt
Host PC

    sudo ./flash_all.sh
    sudo ./linux-x86/bin/fastboot oem format
uBoot

    fastboot
### 4. M3 20230515_M3e_New_demo_by_switches
IPL 直接運行 ttl (包含燒錄 MiniMonitor)  
`Hyper-AutoOrSelectTransfer_UltraSpeed_M3.ttl`

    ====================
    SERIAL DOWNLOAD MODE
    ====================
    SW10: ON OFF OFF OFF OFF OFF OFF OFF
    ==> Boot from C53, DDR1600, SCIF download mode


    ====================
    Hyper Flash Mode
    ====================
    SW10: ON OFF OFF OFF ON ON OFF ON
    ==> Boot from C53, DDR1600, boot from HyperFlash

TFTP 設定

    setenv ethaddr '2e:09:0a:06:d5:ba'
    setenv ipaddr '192.168.2.3'
    setenv serverip '192.168.2.1'
    setenv netmask '255.255.255.0'


    setenv bootcmd 'tftp 0x48080000 Image;tftp 0x48000000 r8a7796-salvator-xs-2x4g_cr7_rproc_lvds.dtb;booti 0x48080000 - 0x48000000'
    setenv bootargs 'consoleblank=0 console=ttySC0,115200 rw root=/dev/nfs nfsroot=192.168.2.1:/home/p/rcar/m3/rfs,nfsvers=3 ip=192.168.2.3'

    saveenv

SD0 設定

    setenv bootargs 'consoleblank=0 console=ttySC0,115200 rw root=/dev/mmcblk1p1 rootwait'
    setenv bootcmd 'ext4load mmc 0:1 0x48080000 boot/Image;ext4load mmc 0:1 0x48000000 boot/r8a7796-salvator-xs-2x4g_cr7_rproc.dtb;booti 0x48080000 - 0x48000000'
    saveenv

SD2 設定

    setenv bootargs 'consoleblank=0 console=ttySC0,115200 rw root=/dev/mmcblk2p1 rootfstype=ext4 rootwait'
    setenv bootcmd 'ext4load mmc 2:1 0x48080000 /boot/Image;ext4load mmc 2:1 0x48000000 /boot/r8a7796-salvator-xs-2x4g_cr7_rproc_lvds.dtb;booti 0x48080000 - 0x48000000'
    saveenv

### E3 Ebisu BSP
    sudo apt-get install gawk wget git-core diffstat unzip texinfo gcc-multilib build-essential chrpath socat cpio python3 python3-pip python3-pexpect xz-utils debianutils iputils-ping python3-git python3-jinja2 libegl1-mesa libsdl1.2-dev pylint3 xterm

    git clone git://git.yoctoproject.org/poky
    git clone git://git.openembedded.org/meta-openembedded 
    git clone https://github.com/renesas-rcar/meta-renesas

    $ cd ${WORK}/poky 
    $ git checkout -b tmp 74b22db6879b388d700f61e08cb3f239cf940d18 
     
    $ cd ${WORK}/meta-openembedded 
    $ git checkout -b tmp 814eec96c2a29172da57a425a3609f8b6fcc6afe 
     
    $ cd ${WORK}/meta-renesas 
    $ git checkout -b tmp 13fd24957b9acc29a235ee0c7f398fd867f38b47 

這應該不能用了

    git clone git://github.com/renesas-rcar/meta-renesas.git


rootfs 檔案

    sudo tar xvf core-image-minimal-ebisu.tar.bz2

要支援 mkfs.ext4

    $ source poky/oe-init-build-env
    $ bitbake e2fsprogs

mkfs.ext4 命令會存在以下目錄，將目錄下所有的目錄複制到rootbs的目錄下即可

    ~/build/tmp/work/aarch64-poky-linux/e2fsprogs/1.45.4-r0/image$

rootfs存放於TF插槽

    setenv bootargs 'rw root=/dev/mmcblk2p1 rootwait'
    setenv bootcmd 'ext4load mmc 1:1 0x48080000 boot/Image;ext4load mmc 1:1 0x48000000 boot/r8a77990-ebisu-4d.dtb;booti 0x48080000 - 0x48000000'

## Appendix
### 錯誤排除
##### M3 開機發生錯誤
發生以下錯誤時，嚐試將 SW7 切到 Pin1

    >[    0.000165] NOTICE:  BL2: R-Car M3 Initial Program Loader(CA53)
    [    0.004614] NOTICE:  BL2: Initial Program Loader(Rev.2.0.6)
    [    0.010156] NOTICE:  BL2: PRR is R-Car M3 Ver.3.0
    [    0.014831] NOTICE:  BL2: Board is Salvator-XS Rev.1.0
    [    0.019951] NOTICE:  BL2: Boot device is HyperFlash(160MHz)
    [    0.025473] NOTICE:  BL2: LCM state is CM
    [    0.029520] NOTICE:  BL2: AVS setting succeeded. DVFS_SetVID=0x53
    [    0.035543] NOTICE:  BL2: DDR1600(rev.0.40)
    [    0.046276] NOTICE:  BL2: [WARM_BOOT]
    [    0.048953] ERROR:
    Warm booting...
     The potential of BKUP_TRG did not switch to Low.
     If you expect the operation of cold boot,
     check the board configuration (ex, Dip-SW) and/or the H/W failure.
    [    0.066369] NOTICE:  BL2: [BOOT_STATUS_UPDATE_ERROR]
    [    0.071304] NOTICE:  BL2: Failed to DRAM initialize (-1).
    [    5.007801] ERROR:
    [    5.008596] ERROR:   BL2: System WDT overflow, occured address is 0xe6304780


### Settings
##### Current setting (from Garmin)
    baudrate=115200
    bootargs=video=HDMI-A-1:704x704-32@30 androidboot.display.res.HDMI1=704x704-32@30
    bootcmd=boota 2 boot 0x4C000000
    bootdelay=3
    bootmode=android
    fdt_high=0xffffffffffffffff
    fdtcontroladdr=b5eff750
    filesize=42c19c
    initrd_high=0xffffffffffffffff
    loadaddr=0x58000000
    mem=1933312k
    platform=r8a77990
    product=ebisu
    recovery=setenv bootmode recovery; run bootcmd;
    rtc_time=1681370557
    serialno=0000
    stderr=serial@e6e88000
    stdin=serial@e6e88000
    stdout=serial@e6e88000
    use_usb_wifi=0
    ver=U-Boot 2020.10 (Dec 12 2022 - 04:20:45 +0000)
    
    Environment size: 539/131068 bytes
    =>


    setenv bootargs 'video=HDMI-A-1:704x704-32@30 androidboot.display.res.HDMI1=704x704-32@30'
    setenv bootcmd 'boota 2 boot 0x4C000000'
    setenv bootdelay '3'
    setenv bootmode 'android'

##### working
```
    setenv bootcmd 'ext4load mmc 0:1 0x48080000 /boot/Image;ext4load mmc 2:1 0x48000000 /boot/r8a77990-ebisu-4d_cr7_rproc.dtb;booti 0x48080000 - 0x48000000'

    setenv bootcmd 'ext4load mmc 1:0 0x48080000 /boot/Image;ext4load mmc 2:1 0x48000000 /boot/r8a77990-ebisu-4d_cr7_rproc.dtb;booti 0x48080000 - 0x48000000'


    https://elinux.org/R-Car/Boards/Yocto-Gen3/v3.21.0#Loading_kernel_and_rootfs_via_eMMC.2FSD_card

    setenv dtb 'r8a77990-ebisu-4d_cr7_rproc.dtb'
    setenv booti_cmd 'booti 0x48080000 - 0x48000000'
    setenv bootargs_tf 'rw root=/dev/mmcblk1p0 rootwait'
    setenv bootcmd_tf 'setenv bootargs ${bootargs_tf};ext4load mmc 1:0 0x48080000 /boot/Image;ext2load mmc 1:1 0x48000000 /boot/${dtb};run booti_cmd'
    setenv bootargs 'rw root=/dev/mmcblk1p0 rootwait'
    setenv bootcmd 'ext4load mmc 1:0 0x48080000 /boot/Image;ext4load mmc 1:1 0x48000000 /boot/r8a77990-ebisu-4d_cr7_rproc.dtb;booti 0x48080000 - 0x48000000'

    setenv bootcmd 'ext4load mmc 1:1 0x48080000 /boot/Image;ext4load mmc 1:1 0x48000000 /boot/r8a77990-ebisu-4d_cr7_rproc.dtb;booti 0x48080000 - 0x48000000'
    setenv bootargs 'console=null rw rootwait quiet root=/dev/mmcblk0p1 rootfs=ext4 noinitrd selinux=0'


    setenv bootcmd 'ext4load mmc 0:1 0x48080000 /boot/Image;ext4load mmc 0:1 0x48000000 /boot/r8a77990-ebisu-4d_cr7_rproc.dtb;booti 0x48080000 - 0x48000000'
    setenv bootargs 'console=null rw rootwait quiet root=/dev/mmcblk0p1 rootfs=ext4 noinitrd selinux=0'
    
    setenv bootcmd 'ext4load mmc 1:1 0x48080000 /boot/Image;ext4load mmc 1:1 0x48000000 /boot/r8a77990-ebisu-4d_cr7_rproc.dtb;booti 0x48080000 - 0x48000000'
    setenv bootargs 'rw root=/dev/mmcblk1p1 rootwait rootfs=ext4'
    
    
    setenv bootcmd 'ext4load mmc 0:1 0x48080000 /boot/Image;ext4load mmc 0:1 0x48000000 /boot/r8a77990-ebisu-4d_cr7_rproc.dtb;booti 0x48080000 - 0x48000000'
    setenv bootargs 'rw root=/dev/mmcblk0p1 rootwait rootfs=ext4'
    
    
    Slash's parameter
    setenv bootargs 'root=/dev/mmcblk0p1 rootwait'
    setenv bootcmd 'ext4load mmc 0:1 0x48080000 boot/Image; ext4load mmc 0:1 0x48000000 boot/r8a77990-ebisu-4d_cr7_rproc.dtb; booti 0x48080000 - 0x48000000'
    
    
    git clone https://github.com/renesas-rcar/meta-renesas
```
