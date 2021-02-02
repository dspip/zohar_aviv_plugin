# Zohar/Aviv - Plugin

# overview

This repo contains an example for extracting video frames from a gstreamer pipeline, as-well as a few plugins enabling pipeline creation.

in **_main()_** you can modify which pipeline will be played

```
int main(int argc, char *argv[]) {
    App app;
    app.init();
    app.play_screen();
    // app.play_get_video_packets();
    app.start();
    return 0;
}
```

-  **app.play_screen()**  
   plays the video to the screen via `autovideosink`
-  **app.play_get_video_packets()**  
   extracts video frames, accessible via the `on_video_frame` method.

Both options run the pipeline from file located in `video` folder, and not from the camera.

If you wish to use a live source, ie view `udpsrc` switch between `filesrc` and `udpsrc` inside both `play_get_video_packets` or `play_screen`

```
s
    << "filesrc location=\"videos/h264FirstSensorD.pcap\" ! pcapparse dst-port=5002 ! "
    // << "udpsrc address=0.0.0.0 port=5002 ! "
    << "capsfilter caps=\"application/x-rtp, encoding-name=H264, clock-rate=90000\" ! "
    << "rtpjitterbuffer latency=0 ! "
    << "rtph264pass ! "
    << "h264parse ! "
    << "avdec_h264 ! "
    << "videocrop top=0 left=0 right=320 bottom=240 ! "
    << "appsink name=appsink emit-signals=true";
```

## rtph264pass

this repo include a plugin called rtph264pass, that serves as a direct replacement for rtph264depay. the plugin is meant to deal with invalid rtp h264 packets, where h264 was sliced and inserted into rtp packets without proper nal-u.

the plugin extracts the rtp payload buffer and passes it forward as is.

### usage:

```
gst-launch-1.0 filesrc location="./videos/h264FirstSensorD.pcap" ! pcapparse dst-ip=10.201.41.100 dst-port=5002 ! rtph264pass ! h264parse ! avdec_h264 ! autovideosink
```

```
gst-launch-1.0 udpsrc address=?? port=?? ! capsfilter caps="application/x-rtp, encoding-name=H264, clock-rate=90000" ! rtpjitterbuffer ! rtph264pass ! h264parse ! avdec_h264 ! autovideosink -v
```

## test using docker

just run the shell script `./run.sh`

```
./run.sh
```

or manually

```
docker build -t test:latest .
docker run -it --rm --network=host -v /tmp/.X11-unix:/tmp/.X11-unix -e DISPLAY=$DISPLAY test:latest
```

## compilation

```
sudo mkdir build &&
    cd build &&
    cmake .. &&
    cmake --build . &&
    make install
```

after compilation, the plugin should appear when using `gst-inspect-1.0`:

```
$ gst-inspect-1.0 rtph264pass
```

for a more comprehensive guide, including prerequisites follow dockerfile.
