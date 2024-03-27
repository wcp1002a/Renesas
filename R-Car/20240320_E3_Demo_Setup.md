20240320_E3_Demo_Setup
======================
[TFTP & NFS](TFTP&NFS.md)
2024/03/20 TEST
----------------------------------------
```
# Format TF card
sudo mkfs.ext3 /dev/sdb1

# 解壓縮 rootfs
sudo tar -jxv -f e3_backup/core-image-minimal-ebisu.tar.bz2 -C pmnt/
sync
# copy dtb
sudo cp /mnt/hgfs/share/e3_bsp/r8a77990-ebisu.dtb pmnt/boot/

```

ext4load mmc 1:0 0x48080000 boot/Image

```
# Original sd1load
setenv sd1load 'fatload mmc 1:1 0x48080000 Image;fatload mmc 1:1 0x48000000 r8a77990-ebisu-4d.dtb;booti 0x48080000 - 0x48000000;run prodsd1bootargs'
setenv prodsd1bootargs 'rw rootwait earlycon root=/dev/mmcblk2p2'

# New
setenv sd1load 'ext4load mmc 1:1 0x48080000 /boot/Image;ext4load mmc 1:1 0x48000000 /boot/r8a77990-ebisu.dtb;booti 0x48080000 - 0x48000000; run prodsd1bootargs'
setenv prodsd1bootargs 'rw root=/dev/mmcblk2p1 rootwait'
```

```
setenv bootargs 'rw root=/dev/mmcblk2p1 rootwait'
setenv bootcmd 'ext4load mmc 1:1 0x48080000 boot/Image;ext4load mmc 1:1 0x48000000 boot/r8a77990-ebisu-4d.dtb;booti 0x48080000 - 0x48000000'
saveenv
```




```
temp

mount /dev/mmcblk0p1 /mnt
cd /mnt/usr
cp /home/mmc/usr/ . -r

```
sudo tar xvf core-image-minimal-ebisu.tar.bz2

mkfs.ext4 /dev/mmcblk0p1
mount /dev/mmcblk0p1 /mnt
cd /mnt
tar vxf /home/rootfs_QtPoC_fastboot.tar.gz

IPL 直接運行 ttl (包含燒錄 MiniMonitor)  
    `Hyper-AutoOrSelectTransfer_UltraSpeed.ttl`

    ====================
    SERIAL DOWNLOAD MODE
    ====================
    SW10: ON OFF OFF OFF OFF OFF OFF OFF
    ==> Boot from C53, DDR1600, SCIF download mode


    ====================
    Hyper Flash Mode (booted by CR7)
    ====================
    SW10: OFF OFF OFF ON ON ON OFF ON
    ==> Boot from C53, DDR1600, boot from HyperFlash

##### note
要支援 mkfs.ext4

    $ source poky/oe-init-build-env
    $ bitbake e2fsprogs

mkfs.ext4 命令會存在以下目錄，將目錄下所有的目錄複制到rootbs的目錄下即可

    ~/build/tmp/work/aarch64-poky-linux/e2fsprogs/1.45.4-r0/image$

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

##### tftp setting

Clear uBoot settings

    env default -a

tf Card
```
    setenv bootargs 'rw root=/dev/mmcblk2p1 rootwait'
    setenv bootcmd 'ext4load mmc 1:1 0x48080000 boot/Image;ext4load mmc 1:1 0x48000000 boot/r8a77990-ebisu.dtb;booti 0x48080000 - 0x48000000'
    saveenv

    setenv bootargs 'rw root=/dev/mmcblk2p1 rootwait'
    setenv bootcmd 'fatload mmc 1:1 0x48080000 boot/Image;fatload mmc 1:1 0x48000000 boot/r8a77990-ebisu-4d_cr7_rproc.dtb;booti 0x48080000 - 0x48000000'
    saveenv
```

