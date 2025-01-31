# Sync files with Unison

## Install unison

```apt-get install unison```

On both: server and client

## Config file

.ssh/config
```
Host server
   User user
   Hostname myserver.oceanspy.net
   Port 22
```

.unison/todoos-sync.prf
```
# Unison synchronization profile
root = /path
root = ssh://*server*//path
auto = true
batch = true
confirmbigdel = false
```

## Add to cron

```
crontab -e
```

```
* * * * * unison todoos-sync
```
