YOCTO NOTE
====================
系統設定
--------------------
#### ■ 不進入睡眠模式

`Settings` -> `Power` -> `Power Saving` -> `Blank Screen`

#### ■ Log in automatically 自動登入

`Settings` -> `Users` -> `Unlock...` (右上角) -> `Automatic Login`
    
#### ■ 自動 mount 最後的裝置
>mount_rcar_storage.sh
```
tar_part=$(lsblk | tail -2 | head -1 | awk '{print $1}' | cut -c 7-)
sudo mount /dev/$tar_part rcar
```

--------------------
問題排除
--------------------
#### ■  Seagate 2TB USB HDD 無法被偵測
以下兩種方法解決  
 1. 使用 USB2.0 HUB  
 2. 使用以下命令來停用 UAS
> go to `/var/log/syslog` to find idVendor and idProduct:
```
echo options usb-storage quirks=0bc2:ac31:u | sudo tee /etc/modprobe.d/ignore_uas.conf
sudo locale-gen --purge --no-archive
sudo reboot
```
#### ■ Seagate 2TB USB HDD 無法被偵測(ubuntu 20.04+VMWare)
使用以下設定成功  
```
Player -> Manager -> Virtual Machine Settings -> USB Controller -> Connectors  
USB compatibility: USB3.1
```

#### ■ Structure needs cleaning
    sudo fsck.ext4 /dev/sdb1   

#### ■ 在 VMware Player 分享資料夾不會出現
**1. 方法一：**
```
Player -> Manager -> Virtual Machine Settings -> Option -> Shared folders   
Disable 後再 Eanble   
```
**2. 方法二**： 可設為 shell，開機執行
```
sudo /usr/bin/vmhgfs-fuse .host:/ /mnt/hgfs -o subtype=vmhgfs-fuse,allow_other
```

#### ■ 出現 cannot find -lSM collect2: error: ld returned 1 exit status
似乎是找不到libsm
```
    bitbake -s | grep libsm     # 檢查包含 libsm 的 package
    libsm
    libsm-native
    libsmi
    libpcre
    lib-
```

--------------------
BITBKAE1
--------------------
### Bitbake 是一個工作排程器
```
bitbake recipe
```

Bitbake編譯時，使用多工來排程編譯工作。  
以上的指令會在 BBFILES 找查 recipe.bb，每個 recipe 定義了編譯工作

#### ■ SOURCE_DATE_EPOCH issue
如下錯誤訊息：  
error: environment variable SOURCE_DATE_EPOCH must expand to a non-negative integer less than or equal to 253402300799  
利用以下方法：

    SOURCE_DATE_EPOCH=$(date +%s)
    export SOURCE_DATE_EPOCH

#### ■ Invalid line
發生如下錯誤
```
ERROR: Invalid line '' in sstate manifest '/home/p/mnt/v3h/build/tmp/sstate-control/index-condor'  
```
直接刪除 `tmp/sstate-control/index-condor` 即可繼續

#### ■ bitbake error 處理方法
```
bitbake <package name> -c cleansstate
Ex.
    bitbake <package> -c cleansstate
    bitbake <package> -c compile
    bitbake <package>
```
 When reusing the build directory, sometimes compilation errors are seen; to overcome these, a fast solution is to clean the Share State Cache of the particular recipe/package
 
    $ bitbake <name of the recipe> -c cleansstate

Re-run the recipe  

    $ bitbake <name of the recipe>

Re-run the bitbake command you were running, before getting into trouble. For example:

    $ bitbake fsl-image-gui

Fetches all remote sources required to build a target

    bitbake -c do_fetchall <target>

新版的(如sumo)要用以下參數

    bitbake <target> --runall=fetch

#### ■ 顯示所有可用的套件 Show all available package
shows any *available* package locally. Grep for the name of desired package:

    bitbake -s  
    bitbake -s | grep python

#### ■ 檢查版本
    git --version
    tar --version
    python --version

#### ■ 編譯越多越好
    bitbake -k core-image-minimal
一般來講，bitbake會在第一個錯誤發生的時候停止，-k 參數在發生錯誤後，儘可能的編譯。

#####■  列出工作
    bitbake -c listtasks myrecipe

local.conf
```
# Uncomment and set to allow bitbake to execute multiple tasks at once.
# For a quadcore, BB_NUMBER_THREADS = "4", PARALLEL_MAKE = "-j 4" would
# be appropriate.
# BB_NUMBER_THREADS = "4"
# Also, make can be passed flags so it run parallel threads e.g.:
# PARALLEL_MAKE = "-j 4"
```

#### ■ 學習網站
    https://www.cnblogs.com/zxc2man/p/14545663.html

--------------------
LINUX
--------------------

Kconfig 文件的用途 (Make 選單的建立)