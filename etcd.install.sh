#!/bin/bash
if [ -f "/usr/local/bin/etcd" ]; then
	echo "etcd is existing"
    svr=`ps -ef | grep etcd | grep data-dir | grep -v grep`
    if [ ! "$svr" ]; then
        nohup etcd --data-dir '/tmp/etcd' > /dev/null &
    fi
	exit
fi
wget https://github.com/coreos/etcd/releases/download/v3.3.5/etcd-v3.3.5-linux-amd64.tar.gz --no-check-certificate
tar zxvf etcd-v3.3.5-linux-amd64.tar.gz
cp etcd-v3.3.5-linux-amd64/etcd etcd-v3.3.5-linux-amd64/etcdctl /usr/local/bin
rm -rf /tmp/etcd
mkdir -p /tmp/etcd
nohup etcd --data-dir '/tmp/etcd' 1> /dev/null 2>&1 &
rm -rf etcd-v3.3.5-linux-amd64 etcd-v3.3.5-linux-amd64.tar.gz
