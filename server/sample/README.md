**Do not run install.sh unless you have read this!!**

This directory contains some scripts for starting and stopping vlc-server,
along with an rc script containing configuration. These files *will* need
to be edited before installation. Some will be inappropriate.

`vlc-server.rc`  
This is the main configuration file. Please read it carefully, and edit the
appropriate settings.

`start-vlc-server.sh` and `stop-vlc-server.sh`  
These scripts are designed to be run by a user (not by root). They can
be linked to a desktop initialization command, for example, to start
and stop with login and logout.

`vlcserverd.openrc` 
This is an init script for use by openrc. This needs to be copied to
`/etc/init.d`. If using this, ensure that the USER and GROUP settings
in `vlc-server.rc` are correct -- you don't want to run this server
as `root` -- it would be a terrible security risk.