NFS boot
```
setenv ethaddr '2e:09:0a:03:9b:12'
setenv ipaddr '192.168.2.3'
setenv serverip '192.168.2.1'
setenv netmask '255.255.255.0'

setenv 'bootargs' 'consoleblank=0 console=ttySC0,115200 rw root=/dev/nfs nfsroot=192.168.2.1:/home/p/rcar/e3/rfs/,nfsvers=3 ip=192.168.2.3'
setenv bootcmd 'tftp 0x48080000 Image;tftp 0x48000000 r8a77990-ebisu.dtb;booti 0x48080000 - 0x48000000'
```

##### 20240325 13:06 uBoot parameters backup
```
Usage:
ls <interface> [<dev[:part]> [directory]]
    - List files in directory 'directory' of partition 'part' on
      device type 'interface' instance 'dev'.
=> pri
baudrate=115200
bootargs=consoleblank=0 console=ttySC0,115200 rw root=/dev/nfs nfsroot=192.168.2.1:/home/p/rcar/e3/rfs/,nfsvers=3 ip=192.168.2.3
bootcmd=tftp 0x48080000 Image;tftp 0x48000000 r8a77990-ebisu.dtb;booti 0x48080000 - 0x48000000
bootdelay=0
bootm_size=0x10000000
emmcload=ext4load mmc 2:1 0x48080000 /boot/Image;ext4load mmc 2:1 0x48000000 /boot/r8a77990-ebisu-4d_cr7_rproc.dtb;booti 0x48080000 - 0x48000000;run prodemmcbootargs
ethact=ethernet@e6800000
ethaddr=2e:09:0a:03:9b:12
fdtcontroladdr=bfe17c68
fileaddr=48080000
filesize=1fa1a00
ipaddr=192.168.2.3
loadaddr=0x58000000
netmask=255.255.255.0
nfsinfo=tftp 0x48080000 Image;tftp 0x48000000 r8a77990-ebisu-4d.dtb;booti 0x48080000 - 0x48000000; run nfsroot
nfsroot=setenv bootargs rw root=/dev/nfs nfsroot=10.1.82.10:/mnt/hardisk1/tftpboot/rootfs,nfsvers=3 ip=10.1.82.9
platform=r8a77990
prodemmcbootargs=setenv bootargs rw rootwait earlycon root=/dev/mmcblk0p1
prodsd1bootargs=rw rootwait earlycon root=/dev/mmcblk2p2
sd1load=fatload mmc 1:1 0x48080000 Image;fatload mmc 1:1 0x48000000 r8a77990-ebisu-4d.dtb;booti 0x48080000 - 0x48000000;run prodsd1bootargs
serverip=192.168.2.1
stderr=serial@e6e88000
stdin=serial@e6e88000
stdout=serial@e6e88000
usb_pgood_delay=2000
ver=U-Boot 2018.09 (Aug 04 2020 - 09:20:33 +0000)

Environment size: 1284/131068 bytes
=>
```
##### 20240320 16:41 uBoot parameters backup

```
baudrate=115200
bootargs=rw rootwait earlycon root=/dev/mmcblk0p1
bootcmd=if mmc dev 1; then run sd1load; else run emmcload; fi
bootdelay=0
bootm_size=0x10000000
emmcload=ext4load mmc 2:1 0x48080000 /boot/Image;ext4load mmc 2:1 0x48000000 /boot/r8a77990-ebisu-4d_cr7_rproc.dtb;booti 0x48080000 - 0x48000000;run prodemmcbootargs
ethact=ethernet@e6800000
ethaddr=00:12:34:56:78:9A
fdtcontroladdr=bfe17c68
fileaddr=48080000
filesize=1fa1a00
ipaddr=10.1.82.9
loadaddr=0x58000000
nfsinfo=tftp 0x48080000 Image;tftp 0x48000000 r8a77990-ebisu-4d.dtb;booti 0x48080000 - 0x48000000; run nfsroot
nfsroot=setenv bootargs rw root=/dev/nfs nfsroot=10.1.82.10:/mnt/hardisk1/tftpboot/rootfs,nfsvers=3 ip=10.1.82.9
platform=r8a77990
prodemmcbootargs=setenv bootargs rw rootwait earlycon root=/dev/mmcblk0p1
prodsd1bootargs=rw rootwait earlycon root=/dev/mmcblk2p2
sd1load=fatload mmc 1:1 0x48080000 Image;fatload mmc 1:1 0x48000000 r8a77990-ebisu-4d.dtb;booti 0x48080000 - 0x48000000;run prodsd1bootargs
serverip=10.1.82.10
stderr=serial@e6e88000
stdin=serial@e6e88000
stdout=serial@e6e88000
usb_pgood_delay=2000
ver=U-Boot 2018.09 (Aug 04 2020 - 09:20:33 +0000)

Environment size: 1147/131068 bytes
=> 60, 60
```

