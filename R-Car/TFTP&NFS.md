TFTP&NFS
========

Setup a TFTP server.
----------------------------------------
<!-- ### TFTP -->
**1.** Install tftpd-hpa package along with tftp tools:
```
sudo apt-get install tftp tftpd-hpa
```

**2.** Copy Image and `XXXX.dtb` from `$WORK/build/tmp/deploy/images/<board_name>/` to TFTP server root.
```
cp $WORK/build/tmp/deploy/images/h3ulcb/Image /srv/tftp/
cp $WORK/build/tmp/deploy/images/h3ulcb/XXXX.dtb /srv/tftp/
```

**3.** Verify that TFTP server is working.
```
tftp localhost
tftp> get Image
tftp> quit
ls Image
```

**4.** sEdit `tftpd-hpa`
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
```
----------------------------------------
Setup NFS server.
----------------------------------------
**1.** Install necessary packages:
```
sudo apt-get install nfs-kernel-server nfs-common
```


**2.** Edit `/etc/exports`:
```
sudo vi /etc/exports
```

add  rootfs folder
```
/nfs/h3ulcb *(rw,no_subtree_check,sync,no_root_squash,no_all_squash)
/nfs/m3ulcb *(rw,no_subtree_check,sync,no_root_squash,no_all_squash)
```
Save the file and exit.

Force NFS server to re-read /etc/exports
```
sudo exportfs -a
```

**3.** Start NFS server:
```
sudo /etc/init.d/nfs-kernel-server start
```



Verify that NFS is working.
```
[builduser@buildmachine ~]$ showmount -e localhost
Export list for localhost:
/nfs/h3ulcb *
/nfs/m3ulcb *
```

**4.** Export rootfs to NFS. (Change `IMAGE` and `MACHINE` to fit your build).  
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


----------------------------------------
Other
----------------------------------------

#### mkfs.ext4 support

```
tar -jcvf mkfs.tar.bz2
```
tar -jxvf mkfs.tar.bz2 -C ./rfs

