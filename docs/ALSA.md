# ALSA notes

## Basics

`vlc-server` should work without any specific audio configuration, if the
host system as a whole is set up properly. VLC will output to its default
audio driver, which is probably Pulse on mainstream Linux installations.
With Pulse, if you want to change how the audio is routed, you'll need to
use Pulse tools (like `pavucontrol`).

However, `vlc-server` is capable of using whatever audio configuration is
available to VLC, including controlling the audio output. You might want to
do that if you're using high-quality audio hardware like an external DAC and
amplifier, to avoid Pulse and the like fiddling with the audio. Although I
believe Pulse has improved in this respect, it has a reputation for
unnecessary modification (e.g., resampling) of the audio stream. 

Sending the audio output directly to ALSA avoids this kind of silliness,
although Linux distributions vary in the extent to which they will allow
applications to do so. If you're building a customer Linux for an
embedded audio player, you probably won't be including Pulse or Pipewire
or such-like, and the problem won't arise. For mainstream desktop
systems, however, you may have a bit of fight.

To set a specific ALSA device, run `vlc-server` like this:

    vlc-server ... -- --no-video \
        -A alsa --alsa-audio-device iec958:CARD=I20,DEV=0
 
If you have `aplay` installed, you can get a list of available devices
using `aplay -L`. Some trial-and-error may be required, to find the 
device that works best.

## Fighting the desktop

I mostly run `vlc-server` on Raspberry Pi systems, using a custom Linux,
so the above is all I need to know. Running it on a mainstream desktop
(Fedora, Ubuntu...) is, however, not straightforward.

First, Pulse, Pipewire, etc., all fight for access to ALSA. It 
_should_ be possible to configure shared access, but I've never found
a reliable way to do it. Sometimes, if `vlc-server` appears to be
playing but no sound is output, I have to `kill` `pulseaudio`
or `pipewire-pulse`. This does no harm, because they will just get
restarted.

Some (most?) desktop Linux distributions use various kluges to 
set the permissions on entries in `/dev/` when a user logs into 
the console. Generally this doesn't work for entries in `/dev/snd`,
which is where the ALSA devices are. These devices are usually in
the ground `audio`, so the user that runs `vlc-server` needs to be
in that group. I've noticed that VLC doesn't complain when asked
to use an ALSA device to which the user has no access -- it just
doesn't produce any sound. 