Appendix
----------------------------------------
### Setup a TFTP server.
<!-- ### TFTP -->
#### 1 Install tftpd-hpa package along with tftp tools:
```
    sudo apt-get install tftp tftpd-hpa
```
#### 2 Copy Image and XXXX.dtb from $WORK/build/tmp/deploy/images/<board_name>/ to TFTP server root.
```
    cp $WORK/build/tmp/deploy/images/h3ulcb/Image /srv/tftp/
    cp $WORK/build/tmp/deploy/images/h3ulcb/XXXX.dtb /srv/tftp/
```
#### 3 Verify that TFTP server is working.
```
    tftp localhost
    tftp> get Image
    tftp> quit
    ls Image
```
#### 4 Setup NFS server.
Install necessary packages:
```
sudo apt-get install nfs-kernel-server nfs-common
```
Start NFS server:
```
sudo /etc/init.d/nfs-kernel-server start
```
#### 5 Export root FS to NFS. (Change IMAGE and MACHINE to fit your build).
Unpack rootfs to a dedicated directory:
```
IMAGE=weston
MACHINE=h3ulcb|m3ulcb
NFS_ROOT=/nfs/${MACHINE}
sudo mkdir -p "${NFS_ROOT}"
sudo rm -rf "${NFS_ROOT}"/*
sudo tar -xjf "${WORK}/build/tmp/deploy/images/${MACHINE}/core-image-${IMAGE}-${MACHINE}-*.tar.bz2" -C "${NFS_ROOT}"
sync
```
Edit /etc/exports:
```
sudo vi /etc/exports
```
add  
```
    /nfs/h3ulcb *(rw,no_subtree_check,sync,no_root_squash,no_all_squash)
    /nfs/m3ulcb *(rw,no_subtree_check,sync,no_root_squash,no_all_squash)
```
Save the file and exit.

Force NFS server to re-read /etc/exports
```
sudo exportfs -a
```

Verify that NFS is working.
```
[builduser@buildmachine ~]$ showmount -e localhost
Export list for localhost:
/nfs/h3ulcb *
/nfs/m3ulcb *
```


    sudo vi /etc/default/tftpd-hpa

    sudo systemctl status tftpd-hpa
    p@ubuntu:~/rcar/e3/rfs$ cat /etc/default/tftpd-hpa
    # /etc/default/tftpd-hpa
    
    TFTP_USERNAME="tftp"
    #TFTP_DIRECTORY="/home/p/rcar/m3/tftp"
    TFTP_DIRECTORY="/home/p/rcar/e3/tftp"
    TFTP_ADDRESS=":69"
    TFTP_OPTIONS="--secure --create"

mkfs
----------------------------------------
```
tar -jcvf mkfs.tar.bz2
```
tar -jxvf mkfs.tar.bz2 -C ./rfs
### NFS

    sudo vi /etc/exports

    /home/p/rcar/e3/rfs *(rw,async,no_root_squash)

    sudo /etc/init.d/nfs-kernel-server start
